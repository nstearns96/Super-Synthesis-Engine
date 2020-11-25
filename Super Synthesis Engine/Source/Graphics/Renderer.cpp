#include "Graphics/Renderer.h"

#include "Logging/Logger.h"

#include "Model/Model.h"

#include "Resources/ResourceManager.h"

#include "Vulkan/Devices/VulkanDeviceManager.h"

#include "Vulkan/Shaders/Descriptors/VulkanDescriptorPool.h"

#include "Window/WindowManager.h"

namespace SSE
{
	extern Logger gLogger;

	namespace Graphics
	{
		Renderer Renderer::gRenderer;

		bool Renderer::create()
		{
			if (!swapChain.create())
			{
				return false;
			}

			if (!renderPass.create(swapChain))
			{
				return false;
			}

			if (!swapChain.createFramebuffers(renderPass, frameBuffers))
			{
				return false;
			}

			if (!commandPool.create(VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT))
			{
				return false;
			}

			if (!commandPool.allocateBuffers((u32)frameBuffers.size()))
			{
				return false;
			}

#pragma message("TODO: Abstract sync object creation")
			imageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
			renderFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
			inFlightFences.resize(MAX_FRAMES_IN_FLIGHT);
			imagesInFlight.resize(swapChain.getImagesInFlight(), VK_NULL_HANDLE);

			VkSemaphoreCreateInfo semaphoreInfo{};
			semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

			VkFenceCreateInfo fenceInfo{};
			fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
			fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

			for (st s = 0; s < MAX_FRAMES_IN_FLIGHT; ++s)
			{
				if (vkCreateSemaphore(LOGICAL_DEVICE_DEVICE, &semaphoreInfo, nullptr, &imageAvailableSemaphores[s]) != VK_SUCCESS ||
					vkCreateSemaphore(LOGICAL_DEVICE_DEVICE, &semaphoreInfo, nullptr, &renderFinishedSemaphores[s]) != VK_SUCCESS ||
					vkCreateFence(LOGICAL_DEVICE_DEVICE, &fenceInfo, nullptr, &inFlightFences[s]) != VK_SUCCESS)
				{
					GLOG_CRITICAL("Failed to create semaphores.");
					return false;
				}
			}

			pipelines.resize(1024);

			currentFrame = 0;

			return true;
		}

		void Renderer::destroy()
		{
			vkDeviceWaitIdle(LOGICAL_DEVICE_DEVICE);

			for (auto& pipeline : pipelines)
			{
				pipeline.destroy();
			}

			pipelines.clear();
			pipelineHandles.clear();

			for (st s = 0; s < MAX_FRAMES_IN_FLIGHT; ++s)
			{
				vkDestroySemaphore(LOGICAL_DEVICE_DEVICE, renderFinishedSemaphores[s], nullptr);
				vkDestroySemaphore(LOGICAL_DEVICE_DEVICE, imageAvailableSemaphores[s], nullptr);
				vkDestroyFence(LOGICAL_DEVICE_DEVICE, inFlightFences[s], nullptr);
			}

			for (auto framebuffer : frameBuffers)
			{
				vkDestroyFramebuffer(LOGICAL_DEVICE_DEVICE, framebuffer, nullptr);
			}

			commandPool.destroy();

			renderPass.destroy();
			swapChain.destroy();
		}

		bool Renderer::executeRenderPass()
		{
			vkWaitForFences(LOGICAL_DEVICE_DEVICE, 1, &inFlightFences[currentFrame], VK_TRUE, UINT64_MAX);

			if (WindowManager::gWindowManager.getWindowCount() != 0 && WindowManager::gWindowManager.getActiveWindow().getStatus() & WindowStatusFlags::WINDOW_MINIMIZED)
			{
				return false;
			}

			u32 imageIndex;
			VkResult result = vkAcquireNextImageKHR(LOGICAL_DEVICE_DEVICE, swapChain.getSwapChain(), UINT64_MAX,
				imageAvailableSemaphores[currentFrame], VK_NULL_HANDLE, &imageIndex);

			if (result == VK_ERROR_OUT_OF_DATE_KHR)
			{
				GLOG_WARNING("Swap chain out of date. Recreating swap chain.");
				recreateSwapChain();
				return false;
			}
			else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
			{
				GLOG_CRITICAL("Failed to acquire swap chain image.");
				return false;
			}

			if (imagesInFlight[imageIndex] != VK_NULL_HANDLE)
			{
				vkWaitForFences(LOGICAL_DEVICE_DEVICE, 1, &imagesInFlight[imageIndex], VK_TRUE, UINT64_MAX);
			}

			if (!constructCommandBuffer(imageIndex))
			{
				return false;
			}

			imagesInFlight[imageIndex] = inFlightFences[currentFrame];

			VkSubmitInfo submitInfo{};
			submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

			VkSemaphore waitSemaphores[] = { imageAvailableSemaphores[currentFrame] };
#pragma message("TODO: Query pipeline layout for attachments")
			VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
			submitInfo.waitSemaphoreCount = 1;
			submitInfo.pWaitSemaphores = waitSemaphores;
			submitInfo.pWaitDstStageMask = waitStages;
			submitInfo.commandBufferCount = 1;
			submitInfo.pCommandBuffers = commandPool.getCommandBuffer(imageIndex);

			VkSemaphore signalSemaphores[] = { renderFinishedSemaphores[currentFrame] };
			submitInfo.signalSemaphoreCount = 1;
			submitInfo.pSignalSemaphores = signalSemaphores;

			vkResetFences(LOGICAL_DEVICE_DEVICE, 1, &inFlightFences[currentFrame]);

			Vulkan::VulkanLogicalDevice logicalDevice = LOGICAL_DEVICE;
			VkResult submitResult = logicalDevice.submit(submitInfo, &inFlightFences[currentFrame]);

			VkPresentInfoKHR presentInfo{};
			presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
			presentInfo.waitSemaphoreCount = 1;
			presentInfo.pWaitSemaphores = signalSemaphores;

			VkSwapchainKHR swapChains[] = { swapChain.getSwapChain() };
			presentInfo.swapchainCount = 1;
			presentInfo.pSwapchains = swapChains;
			presentInfo.pImageIndices = &imageIndex;
			presentInfo.pResults = nullptr;

			logicalDevice.present(presentInfo);

			currentFrame = (++currentFrame) % MAX_FRAMES_IN_FLIGHT;

			if (submitResult == VK_ERROR_OUT_OF_DATE_KHR || submitResult == VK_SUBOPTIMAL_KHR || isFrameBufferDirty)
			{
				if (isFrameBufferDirty)
				{
					GLOG_WARNING("Frame buffer size changed. Recreating swap chain.");
					isFrameBufferDirty = false;
				}
				else
				{
					GLOG_WARNING("Swap chain out of date or suboptimal. Recreating swap chain.");
				}

				recreateSwapChain();
				return false;
			}

			return true;
		}

		bool Renderer::recreateSwapChain()
		{
			vkDeviceWaitIdle(LOGICAL_DEVICE_DEVICE);

			for (auto framebuffer : frameBuffers)
			{
				vkDestroyFramebuffer(LOGICAL_DEVICE_DEVICE, framebuffer, nullptr);
			}

			commandPool.freeBuffers();

			vkDestroyRenderPass(LOGICAL_DEVICE_DEVICE, renderPass.getRenderPass(), nullptr);

			swapChain.destroy();

#pragma message("TODO: If the active window has changed, this may cause a crash. Add functionality to check for that")
			if (!swapChain.create())
			{
				return false;
			}

			if (!renderPass.create(swapChain))
			{
				return false;
			}

			if (!swapChain.createFramebuffers(renderPass, frameBuffers))
			{
				return false;
			}

			for (auto& pipelineHandle : pipelineHandles.getActiveHandles())
			{
				pipelines[pipelineHandle].recreate(swapChain, renderPass);
			}

			if (!commandPool.allocateBuffers(MAX_FRAMES_IN_FLIGHT))
			{
				return false;
			}

			return true;
		}

		bool Renderer::constructCommandBuffer(u32 index)
		{
			VkCommandBuffer& currentCommandBuffer = commandPool.getNewCommandBuffer(index);

			VkCommandBufferBeginInfo beginInfo{};
			beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
			beginInfo.flags = 0;
			beginInfo.pInheritanceInfo = nullptr;

			if (vkBeginCommandBuffer(currentCommandBuffer, &beginInfo) != VK_SUCCESS)
			{
				GLOG_CRITICAL("Failed to begin command buffer.");
				return false;
			}

			VkRenderPassBeginInfo renderPassInfo{};
			renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
			renderPassInfo.renderPass = renderPass.getRenderPass();
			renderPassInfo.framebuffer = frameBuffers[index];
			renderPassInfo.renderArea.offset = { 0, 0 };
			renderPassInfo.renderArea.extent = swapChain.getExtent();

			std::vector<VkClearValue> clearValues = { { 0.0f, 0.0f, 0.0f, 1.0f }, { 1.0f, 0 } };
			renderPassInfo.clearValueCount = (u32)clearValues.size();
			renderPassInfo.pClearValues = clearValues.data();

			vkCmdBeginRenderPass(currentCommandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

			for (const auto& commandList : queue.getCommands())
			{
				GraphicsPipeline commandPipeline = pipelines[commandList.first.pipeline];
				vkCmdBindPipeline(currentCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, commandPipeline.getPipeline());
				vkCmdBindDescriptorSets(currentCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, commandPipeline.getLayout(), 0, 1, &Vulkan::gDescriptorPool.getDescriptorSet(currentCamera->getViewResourceHandle()), 0, nullptr);

				const Model* commandModel = ResourceManager::getModel(commandList.first.model);
				VkBuffer vertexBuffers[] = { commandModel->getVertexBuffer().getVertexBuffer() };
				VkDeviceSize offsets[] = { 0 };
				vkCmdBindVertexBuffers(currentCommandBuffer, 0, 1, vertexBuffers, offsets);
				vkCmdBindIndexBuffer(currentCommandBuffer, commandModel->getVertexBuffer().getIndexBuffer(), 0, VK_INDEX_TYPE_UINT16);

				for (const auto object : commandList.second)
				{
					const ResourceHandle* modelHandles = object;

					VkDescriptorSet modelDescriptors[2];
					modelDescriptors[0] = Vulkan::gDescriptorPool.getDescriptorSet(modelHandles[0]);
					modelDescriptors[1] = Vulkan::gDescriptorPool.getDescriptorSet(modelHandles[1]);
					vkCmdBindDescriptorSets(currentCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, commandPipeline.getLayout(), 1, 2, modelDescriptors, 0, nullptr);

					vkCmdDrawIndexed(currentCommandBuffer, (u32)commandModel->getVertexBuffer().getIndexCount(), 1, 0, 0, 0);
				}
			}

			vkCmdEndRenderPass(currentCommandBuffer);

			if (vkEndCommandBuffer(currentCommandBuffer) != VK_SUCCESS)
			{
				GLOG_CRITICAL("Failed to end command buffer.");
				return false;
			}

			queue.clear();

			return true;
		}

		bool Renderer::createPipeline(ResourceHandle shaderHandle)
		{
			GraphicsPipeline newPipeline;
			if (newPipeline.create(shaderHandle, swapChain, renderPass))
			{
				ResourceHandle resourceHandle = pipelineHandles.getHandle();
				pipelines[resourceHandle] = newPipeline;
				return true;
			}

			return false;
		}

		bool Renderer::setPipeline(ResourceHandle resourceHandle)
		{
			if (pipelineHandles.isHandleActive(resourceHandle))
			{
				currentPipelineHandle = resourceHandle;
				return true;
			}

			return false;
		}
		
		bool Renderer::setCamera(Camera* camera)
		{
			if (camera)
			{
				currentCamera = camera;
				return true;
			}

			return false;
		}

		bool Renderer::renderObject(const Object& object)
		{
			queue.enqueue({currentPipelineHandle, object.getModel()}, object.getModelDescriptorHandles());
			return true;
		}
	}
}
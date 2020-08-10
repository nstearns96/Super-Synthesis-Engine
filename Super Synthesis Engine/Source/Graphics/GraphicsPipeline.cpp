#include "Graphics/GraphicsPipeline.h"

#include <SDL/SDL_vulkan.h>
#include <glm/gtc/matrix_transform.hpp>

#include <chrono>

#include "Vulkan/Devices/VulkanDeviceManager.h"
#include "Logging/Logger.h"
#include "Graphics/Shader.h"
#include "Window/WindowManager.h"
#include "Graphics/Texture2D.h"
#include "Resources/ResourceManager.h"

namespace SSE
{
	extern Logger gLogger;

	namespace Graphics
	{
#pragma message("TODO: Add functionality for pipeline parameters")
		bool GraphicsPipeline::constructPipeline()
		{
			VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
			vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
			auto bindingDescription = vertexFormat.getBindingDescription();
			auto attributeDescriptions = vertexFormat.getAttributeDescriptions();

			vertexInputInfo.vertexBindingDescriptionCount = 1;
			vertexInputInfo.vertexAttributeDescriptionCount = (u32)attributeDescriptions.size();
			vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
			vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();

			VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
			inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
			inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
			inputAssembly.primitiveRestartEnable = VK_FALSE;

			VkViewport viewport{};
			viewport.x = 0.0f;
			viewport.y = 0.0f;
			viewport.width = (r32)swapChain.getExtent().width;
			viewport.height = (r32)swapChain.getExtent().height;
			viewport.minDepth = 0.0f;
			viewport.maxDepth = 1.0f;

			VkRect2D scissor{};
			scissor.offset = { 0, 0 };
			scissor.extent = swapChain.getExtent();

			VkPipelineViewportStateCreateInfo viewportState{};
			viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
			viewportState.viewportCount = 1;
			viewportState.pViewports = &viewport;
			viewportState.scissorCount = 1;
			viewportState.pScissors = &scissor;

			VkPipelineRasterizationStateCreateInfo rasterizer{};
			rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
			rasterizer.depthClampEnable = VK_FALSE;
			rasterizer.rasterizerDiscardEnable = VK_FALSE;
			rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
			rasterizer.lineWidth = 1.0f;
			rasterizer.cullMode = VK_CULL_MODE_NONE;
			rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
			rasterizer.depthBiasEnable = VK_FALSE;

			VkPipelineMultisampleStateCreateInfo multisampling{};
			multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
			multisampling.sampleShadingEnable = VK_FALSE;
			multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

			VkPipelineColorBlendAttachmentState colorBlendAttachment{};
			colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
			colorBlendAttachment.blendEnable = VK_FALSE;

			VkPipelineColorBlendStateCreateInfo colorBlending{};
			colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
			colorBlending.logicOpEnable = VK_FALSE;
			colorBlending.logicOp = VK_LOGIC_OP_COPY;
			colorBlending.attachmentCount = 1;
			colorBlending.pAttachments = &colorBlendAttachment;
			colorBlending.blendConstants[0] = 0.0f;
			colorBlending.blendConstants[1] = 0.0f;
			colorBlending.blendConstants[2] = 0.0f;
			colorBlending.blendConstants[3] = 0.0f;

			VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
			pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
			pipelineLayoutInfo.setLayoutCount = 1;
			pipelineLayoutInfo.pSetLayouts = descriptorPool.getLayouts();
			pipelineLayoutInfo.pushConstantRangeCount = 0;

			if (vkCreatePipelineLayout(LOGICAL_DEVICE_DEVICE, &pipelineLayoutInfo, nullptr, &layout) != VK_SUCCESS)
			{
				GLOG_CRITICAL("Failed to create pipeline layout.");
				return false;
			}

			VkPipelineDepthStencilStateCreateInfo depthStencil{};
			depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
			depthStencil.depthTestEnable = VK_TRUE;
			depthStencil.depthWriteEnable = VK_TRUE;
			depthStencil.depthCompareOp = VK_COMPARE_OP_LESS;
			depthStencil.depthBoundsTestEnable = VK_FALSE;
			depthStencil.minDepthBounds = 0.0f;
			depthStencil.maxDepthBounds = 1.0f;
			depthStencil.stencilTestEnable = VK_FALSE;
			depthStencil.front = {};
			depthStencil.back = {};

			std::vector<VkPipelineShaderStageCreateInfo> shaderStages = ResourceManager::getShader("main").getShaderStages();

			VkGraphicsPipelineCreateInfo pipelineInfo{};
			pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
			pipelineInfo.stageCount = (u32)shaderStages.size();
			pipelineInfo.pStages = shaderStages.data();
			pipelineInfo.pVertexInputState = &vertexInputInfo;
			pipelineInfo.pInputAssemblyState = &inputAssembly;
			pipelineInfo.pViewportState = &viewportState;
			pipelineInfo.pRasterizationState = &rasterizer;
			pipelineInfo.pMultisampleState = &multisampling;
			pipelineInfo.pColorBlendState = &colorBlending;
			pipelineInfo.layout = layout;
			pipelineInfo.renderPass = renderPass.getRenderPass();
			pipelineInfo.subpass = 0;
			pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;
			pipelineInfo.pDepthStencilState = &depthStencil;

			if (vkCreateGraphicsPipelines(LOGICAL_DEVICE_DEVICE, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &pipeline) != VK_SUCCESS)
			{
				GLOG_CRITICAL("Failed to create graphics pipeline.");
				return false;
			}

			return true;
		}

		bool GraphicsPipeline::create()
		{
			glm::uvec2 frameBufferDimensions = WindowManager::gWindowManager.getWindowFrameBufferDimensions();

			if (!swapChain.create(surface, frameBufferDimensions))
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

#pragma message("TODO: Load data from file via Asset namespace")
			float data[] = 
			{
				0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f,
				0.5f, 0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
				0.5f, -0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f,
				0.5f, -0.5f, -0.5f, 0.0f, 1.0f, 1.0f, 1.0f, 0.0f,
				-0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f,
				-0.5f, 0.5f, -0.5f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f,
				-0.5f, -0.5f, 0.5f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
				-0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f
			};

			vertexFormat.create({ VA_POS3F, VA_COL3F, VA_TEX2F });

			VertexData vertexData;
			vertexData.create
			(
				data,
				64 * sizeof(r32),
				vertexFormat
			);

			if (!vertexBuffer.create
				(
					vertexData,
					{
						0, 2, 1, 1, 2, 3,
						0, 1, 4, 1, 5, 4,
						4, 5, 6, 5, 7, 6,
						2, 6, 7, 2, 7, 3,
						0, 4, 6, 0, 6, 2,
						1, 3, 7, 1, 7, 5
					}
				)
			)
			{
				return false;
			}

			for (u32 i = 0; i < frameBuffers.size(); ++i)
			{
				UniformBuffer<MatricesObject> uniformBuffer;
				if (!uniformBuffer.create())
				{
					return false;
				}
				uniformBuffers.push_back(uniformBuffer);
			}

			if (!descriptorPool.create((u32)frameBuffers.size()))
			{
				return false;
			}

			for (u32 i = 0; i < frameBuffers.size(); ++i)
			{
				Texture2D texture = ResourceManager::getTexture("texture");
				descriptorPool.updateDescriptorSet(i, uniformBuffers[i].getBuffer(), texture.getImageView(), texture.getSampler());
			}

			if (!commandPool.create())
			{
				return false;
			}

			if (!constructPipeline())
			{
				return false;
			}

			if (!constructCommandBuffers())
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

			currentFrame = 0;

			return true;
		}

		void GraphicsPipeline::destroy()
		{
			vkDeviceWaitIdle(LOGICAL_DEVICE_DEVICE);
		
			vertexBuffer.destroy();

			for (st u = 0; u < uniformBuffers.size(); ++u)
			{
				uniformBuffers[u].destroy();
			}

			uniformBuffers.clear();

			descriptorPool.destroy();

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

			vkDestroyPipeline(LOGICAL_DEVICE_DEVICE, pipeline, nullptr);
			vkDestroyPipelineLayout(LOGICAL_DEVICE_DEVICE, layout, nullptr);

			commandPool.destroy();

			renderPass.destroy();
			swapChain.destroy();

			surface.destroy();
		}

		bool GraphicsPipeline::executeRenderPass()
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

			updateUniformBuffers(imageIndex);

			if (imagesInFlight[imageIndex] != VK_NULL_HANDLE) 
			{
				vkWaitForFences(LOGICAL_DEVICE_DEVICE, 1, &imagesInFlight[imageIndex], VK_TRUE, UINT64_MAX);
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

		bool GraphicsPipeline::recreateSwapChain()
		{
			vkDeviceWaitIdle(LOGICAL_DEVICE_DEVICE);

			for (auto framebuffer : frameBuffers)
			{
				vkDestroyFramebuffer(LOGICAL_DEVICE_DEVICE, framebuffer, nullptr);
			}

			commandPool.freeBuffers();

			vkDestroyPipeline(LOGICAL_DEVICE_DEVICE, pipeline, nullptr);
			vkDestroyPipelineLayout(LOGICAL_DEVICE_DEVICE, layout, nullptr);
			vkDestroyRenderPass(LOGICAL_DEVICE_DEVICE, renderPass.getRenderPass(), nullptr);

			swapChain.destroy();

#pragma message("TODO: If the active window has changed, this may cause a crash. Add functionality to check for that")
			glm::uvec2 frameBufferDimensions = WindowManager::gWindowManager.getWindowFrameBufferDimensions();

			if (!swapChain.create(surface, frameBufferDimensions))
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

			for (u32 i = 0; i < frameBuffers.size(); ++i)
			{
				UniformBuffer<MatricesObject> uniformBuffer;
				if (!uniformBuffer.create())
				{
					return false;
				}
				uniformBuffers.push_back(uniformBuffer);
			}

			if (!constructPipeline())
			{
				return false;
			}

			if (!constructCommandBuffers())
			{
				return false;
			}

			return true;
		}

		bool GraphicsPipeline::constructCommandBuffers()
		{
			if (!commandPool.allocateBuffers((u32)frameBuffers.size()))
			{
				return false;
			}

			for (u32 i = 0; i < frameBuffers.size(); i++)
			{
				VkCommandBuffer& currentCommandBuffer = commandPool.getNewCommandBuffer(i);

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
				renderPassInfo.framebuffer = frameBuffers[i];
				renderPassInfo.renderArea.offset = { 0, 0 };
				renderPassInfo.renderArea.extent = swapChain.getExtent();

				std::vector<VkClearValue> clearValues = { { 0.0f, 0.0f, 0.0f, 1.0f }, { 1.0f, 0 } };
				renderPassInfo.clearValueCount = (u32)clearValues.size();
				renderPassInfo.pClearValues = clearValues.data();

				vkCmdBeginRenderPass(currentCommandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
				vkCmdBindPipeline(currentCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);

				VkBuffer vertexBuffers[] = { vertexBuffer.getVertexBuffer() };
				VkDeviceSize offsets[] = { 0 };
				vkCmdBindVertexBuffers(currentCommandBuffer, 0, 1, vertexBuffers, offsets);

				vkCmdBindIndexBuffer(currentCommandBuffer, vertexBuffer.getIndexBuffer(), 0, VK_INDEX_TYPE_UINT16);

				vkCmdBindDescriptorSets(currentCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, layout, 0, 1, &descriptorPool.getDescriptorSet(i), 0, nullptr);

				vkCmdDrawIndexed(currentCommandBuffer, (u32)vertexBuffer.getIndexCount(), 1, 0, 0, 0);

				vkCmdEndRenderPass(currentCommandBuffer);

				if (vkEndCommandBuffer(currentCommandBuffer) != VK_SUCCESS)
				{
					GLOG_CRITICAL("Failed to end command buffer.");
					return false;
				}
			}

			return true;
		}

		void GraphicsPipeline::updateUniformBuffers(u32 imageIndex)
		{
			static auto startTime = std::chrono::high_resolution_clock::now();

			auto currentTime = std::chrono::high_resolution_clock::now();
			r32 time = std::chrono::duration<r32, std::chrono::seconds::period>(currentTime - startTime).count();

			MatricesObject ubo;

			ubo.model = glm::rotate(glm::mat4(1.0f), time * glm::radians(90.0f), glm::vec3(sin(time), cos(time), 1.0f));
			ubo.view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
			ubo.proj = glm::perspective(glm::radians(45.0f), (r32)swapChain.getExtent().width / (r32)swapChain.getExtent().height, 0.1f, 10.0f);
			ubo.proj[1][1] *= -1;

			uniformBuffers[imageIndex].updateBuffer(ubo);
		}
	}
}
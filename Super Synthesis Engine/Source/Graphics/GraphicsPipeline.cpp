#include "Graphics/GraphicsPipeline.h"

#include <SDL/SDL_vulkan.h>

#include "Vulkan/Devices/VulkanDeviceManager.h"
#include "Logging/Logger.h"
#include "Graphics/Shader.h"
#include "Window/WindowManager.h"
#include "Model/Vertex.h"

namespace SSE
{
	extern Logger gLogger;

	namespace Graphics
	{
#pragma message("TODO: Clean this function up")
		bool GraphicsPipeline::create()
		{
			Shader shader;
			if (!shader.create({ "frag.spv","vert.spv" }, { Vulkan::ShaderModuleType::SMT_FRAGMENT, Vulkan::ShaderModuleType::SMT_VERTEX }))
			{
				return false;
			}

#pragma message("TODO: This functionality should be wrapped in WindowManager")
			int w;
			int h;
			SDL_Vulkan_GetDrawableSize(WindowManager::gWindowManager.getActiveWindow().getWindow(), &w, &h);

			if (!swapChain.create(surface, w, h))
			{
				shader.destroy();
				return false;
			}

			if (!renderPass.create(swapChain))
			{
				shader.destroy();
				return false;
			}

			VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
			vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
			vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
			vertShaderStageInfo.module = shader.getShaderModule(Vulkan::ShaderModuleType::SMT_VERTEX).getModule();
			vertShaderStageInfo.pName = "main";

			VkPipelineShaderStageCreateInfo fragShaderStageInfo{};
			fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
			fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
			fragShaderStageInfo.module = shader.getShaderModule(Vulkan::ShaderModuleType::SMT_FRAGMENT).getModule();
			fragShaderStageInfo.pName = "main";

			VkPipelineShaderStageCreateInfo shaderStages[] = { vertShaderStageInfo, fragShaderStageInfo };

			VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
			vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
			auto bindingDescription = Vertex::getBindingDescription();
			auto attributeDescriptions = Vertex::getAttributeDescriptions();

			vertexInputInfo.vertexBindingDescriptionCount = 1;
			vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
			vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
			vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();

			VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
			inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
			inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
			inputAssembly.primitiveRestartEnable = VK_FALSE;

			VkViewport viewport{};
			viewport.x = 0.0f;
			viewport.y = 0.0f;
			viewport.width = (float)swapChain.getExtent().width;
			viewport.height = (float)swapChain.getExtent().height;
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
			rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
			rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
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
			pipelineLayoutInfo.setLayoutCount = 0;
			pipelineLayoutInfo.pushConstantRangeCount = 0;

			if (vkCreatePipelineLayout(LOGICAL_DEVICE_DEVICE, &pipelineLayoutInfo, nullptr, &layout) != VK_SUCCESS) 
			{
				gLogger.logError(ErrorLevel::EL_CRITICAL, "Failed to create pipeline layout.");
				shader.destroy();
				return false;
			}

			VkGraphicsPipelineCreateInfo pipelineInfo{};
			pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
			pipelineInfo.stageCount = 2;
			pipelineInfo.pStages = shaderStages;
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

			if (vkCreateGraphicsPipelines(LOGICAL_DEVICE_DEVICE, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &pipeline) != VK_SUCCESS) 
			{
				gLogger.logError(ErrorLevel::EL_CRITICAL, "Failed to create graphics pipeline.");
				shader.destroy();
				return false;
			}

			if (!swapChain.createFramebuffers(renderPass, frameBuffers))
			{
				shader.destroy();
				return false;
			}

			if (!vertexBuffer.create({
				{{-0.5f, -0.5f}, {0.0f, 0.0f, 0.0f}},
				{{0.5f, 0.5f}, {0.0f, 0.0f, 0.0f}},
				{{-0.5f, 0.5f}, {1.0f, 1.0f, 1.0f}},
				{{-0.5f, -0.5f}, {0.0f, 0.0f, 0.0f}},
				{{0.5f, -0.5f}, {1.0f, 1.0f, 1.0f}},
				{{0.5f, 0.5f}, {0.0f, 0.0f, 0.0f}}
				}))
			{
				shader.destroy();
				return false;
			}

			if (!commandPool.create(swapChain, renderPass, frameBuffers, pipeline, vertexBuffer))
			{
				shader.destroy();
				return false;
			}

			imageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
			renderFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
			inFlightFences.resize(MAX_FRAMES_IN_FLIGHT);
			imagesInFlight.resize(swapChain.getImagesInFlight(), VK_NULL_HANDLE);

			VkSemaphoreCreateInfo semaphoreInfo{};
			semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

			VkFenceCreateInfo fenceInfo{};
			fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
			fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

			for (int s = 0; s < MAX_FRAMES_IN_FLIGHT; ++s)
			{
				if (vkCreateSemaphore(LOGICAL_DEVICE_DEVICE, &semaphoreInfo, nullptr, &imageAvailableSemaphores[s]) != VK_SUCCESS ||
					vkCreateSemaphore(LOGICAL_DEVICE_DEVICE, &semaphoreInfo, nullptr, &renderFinishedSemaphores[s]) != VK_SUCCESS ||
					vkCreateFence(LOGICAL_DEVICE_DEVICE, &fenceInfo, nullptr, &inFlightFences[s]) != VK_SUCCESS)
				{
					gLogger.logError(ErrorLevel::EL_CRITICAL, "Failed to create semaphores.");
					shader.destroy();
					return false;
				}
			}

			shader.destroy();

			currentFrame = 0;

			return true;
		}

		void GraphicsPipeline::destroy()
		{
			vkDeviceWaitIdle(LOGICAL_DEVICE_DEVICE);
		
			vertexBuffer.destroy();

			for (int s = 0; s < MAX_FRAMES_IN_FLIGHT; ++s)
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

			uint32_t imageIndex;
			VkResult result = vkAcquireNextImageKHR(LOGICAL_DEVICE_DEVICE, swapChain.getSwapChain(), UINT64_MAX,
				imageAvailableSemaphores[currentFrame], VK_NULL_HANDLE, &imageIndex);

			if (result == VK_ERROR_OUT_OF_DATE_KHR) 
			{
				gLogger.logError(ErrorLevel::EL_WARNING, "Swap chain out of date. Recreating swap chain.");
				recreateSwapChain();
				return false;
			}
			else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) 
			{
				gLogger.logError(ErrorLevel::EL_CRITICAL, "Failed to acquire swap chain image.");
				return false;
			}

			if (imagesInFlight[imageIndex] != VK_NULL_HANDLE) 
			{
				vkWaitForFences(LOGICAL_DEVICE_DEVICE, 1, &imagesInFlight[imageIndex], VK_TRUE, UINT64_MAX);
			}
			
			imagesInFlight[imageIndex] = inFlightFences[currentFrame];

			VkSubmitInfo submitInfo{};
			submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

			VkSemaphore waitSemaphores[] = { imageAvailableSemaphores[currentFrame] };
			VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
			submitInfo.waitSemaphoreCount = 1;
			submitInfo.pWaitSemaphores = waitSemaphores;
			submitInfo.pWaitDstStageMask = waitStages;
			submitInfo.commandBufferCount = 1;
			submitInfo.pCommandBuffers = commandPool.getCommandBuffer(imageIndex);

			VkSemaphore signalSemaphores[] = { renderFinishedSemaphores[currentFrame] };
			submitInfo.signalSemaphoreCount = 1;
			submitInfo.pSignalSemaphores = signalSemaphores;

			Vulkan::VulkanLogicalDevice logicalDevice = LOGICAL_DEVICE;

			bool swapChainNeedsRecreate;
			vkResetFences(LOGICAL_DEVICE_DEVICE, 1, &inFlightFences[currentFrame]);
			logicalDevice.submit(submitInfo, inFlightFences[currentFrame], swapChainNeedsRecreate);
			if (swapChainNeedsRecreate || isFrameBufferDirty)
			{
				if (swapChainNeedsRecreate)
				{
					gLogger.logError(ErrorLevel::EL_WARNING, "Swap chain out of date or suboptimal. Recreating swap chain.");
				}
				else if (isFrameBufferDirty)
				{
					gLogger.logError(ErrorLevel::EL_WARNING, "Frame buffer size changed. Recreating swap chain.");

				}
				
				isFrameBufferDirty = false;
				recreateSwapChain();
				return false;
			}

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

			currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;

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

			Shader shader;
			if (!shader.create({ "frag.spv","vert.spv" }, { Vulkan::ShaderModuleType::SMT_FRAGMENT, Vulkan::ShaderModuleType::SMT_VERTEX }))
			{
				return false;
			}

#pragma message("TODO: This functionality should be wrapped in WindowManager")
#pragma message("TODO: If the active window has changed, this may cause a crash. Add functionality to check for that")
			int w;
			int h;
			SDL_Vulkan_GetDrawableSize(WindowManager::gWindowManager.getActiveWindow().getWindow(), &w, &h);

			if (!swapChain.create(surface, w, h))
			{
				shader.destroy();
				return false;
			}

			if (!renderPass.create(swapChain))
			{
				shader.destroy();
				return false;
			}

			VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
			vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
			vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
			vertShaderStageInfo.module = shader.getShaderModule(Vulkan::ShaderModuleType::SMT_VERTEX).getModule();
			vertShaderStageInfo.pName = "main";

			VkPipelineShaderStageCreateInfo fragShaderStageInfo{};
			fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
			fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
			fragShaderStageInfo.module = shader.getShaderModule(Vulkan::ShaderModuleType::SMT_FRAGMENT).getModule();
			fragShaderStageInfo.pName = "main";

			VkPipelineShaderStageCreateInfo shaderStages[] = { vertShaderStageInfo, fragShaderStageInfo };

			VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
			vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
			auto bindingDescription = Vertex::getBindingDescription();
			auto attributeDescriptions = Vertex::getAttributeDescriptions();

			vertexInputInfo.vertexBindingDescriptionCount = 1;
			vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
			vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
			vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();

			VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
			inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
			inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
			inputAssembly.primitiveRestartEnable = VK_FALSE;

			VkViewport viewport{};
			viewport.x = 0.0f;
			viewport.y = 0.0f;
			viewport.width = (float)swapChain.getExtent().width;
			viewport.height = (float)swapChain.getExtent().height;
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
			rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
			rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
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
			pipelineLayoutInfo.setLayoutCount = 0;
			pipelineLayoutInfo.pushConstantRangeCount = 0;

			if (vkCreatePipelineLayout(LOGICAL_DEVICE_DEVICE, &pipelineLayoutInfo, nullptr, &layout) != VK_SUCCESS)
			{
				gLogger.logError(ErrorLevel::EL_CRITICAL, "Failed to create pipeline layout.");
				shader.destroy();
				return false;
			}

			VkGraphicsPipelineCreateInfo pipelineInfo{};
			pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
			pipelineInfo.stageCount = 2;
			pipelineInfo.pStages = shaderStages;
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

			if (vkCreateGraphicsPipelines(LOGICAL_DEVICE_DEVICE, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &pipeline) != VK_SUCCESS)
			{
				gLogger.logError(ErrorLevel::EL_CRITICAL, "Failed to create graphics pipeline.");
				shader.destroy();
				return false;
			}

			if (!swapChain.createFramebuffers(renderPass, frameBuffers))
			{
				shader.destroy();
				return false;
			}

			if (!commandPool.allocateBuffers(swapChain, renderPass, frameBuffers, pipeline, vertexBuffer))
			{
				shader.destroy();
				return false;
			}

			shader.destroy();
			return true;
		}
	}
}
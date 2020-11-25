#include "Graphics/GraphicsPipeline.h"

#include <chrono>

#include <glm/gtc/matrix_transform.hpp>
#include <SDL/SDL_vulkan.h>

#include "Graphics/Camera.h"
#include "Graphics/Texture2D.h"

#include "Logging/Logger.h"

#include "Resources/ResourceManager.h"

#include "Vulkan/Devices/VulkanDeviceManager.h"

#include "Window/WindowManager.h"

namespace SSE
{
	extern Logger gLogger;

	namespace Graphics
	{
#pragma message("TODO: Add functionality for pipeline parameters")
		bool GraphicsPipeline::create(ResourceHandle _shaderHandle, const Vulkan::VulkanSwapChain& swapChain, const Vulkan::VulkanRenderPass& renderPass)
		{
			const Shader* shader = ResourceManager::getShader(_shaderHandle);
			if (!shader)
			{
				GLOG_CRITICAL("Failed to get shader for pipeline.");
				return false;
			}
			
			shaderHandle = _shaderHandle;
			VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
			vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
			VertexFormat inputFormat = shader->getInputFormat();
			auto bindingDescription = inputFormat.getBindingDescription();
			auto attributeDescriptions = inputFormat.getAttributeDescriptions();

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

			const std::vector<VkDescriptorSetLayout>& setLayouts = shader->getDescriptorSetLayouts();
			VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
			pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
			pipelineLayoutInfo.setLayoutCount = (u32)setLayouts.size();
			pipelineLayoutInfo.pSetLayouts = setLayouts.data();
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

			std::vector<VkPipelineShaderStageCreateInfo> shaderStages = shader->getShaderStages();

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

		bool GraphicsPipeline::recreate(const Vulkan::VulkanSwapChain& swapChain, const Vulkan::VulkanRenderPass& renderPass)
		{
			destroy();
			return create(shaderHandle, swapChain, renderPass);
		}

		void GraphicsPipeline::destroy()
		{
			vkDestroyPipeline(LOGICAL_DEVICE_DEVICE, pipeline, nullptr);
			vkDestroyPipelineLayout(LOGICAL_DEVICE_DEVICE, layout, nullptr);
		}

		VkPipelineLayout GraphicsPipeline::getLayout()
		{
			return layout;
		}

		VkPipeline GraphicsPipeline::getPipeline()
		{
			return pipeline;
		}
	}
}
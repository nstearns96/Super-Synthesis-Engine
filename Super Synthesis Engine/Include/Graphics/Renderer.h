#ifndef _SSE_RENDERER_H
#define _SSE_RENDERER_H

#include <vector>

#include <vulkan/vulkan.h>

#include "EngineTypeDefs.h"

#include "Graphics/Camera.h"
#include "Graphics/GraphicsPipeline.h"

#include "Graphics/Shaders/Shader.h"

#include "Object.h"

#include "Resources/ResourceHandleManager.h"

#include "Vulkan/VulkanCommandPool.h"

#include "Vulkan/Graphics/VulkanRenderPass.h"
#include "Vulkan/Graphics/VulkanSwapChain.h"

#include "Vulkan/Shaders/Descriptors/VulkanDescriptorPool.h"

namespace SSE::Graphics
{
	static const i32 MAX_FRAMES_IN_FLIGHT = 2;

	struct RenderCommand
	{
		ResourceHandle pipeline;
		ResourceHandle model;

		bool operator<(const RenderCommand& a) const { return pipeline < a.pipeline || ((pipeline == a.pipeline) && (model < a.model)); };
	};

	class RenderQueue
	{
	private:
		std::map<RenderCommand, std::vector<const ResourceHandle*>> queuedCommands;

	public:
		void enqueue(const RenderCommand& command, const ResourceHandle* modelDescriptor)
		{
			queuedCommands[command].push_back(modelDescriptor);
		}

		const std::map<RenderCommand, std::vector<const ResourceHandle*>>& getCommands() { return queuedCommands; }

		void clear() { queuedCommands.clear(); }
	};

	class Renderer
	{
	private:
		Renderer() {};

		RenderQueue queue;

		ResourceHandleManager pipelineHandles;
		std::vector<GraphicsPipeline> pipelines;

		std::vector<VkSemaphore> imageAvailableSemaphores;
		std::vector<VkSemaphore> renderFinishedSemaphores;
		std::vector<VkFence> inFlightFences;
		std::vector<VkFence> imagesInFlight;
		std::vector<VkFramebuffer> frameBuffers;
		Vulkan::VulkanSwapChain swapChain;
		Vulkan::VulkanRenderPass renderPass;
#pragma message("TODO: This should be interfaced through some CommandPool cache rather than stored here.")
		Vulkan::VulkanCommandPool commandPool;

		ResourceHandle currentPipelineHandle;
		Camera* currentCamera;

		u32 currentFrame;
		
		bool recreateSwapChain();
		bool constructCommandBuffer(u32 index);

	public:
		bool isFrameBufferDirty = false;

		bool create();

		void destroy();

		bool renderObject(const Object& object);

		bool executeRenderPass();

		bool createPipeline(ResourceHandle shaderHandle);
		bool setPipeline(ResourceHandle resourceHandle);
		bool setCamera(Camera* camera);

		static Renderer gRenderer;
	};
}

#endif
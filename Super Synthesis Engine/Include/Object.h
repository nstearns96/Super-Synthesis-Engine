#ifndef _SSE_OBJECT_H
#define _SSE_OBJECT_H

#include <glm/glm.hpp>

#include "EngineTypeDefs.h"

#include "Vulkan/Memory/VulkanBuffer.h"

namespace SSE
{
	class Object
	{
	private:
		ResourceHandle modelHandle = INVALID_HANDLE;
		ResourceHandle textureHandle = INVALID_HANDLE;
		ResourceHandle modelDescriptorHandles[2] = { INVALID_HANDLE, INVALID_HANDLE };

		glm::vec3 position;
		glm::vec3 scale;
		glm::mat4 rotation;

		Vulkan::VulkanBuffer modelBuffer;

	public:
		bool create(ResourceHandle _modelHandle, ResourceHandle _textureHandle);

		void destroy();

		void transform(glm::vec3 translation, glm::vec3 _scale, glm::mat4 _rotation);

		ResourceHandle getModel() const;
		const ResourceHandle* getModelDescriptorHandles() const;
	};
};

#endif
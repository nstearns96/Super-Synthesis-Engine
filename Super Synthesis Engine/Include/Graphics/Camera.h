#ifndef _SSE_CAMERA_H
#define _SSE_CAMERA_H

#include <glm/glm.hpp>

#include "Resources/ResourceHandleManager.h"

#include "Vulkan/Memory/VulkanBuffer.h"

namespace SSE
{
	class Camera
	{
	private:
		glm::vec3 forward;
		glm::vec3 up;

		glm::vec2 totalRotation = {0,0};

		ResourceHandle viewResourceHandle = INVALID_HANDLE;
		Vulkan::VulkanBuffer viewBuffer;

		glm::vec3 position;
	public:

		bool create(const glm::vec3& _position, const glm::vec3& _forward, const glm::vec3& _up = glm::vec3(0.0f, 0.0f, 1.0f));
		void update();
		void destroy();

		glm::vec3 getForwardVector();
		glm::vec3 getUpVector();
		glm::vec3 getRightVector();

		void rotateByAxes(glm::vec2 rotation);
		void translate(glm::vec3 translation);

		ResourceHandle getViewResourceHandle();
	};

	extern Camera gCamera;
}

#endif
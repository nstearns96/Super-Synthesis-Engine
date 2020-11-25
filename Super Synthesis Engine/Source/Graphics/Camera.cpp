#include "Graphics/Camera.h"

#include <iostream>

#include <glm/gtx/transform.hpp>

#include "Logging/Logger.h"

#include "Resources/ResourceManager.h"

#include "Vulkan/Shaders/Descriptors/VulkanDescriptorPool.h"

namespace SSE
{
	Camera gCamera;

	extern Logger gLogger;

	bool Camera::create(const glm::vec3& _position, const glm::vec3& _forward, const glm::vec3& _up)
	{
		if (glm::length(_forward) == 0 || glm::length(_up) == 0)
		{
			GLOG_CRITICAL("Failed to create camera. Supplied rotation vector was 0.");
			return false;
		}

		position = _position;
		forward = glm::normalize(_forward);
		totalRotation = { glm::acos(forward.x), -glm::asin(forward.z)};
		up = glm::normalize(_up);

#pragma message("TODO: Get this from the shader metadata")
		std::vector<ResourceHandle> result = Vulkan::gDescriptorPool.allocateDescriptorSets({ ResourceManager::getShader(0)->getDescriptorSetLayouts()[0] });
		if (result.size() > 0)
		{
			viewResourceHandle = result[0];
		}

		if (!viewBuffer.create(2 * sizeof(glm::mat4), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT))
		{
			return false;
		}

		update();

		VkDescriptorBufferInfo bufferInfo{};
		bufferInfo.buffer = viewBuffer.getBuffer();
		bufferInfo.offset = 0;
		bufferInfo.range = VK_WHOLE_SIZE;

		std::vector<VkWriteDescriptorSet>modelWrite(1);
		modelWrite[0] = {};
		modelWrite[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		modelWrite[0].descriptorCount = 1;
		modelWrite[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		modelWrite[0].dstBinding = 0;
		modelWrite[0].pBufferInfo = &bufferInfo;
		Vulkan::gDescriptorPool.updateDescriptorSet(viewResourceHandle, modelWrite);

		return true;
	}

	void Camera::update()
	{
#pragma message("TODO: Add dirty flag for updating these resources")
		glm::mat4 proj = glm::perspective(glm::radians(70.0f), 1280 / (float)720, 0.1f, 10.0f);
		proj[1][1] *= -1.0f;
		glm::mat4 view[2] = { glm::lookAt(position, position+forward, up), proj };

		viewBuffer.bufferData((void*)view, false);
	}

	void Camera::destroy()
	{
		viewBuffer.destroy();
		Vulkan::gDescriptorPool.freeDescriptorSets({viewResourceHandle});
	}

	glm::vec3 Camera::getUpVector()
	{
		return up;
	}	
	
	glm::vec3 Camera::getForwardVector()
	{
		return forward;
	}

	glm::vec3 Camera::getRightVector()
	{
		return glm::cross(forward, up);
	}

	void Camera::rotateByAxes(glm::vec2 rotation)
	{
		glm::vec2 rotationActual = { rotation.x, rotation.y < 0.0 ? glm::max(rotation.y, -totalRotation.y - 0.5f * 0.95f * glm::pi<float>()) : glm::min(rotation.y, 0.5f * 0.95f * glm::pi<float>() - totalRotation.y)};
		glm::mat4 rotationTransform = glm::mat4(1.0f);
		totalRotation += rotationActual;
		rotationTransform = glm::rotate(rotationTransform, -rotationActual.x, glm::vec3(0.0f, 0.0f, 1.0f));
		rotationTransform = glm::rotate(rotationTransform, -rotationActual.y, getRightVector());
		forward = glm::normalize(rotationTransform * glm::vec4(forward, 0.0f));
		up = glm::normalize(rotationTransform * glm::vec4(up, 0.0f));
	}

	void Camera::translate(glm::vec3 translation)
	{
		position += translation;
	}

	ResourceHandle Camera::getViewResourceHandle()
	{
		return viewResourceHandle;
	}
}
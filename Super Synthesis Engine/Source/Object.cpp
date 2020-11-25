#include "Object.h"

#include <vector>

#include <glm/gtc/matrix_transform.hpp>

#include "Resources/ResourceManager.h"

#include "Vulkan/Shaders/Descriptors/VulkanDescriptorPool.h"

namespace SSE
{
	bool Object::create(ResourceHandle _modelHandle, ResourceHandle _textureHandle)
	{
		position = glm::vec3();
		scale = glm::vec3(1.0f);
		rotation = glm::mat4(1.0f);

		modelHandle = _modelHandle;
		textureHandle = _textureHandle;

		std::vector<VkDescriptorSetLayout> shaderLayouts = ResourceManager::getShader(0)->getDescriptorSetLayouts();
		std::vector<VkDescriptorSetLayout> setLayouts{ shaderLayouts.begin() + 1, shaderLayouts.end() };
		std::vector<ResourceHandle> result = Vulkan::gDescriptorPool.allocateDescriptorSets(setLayouts);
		if (result.size() > 0)
		{
			modelDescriptorHandles[0] = result[0];
			modelDescriptorHandles[1] = result[1];
		}

		const Graphics::Texture2D* texture = ResourceManager::getTexture(textureHandle);

		VkDescriptorImageInfo textureInfo = {};
		textureInfo.sampler = texture->getSampler();
		textureInfo.imageView = texture->getImageView().getImageView();
		textureInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

		std::vector<VkWriteDescriptorSet> textureWrite(1);
		textureWrite[0] = {};
		textureWrite[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		textureWrite[0].descriptorCount = 1;
		textureWrite[0].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		textureWrite[0].dstBinding = 0;
		textureWrite[0].pImageInfo = &textureInfo;
		Vulkan::gDescriptorPool.updateDescriptorSet(modelDescriptorHandles[0], textureWrite);

		if (!modelBuffer.create(sizeof(glm::mat4), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT))
		{
			return false;
		}

		glm::mat4 transform = glm::scale(glm::translate(glm::mat4(1.0f), position), scale) * rotation;
		modelBuffer.bufferData((void*)&transform, false);

		VkDescriptorBufferInfo bufferInfo{};
		bufferInfo.buffer = modelBuffer.getBuffer();
		bufferInfo.offset = 0;
		bufferInfo.range = VK_WHOLE_SIZE;

		std::vector<VkWriteDescriptorSet>modelWrite(1);
		modelWrite[0] = {};
		modelWrite[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		modelWrite[0].descriptorCount = 1;
		modelWrite[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		modelWrite[0].dstBinding = 0;
		modelWrite[0].pBufferInfo = &bufferInfo;
		Vulkan::gDescriptorPool.updateDescriptorSet(modelDescriptorHandles[1], modelWrite);

		return true;
	}

	void Object::destroy()
	{
		modelBuffer.destroy();
		Vulkan::gDescriptorPool.freeDescriptorSets({ modelDescriptorHandles[0], modelDescriptorHandles[1] });
	}

	ResourceHandle Object::getModel() const
	{
		return modelHandle;
	}

	const ResourceHandle* Object::getModelDescriptorHandles() const
	{
		return modelDescriptorHandles;
	}

	void Object::transform(glm::vec3 translation, glm::vec3 _scale, glm::mat4 _rotation)
	{
		if (translation != glm::vec3() || _scale != glm::vec3(1.0f) || _rotation != glm::mat4(1.0f))
		{
			position += translation;
			scale *= _scale;
			rotation *= _rotation;

			glm::mat4 transform = glm::scale(glm::translate(glm::mat4(1.0f), position), scale) * rotation;
			modelBuffer.bufferData(&transform, false);
		}
	}
}
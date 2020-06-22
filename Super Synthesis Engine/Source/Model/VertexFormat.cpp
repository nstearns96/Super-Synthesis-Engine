#include "Model/VertexFormat.h"

namespace SSE
{
	void VertexFormat::create(const std::vector<VertexAspectType>& _aspects)
	{
		aspects = _aspects;
	}

	st VertexFormat::getFormatStride()
	{
		st result = 0;

		for (const auto& aspect : aspects)
		{
			result += aspectPropertyMap.find(aspect)->second;
		};

		return result;
	}

	VkVertexInputBindingDescription VertexFormat::getBindingDescription()
	{
		VkVertexInputBindingDescription bindingDescription{};
		bindingDescription.binding = 0;
		bindingDescription.stride = (u32)getFormatStride()*sizeof(r32);
		bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

		return bindingDescription;
	}

	std::vector<VkVertexInputAttributeDescription> VertexFormat::getAttributeDescriptions()
	{
		std::vector<VkVertexInputAttributeDescription> result;
		result.resize(aspects.size());

		st offset = 0;
		for (u32 i = 0; i < aspects.size(); ++i)
		{
			st aspectSize = aspectPropertyMap.find(aspects[i])->second;
			result[i].binding = 0;
			result[i].location = i;
			result[i].format = sizeToFormat[aspectSize - 1];
			result[i].offset = (u32)offset;

			offset += aspectSize * sizeof(r32);
		}

		return result;
	}

	std::vector<VertexAspectType> VertexFormat::getAspects()
	{
		return aspects;
	}
}
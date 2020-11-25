#include "Model/VertexFormat.h"

namespace SSE
{
	bool VertexFormat::create(const std::vector<VertexAspectType>& _aspects)
	{
		aspects = _aspects;
		return true;
	}

	void VertexFormat::destroy()
	{
		aspects.clear();
	}

	st VertexFormat::getFormatStride()
	{
		st result = 0;

		for (const auto& aspect : aspects)
		{
			result += aspectPropertyMap[aspect];
		};

		return result;
	}

	VkVertexInputBindingDescription VertexFormat::getBindingDescription()
	{
		VkVertexInputBindingDescription bindingDescription{};

#pragma message("NOTE: SPIRV-Reflect only supports binding point 0 for vertex buffers.")
		bindingDescription.binding = 0;
		bindingDescription.stride = (u32)getFormatStride() * sizeof(r32);
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
			st aspectSize = aspectPropertyMap[aspects[i]];
#pragma message("NOTE: SPIRV-Reflect only supports binding point 0 for vertex buffers.")
			result[i].binding = 0;
			result[i].location = i;
			result[i].format = sizeToFormat[aspectSize];
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
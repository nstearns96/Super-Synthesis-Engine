#ifndef _SSE_VERTEX_FORMAT_H
#define _SSE_VERTEX_FORMAT_H

#include <vector>

#include <vulkan/vulkan.h>

#include "Model/VertexAspect.h"
#include "EngineTypeDefs.h"

namespace SSE
{
	class VertexFormat
	{
	private:
		std::vector<VertexAspectType> aspects;

	public:
		void create(const std::vector<VertexAspectType>& _aspects);

		st getFormatStride();
		std::vector<VertexAspectType> getAspects();

		VkVertexInputBindingDescription getBindingDescription();

		std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions();
	};
}

#endif
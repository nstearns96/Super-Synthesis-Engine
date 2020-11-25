#ifndef _SSE_VERTEX_FORMAT_H
#define _SSE_VERTEX_FORMAT_H

#include <vector>

#include <vulkan/vulkan.h>

#include "EngineTypeDefs.h"

#include "Model/VertexAspect.h"

namespace SSE
{
	class VertexFormat
	{
	private:
		std::vector<VertexAspectType> aspects;

	public:
		bool create(const std::vector<VertexAspectType>& _aspects);

		void destroy();

		st getFormatStride();
		std::vector<VertexAspectType> getAspects();

		VkVertexInputBindingDescription getBindingDescription();

		std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions();
	};
}

#endif
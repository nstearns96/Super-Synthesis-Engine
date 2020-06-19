#ifndef _SSE_VERTEX_H
#define _SSE_VERTEX_H

#include <glm/glm.hpp>
#include <vulkan/vulkan.h>

#include <array>

namespace SSE
{
#pragma message("TODO: Support generalized vertex format")
	struct Vertex
	{
		glm::vec2 pos;
		glm::vec3 color;
		glm::vec2 texCoord;

		static VkVertexInputBindingDescription getBindingDescription();

		static std::array<VkVertexInputAttributeDescription, 3> getAttributeDescriptions();
	};
}

#endif
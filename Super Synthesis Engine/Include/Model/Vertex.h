#ifndef _SSE_VERTEX_H
#define _SSE_VERTEX_H

#include <glm/glm.hpp>
#include <vulkan/vulkan.h>

#include <array>

namespace SSE
{
	struct Vertex
	{
		glm::vec2 pos;
		glm::vec3 color;

		static VkVertexInputBindingDescription getBindingDescription();

		static std::array<VkVertexInputAttributeDescription, 2> getAttributeDescriptions();
	};
}

#endif
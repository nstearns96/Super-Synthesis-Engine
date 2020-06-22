#ifndef _SSE_VERTEX_ASPECT_H
#define _SSE_VERTEX_ASPECT_H

#include <glm/glm.hpp>

#include <map>

#include "EngineTypeDefs.h"

namespace SSE
{
	enum VertexAspectType
	{
		VA_POS2F,
		VA_POS3F,
		VA_COL3F,
		VA_COL4F,
		VA_TEX1F,
		VA_TEX2F,
		VA_TEX3F
	};

	const static std::map<VertexAspectType, st> aspectPropertyMap =
	{
		{VA_POS2F, 2},
		{VA_POS3F, 3},
		{VA_COL3F, 3},
		{VA_COL4F, 4},
		{VA_TEX1F, 1},
		{VA_TEX2F, 2},
		{VA_TEX3F, 2}
	};

	const static VkFormat sizeToFormat[] = 
	{
		 VK_FORMAT_R32_SFLOAT, 
		 VK_FORMAT_R32G32_SFLOAT, 
		 VK_FORMAT_R32G32B32_SFLOAT, 
		 VK_FORMAT_R32G32B32A32_SFLOAT
	};
}

#endif
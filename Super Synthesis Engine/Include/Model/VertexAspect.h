#ifndef _SSE_VERTEX_ASPECT_H
#define _SSE_VERTEX_ASPECT_H

#include <map>

#include <glm/glm.hpp>

#include "EngineTypeDefs.h"

namespace SSE
{
	enum VertexAspectType
	{
		VA_NONE,
		VA_POS2F,
		VA_POS3F,
		VA_COL3F,
		VA_COL4F,
		VA_TEX1F,
		VA_TEX2F,
		VA_TEX3F
	};

	const static st aspectPropertyMap[] =
	{
		0,
		2,
		3,
		3,
		4,
		1,
		2,
		2
	};

	const static VkFormat sizeToFormat[] = 
	{
		VK_FORMAT_UNDEFINED,
		VK_FORMAT_R32_SFLOAT, 
		VK_FORMAT_R32G32_SFLOAT, 
		VK_FORMAT_R32G32B32_SFLOAT, 
		VK_FORMAT_R32G32B32A32_SFLOAT
	};
}

#endif
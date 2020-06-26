#ifndef _SSE_TEXTURE_ASSET_UTILS_H
#define _SSE_TEXTURE_ASSET_UTILS_H

#include <glm/glm.hpp>

#include <string>

#include "TextureSupportBMP.h"

namespace SSE::Assets::TextureAssetUtils
{
	enum TextureFormat
	{
		TF_RAW = 0x0,
		TF_BMP,

		TF_NUM_SUPPORTED
	};

	byte* loadTextureFromFile(const std::string& path, glm::uvec2& dimensions);
}

#endif
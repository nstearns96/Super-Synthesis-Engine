#include "Resources/Assets/TextureAssetUtils.h"

#include <vector>

#include "Logging/Logger.h"
#include "EngineTypeDefs.h"

namespace SSE
{
	extern Logger gLogger;

	namespace Assets::TextureAssetUtils
	{
		TextureFormat extractTextureFormat(const std::vector<byte>& data)
		{
			if (BMP::checkBMPHeader(data.data()))
			{
				return TF_BMP;
			}
			return TF_RAW;
		}

#pragma message("TODO: Add load options (see stb_image.h)")
		byte* loadTextureFromFile(const std::string& path, glm::uvec2& dimensions)
		{	
			FileHandle fh;
			fh.create(path, FioMode::FIOM_READ | FioMode::FIOM_BINARY);

			std::vector<byte> imageData = fh.readIntoVector();

			TextureFormat format = extractTextureFormat(imageData);
			switch (format)
			{
			case TF_RAW:
			default:
			{
				byte* rawData = new byte[imageData.size()];
				memcpy(rawData, imageData.data(), imageData.size());
				return rawData;
			}
			case TF_BMP:
			{
				return BMP::extractBitmap(imageData.data(), dimensions);
			}
			}
		}
	}
}
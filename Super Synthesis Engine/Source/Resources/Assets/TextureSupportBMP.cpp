#include "Resources/Assets/TextureSupportBMP.h"

#include "Logging/Logger.h"

namespace SSE
{
	extern Logger gLogger;

	namespace Assets::BMP
	{
		bool checkBMPHeader(const byte* data)
		{
			return (data[0] == 'B' && data[1] == 'M');
		}

		byte* extractBitmap(const byte* data, glm::uvec2& dimensions)
		{
			const byte* headerParser = data;
			BitmapFileHeader fileHeader;
			memcpy(&fileHeader, data, sizeof(fileHeader));
			headerParser += sizeof(fileHeader);

			u16 bitsPerPixel;
			if (*(u32 *)headerParser == (u32)sizeof(BitmapCoreHeader))
			{
				BitmapCoreHeader dibSection;
				memcpy(&dibSection, headerParser, sizeof(dibSection));
				dimensions = { dibSection.bitmapWidth, dibSection.bitmapHeight };
				bitsPerPixel = dibSection.bitCount;
			}
			else if (*(u32 *)headerParser == (u32)sizeof(BitmapInfoHeader) || *(u32 *)headerParser == (u32)sizeof(BitmapV5Header))
			{
				BitmapInfoHeader dibSection;
				memcpy(&dibSection, headerParser, sizeof(dibSection));
				dimensions = { dibSection.bitmapWidth, dibSection.bitmapHeight };
				bitsPerPixel = dibSection.bitCount;
			}
			else
			{
				return nullptr;
			}

			u32 bytesPerPixel = bitsPerPixel / 8;

			byte* result = new byte[dimensions.x * dimensions.y * (bytesPerPixel)];

			byte* writeIndex = result;
			const byte* readIndex = data + fileHeader.arrayOffset;
			for (u32 y = 0; y < dimensions.y; ++y)
			{
				u32 bytesToWrite = dimensions.x * bytesPerPixel;
				u32 bytesToRead = (((dimensions.x * bytesPerPixel) + sizeof(u32)-1)/sizeof(u32)) * sizeof(u32);
				memcpy(writeIndex, readIndex, dimensions.x * bytesPerPixel);
				writeIndex += bytesToWrite;
				readIndex += bytesToRead;
			}

			return result;
		}
	}
}
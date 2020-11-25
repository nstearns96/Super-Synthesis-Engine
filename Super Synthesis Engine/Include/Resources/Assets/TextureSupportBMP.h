#ifndef _SSE_TEXTURE_SUPPORT_BMP_H
#define _SSE_TEXTURE_SUPPORT_BMP_H

#include <vector>

#include <glm/glm.hpp>

#include "EngineTypeDefs.h"

namespace SSE::Assets::BMP
{
	enum DiBSectionFormat
	{
		BITMAPCOREHEADER = 0x0,
		BITMAPINFOHEADER,
	};

	enum BitmapCompression
	{
		BC_RGB,
		BC_RLE8,
		BC_RLE4,
		BC_BITFIELDS,
		BC_JPEG,
		BC_PNG
	};

#pragma pack(push, 1)
	struct BitmapFileHeader
	{
		u16 signature;
		u32 numBytes;
		u16 reserved1;
		u16 reserved2;
		u32 arrayOffset;
	};

	struct BitmapCoreHeader
	{
		u32 headerSize;
		u16 bitmapWidth;
		u16 bitmapHeight;
		u16 numPlanes;
		u16 bitCount;
	};

	struct BitmapPaletteEntry
	{
		u8 rgbTriple[3];
	};

	struct BitmapInfoHeader
	{
		u32 headerSize;
		i32 bitmapWidth;
		i32 bitmapHeight;
		u16 numPlanes;
		u16 bitCount;
		BitmapCompression compression;
		u32 imageSize;
		i32 xResolution;
		i32 yResolution;
		u32 paletteSize;
		u32 requiredColors;
	};

	struct BitmapContext
	{
		BitmapFileHeader header;
		BitmapInfoHeader dibSection;
	};

	struct BitmapV5Header
	{
		u32 headerSize;
		i32 bitmapWidth;
		i32 bitmapHeight;
		u16 numPlanes;
		u16 bitCount;
		BitmapCompression compression;
		u32 imageSize;
		i32 xResolution;
		i32 yResolution;
		u32 paletteSize;
		u32 requiredColors;
		u32 bV5RedMask;
		u32 bV5GreenMask;
		u32 bV5BlueMask;
		u32 bV5AlphaMask;
		u32 bV5CSType;
		u32 bV5Endpoints[3][3];
		u32 bV5GammaRed;
		u32 bV5GammaGreen;
		u32 bV5GammaBlue;
		u32 bV5Intent;
		u32 bV5ProfileData;
		u32 bV5ProfileSize;
		u32 bV5Reserved;
	};
#pragma pack(pop)

	bool checkBMPHeader(const byte* data);

	byte* extractBitmap(const byte* data, glm::uvec2& dimensions);
}

#endif

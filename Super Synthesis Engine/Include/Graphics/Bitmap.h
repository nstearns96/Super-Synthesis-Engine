#ifndef _SSE_BITMAP_H
#define _SSE_BITMAP_H

#include <glm/glm.hpp>
#include <vulkan/vulkan.h>
#include <vector>
#include <map>
#include <array>

#include "EngineTypeDefs.h"

namespace SSE::Graphics
{
	enum ColorChannel
	{
		CHANNEL_RED,
		CHANNEL_GREEN,
		CHANNEL_BLUE,
		CHANNEL_ALPHA,
		NUM_COLOR_CHANNELS
	};

	const static std::vector<VkFormat> supportedBitmapFormats
	{
		VK_FORMAT_R8_UINT,
		VK_FORMAT_R8G8B8_UINT,
		VK_FORMAT_B8G8R8_UINT,
		VK_FORMAT_R8G8B8A8_UINT,
		VK_FORMAT_B8G8R8A8_UINT,
		VK_FORMAT_R64_SFLOAT
	};

	const static std::map<VkFormat, st> formatToPixelSize
	{
		{VK_FORMAT_R8_UINT, sizeof(u8) },
		{VK_FORMAT_R8G8B8_UINT, 3 * sizeof(u8) },
		{VK_FORMAT_B8G8R8_UINT, 3 * sizeof(u8) },
		{VK_FORMAT_R8G8B8A8_UINT, 4 * sizeof(u8) },
		{VK_FORMAT_B8G8R8A8_UINT, 4 * sizeof(u8) },
		{VK_FORMAT_R64_SFLOAT, sizeof(r64) }
	};

	const static std::map<VkFormat, st> formatToNumChannels
	{
		{VK_FORMAT_R8_UINT, 1},
		{VK_FORMAT_R8G8B8_UINT, 3 },
		{VK_FORMAT_B8G8R8_UINT, 3 },
		{VK_FORMAT_R8G8B8A8_UINT, 4 },
		{VK_FORMAT_B8G8R8A8_UINT, 4 },
		{VK_FORMAT_R64_SFLOAT, 1 }
	};

	const static std::map<VkFormat, const std::array<bool, NUM_COLOR_CHANNELS>> formatToRequiredChannels
	{
		{VK_FORMAT_R8_UINT, {true, false, false, false}},
		{VK_FORMAT_R8G8B8_UINT, {true, true, true, false}},
		{VK_FORMAT_B8G8R8_UINT, {true, true, true, false}},
		{VK_FORMAT_R8G8B8A8_UINT, {true, true, true, true}},
		{VK_FORMAT_B8G8R8A8_UINT, {true, true, true, true}},
		{VK_FORMAT_R64_SFLOAT, {true, false, false, false}}
	};

	const static std::map<VkFormat, const std::vector<st>> formatToChannelOffsetList
	{
		{VK_FORMAT_R8_UINT, 
			{
				0
			}
		},
		{VK_FORMAT_R8G8B8_UINT, 
			{
				0,
				1,
				2
			}
		},
		{VK_FORMAT_B8G8R8_UINT, 
			{
				2,
				1,
				0
			}
		},
		{VK_FORMAT_R8G8B8A8_UINT, 
			{
				0,
				1,
				2,
				3
			}
		},
		{VK_FORMAT_B8G8R8A8_UINT, 
			{
				2,
				1,
				0,
				3
			}
		},
		{VK_FORMAT_R32_SFLOAT, 
			{
				0
			}
		}
	};

	struct BitmapFormatTransitionChannelParams
	{
		byte* data;
		u32 constant;
		ColorChannel destinationChannel;
	};

	struct BitmapFormatTransitionParams
	{
		VkFormat newFormat;
		BitmapFormatTransitionChannelParams channelParams[NUM_COLOR_CHANNELS];
#pragma message("TODO: Engine-level color class")
		u32 palette[2];
	};

	class Bitmap
	{
	private:
		byte* data;
		glm::uvec2 dimensions;
		VkFormat format;

	public:
		bool create(const byte* _data, glm::uvec2 _dimensions, VkFormat _format);

		bool transitionFormat(const BitmapFormatTransitionParams& params);

		byte* getData() const;
		glm::uvec2 getDimensions() const;
		VkFormat getFormat() const;

		st getPixelSize() const;
		st getStride() const;

		byte* at(st x, st y);

		void destroy();
	};
}

#endif
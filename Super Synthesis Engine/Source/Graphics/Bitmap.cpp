#include "Graphics/Bitmap.h"

#include "Logging/Logger.h"

namespace SSE
{
	extern Logger gLogger;

	namespace Graphics
	{
		bool Bitmap::create(const byte* _data, glm::uvec2 _dimensions, VkFormat _format)
		{
			if (std::find(supportedBitmapFormats.begin(), supportedBitmapFormats.end(), _format) == supportedBitmapFormats.end())
			{
				GLOG_CRITICAL("Input data uses unsupported format.");
				return false;
			}

			data = new byte[formatToPixelSize.at(_format) * _dimensions.x * _dimensions.y];
			if (_data)
			{
				memcpy(data, _data, formatToPixelSize.at(_format) * _dimensions.x * _dimensions.y);
			}
			else
			{
				memset(data, 0, formatToPixelSize.at(_format) * _dimensions.x * _dimensions.y);
			}

			dimensions = _dimensions;
			format = _format;

			return true;
		}

		bool Bitmap::transitionFormat(const BitmapFormatTransitionParams& params)
		{
			if (std::find(supportedBitmapFormats.begin(), supportedBitmapFormats.end(), params.newFormat) == supportedBitmapFormats.end())
			{
				GLOG_WARNING("Could not transition bitmap format. Unsupported format specified.");
				return false;
			}
			
#pragma message("TODO: Combine this into a single struct")
			const byte* srcPointers[NUM_COLOR_CHANNELS]{};
			st srcTypeSize = format == VK_FORMAT_R64_SFLOAT ? sizeof(r64) : sizeof(u8);
			st srcUnitSize = formatToPixelSize.at(format);
			const std::array<bool, NUM_COLOR_CHANNELS> srcIsChannelRequired = formatToRequiredChannels.at(format);
			st srcNumChannels = formatToNumChannels.at(format);

			byte* dstPointers[NUM_COLOR_CHANNELS]{};
			st dstTypeSize = format == VK_FORMAT_R64_SFLOAT ? sizeof(r64) : sizeof(u8);
			st dstUnitSize = formatToPixelSize.at(params.newFormat);
			const std::array<bool, NUM_COLOR_CHANNELS> dstIsChannelRequired = formatToRequiredChannels.at(params.newFormat);
			st dstNumChannels = formatToNumChannels.at(params.newFormat);

			std::array<bool, NUM_COLOR_CHANNELS> isConstantUsed = {};

			bool isPaletteUsed = false;

			byte* newData = new byte[dstUnitSize * dimensions.x * dimensions.y];

			for (st c = 0; c < dstNumChannels; ++c)
			{
				if (!dstIsChannelRequired[c])
				{
					continue;
				}

				dstPointers[c] = newData + dstTypeSize * formatToChannelOffsetList.at(params.newFormat)[c];

				if (isPaletteUsed)
				{
					continue;
				}

				if (!srcIsChannelRequired[c])
				{
					bool foundParamSource = false;
					for (st paramChannel = c; paramChannel < NUM_COLOR_CHANNELS; ++paramChannel)
					{
						if (params.channelParams[paramChannel].destinationChannel == (ColorChannel)paramChannel)
						{
							isConstantUsed[paramChannel] = params.channelParams->data == nullptr;
							foundParamSource = true;
							break;
						}
						else
						{
							continue;
						}
					}

					if (!foundParamSource)
					{
						if (srcNumChannels == 1)
						{	
							isPaletteUsed = true;
						}
						else
						{
							GLOG_WARNING("Failed to find a source for a channel.");
							delete[] newData;
							return false;
						}
					}
					else
					{
						srcPointers[c] = isConstantUsed[c] ? (byte*)&params.channelParams[c].constant : params.channelParams[c].data;
					}
				}
				else
				{
					srcPointers[c] = data + srcTypeSize * formatToChannelOffsetList.at(format)[c];
				}
			}

			if (isPaletteUsed)
			{
				if (srcTypeSize == sizeof(r64))
				{
					for (st p = 0; p < dimensions.x * dimensions.y; ++p)
					{
						for (st c = 0; c < dstNumChannels; ++c)
						{
#pragma message("TODO: Get rid of magic numbers")
							r64 firstPaletteChannel = (r64)((params.palette[0] >> (8 * c)) & 0xff);
							r64 secondPaletteChannel = (r64)((params.palette[1] >> (8 * c)) & 0xff);
							*dstPointers[c] =
								(u8)std::round(firstPaletteChannel + glm::clamp(*(((r64 *)srcPointers[CHANNEL_RED]) + p), 0.0, 1.0) * (secondPaletteChannel - firstPaletteChannel));
							dstPointers[c] += dstUnitSize;
						}
					}
				}
				else
				{
					for (st p = 0; p < dimensions.x * dimensions.y; ++p)
					{
						for (st c = 0; c < dstNumChannels; ++c)
						{
							r64 firstPaletteChannel = (r64)((params.palette[0] >> (8 * c)) & 0xff);
							r64 secondPaletteChannel = (r64)((params.palette[1] >> (8 * c)) & 0xff);
							*dstPointers[c] =
								(u8)std::round(firstPaletteChannel + (r64)*(srcPointers[CHANNEL_RED] + p)/UCHAR_MAX * (secondPaletteChannel - firstPaletteChannel));
							dstPointers[c] += dstUnitSize;
						}
					}
				}
			}
			else
			{
				for (st p = 0; p < dimensions.x * dimensions.y; ++p)
				{
					for (st c = 0; c < dstNumChannels; ++c)
					{
						*dstPointers[c] = *srcPointers[c];
						dstPointers[c] += dstUnitSize;
						srcPointers[c] += isConstantUsed[c] ? 0 : srcUnitSize;
					}
				}
			}

			delete[] data;
			data = newData;
			format = params.newFormat;
			return true;
		}

		byte* Bitmap::getData() const
		{
			return data;
		}

		glm::uvec2 Bitmap::getDimensions() const
		{
			return dimensions;
		}

		VkFormat Bitmap::getFormat() const
		{
			return format;
		}

		st Bitmap::getPixelSize() const
		{
			return formatToPixelSize.at(format);
		}

		st Bitmap::getStride() const
		{
			return getPixelSize() * dimensions.x;
		}

		byte* Bitmap::at(st x, st y)
		{
			return data + ((x%dimensions.x)*getPixelSize() + ((y % dimensions.y) * getStride()));
		}

		void Bitmap::destroy()
		{
			delete[] data;
			dimensions = { 0, 0 };
			format = VK_FORMAT_UNDEFINED;
		}
	}
}
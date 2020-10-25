#include "Graphics/Font.h"

#include "Logging/Logger.h"

#define PARSE_ULONG(a) (*(u32*)(a))
#define PARSE_ULONG_BE(a) ((u32)((((const byte*)a)[0] << 24) + (((const byte*)a)[1] << 16) + (((const byte*)a)[2] << 8) + ((const byte*)a)[3]))

#define PARSE_LONG(a) (*(i32 *)(a))
#define PARSE_LONG_BE(a) ((i32)((((const byte*)a)[0] << 24) + (((const byte*)a)[1] << 16) + (((const byte*)a)[2] << 8) + ((const byte*)a)[3]))

#define PARSE_USHORT(a) (*(u16*)(a))
#define PARSE_USHORT_BE(a) ((u16)((((const byte*)a)[0] << 8) + ((const byte*)a)[1]))

#define PARSE_SHORT(a) (*(i16 *)(a))
#define PARSE_SHORT_BE(a) ((i16)((((const byte*)a)[0] << 8) + ((const byte*)a)[1]))

#pragma message("TODO: Get rid of 'magic numbers'")

#pragma message("TODO: Find better number for this or make it more dynamic to the scale of the font")
#define LINEARITY 0.01

namespace SSE
{
	extern Logger gLogger;
	
	namespace Graphics
	{
		bool Font::create(const byte* _data)
		{
#pragma message("TODO: Support more ttf formats")
#pragma message("TODO: Move most of this to FontAssetUtils and FontSupport namespaces. Font should only store data tables and glyph cache")
			const byte* parser = _data;
			if (PARSE_ULONG_BE(parser) != PARSE_ULONG_BE("\0\1\0\0"))
			{
				GLOG_CRITICAL("Font file is not in supported format.");
				return false;
			}
			parser += 4;

			u16 numTables = PARSE_USHORT_BE(parser);

			parser = _data + 12;
			for (u16 i = 0; i < numTables; ++i, parser += 16)
			{
				tableMap[PARSE_ULONG(parser)] = (_data + PARSE_ULONG_BE(parser + 8));
			}

			if (tableMap.find(PARSE_ULONG("cmap")) == tableMap.end() ||
				tableMap.find(PARSE_ULONG("head")) == tableMap.end() ||
				tableMap.find(PARSE_ULONG("hhea")) == tableMap.end() ||
				tableMap.find(PARSE_ULONG("maxp")) == tableMap.end() ||
				tableMap.find(PARSE_ULONG("hmtx")) == tableMap.end() ||
				tableMap.find(PARSE_ULONG("GPOS")) == tableMap.end() ||
				(tableMap.find(PARSE_ULONG("glyf")) != tableMap.end() &&
					tableMap.find(PARSE_ULONG("loca")) == tableMap.end()))
			{
				tableMap.clear();
				GLOG_CRITICAL("Font does not contain a required table.");
				return false;
			}

			numGlyphs = PARSE_USHORT_BE(tableMap.at(PARSE_ULONG("maxp")) + 4);

			parser = tableMap.at(PARSE_ULONG("cmap")) + 2;
			u16 numEncodingTables = PARSE_USHORT_BE(parser);
			parser += 2;

#pragma message("TODO: Support more cmap encodings")
			bool foundCMap = false;
			for (u16 i = 0; i < numEncodingTables; ++i, parser += 8)
			{
				if (PARSE_USHORT_BE(parser) == (u16)3 && PARSE_USHORT_BE(parser + 2) == (u16)1)
				{
					glyphIndexTable = tableMap.at(PARSE_ULONG("cmap")) + PARSE_ULONG_BE(parser + 4);
					foundCMap = true;
					break;
				}
			}

			if (!foundCMap)
			{
				tableMap.clear();
				numGlyphs = 0;
				GLOG_CRITICAL("Font does not contain a supported cmap.");
				return false;
			}

			locaFormatLong = PARSE_USHORT_BE(tableMap.at(PARSE_ULONG("head")) + 50) == 1;

			data = _data;
			return true;
		}

		void Font::destroy()
		{
			delete[] data;
			glyphIndexTable = nullptr;
			tableMap.clear();
		}

		i16 Font::getFullHeight()
		{
			return PARSE_SHORT_BE(tableMap.at(PARSE_ULONG("hhea")) + 4) - PARSE_SHORT_BE(tableMap.at(PARSE_ULONG("hhea")) + 6);
		}

		i32 Font::getBaselineForScale(r32 scale)
		{
			r64 interm = scale;
			return (i32)std::round(interm * PARSE_SHORT_BE(tableMap.at(PARSE_ULONG("hhea")) + 4));
		}

		i32 Font::getCharLSBForScale(r32 scale, i32 letter)
		{
			r64 interm = scale;
			u16 glyphIndex = getGlyphIndex(letter);

			i16 glyphLSB = getGlyphLSB(glyphIndex);

			return (i32)std::round(interm*glyphLSB);
		}

		Rect Font::getGlyphBoundingBox(u16 glyphIndex)
		{
			const byte* glyfOffset = getGlyfOffset(glyphIndex);
			
			Rect result;
			result.xVals[0] = PARSE_SHORT_BE(glyfOffset + 2);
			result.yVals[0] = PARSE_SHORT_BE(glyfOffset + 4);
			result.xVals[1] = PARSE_SHORT_BE(glyfOffset + 6);
			result.yVals[1] = PARSE_SHORT_BE(glyfOffset + 8);

			return result;
		}

		glm::uvec2 Font::getGlyphDimensionsForScale(r32 scale, const Rect& glyphRect, glm::dvec2 offset)
		{
			r64 interm = scale;

			glm::ivec2 result;
			result.x = 1 + (i32)(interm * (r64)(glyphRect.xVals[1] - glyphRect.xVals[0]) + glm::fract(offset.x));
			result.y = 1 + (i32)(interm * (r64)(glyphRect.yVals[1] - glyphRect.yVals[0]) + glm::fract(offset.y));

			return result;
		}

		i16 Font::getAscent()
		{
			return PARSE_SHORT_BE(tableMap.at(PARSE_ULONG("hhea")) + 4);
		}

		i16 Font::getAdvance(u16 glyphIndex)
		{
			u16 numHMetrics = PARSE_USHORT_BE(tableMap.at(PARSE_ULONG("hhea")) + 34);
			if (glyphIndex < numHMetrics)
			{
				return PARSE_SHORT_BE(tableMap.at(PARSE_ULONG("hmtx")) + 4 * glyphIndex);
			}
			else
			{
				return PARSE_SHORT_BE(tableMap.at(PARSE_ULONG("hmtx")) + 4 * (numHMetrics - 1));
			}
		}

		i32 Font::getCoverageIndex(const byte* coverageTable, u16 glyphIndex)
		{
			u16 coverageFormat = PARSE_USHORT_BE(coverageTable);
			switch (coverageFormat)
			{
			case 1:
			{
				u16 glyphCount = PARSE_SHORT_BE(coverageTable + 2);

				i32 left = 0, right = glyphCount - 1, middle = -1;
				u16 search = glyphIndex;
				while (left < right)
				{
					middle = (left + right) >> 1;

					if (search < PARSE_USHORT_BE(coverageTable + 4 + 2 * middle))
					{
						right = middle - 1;
					}
					else
					{
						left = middle;
					}
				}

				return left == right ? middle : -1;
				break;
			}

			case 2:
			{
				u16 rangeCount = PARSE_USHORT_BE(coverageTable + 2);
				const byte* rangeArray = coverageTable + 4;

				i32 left = 0, right = rangeCount - 1, middle = -1;
				u16 searchStart, searchEnd, search = glyphIndex;
				while (left <= right)
				{
					middle = (left + right) >> 1;
					const byte* rangeRecord = rangeArray + 6 * middle;
					searchStart = PARSE_USHORT_BE(rangeRecord);
					searchEnd = PARSE_USHORT_BE(rangeRecord + 2);
					if (search < searchStart)
					{
						right = middle - 1;
					}
					else if (search > searchEnd)
					{
						left = middle + 1;
					}
					else
					{
						u16 startCoverageIndex = PARSE_USHORT_BE(rangeRecord + 4);
						return (i32)startCoverageIndex + glyphIndex - searchStart;
					}
				}
				break;
			}

			default:
			{
				break;
			}
			}

			return -1;
		}

		i16 Font::getGlyphClass(const byte* classDefBegin, u16 glyphIndex)
		{
			u16 classDefFormat = PARSE_USHORT_BE(classDefBegin);
			switch (classDefFormat)
			{
			case 1:
			{
				u16 startGlyphID = PARSE_USHORT_BE(classDefBegin + 2);
				u16 glyphCount = PARSE_USHORT_BE(classDefBegin + 4);
				const byte* firstClassDefValueArray = classDefBegin + 6;

				if (glyphIndex >= startGlyphID && glyphIndex < startGlyphID + glyphCount)
					return PARSE_SHORT_BE(firstClassDefValueArray + 2 * (glyphIndex - startGlyphID));

				classDefBegin = firstClassDefValueArray + 2 * glyphCount;
				break;
			}

			case 2:
			{
				u16 classRangeCount = PARSE_USHORT_BE(classDefBegin + 2);
				const byte* classRangeRecords = classDefBegin + 4;

				i32 left = 0, right = classRangeCount - 1, middle;
				i16 searchStart, searchEnd, search = glyphIndex;
				while (left <= right)
				{
					middle = (left + right) >> 1;
					const byte* classRangeRecord = classRangeRecords + 6 * middle;
					searchStart = PARSE_USHORT_BE(classRangeRecord);
					searchEnd = PARSE_USHORT_BE(classRangeRecord + 2);
					if (search < searchStart)
					{
						right = middle - 1;
					}
					else if (search > searchEnd)
					{
						left = middle + 1;
					}
					else
					{
						return PARSE_SHORT_BE(classRangeRecord + 4);
					}
				}

				classDefBegin = classRangeRecords + 6 * classRangeCount;
				break;
			}
			default:
			{
				break;
			}
			}

			return -1;
		}

		i16 Font::getKerningForPair(u16 firstIndex, u16 secondIndex)
		{
#pragma message("TODO: Add support for kern table if GPOS table is not available")
			const byte* gposOffset = tableMap.at(PARSE_ULONG("GPOS"));

			if (PARSE_USHORT_BE(gposOffset) != 1 || PARSE_USHORT_BE(gposOffset + 2) != 0)
			{
				return 0;
			}

			const byte* lookupList = gposOffset + PARSE_USHORT_BE(gposOffset + 8);
			u16 lookupCount = PARSE_USHORT_BE(lookupList);

#pragma message("TODO: Add support for more kerning formats")
			for (u16 l = 0; l < lookupCount; ++l)
			{
				const byte* lookupTable = lookupList + PARSE_USHORT_BE(lookupList + 2 * (l + 1));
				u16 tableFormat = PARSE_USHORT_BE(lookupTable);
				switch (tableFormat)
				{
				case 2:
				{
					u16 subTableCount = PARSE_USHORT_BE(lookupTable + 4);
					const byte* subTableOffsets = lookupTable + 6;

					for (i32 subTableIndex = 0; subTableIndex < subTableCount; ++subTableIndex)
					{
						const byte* subTable = lookupTable + PARSE_USHORT_BE(subTableOffsets + 2 * subTableIndex);
						const byte* coverageIndices = subTable + PARSE_USHORT_BE(subTable + 2);
						i32 coverageIndex = getCoverageIndex(coverageIndices, firstIndex);
						if (coverageIndex == -1)
						{
							continue;
						}

						switch (PARSE_USHORT_BE(subTable))
						{
						case 1:
						{
							const byte* pairValueTable = subTable + PARSE_USHORT_BE(subTable + 10 + 2 * coverageIndex);
							u16 pairValueCount = PARSE_USHORT_BE(pairValueTable);

							i32 left = 0, right = pairValueCount - 1, middle, search;
							while (left <= right)
							{
								middle = (left + right) >> 1;

								const byte* pairValue = pairValueTable + 2 + (2 + 2) * middle;

								search = PARSE_USHORT_BE(pairValue);
								if (secondIndex < search)
								{
									right = middle - 1;
								}
								else if (secondIndex > search)
								{
									left = middle + 1;
								}
								else
								{
									return PARSE_SHORT_BE(pairValue + 2);
								}
							}
							break;
						}
						case 2:
						{
							i16 firstClass = getGlyphClass(subTable + PARSE_USHORT_BE(subTable + 8), firstIndex);
							i16 secondClass = getGlyphClass(subTable + PARSE_USHORT_BE(subTable + 10), secondIndex);

							u16 firstClassCount = PARSE_USHORT_BE(subTable + 12);
							u16 secondClassCount = PARSE_USHORT_BE(subTable + 14);

							if (firstClass >= 0 && firstClass < firstClassCount && secondClass >= 0 && secondClass < secondClassCount)
							{
								const byte* firstClassRecords = subTable + 16;
								const byte* secondClassRecords = firstClassRecords + 2 * (firstClass * secondClassCount);
								return PARSE_SHORT_BE(secondClassRecords + 2 * secondClass);
							}
							break;
						}
						default:
						{
							break;
						}
						}
					}

					l = lookupCount;
					break;
				}
				default:
				{
					break;
				}
				}
			}

			return 0;
		}

		i16 Font::getKerningAdvance(const char c1, const char c2)
		{
			u16 firstIndex = getGlyphIndex(c1);
			i16 kernAdv = getAdvance(firstIndex);
			kernAdv += getKerningForPair(firstIndex, getGlyphIndex(c2));
			return kernAdv;
		}

		glm::uvec2 Font::getStringDimensionsForScale(const char* string, r32 scale)
		{
			r64 interm = scale;

			glm::ivec2 result;
			result.y = 1 + (u32)(interm*getFullHeight());
			r64 caretPos = getCharLSBForScale(scale, string[0]);
			
			for (st c = 0; string[c]; ++c)
			{
				caretPos += interm * getKerningAdvance(string[c], string[c + 1]);
			}

			result.x = 1 + (u32)(caretPos);
			return result;
		}

		std::vector<GlyphVertex> Font::getVerticesForChar(const i32 letter)
		{
			if (PARSE_USHORT_BE(glyphIndexTable) != u16(4))
			{
				return {};
			}

			u16 glyphIndex = getGlyphIndex(letter);
			const byte* glyfTableOffset = getGlyfOffset(glyphIndex);

			if (glyfTableOffset == nullptr)
			{
				return {};
			}

			i16 numContours = PARSE_SHORT_BE(glyfTableOffset);

#pragma message("TODO: Check if switching to std::list would be faster")
			std::vector<GlyphVertex> vertices;
			if (numContours == 0)
			{
				return {};
			}
			else if (numContours > 0)
			{
				vertices = getGlyphVertices(glyphIndex);
			}
			else
			{
				bool moreComponents = true;
				const byte* componentParser = glyfTableOffset + 10;
				while (moreComponents)
				{
#pragma message("TODO: Clean this up using glm types")
					r32 mtx[6] = { 1,0,0,1,0,0 }, xScale, yScale;

					i16 flags = PARSE_SHORT_BE(componentParser);
					componentParser += 2;

					i16 componentGlyphIndex = PARSE_SHORT_BE(componentParser);
					componentParser += 2;

					if (flags & 2)
					{
						if (flags & 1)
						{
							mtx[4] = PARSE_SHORT_BE(componentParser);
							componentParser += 2;
							mtx[5] = PARSE_SHORT_BE(componentParser);
							componentParser += 2;
						}
						else
						{
							mtx[4] = *componentParser;
							componentParser += 1;
							mtx[5] = *componentParser;
							componentParser += 1;
						}
					}
					else
					{
#pragma message("TODO: Add support for point numbers")
						GLOG_CRITICAL("Font uses unsupported compound glyph flags");
						return {};
					}
					if (flags & (1 << 3))
					{
						mtx[0] = mtx[3] = PARSE_SHORT_BE(componentParser) / 16384.0f;
						componentParser += 2;
						mtx[1] = mtx[2] = 0;
					}
					else if (flags & (1 << 6))
					{
						mtx[0] = PARSE_SHORT_BE(componentParser) / 16384.0f;
						componentParser += 2;
						mtx[1] = 0;
						mtx[2] = 0;
						mtx[3] = PARSE_SHORT_BE(componentParser) / 16384.0f;
						componentParser += 2;
					}
					else if (flags & (1 << 7))
					{
						mtx[0] = PARSE_SHORT_BE(componentParser) / 16384.0f;
						componentParser += 2;
						mtx[1] = PARSE_SHORT_BE(componentParser) / 16384.0f;
						componentParser += 2;
						mtx[2] = PARSE_SHORT_BE(componentParser) / 16384.0f;
						componentParser += 2;
						mtx[3] = PARSE_SHORT_BE(componentParser) / 16384.0f;
						componentParser += 2;
					}

					xScale = std::sqrtf(mtx[0] * mtx[0] + mtx[1] * mtx[1]);
					yScale = std::sqrtf(mtx[2] * mtx[2] + mtx[3] * mtx[3]);
					std::vector<GlyphVertex> componentVertices = getGlyphVertices(componentGlyphIndex);

					for (auto& v : componentVertices)
					{
						v.position.x = (xScale * (mtx[0] * v.position.x + mtx[2] * v.position.y + mtx[4]));
						v.position.y = (yScale * (mtx[1] * v.position.x + mtx[3] * v.position.y + mtx[5]));
					}

					vertices.insert(vertices.end(), componentVertices.begin(), componentVertices.end());

					moreComponents = (flags & 0x20) == 0x20;
				}
			}

			return vertices;
		}

		u16 Font::getGlyphIndex(const i32 letter)
		{
			if (letter > UINT_MAX)
			{
				return 0;
			}

			u16 segmentCount = PARSE_USHORT_BE(glyphIndexTable + 6) >> 1;
			u16 searchRange = PARSE_USHORT_BE(glyphIndexTable + 8) >> 1;
			u16 searchCounter = PARSE_USHORT_BE(glyphIndexTable + 10);
			u16 searchRangeOffset = PARSE_USHORT_BE(glyphIndexTable + 12) >> 1;

			byte* endCountArray = (byte*)(glyphIndexTable + 14);
			byte* search = endCountArray;

			if (letter >= PARSE_USHORT_BE(search + searchRangeOffset * 2))
			{
				search += searchRangeOffset * 2;
			}

			search -= 2;
			while (searchCounter)
			{
				searchRange >>= 1;
				if (letter > PARSE_USHORT_BE(search + searchRange * 2))
				{
					search += searchRange * 2;
				}
				--searchCounter;
			}
			search += 2;

			u16 itemOffset = (u16)((search - endCountArray) / 2);

			u16 startCount = PARSE_USHORT_BE(glyphIndexTable + 14 + segmentCount * 2 + 2 + 2 * itemOffset);
			if (letter < startCount)
				return 0;

			u16 rangeOffset = PARSE_SHORT_BE(glyphIndexTable + 14 + segmentCount * 6 + 2 + 2 * itemOffset);

			u16 glyphIndex = (rangeOffset == 0) ? (u16)(letter + PARSE_SHORT_BE(glyphIndexTable + 14 + segmentCount * 4 + 2 + 2 * itemOffset)) :
				PARSE_USHORT_BE(glyphIndexTable + rangeOffset + (letter - startCount) * 2 + 14 + segmentCount * 6 + 2 + 2 * itemOffset);

			return glyphIndex;
		}

		i16 Font::getGlyphLSB(u16 glyphIndex)
		{
			u16 numHMetrics = PARSE_USHORT_BE(tableMap.at(PARSE_ULONG("hhea")) + 34);
			if (glyphIndex < numHMetrics)
			{
				return PARSE_SHORT_BE(tableMap.at(PARSE_ULONG("hmtx")) + 4 * glyphIndex + 2);
			}
			else
			{
				return PARSE_SHORT_BE(tableMap.at(PARSE_ULONG("hmtx")) + 4 * numHMetrics + 2 * (glyphIndex - numHMetrics));
			}
		}

		const byte* Font::getGlyfOffset(u16 glyphIndex)
		{
			if (glyphIndex >= numGlyphs)
			{
				return nullptr;
			}

			const byte* glyfTableOffset = tableMap.at(PARSE_ULONG("glyf"));
			const byte* locaTableOffset = tableMap.at(PARSE_ULONG("loca"));

			const byte* g1, *g2;
			if (locaFormatLong)
			{
				g1 = glyfTableOffset + PARSE_LONG_BE(locaTableOffset + glyphIndex * 4);
				g2 = glyfTableOffset + PARSE_LONG_BE(locaTableOffset + glyphIndex * 4 + 4);
			}
			else
			{
				g1 = glyfTableOffset + PARSE_LONG_BE(locaTableOffset + glyphIndex * 2) * 2;
				g2 = glyfTableOffset + PARSE_LONG_BE(locaTableOffset + glyphIndex * 2 + 2) * 2;
			}

			return g1 == g2 ? nullptr : g1;
		}

		std::vector<GlyphVertex> Font::flattenCurve(const std::array<GlyphVertex, 3>& points, r64 linearity)
		{
			if (linearity < 0.0f)
			{
				return {};
			}

			std::vector<GlyphVertex> result;
			result.push_back(points[0]);
			result.push_back(points[2]);

			std::vector<u32> depths;
			depths.push_back(0);
			depths.push_back(0);

			st depthIndex = 0;
			r64 t = 0;
			do
			{
				u32 newDepth = (depths[depthIndex] > depths[depthIndex + 1] ? depths[depthIndex] + 1 : depths[depthIndex + 1] + 1);
				r64 halfTime = t + 1.0 / std::pow(2.0, (r64)newDepth);
				r64 inverseHalfTime = 1.0 - halfTime;
				r64 halfTimeSquared = halfTime * halfTime;
				r64 inverseHalfTimeSquared = inverseHalfTime * inverseHalfTime;

				glm::dvec2 newPoint =
				{
					points[1].position.x + inverseHalfTimeSquared * (points[0].position.x - points[1].position.x) + halfTimeSquared * (points[2].position.x - points[1].position.x),
					points[1].position.y + inverseHalfTimeSquared * (points[0].position.y - points[1].position.y) + halfTimeSquared * (points[2].position.y - points[1].position.y)
				};

				glm::dvec2 begin = { result[depthIndex].position.x, result[depthIndex].position.y };
				glm::dvec2 end = { result[depthIndex + 1].position.x, result[depthIndex + 1].position.y };

				glm::dvec2 edgeOne = { newPoint.x - begin.x, newPoint.y - begin.y };
				glm::dvec2 edgeTwo = { end.x - newPoint.x, end.y - newPoint.y };

				r64 cross = glm::length(glm::cross(glm::vec3(edgeOne, 0.0), glm::vec3(edgeTwo, 0.0)));

				if (cross > linearity)
				{
					result.emplace(result.begin() + depthIndex + 1, GlyphVertex{ newPoint, (u8)0x1 });
					depths.emplace(depths.begin() + depthIndex + 1, newDepth);
				}
				else
				{
					++depthIndex;
					t += 1.0 / std::pow(2.0, (r64)(newDepth - 1));
				}

			} while (depthIndex != depths.size() - 1 && result.size() < USHRT_MAX);

			return { result.begin() + 1, result.begin() + result.size() - 1 };
		}

#pragma message("TODO: This function needs a rewrite. Lots of repeated logic")
		void Font::expandImpliedPoints(std::vector<GlyphVertex>& vertices, std::vector<u16>& contourEndIndices)
		{
			std::vector<GlyphVertex> expandedVertices;
			std::vector<u16> expandedContourEndIndices = contourEndIndices;

			st nextContourEnd = 0;
			for (u16 p = 1; p < vertices.size(); ++p)
			{
				u16 pointsAdded = 0;
				expandedVertices.emplace_back(vertices[p - 1]);
				if (p != contourEndIndices[nextContourEnd] + 1)
				{
					if ((vertices[p].type & 0x1) == 0 && (vertices[p - 1].type & 0x1) == 0)
					{
						expandedVertices.emplace_back(GlyphVertex{
							{
								(vertices[p].position.x + vertices[p - 1].position.x) / 2,
								(vertices[p].position.y + vertices[p - 1].position.y) / 2
							},
							0x1 });

						++pointsAdded;

						for (st i = nextContourEnd; i < expandedContourEndIndices.size(); ++i)
						{
							expandedContourEndIndices[i] += pointsAdded;
						}
					}
				}
				else
				{
					u16 beginContourIndex = nextContourEnd == 0 ? 0 : contourEndIndices[nextContourEnd - 1] + 1;
					if ((vertices[p].type & 0x1) == 0 && (vertices[beginContourIndex].type & 0x1) == 0)
					{
						expandedVertices.emplace_back(GlyphVertex{
							{
								(vertices[p].position.x + vertices[beginContourIndex].position.x) / 2,
								(vertices[p].position.y + vertices[beginContourIndex].position.y) / 2
							},
							0x1 });

						expandedVertices.emplace_back(vertices[beginContourIndex]);
						pointsAdded += 2;
					}
					else if ((vertices[p].type & 0x1) && (vertices[beginContourIndex].type & 0x1))
					{
						expandedVertices.emplace_back(vertices[beginContourIndex]);
						++pointsAdded;
					}
					else
					{
						expandedVertices.emplace_back(vertices[p].type & 0x1 ? vertices[p] : vertices[beginContourIndex]);
						++pointsAdded;
					}

					if (pointsAdded != 0)
					{
						for (st i = nextContourEnd; i < expandedContourEndIndices.size(); ++i)
						{
							expandedContourEndIndices[i] += pointsAdded;
						}
					}

					++nextContourEnd;
				}
			}

			u16 pointsAdded = 0;
			expandedVertices.emplace_back(vertices.back());
			u16 beginContourIndex = nextContourEnd == 0 ? 0 : contourEndIndices[nextContourEnd - 1] + 1;
			if ((vertices.back().type & 0x1) == 0 && (vertices[beginContourIndex].type & 0x1) == 0)
			{
				expandedVertices.emplace_back(GlyphVertex{
					{
						(vertices.back().position.x + vertices[beginContourIndex].position.x) / 2,
						(vertices.back().position.y + vertices[beginContourIndex].position.y) / 2
					},
					0x1 });

				expandedVertices.emplace_back(vertices[beginContourIndex]);
				pointsAdded += 2;
			}
			else if ((vertices.back().type & 0x1) && (vertices[beginContourIndex].type & 0x1))
			{
				expandedVertices.emplace_back(vertices[beginContourIndex]);
				++pointsAdded;
			}
			else
			{
				expandedVertices.emplace_back(vertices.back().type & 0x1 ? vertices.back() : vertices[beginContourIndex]);
				++pointsAdded;
			}
			expandedContourEndIndices[expandedContourEndIndices.size() - 1] += pointsAdded;

			vertices = expandedVertices;
			contourEndIndices = expandedContourEndIndices;
		}

#pragma message("TODO: This could likely just return a std::vector<glm::dvec2> since vertex type is not used outside of this function")
		std::vector<GlyphVertex> Font::getGlyphVertices(u16 glyphIndex)
		{
			const byte* glyfTable = getGlyfOffset(glyphIndex);
			i16 numContours = PARSE_SHORT_BE(glyfTable);

			const byte* contourEndPoints = glyfTable + 10;
			u16 ins = PARSE_USHORT_BE(contourEndPoints + numContours * 2);
			const byte* pointIter = contourEndPoints + numContours * 2 + 2 + ins;

			u16 numPoints = 1 + PARSE_USHORT_BE(contourEndPoints + (numContours - 1) * 2);

			std::vector<GlyphVertex> vertices;
			for (u16 p = 0; p < numPoints;)
			{
				unsigned char flags = *pointIter++;
				unsigned char flagCount = 1;
				if (flags & 8)
				{
					flags &= ~(0x8);
					flagCount += *pointIter++;
				}

				for (i32 f = 0; f < flagCount; ++f)
				{
					vertices.emplace_back(GlyphVertex{ {}, flags });
				}

				p += (u16)flagCount;
			}

			for (st p = 0; p < numPoints; ++p)
			{
				i16 x = p == 0 ? 0 : (i16)vertices[p - 1].position.x;
				byte flags = vertices[p].type;
				if (flags & 2)
				{
					byte deltaX = *pointIter++;
					x += (flags & 16) ? deltaX : -deltaX;
				}
				else
				{
					if (!(flags & 16))
					{
						x += PARSE_SHORT_BE(pointIter);
						pointIter += 2;
					}
				}
				vertices[p].position.x = (r64)x;
			}

			for (st p = 0; p < numPoints; ++p)
			{
				i16 y = p == 0 ? 0 : (i16)vertices[p - 1].position.y;
				byte flags = vertices[p].type;
				if (flags & 4)
				{
					i16 deltaY = *pointIter++;
					y += (flags & 32) ? deltaY : -deltaY;
				}
				else
				{
					if (!(flags & 32))
					{
						y += PARSE_SHORT_BE(pointIter);
						pointIter += 2;
					}
				}
				vertices[p].position.y = (r64)y;
			}

			std::vector<u16> endContourIndices;

			for (st c = 0; c < (u32)numContours; ++c)
			{
				endContourIndices.push_back(PARSE_USHORT_BE(contourEndPoints + c * 2));
			}

			expandImpliedPoints(vertices, endContourIndices);

			{
				st contourIndex = 0;
				for (u16 p = 0; p < vertices.size();)
				{
					if (!(vertices[p].type & 0x1))
					{
						std::vector<GlyphVertex> curveLinePoints = 
							flattenCurve(
							{
								p == 0 ? vertices[endContourIndices[contourIndex]] : vertices[p - 1],
								vertices[p],
								p == endContourIndices[contourIndex] ? vertices[contourIndex == 0 ? 0 : endContourIndices[contourIndex - 1] + 1] : vertices[p + 1] 
							},
							LINEARITY);

						vertices.erase(vertices.begin() + p);

						for (st l = 0; l < curveLinePoints.size(); ++l)
						{
							vertices.emplace(vertices.begin() + p, curveLinePoints[curveLinePoints.size() - l - 1]);
						}

						p += (u16)curveLinePoints.size();

						for (st i = contourIndex; i < endContourIndices.size(); ++i)
						{
							endContourIndices[i] += (u16)curveLinePoints.size() - 1;
						}
					}
					else
					{
						++p;
					}

					if (p == endContourIndices[contourIndex] + 1)
					{
						++contourIndex;
					}
				}
			}

			return vertices;
		}

#pragma message("TODO: Optimize by sorting edges by scanline and iterate over each scanline once")
		void Font::rasterizeGlyph(const std::vector<GlyphVertex>& vertices, Bitmap& dstBitmap, u16 glyphIndex, r32 scale, glm::dvec2 offset)
		{
			if (vertices.size() == 0)
			{
				return;
			}

			Rect glyphRect = getGlyphBoundingBox(glyphIndex);

			glm::uvec2 glyphDimensions = getGlyphDimensionsForScale(scale, glyphRect, offset);

			Bitmap signedArea;
			signedArea.create(nullptr, glyphDimensions, VK_FORMAT_R64_SFLOAT);

			st beginContourIndex = SIZE_MAX;
			for (st l = 0; l < vertices.size(); ++l)
			{
				if (beginContourIndex == SIZE_MAX)
				{
					beginContourIndex = l;
				}
				else if (vertices[l].position.x == vertices[beginContourIndex].position.x
					&& vertices[l].position.y == vertices[beginContourIndex].position.y)
				{
					beginContourIndex = SIZE_MAX;
					continue;
				}

				if (vertices[l].position.y == vertices[l + 1].position.y)
				{
					continue;
				}

				i32 winding = vertices[l].position.y < vertices[l + 1].position.y ? 1 : -1;

				r64 subPixelStartY = (r64)scale * (std::min(vertices[l].position.y, vertices[l + 1].position.y) - glyphRect.yVals[0]) + glm::fract(offset.y);
				r64 subPixelEndY = (r64)scale * (std::max(vertices[l].position.y, vertices[l + 1].position.y) - glyphRect.yVals[0]) + glm::fract(offset.y);

				u32 startY = (u32)subPixelStartY;
				u32 endY = (u32)subPixelEndY;

				r64 subPixelStartOffsetY = subPixelStartY - startY;
				r64 subPixelEndOffsetY = subPixelEndY - endY;

				for (u32 y = startY; y <= endY; ++y)
				{
					r64 t = glm::clamp(((r64)(y)-subPixelStartY) / (subPixelEndY - subPixelStartY), 0.0, 1.0);

					r64 firstX = winding == 1 ? vertices[l].position.x : vertices[l + 1].position.x;
					r64 secondX = winding == 1 ? vertices[l + 1].position.x : vertices[l].position.x;

					r64 subPixelStartX = scale * (firstX + t * (secondX - firstX) - glyphRect.xVals[0]) + glm::fract(offset.x);

					u32 startX = (u32)subPixelStartX;

					r64 subPixelStartOffsetX = subPixelStartX - startX;

					for (u32 x = startX; x < glyphDimensions.x; ++x)
					{
						r64 area;
						if (startY == endY)
						{
							area = subPixelEndOffsetY - subPixelStartOffsetY;
						}
						else if (y == startY)
						{
							area = 1.0 - subPixelStartOffsetY;
						}
						else if (y == endY)
						{
							area = subPixelEndOffsetY;
						}
						else
						{
							area = 1.0;
						}

						if (x == startX)
						{
							area *= (1.0 - subPixelStartOffsetX);
						}

						r64 sArea = *(r64*)signedArea.at(x, y);
						sArea += winding * area;
						*(r64*)signedArea.at(x, y) = sArea;
					}
				}
			}

			for (st y = 0; y < glyphDimensions.y; ++y)
			{
				for (st x = 0; x < glyphDimensions.x; ++x)
				{
					st dstX = (x + (i32)std::round(offset.x + scale * glyphRect.xVals[0]));
					st dstY = dstBitmap.getDimensions().y - ((i32)std::round(offset.y - scale * glyphRect.yVals[0]) - y);
					r64 dstAlpha = (r64)*dstBitmap.at(dstX, dstY) / UCHAR_MAX;
					r64 srcAlpha = glm::clamp(std::abs(*(r64*)signedArea.at(x, y)), 0.0, 1.0);
					*dstBitmap.at(dstX, dstY) = (u8)std::round(glm::mix(srcAlpha, 1.0, dstAlpha) * UCHAR_MAX);
				}
			}

			signedArea.destroy();
		}

		Bitmap Font::getBitmapForChar(const i32 letter, r32 scale)
		{
			u16 glyphIndex = getGlyphIndex(letter);

			std::vector<GlyphVertex> vertices = getVerticesForChar(letter);

			Rect glyphRect = getGlyphBoundingBox(glyphIndex);

			glm::uvec2 dimensions = getGlyphDimensionsForScale(scale, glyphRect, { 0.0, 0.0 });

			Bitmap bitmap;
			bitmap.create(nullptr, dimensions, VK_FORMAT_R8_UINT);

			rasterizeGlyph(vertices, bitmap, glyphIndex, scale, { 0.0, 0.0 });

			return bitmap;
		}

		Bitmap Font::getBitmapForString(const char* string, r32 scale)
		{
			r64 interm = scale;
			glm::uvec2 dimensions = getStringDimensionsForScale(string, scale);

			Bitmap bitmap;
			bitmap.create(nullptr, dimensions, VK_FORMAT_R8_UINT);

			glm::dvec2 caretOffset{ 0, interm * getAscent() };
			for (st c = 0; string[c]; ++c)
			{
				std::vector<GlyphVertex> vertices = getVerticesForChar(string[c]);
				u16 glyphIndex = getGlyphIndex(string[c]);

				rasterizeGlyph(vertices, bitmap, glyphIndex, scale, caretOffset);
				caretOffset.x += interm * (getKerningAdvance(string[c], string[c + 1]));
			}

			return bitmap;
		}
	}
}
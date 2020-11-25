#ifndef _SSE_FONT_H
#define _SSE_FONT_H

#include <array>
#include <map>
#include <vector>

#include <glm/glm.hpp>

#include "EngineTypeDefs.h"

#include "Graphics/Bitmap.h"

namespace SSE::Graphics
{
	struct GlyphVertex
	{
		glm::dvec2 position;
#pragma message("TODO: Implement GlyphVertexType enum")
		u8 type;
	};

	struct Rect
	{
		glm::ivec2 xVals;
		glm::ivec2 yVals;
	};


	class Font
	{
	private:
		std::map<u32, const byte*> tableMap;
		const byte* data;
		const byte* glyphIndexTable;

		u32 numGlyphs;
		bool locaFormatLong;

#pragma message("TODO: Rewrite interface and private refactoring to have more consistent design")
		u16 getGlyphIndex(const i32 letter);
		const byte* getGlyfOffset(u16 glyphIndex);
		i16 getGlyphLSB(u16 glyphIndex);
		Rect getGlyphBoundingBox(u16 glyphIndex);
		glm::uvec2 getGlyphDimensionsForScale(r32 scale, const Rect& glyphRect, glm::dvec2 offset);
		glm::uvec2 getStringDimensionsForScale(const char* string, r32 scale);
		i16 getKerningAdvance(const char c1, const char c2);
		i16 getAdvance(u16 glyphIndex);
		i16 getKerningForPair(u16 firstIndex, u16 secondIndex);
		i32 getCoverageIndex(const byte* coverageIndices, u16 glyphIndex);
		i16 getGlyphClass(const byte* classDefBegin, u16 glyphIndex);
		i16 getAscent();
		std::vector<GlyphVertex> getVerticesForChar(const i32 letter);

		std::vector<GlyphVertex> flattenCurve(const std::array<GlyphVertex, 3>& points, r64 linearity);
		std::vector<GlyphVertex> getGlyphVertices(u16 glyphIndex);

		void expandImpliedPoints(std::vector<GlyphVertex>& vertices, std::vector<u16>& contourEndIndices);

		void rasterizeGlyph(const std::vector<GlyphVertex>& vertices, Bitmap& dstBitmap, u16 glyphIndex, r32 scale, glm::dvec2 offset);

	public:
#pragma message("TODO: Glyphs need to be cached!")
		bool create(const byte* _data);

		i16 getFullHeight();
		i32 getBaselineForScale(r32 scale);
		i32 getCharLSBForScale(r32 scale, i32 letter);

#pragma message("TODO: Once glyph caching is done, these will need to be rewritten to pull from the cache")
		Bitmap getBitmapForChar(const i32 letter, r32 scale);
		Bitmap getBitmapForString(const char* string, r32 scales);

		void destroy();
	};
}

#endif
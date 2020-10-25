#include "Resources/Assets/ModelAssetUtils.h"

#include <map>
#include <vector>
#include <array>

#include "Logging/Logger.h"

struct IndexPair
{
	union
	{
		u64 pair;
		struct
		{
			u32 pos;
			u32 tex;
		};
	};

	bool operator < (const IndexPair& rhs) const { return pair < rhs.pair; }
};

namespace SSE
{
	extern Logger gLogger;

	namespace Assets::ModelAssetUtils
	{
		Model* loadModelFromFile(const std::string& path)
		{
			FileHandle fh;
			if (fh.create(path, FIOM_BINARY | FIOM_READ))
			{
				std::vector<byte> modelData = fh.readIntoVector();
				const char* parser = (const char*)modelData.data();
				std::map<IndexPair, std::pair<u16, std::array<r32,5>>> indexPairMap;
				std::vector<r32> verts;
				std::vector<u16> indices;
				std::vector<glm::vec4> posCoords;
				std::vector<glm::vec3> texCoords;
				while (*parser != '\0')
				{
					if (strncmp(parser, "v ", 2) == 0)
					{
						const char* element = parser + 2;
						glm::vec4 vertex{1.0f,1.0f,1.0f,1.0f};
						st elementIndex = 0;
						while (*element != '\r' && *element != '\0' && elementIndex < 4)
						{
							st elementSize;
							vertex[(glm::length_t)elementIndex] = std::stof(element, &elementSize);
							element += elementSize;
							++elementIndex;
						}

						posCoords.emplace_back(vertex);
					}
					else if (strncmp(parser, "vt ", 3) == 0)
					{
						const char* element = parser + 3;
						glm::vec3 vertex{ 1.0f, 1.0f, 1.0f };
						st elementIndex = 0;
						while (*element != '\r' && *element != '\0' && elementIndex < 3)
						{
							st elementSize;
							vertex[(glm::length_t)elementIndex] = std::stof(element, &elementSize);
							element += elementSize;
							++elementIndex;
						}

						texCoords.emplace_back(vertex);
					}
					else if (strncmp(parser, "f ", 2) == 0)
					{
						const char* element = parser + 2;
						IndexPair pairs[3];
						st elementSize;
						st p = 0;
						for (; p < 3; ++p)
						{
							pairs[p].pos = std::stoi(element, &elementSize) - 1;
							element += elementSize;
							if (*element != '/' || *(element + 1) == '/')
							{
								break;
							}
							++element;
							pairs[p].tex = std::stoi(element, &elementSize) - 1;
							element += elementSize;
							while (*element != ' ' && *element != '\0' && *element != '\r') ++element;

							if (*element == ' ') ++element;
						}

						if (p == 3)
						{
							for (const auto& pair : pairs)
							{
								if (indexPairMap.find(pair) != indexPairMap.end())
								{
									indices.emplace_back(indexPairMap.at(pair).first);
								}
								else
								{
									std::array<r32, 5> vert;
									for (st i = 0; i < 5; ++i)
									{
										if (i < 3)
										{
											vert[i] = posCoords[pair.pos][(glm::length_t)i];
										}
										else
										{
											vert[i] = texCoords[pair.tex][(glm::length_t)(i - 3)];
										}
									}

									indexPairMap[pair] = std::pair<u16, std::array<r32, 5>>( (u16)(verts.size()/5), vert );
									indices.emplace_back((u16)(verts.size()/5));
									verts.insert(verts.end(), vert.begin(), vert.end());
								}
							}
						}
					}

					while (*parser != '\n' && *parser != '\0')
					{
						++parser;
					}

					if (*parser == '\n') ++parser;
				}

				Model* result = new Model;
				VertexFormat format;
				format.create({ VA_POS3F, VA_TEX2F });
				r32* vertexData = new r32[verts.size()];
				memcpy(vertexData, verts.data(), verts.size() * sizeof(r32));
				if (result->create(vertexData, verts.size() * sizeof(r32), format, indices))
				{
					return result;
				}
				delete result;
			}

			return nullptr;
		}
	}
}
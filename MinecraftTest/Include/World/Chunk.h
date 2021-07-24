#ifndef _CHUNK_H
#define _CHUNK_H

#include <map>
#include <vector>

#include <Object.h>

#include "SparseArray.hpp"

class World;

constexpr st CHUNK_X = 16;
constexpr st CHUNK_Y = 16;
constexpr st CHUNK_Z = 256;
constexpr st CHUNK_SIZE = CHUNK_X * CHUNK_Y * CHUNK_Z;

enum BlockFace
{
	BF_POS_X = 0x1,
	BF_NEG_X = 0x2,
	BF_POS_Y = 0x4,
	BF_NEG_Y = 0x8,
	BF_POS_Z = 0x10,
	BF_NEG_Z = 0x20
};

inline bool blockIsInChunk(const glm::ivec3& blockPos);

class Chunk
{
private:
	u8 light[CHUNK_X][CHUNK_Y][CHUNK_Z];
	u8 blocks[CHUNK_X][CHUNK_Y][CHUNK_Z];
	SparseArray<st> indices;
	std::vector<byte> mesh;
	SSE::Object chunkObject;
	glm::ivec2 chunkIndices;
	st meshSize = 0;
	st indicesOffset = 0;
	World* world;

	bool isMeshDirty = true;

	void generateBlocks();
	void reconstructMesh();
	void constructMesh();
	u8 getLightLevel(const glm::ivec3& blockPos);
	r32 getVertexLight(const glm::ivec3& blockPos, const glm::ivec3& vertexOffset, BlockFace normal);
	bitfield getBlockFacing(glm::ivec3 blockPos);

	inline glm::ivec3 chunkBlockPosToWorldBlockPos(const glm::ivec3& blockPos);

protected:
	std::vector<glm::ivec3> lightChanges;
	std::pair<glm::ivec3, bool> blockChange;
	bool setLightLevel(const glm::ivec3& blockPos, u8 lightLevel);
	bool setMeshDirty();
	void propogateLight();

	friend World;

public:
	bool create(World* _world, glm::ivec2 _chunkIndices);

	void update();

	void destroy();

	glm::ivec2 getChunkIndices();
	
	u8 getBlock(const glm::ivec3& blockPos);
	void deleteBlock(const glm::ivec3& blockPos);
	void setBlock(const glm::ivec3& blockPos, u8 block);
	const SSE::Object& getObject();
};

#endif
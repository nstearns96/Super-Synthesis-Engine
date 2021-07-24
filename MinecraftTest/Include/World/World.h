#ifndef _WORLD_H
#define _WORLD_H

#include <map>
#include <queue>

#include <glm/glm.hpp>

#include "World/Chunk.h"

struct KeyFuncs
{
	bool operator()(const glm::ivec2& a, const glm::ivec2& b)const
	{
		return a.x < b.x || (a.x == b.x && a.y < b.y);
	}
};

class World
{
private:
	std::map<glm::ivec2, Chunk*, KeyFuncs> chunks;
	std::queue<glm::ivec3> lightNodes;

	bool isLightDirty = false;

protected:
	bool setLightLevel(glm::ivec3 blockPos, u8 lightLevel);
	bool addLightNode(const glm::ivec3& blockPos);
	Chunk* getChunk(const glm::ivec2& indices);

	friend Chunk;

public:
	bool generateChunk(glm::ivec2 indices);

	void update();

	void destroy();

	void render();

	u8 getBlock(glm::ivec3 blockPos);
	bool breakBlock(glm::ivec3 blockPos);
	bool setBlock(glm::ivec3 blockPos, u8 block);
	bool getFirstBlock(glm::vec3 rayOrigin, glm::vec3 rayDirection, r32 reach, glm::ivec3& outBlockPos, BlockFace& outBlockFace);

	void propogateLight();
	u8 getLightLevel(glm::ivec3 blockPos);
};

#endif
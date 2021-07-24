#include "World/Chunk.h"

#include <algorithm>
#include <queue>

#include <Random/Random.h>

#include <Resources/ResourceManager.h>

#include <Time/ScopedTimer.h>

#include "World/World.h"

inline bool blockIsInChunk(const glm::ivec3& blockPos)
{
	return blockPos.x >= 0 && blockPos.x < (i32)CHUNK_X&&
		blockPos.y >= 0 && blockPos.y < (i32)CHUNK_Y&&
		blockPos.z >= 0 && blockPos.z < (i32)CHUNK_Z;
}

inline glm::ivec3 Chunk::chunkBlockPosToWorldBlockPos(const glm::ivec3& blockPos)
{
	return blockPos + glm::ivec3(chunkIndices.x * (i32)CHUNK_X, chunkIndices.y * (i32)CHUNK_Y, 0);
}

bool Chunk::create(World* _world, glm::ivec2 _chunkIndices)
{
	world = _world;
	generateBlocks();
	chunkIndices = _chunkIndices;

	propogateLight();
	constructMesh();

	SSE::Model chunkModel;
	chunkModel.create(((sizeof(r32) * 6 * 8 + sizeof(u16) * 6 * 6) * CHUNK_X * CHUNK_Y * CHUNK_Z) / 2, true);
	chunkModel.update((void*)mesh.data(), meshSize, indicesOffset);
	ResourceHandle modelHandle = SSE::ResourceManager::createModel(chunkModel);

	chunkObject.create(modelHandle, 0);
	chunkObject.transform(glm::vec3((r32)CHUNK_X * _chunkIndices.x, (r32)CHUNK_Y * _chunkIndices.y, 0.0f), glm::vec3((r32)CHUNK_Z), glm::mat4(1.0f));
	return true;
}

void Chunk::destroy()
{
	chunkObject.destroy();
}

void Chunk::generateBlocks()
{
	memset(blocks, 0x00, sizeof(blocks));

	glm::vec2 gradLinear(SSE::Random::rand(-1.0f, 1.0f), SSE::Random::rand(-1.0f, 1.0f));
	glm::vec2 gradQuadratic(SSE::Random::rand(-1.0f / CHUNK_X, 1.0f / CHUNK_X), SSE::Random::rand(-1.0f / CHUNK_Y, 1.0f / CHUNK_Y));
	r32 initZ = CHUNK_Z / 4.0f;

	for (st y = 0; y < CHUNK_Y; ++y)
	{
		for (st x = 0; x < CHUNK_X; ++x)
		{
			glm::vec2 gradPartial(gradLinear.x + gradQuadratic.x * x, gradLinear.y + gradQuadratic.y * y);

			r32 coordZ = initZ + gradPartial.x * x + gradPartial.y * y;

			for (st z = 0; z < coordZ; ++z)
			{
				blocks[x][y][z] = 1;
			}
		}
	}
}

bool Chunk::setLightLevel(const glm::ivec3& blockPos, u8 lightLevel)
{
	if (light[blockPos.x][blockPos.y][blockPos.z] != lightLevel)
	{
		light[blockPos.x][blockPos.y][blockPos.z] = lightLevel;
		lightChanges.push_back(blockPos);
		isMeshDirty = true;
		return true;
	}

	return false;
}

void Chunk::propogateLight()
{
	//These are basically const once the neighbor chunk is generated so maybe cache them from a world callback?
	Chunk* neighborChunks[4] =
	{
		world->getChunk(chunkIndices + glm::ivec2(-1, 0)),
		world->getChunk(chunkIndices + glm::ivec2(1, 0)),
		world->getChunk(chunkIndices + glm::ivec2(0, -1)),
		world->getChunk(chunkIndices + glm::ivec2(0, 1)),
	};

	if (isMeshDirty)
	{
		memset(light, 0x0, sizeof(light));

		//sunlight propogate
		glm::ivec2 lightRange(0, CHUNK_Z);

		for (i32 z = CHUNK_Z - 1; z >= 0; --z)
		{
			bool isLightDone = true;
			for (i32 y = 0; y < CHUNK_Y; ++y)
			{
				for (i32 x = 0; x < CHUNK_X; ++x)
				{
					if (blocks[x][y][z] == 0 &&
						(z == CHUNK_Z - 1 ||
							light[x][y][z + 1] == 0xf))
					{
						isLightDone = false;
						light[x][y][z] = 0xf;
					}
					else if (lightRange[1] == CHUNK_Z && blocks[x][y][z] != 0)
					{
						lightRange[1] = z;
					}
				}
			}
			if (isLightDone)
			{
				lightRange[0] = z;
				break;
			}
		}

		for (i32 z = lightRange[1]; z > lightRange[0]; --z)
		{
			for (i32 y = 0; y < (i32)(CHUNK_Y); ++y)
			{
				for (i32 x = 0; x < (i32)(CHUNK_X); ++x)
				{
					u8 lightLevel = light[x][y][z];
					if (lightLevel == 0xf)
					{
						if (((x != 0) ?
							(blocks[x - 1][y][z] == 0 && light[x - 1][y][z] != 0xf) :
							((neighborChunks[0] != nullptr) ? (neighborChunks[0]->getBlock({ (i32)CHUNK_X - 1, y, z }) == 0 && neighborChunks[0]->getLightLevel({ (i32)CHUNK_X - 1,y,z }) != 0xf) : (false))) ||
							((x != ((i32)CHUNK_X - 1)) ?
								(blocks[x + 1][y][z] == 0 && light[x + 1][y][z] != 0xf) :
								((neighborChunks[1] != nullptr) ? (neighborChunks[1]->getBlock({ 0, y, z }) == 0 && neighborChunks[1]->getLightLevel({ 0,y,z }) != 0xf) : (false))) ||
							((y != 0) ?
								(blocks[x][y - 1][z] == 0 && light[x][y - 1][z] != 0xf) :
								((neighborChunks[2] != nullptr) ? (neighborChunks[2]->getBlock({ x, (i32)CHUNK_Y - 1, z }) == 0 && neighborChunks[2]->getLightLevel({ x,(i32)CHUNK_Y - 1,z }) != 0xf) : (false))) ||
							((y != ((i32)CHUNK_Y - 1)) ?
								(blocks[x][y + 1][z] == 0 && light[x][y + 1][z] != 0xf) :
								((neighborChunks[3] != nullptr) ? (neighborChunks[3]->getBlock({ x, 0, z }) == 0 && neighborChunks[3]->getLightLevel({ x, 0,z }) != 0xf) : (false))) ||
							((z != 0) ?
								(blocks[x][y][z - 1] == 0 && light[x][y][z - 1] != 0xf) :
								false) ||
							((z != ((i32)CHUNK_Z - 1)) ?
								(blocks[x][y][z + 1] == 0 && light[x][y][z + 1] != 0xf) :
								false))
						{
							world->addLightNode(chunkBlockPosToWorldBlockPos({ x,y,z }));
						}
					}
				}
			}
		}
	}
}

u8 Chunk::getLightLevel(const glm::ivec3& blockPos)
{
	if (blockIsInChunk(blockPos))
	{
		return light[blockPos.x][blockPos.y][blockPos.z];
	}
	else
	{
		return world->getLightLevel(chunkBlockPosToWorldBlockPos(blockPos));
	}
}

r32 Chunk::getVertexLight(const glm::ivec3& blockPos, const glm::ivec3& vertexOffset, BlockFace normal)
{
	glm::vec3 faceBlocks[4] = {};

	i32 x = blockPos.x;
	i32 y = blockPos.y;
	i32 z = blockPos.z;

	switch (normal)
	{
	case BF_NEG_X:
	case BF_POS_X:
	{
		faceBlocks[0] = { x + vertexOffset.x ,y,z };
		faceBlocks[1] = { x + vertexOffset.x ,y,z + vertexOffset.z };
		faceBlocks[2] = { x + vertexOffset.x ,y + vertexOffset.y,z };
		faceBlocks[3] = { x + vertexOffset.x , y + vertexOffset.y, z + vertexOffset.z };
		break;
	}
	case BF_NEG_Y:
	case BF_POS_Y:
	{
		faceBlocks[0] = { x,y + vertexOffset.y,z };
		faceBlocks[1] = { x,y + vertexOffset.y,z + vertexOffset.z };
		faceBlocks[2] = { x + vertexOffset.x,y + vertexOffset.y,z };
		faceBlocks[3] = { x + vertexOffset.x, y + vertexOffset.y, z + vertexOffset.z };
		break;
	}
	case BF_NEG_Z:
	case BF_POS_Z:
	{
		faceBlocks[0] = { x,y,z + vertexOffset.z };
		faceBlocks[1] = { x,y + vertexOffset.y,z + vertexOffset.z };
		faceBlocks[2] = { x + vertexOffset.x,y,z + vertexOffset.z };
		faceBlocks[3] = { x + vertexOffset.x,y + vertexOffset.y,z + vertexOffset.z };
		break;
	}
	}

	if (getBlock(faceBlocks[1]) == 0 || getBlock(faceBlocks[2]) == 0)
	{
		return (r32)(getLightLevel(faceBlocks[0]) + getLightLevel(faceBlocks[1]) + getLightLevel(faceBlocks[2]) + getLightLevel(faceBlocks[3]) + 4) / (4.0f * 16);
	}
	else
	{
		return (r32)(getLightLevel(faceBlocks[0]) + getLightLevel(faceBlocks[1]) + getLightLevel(faceBlocks[2]) + 3) / (3.0f * 16);
	}
}

void Chunk::reconstructMesh()
{
	st newIndicesOffset = indicesOffset;
	st faceIndexCount = (newIndicesOffset) / (sizeof(r32) * 24);
	mesh.resize(newIndicesOffset + faceIndexCount * 6 * sizeof(u16));
	u16* indexWrite = (u16*)(mesh.data() + newIndicesOffset);
	for (st index = 0; index < faceIndexCount; ++index)
	{
		*(indexWrite + index * 6) = (u16)index * 4;
		*(indexWrite + index * 6 + 1) = (u16)(index * 4 + 1);
		*(indexWrite + index * 6 + 2) = (u16)(index * 4 + 2);
		*(indexWrite + index * 6 + 3) = (u16)(index * 4 + 1);
		*(indexWrite + index * 6 + 4) = (u16)(index * 4 + 3);
		*(indexWrite + index * 6 + 5) = (u16)(index * 4 + 2);
	}

	indicesOffset = newIndicesOffset;
	meshSize = mesh.size();

	for (const auto& lightChange : lightChanges)
	{
		for (i32 dx = -1; dx < 2; ++dx)
		{
			for (i32 dy = -1; dy < 2; ++dy)
			{
				for (i32 dz = -1; dz < 2; ++dz)
				{
					glm::ivec3 blockPos = lightChange + glm::ivec3(dx, dy, dz);
					if (!blockIsInChunk(blockPos) ||
						blocks[blockPos.x][blockPos.y][blockPos.z] == 0)
					{
						continue;
					}

					bitfield neighbors = getBlockFacing(blockPos);
					if (neighbors == ((bitfield)BF_POS_X | BF_NEG_X | BF_POS_Y | BF_NEG_Y | BF_POS_Z | BF_NEG_Z))
					{
						//block is not visible
						continue;
					}

					r32* lightWrite = (r32*)(mesh.data() + indices[blockPos.x * CHUNK_Z * CHUNK_Y + blockPos.y * CHUNK_Z + dz]);
					if ((neighbors & BF_POS_X) == 0)
					{
						*(lightWrite + 5) = getVertexLight(blockPos, { 1,-1,-1 }, BF_POS_X);
						*(lightWrite + 11) = getVertexLight(blockPos, { 1,1,-1 }, BF_POS_X);
						*(lightWrite + 17) = getVertexLight(blockPos, { 1,-1,1 }, BF_POS_X);
						*(lightWrite + 23) = getVertexLight(blockPos, { 1,1,1 }, BF_POS_X);

						lightWrite += 24;
					}
					if ((neighbors & BF_NEG_X) == 0)
					{
						*(lightWrite + 5) = getVertexLight(blockPos, { -1,1,-1 }, BF_NEG_X);
						*(lightWrite + 11) = getVertexLight(blockPos, { -1,-1,-1 }, BF_NEG_X);
						*(lightWrite + 17) = getVertexLight(blockPos, { -1,1,1 }, BF_NEG_X);
						*(lightWrite + 23) = getVertexLight(blockPos, { -1,-1,1 }, BF_NEG_X);

						lightWrite += 24;
					}
					if ((neighbors & BF_POS_Y) == 0)
					{
						*(lightWrite + 5) = getVertexLight(blockPos, { 1,1,-1 }, BF_POS_Y);
						*(lightWrite + 11) = getVertexLight(blockPos, { -1,1,-1 }, BF_POS_Y);
						*(lightWrite + 17) = getVertexLight(blockPos, { 1,1,1 }, BF_POS_Y);
						*(lightWrite + 23) = getVertexLight(blockPos, { -1,1,1 }, BF_POS_Y);

						lightWrite += 24;
					}
					if ((neighbors & BF_NEG_Y) == 0)
					{
						*(lightWrite + 5) = getVertexLight(blockPos, { -1,-1,-1 }, BF_NEG_Y);
						*(lightWrite + 11) = getVertexLight(blockPos, { 1,-1,-1 }, BF_NEG_Y);
						*(lightWrite + 17) = getVertexLight(blockPos, { -1,-1,1 }, BF_NEG_Y);
						*(lightWrite + 23) = getVertexLight(blockPos, { 1,-1,1 }, BF_NEG_Y);

						lightWrite += 24;
					}
					if ((neighbors & BF_POS_Z) == 0)
					{
						*(lightWrite + 5) = getVertexLight(blockPos, { -1,-1,1 }, BF_POS_Z);
						*(lightWrite + 11) = getVertexLight(blockPos, { 1,-1,1 }, BF_POS_Z);
						*(lightWrite + 17) = getVertexLight(blockPos, { -1,1,1 }, BF_POS_Z);
						*(lightWrite + 23) = getVertexLight(blockPos, { 1,1,1 }, BF_POS_Z);

						lightWrite += 24;
					}
					if ((neighbors & BF_NEG_Z) == 0)
					{
						*(lightWrite + 5) = getVertexLight(blockPos, { 1,-1,-1 }, BF_NEG_Z);
						*(lightWrite + 11) = getVertexLight(blockPos, { -1,-1,-1 }, BF_NEG_Z);
						*(lightWrite + 17) = getVertexLight(blockPos, { 1,1,-1 }, BF_NEG_Z);
						*(lightWrite + 23) = getVertexLight(blockPos, { -1,1,-1 }, BF_NEG_Z);

						lightWrite += 24;
					}
				}
			}
		}
	}

	lightChanges.clear();
}

void Chunk::constructMesh()
{
	meshSize = 0;
	for (i32 x = 0; x < CHUNK_X; ++x)
	{
		for (i32 y = 0; y < CHUNK_Y; ++y)
		{
			for (i32 z = 0; z < CHUNK_Z; ++z)
			{
				if (blocks[x][y][z] == 0)
				{
					continue;
				}

				glm::ivec3 blockPos(x, y, z);

				bitfield neighbors = getBlockFacing(glm::ivec3(x, y, z));

				if (neighbors == ((bitfield)BF_POS_X | BF_NEG_X | BF_POS_Y | BF_NEG_Y | BF_POS_Z | BF_NEG_Z))
				{
					//block is not visible
					continue;
				}

				st numFaces = 0;
				for (st bit = 0x1; bit <= (st)BF_NEG_Z; bit <<= 1)
				{
					if ((neighbors & bit) == 0)
					{
						++numFaces;
					}
				}

				mesh.resize(meshSize + numFaces * 24 * sizeof(r32));
				r32* vertWrite = (r32*)(mesh.data() + meshSize);
				if ((neighbors & BF_POS_X) == 0)
				{
					*vertWrite = (((r32)x + 1) / CHUNK_X) / (CHUNK_Z / CHUNK_X);
					*(vertWrite + 1) = (((r32)y) / CHUNK_Y) / (CHUNK_Z / CHUNK_Y);
					*(vertWrite + 2) = (((r32)z) / CHUNK_Z);
					*(vertWrite + 3) = 1.0f / 3;
					*(vertWrite + 4) = 0.0f;
					*(vertWrite + 5) = getVertexLight(blockPos, { 1,-1,-1 }, BF_POS_X);

					*(vertWrite + 6) = (((r32)x + 1) / CHUNK_X) / (CHUNK_Z / CHUNK_X);
					*(vertWrite + 7) = (((r32)y + 1) / CHUNK_Y) / (CHUNK_Z / CHUNK_Y);
					*(vertWrite + 8) = (((r32)z) / CHUNK_Z);
					*(vertWrite + 9) = 2.0f / 3;
					*(vertWrite + 10) = 0.0f;
					*(vertWrite + 11) = getVertexLight(blockPos, { 1,1,-1 }, BF_POS_X);

					*(vertWrite + 12) = (((r32)x + 1) / CHUNK_X) / (CHUNK_Z / CHUNK_X);
					*(vertWrite + 13) = (((r32)y) / CHUNK_Y) / (CHUNK_Z / CHUNK_Y);
					*(vertWrite + 14) = (((r32)z + 1) / CHUNK_Z);
					*(vertWrite + 15) = 1.0f / 3;
					*(vertWrite + 16) = 1.0f;
					*(vertWrite + 17) = getVertexLight(blockPos, { 1,-1,1 }, BF_POS_X);

					*(vertWrite + 18) = (((r32)x + 1) / CHUNK_X) / (CHUNK_Z / CHUNK_X);
					*(vertWrite + 19) = (((r32)y + 1) / CHUNK_Y) / (CHUNK_Z / CHUNK_Y);
					*(vertWrite + 20) = (((r32)z + 1) / CHUNK_Z);
					*(vertWrite + 21) = 2.0f / 3;
					*(vertWrite + 22) = 1.0f;
					*(vertWrite + 23) = getVertexLight(blockPos, { 1,1,1 }, BF_POS_X);

					indices.insert((x * CHUNK_Z * CHUNK_Y + y * CHUNK_Z + z) * 6 + 0, meshSize);

					meshSize += 24 * sizeof(r32);
					vertWrite += 24;
				}
				if ((neighbors & BF_NEG_X) == 0)
				{
					*vertWrite = (((r32)x) / CHUNK_X) / (CHUNK_Z / CHUNK_X);
					*(vertWrite + 1) = (((r32)y + 1) / CHUNK_Y) / (CHUNK_Z / CHUNK_Y);
					*(vertWrite + 2) = (((r32)z) / CHUNK_Z);
					*(vertWrite + 3) = 1.0f / 3;
					*(vertWrite + 4) = 0.0f;
					*(vertWrite + 5) = getVertexLight(blockPos, { -1,1,-1 }, BF_NEG_X);

					*(vertWrite + 6) = (((r32)x) / CHUNK_X) / (CHUNK_Z / CHUNK_X);
					*(vertWrite + 7) = (((r32)y) / CHUNK_Y) / (CHUNK_Z / CHUNK_Y);
					*(vertWrite + 8) = (((r32)z) / CHUNK_Z);
					*(vertWrite + 9) = 2.0f / 3;
					*(vertWrite + 10) = 0.0f;
					*(vertWrite + 11) = getVertexLight(blockPos, { -1,-1,-1 }, BF_NEG_X);

					*(vertWrite + 12) = (((r32)x) / CHUNK_X) / (CHUNK_Z / CHUNK_X);
					*(vertWrite + 13) = (((r32)y + 1) / CHUNK_Y) / (CHUNK_Z / CHUNK_Y);
					*(vertWrite + 14) = (((r32)z + 1) / CHUNK_Z);
					*(vertWrite + 15) = 1.0f / 3;
					*(vertWrite + 16) = 1.0f;
					*(vertWrite + 17) = getVertexLight(blockPos, { -1,1,1 }, BF_NEG_X);

					*(vertWrite + 18) = (((r32)x) / CHUNK_X) / (CHUNK_Z / CHUNK_X);
					*(vertWrite + 19) = (((r32)y) / CHUNK_Y) / (CHUNK_Z / CHUNK_Y);
					*(vertWrite + 20) = (((r32)z + 1) / CHUNK_Z);
					*(vertWrite + 21) = 2.0f / 3;
					*(vertWrite + 22) = 1.0f;
					*(vertWrite + 23) = getVertexLight(blockPos, { -1,-1,1 }, BF_NEG_X);

					indices.insert((x * CHUNK_Z * CHUNK_Y + y * CHUNK_Z + z) * 6 + 1, meshSize);

					meshSize += 24 * sizeof(r32);
					vertWrite += 24;
				}
				if ((neighbors & BF_POS_Y) == 0)
				{
					*vertWrite = (((r32)x + 1) / CHUNK_X) / (CHUNK_Z / CHUNK_X);
					*(vertWrite + 1) = (((r32)y + 1) / CHUNK_Y) / (CHUNK_Z / CHUNK_Y);
					*(vertWrite + 2) = (((r32)z) / CHUNK_Z);
					*(vertWrite + 3) = 1.0f / 3;
					*(vertWrite + 4) = 0.0f;
					*(vertWrite + 5) = getVertexLight(blockPos, { 1,1,-1 }, BF_POS_Y);

					*(vertWrite + 6) = (((r32)x) / CHUNK_X) / (CHUNK_Z / CHUNK_X);
					*(vertWrite + 7) = (((r32)y + 1) / CHUNK_Y) / (CHUNK_Z / CHUNK_Y);
					*(vertWrite + 8) = (((r32)z) / CHUNK_Z);
					*(vertWrite + 9) = 2.0f / 3;
					*(vertWrite + 10) = 0.0f;
					*(vertWrite + 11) = getVertexLight(blockPos, { -1,1,-1 }, BF_POS_Y);

					*(vertWrite + 12) = (((r32)x + 1) / CHUNK_X) / (CHUNK_Z / CHUNK_X);
					*(vertWrite + 13) = (((r32)y + 1) / CHUNK_Y) / (CHUNK_Z / CHUNK_Y);
					*(vertWrite + 14) = (((r32)z + 1) / CHUNK_Z);
					*(vertWrite + 15) = 1.0f / 3;
					*(vertWrite + 16) = 1.0f;
					*(vertWrite + 17) = getVertexLight(blockPos, { 1,1,1 }, BF_POS_Y);

					*(vertWrite + 18) = (((r32)x) / CHUNK_X) / (CHUNK_Z / CHUNK_X);
					*(vertWrite + 19) = (((r32)y + 1) / CHUNK_Y) / (CHUNK_Z / CHUNK_Y);
					*(vertWrite + 20) = (((r32)z + 1) / CHUNK_Z);
					*(vertWrite + 21) = 2.0f / 3;
					*(vertWrite + 22) = 1.0f;
					*(vertWrite + 23) = getVertexLight(blockPos, { -1,1,1 }, BF_POS_Y);

					indices.insert((x * CHUNK_Z * CHUNK_Y + y * CHUNK_Z + z) * 6 + 2, meshSize);

					meshSize += 24 * sizeof(r32);
					vertWrite += 24;
				}
				if ((neighbors & BF_NEG_Y) == 0)
				{
					*vertWrite = (((r32)x) / CHUNK_X) / (CHUNK_Z / CHUNK_X);
					*(vertWrite + 1) = (((r32)y) / CHUNK_Y) / (CHUNK_Z / CHUNK_Y);
					*(vertWrite + 2) = (((r32)z) / CHUNK_Z);
					*(vertWrite + 3) = 1.0f / 3;
					*(vertWrite + 4) = 0.0f;
					*(vertWrite + 5) = getVertexLight(blockPos, { -1,-1,-1 }, BF_NEG_Y);

					*(vertWrite + 6) = (((r32)x + 1) / CHUNK_X) / (CHUNK_Z / CHUNK_X);
					*(vertWrite + 7) = (((r32)y) / CHUNK_Y) / (CHUNK_Z / CHUNK_Y);
					*(vertWrite + 8) = (((r32)z) / CHUNK_Z);
					*(vertWrite + 9) = 2.0f / 3;
					*(vertWrite + 10) = 0.0f;
					*(vertWrite + 11) = getVertexLight(blockPos, { 1,-1,-1 }, BF_NEG_Y);

					*(vertWrite + 12) = (((r32)x) / CHUNK_X) / (CHUNK_Z / CHUNK_X);
					*(vertWrite + 13) = (((r32)y) / CHUNK_Y) / (CHUNK_Z / CHUNK_Y);
					*(vertWrite + 14) = (((r32)z + 1) / CHUNK_Z);
					*(vertWrite + 15) = 1.0f / 3;
					*(vertWrite + 16) = 1.0f;
					*(vertWrite + 17) = getVertexLight(blockPos, { -1,-1,1 }, BF_NEG_Y);

					*(vertWrite + 18) = (((r32)x + 1) / CHUNK_X) / (CHUNK_Z / CHUNK_X);
					*(vertWrite + 19) = (((r32)y) / CHUNK_Y) / (CHUNK_Z / CHUNK_Y);
					*(vertWrite + 20) = (((r32)z + 1) / CHUNK_Z);
					*(vertWrite + 21) = 2.0f / 3;
					*(vertWrite + 22) = 1.0f;
					*(vertWrite + 23) = getVertexLight(blockPos, { 1,-1,1 }, BF_NEG_Y);

					indices.insert((x * CHUNK_Z * CHUNK_Y + y * CHUNK_Z + z) * 6 + 3, meshSize);

					meshSize += 24 * sizeof(r32);
					vertWrite += 24;
				}
				if ((neighbors & BF_POS_Z) == 0)
				{
					*vertWrite = (((r32)x) / CHUNK_X) / (CHUNK_Z / CHUNK_X);
					*(vertWrite + 1) = (((r32)y) / CHUNK_Y) / (CHUNK_Z / CHUNK_Y);
					*(vertWrite + 2) = (((r32)z + 1) / CHUNK_Z);
					*(vertWrite + 3) = 0.0f / 3;
					*(vertWrite + 4) = 0.0f;
					*(vertWrite + 5) = getVertexLight(blockPos, { -1,-1,1 }, BF_POS_Z);

					*(vertWrite + 6) = (((r32)x + 1) / CHUNK_X) / (CHUNK_Z / CHUNK_X);
					*(vertWrite + 7) = (((r32)y) / CHUNK_Y) / (CHUNK_Z / CHUNK_Y);
					*(vertWrite + 8) = (((r32)z + 1) / CHUNK_Z);
					*(vertWrite + 9) = 1.0f / 3;
					*(vertWrite + 10) = 0.0f;
					*(vertWrite + 11) = getVertexLight(blockPos, { 1,-1,1 }, BF_POS_Z);

					*(vertWrite + 12) = (((r32)x) / CHUNK_X) / (CHUNK_Z / CHUNK_X);
					*(vertWrite + 13) = (((r32)y + 1) / CHUNK_Y) / (CHUNK_Z / CHUNK_Y);
					*(vertWrite + 14) = (((r32)z + 1) / CHUNK_Z);
					*(vertWrite + 15) = 0.0f / 3;
					*(vertWrite + 16) = 1.0f;
					*(vertWrite + 17) = getVertexLight(blockPos, { -1,1,1 }, BF_POS_Z);

					*(vertWrite + 18) = (((r32)x + 1) / CHUNK_X) / (CHUNK_Z / CHUNK_X);
					*(vertWrite + 19) = (((r32)y + 1) / CHUNK_Y) / (CHUNK_Z / CHUNK_Y);
					*(vertWrite + 20) = (((r32)z + 1) / CHUNK_Z);
					*(vertWrite + 21) = 1.0f / 3;
					*(vertWrite + 22) = 1.0f;
					*(vertWrite + 23) = getVertexLight(blockPos, { 1,1,1 }, BF_POS_Z);

					indices.insert((x * CHUNK_Z * CHUNK_Y + y * CHUNK_Z + z) * 6 + 4, meshSize);

					meshSize += 24 * sizeof(r32);
					vertWrite += 24;
				}
				if ((neighbors & BF_NEG_Z) == 0)
				{
					*vertWrite = (((r32)x + 1) / CHUNK_X) / (CHUNK_Z / CHUNK_X);
					*(vertWrite + 1) = (((r32)y) / CHUNK_Y) / (CHUNK_Z / CHUNK_Y);
					*(vertWrite + 2) = (((r32)z) / CHUNK_Z);
					*(vertWrite + 3) = 2.0f / 3;
					*(vertWrite + 4) = 0.0f;
					*(vertWrite + 5) = getVertexLight(blockPos, { 1,-1,-1 }, BF_NEG_Z);

					*(vertWrite + 6) = (((r32)x) / CHUNK_X) / (CHUNK_Z / CHUNK_X);
					*(vertWrite + 7) = (((r32)y) / CHUNK_Y) / (CHUNK_Z / CHUNK_Y);
					*(vertWrite + 8) = (((r32)z) / CHUNK_Z);
					*(vertWrite + 9) = 3.0f / 3;
					*(vertWrite + 10) = 0.0f;
					*(vertWrite + 11) = getVertexLight(blockPos, { -1,-1,-1 }, BF_NEG_Z);

					*(vertWrite + 12) = (((r32)x + 1) / CHUNK_X) / (CHUNK_Z / CHUNK_X);
					*(vertWrite + 13) = (((r32)y + 1) / CHUNK_Y) / (CHUNK_Z / CHUNK_Y);
					*(vertWrite + 14) = (((r32)z) / CHUNK_Z);
					*(vertWrite + 15) = 2.0f / 3;
					*(vertWrite + 16) = 1.0f;
					*(vertWrite + 17) = getVertexLight(blockPos, { 1,1,-1 }, BF_NEG_Z);

					*(vertWrite + 18) = (((r32)x) / CHUNK_X) / (CHUNK_Z / CHUNK_X);
					*(vertWrite + 19) = (((r32)y + 1) / CHUNK_Y) / (CHUNK_Z / CHUNK_Y);
					*(vertWrite + 20) = (((r32)z) / CHUNK_Z);
					*(vertWrite + 21) = 3.0f / 3;
					*(vertWrite + 22) = 1.0f;
					*(vertWrite + 23) = getVertexLight(blockPos, { -1,1,-1 }, BF_NEG_Z);

					indices.insert((x * CHUNK_Z * CHUNK_Y + y * CHUNK_Z + z) * 6 + 5, meshSize);

					meshSize += 24 * sizeof(r32);
				}
			}
		}
	}

	st newIndicesOffset = meshSize;
	st faceIndexCount = (meshSize) / (sizeof(r32) * 24);
	mesh.resize(meshSize + faceIndexCount * 6 * sizeof(u16));
	u16* indexWrite = (u16*)(mesh.data() + meshSize);
	for (st index = 0; index < faceIndexCount; ++index)
	{
		*(indexWrite + index * 6) = (u16)index * 4;
		*(indexWrite + index * 6 + 1) = (u16)(index * 4 + 1);
		*(indexWrite + index * 6 + 2) = (u16)(index * 4 + 2);
		*(indexWrite + index * 6 + 3) = (u16)(index * 4 + 1);
		*(indexWrite + index * 6 + 4) = (u16)(index * 4 + 3);
		*(indexWrite + index * 6 + 5) = (u16)(index * 4 + 2);
	}

	meshSize += sizeof(u16) * faceIndexCount * 6;
	indicesOffset = newIndicesOffset;
}

bitfield Chunk::getBlockFacing(glm::ivec3 blockPos)
{
	if (!blockIsInChunk(blockPos))
	{
		return 0;
	}

	bitfield result = 0;
	if (blockPos.x < (i32)CHUNK_X - 1 ?
		blocks[blockPos.x + 1][blockPos.y][blockPos.z] == 1 :
		world->getBlock(chunkBlockPosToWorldBlockPos({ blockPos.x + 1, blockPos.y, blockPos.z })) == 1)
	{
		result |= BF_POS_X;
	}
	if (blockPos.x > 0 ?
		blocks[blockPos.x - 1][blockPos.y][blockPos.z] == 1 :
		world->getBlock(chunkBlockPosToWorldBlockPos({ blockPos.x - 1, blockPos.y, blockPos.z })) == 1)
	{
		result |= BF_NEG_X;
	}
	if (blockPos.y < (i32)CHUNK_Y - 1 ?
		blocks[blockPos.x][blockPos.y + 1][blockPos.z] == 1 :
		world->getBlock(chunkBlockPosToWorldBlockPos({ blockPos.x, blockPos.y + 1, blockPos.z })) == 1)
	{
		result |= BF_POS_Y;
	}
	if (blockPos.y > 0 ?
		blocks[blockPos.x][blockPos.y - 1][blockPos.z] == 1 :
		world->getBlock(chunkBlockPosToWorldBlockPos({ blockPos.x, blockPos.y - 1, blockPos.z })) == 1)
	{
		result |= BF_NEG_Y;
	}
	if (blockPos.z < (i32)CHUNK_Z - 1 &&
		blocks[blockPos.x][blockPos.y][blockPos.z + 1] == 1)
	{
		result |= BF_POS_Z;
	}
	if (blockPos.z > 0 &&
		blocks[blockPos.x][blockPos.y][blockPos.z - 1] == 1)
	{
		result |= BF_NEG_Z;
	}

	return result;
}

bool Chunk::setMeshDirty()
{
	bool wasDirty = isMeshDirty;
	isMeshDirty = true;
	return !wasDirty;
}

void Chunk::update()
{
	if (isMeshDirty)
	{
		reconstructMesh();
		SSE::ResourceManager::updateModel(chunkObject.getModel(), mesh.data(), meshSize, indicesOffset);
		isMeshDirty = false;
	}
}

glm::ivec2 Chunk::getChunkIndices()
{
	return chunkIndices;
}

u8 Chunk::getBlock(const glm::ivec3& blockPos)
{
	if (blockIsInChunk(blockPos))
	{
		return blocks[blockPos.x][blockPos.y][blockPos.z];
	}
	else
	{
		return world->getBlock(chunkBlockPosToWorldBlockPos(blockPos));
	}
}

void Chunk::deleteBlock(const glm::ivec3& blockPos)
{
	setBlock(blockPos, 0);
}

void Chunk::setBlock(const glm::ivec3& blockPos, u8 block)
{
	if (isMeshDirty)
	{
		return;
	}

	isMeshDirty = blocks[blockPos.x][blockPos.y][blockPos.z] != block;
	if (isMeshDirty)
	{
		blockChange = std::make_pair<glm::vec3, bool>(blockPos, block == 0);
		lightChanges.push_back(blockPos);
		blocks[blockPos.x][blockPos.y][blockPos.z] = block;
	}
}

const SSE::Object& Chunk::getObject()
{
	return chunkObject;
}
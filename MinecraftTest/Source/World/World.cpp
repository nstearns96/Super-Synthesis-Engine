#include "World/World.h"

#include <set>

#include <Time/ScopedTimer.h>

#include <Graphics/Renderer.h>

inline glm::ivec2 blockPosToChunkIndices(const glm::ivec3& blockPos)
{
	return { (blockPos.x + ((blockPos.x < 0) ? (1 - (i32)CHUNK_X) : 0)) / (i32)CHUNK_X, (blockPos.y + ((blockPos.y < 0) ? (1 - (i32)CHUNK_Y) : 0)) / (i32)(CHUNK_Y) };
}

inline glm::ivec3 worldBlockPosToChunkBlockPos(const glm::ivec3& blockPos)
{
	glm::ivec2 chunkIndices = blockPosToChunkIndices(blockPos);
	return { blockPos.x - (i32)CHUNK_X * chunkIndices.x, blockPos.y - (i32)CHUNK_Y * chunkIndices.y, blockPos.z };
}

bool World::setLightLevel(glm::ivec3 blockPos, u8 lightLevel)
{
	glm::ivec2 chunkIndices = blockPosToChunkIndices(blockPos);
	const auto& chunk = chunks.find(chunkIndices);
	if (chunk != chunks.end())
	{
		blockPos -= glm::vec3((r32)CHUNK_X * chunkIndices.x, (r32)CHUNK_Y * chunkIndices.y, 0);
		return chunk->second->setLightLevel(blockPos, lightLevel);
	}

	return false;
}

u8 World::getLightLevel(glm::ivec3 blockPos)
{
	if (blockPos.z >= 0 && blockPos.z < CHUNK_Z)
	{
		glm::ivec2 chunkIndices = blockPosToChunkIndices(blockPos);
		const auto& chunk = chunks.find(chunkIndices);
		if (chunk != chunks.end())
		{
			blockPos -= glm::vec3((r32)CHUNK_X * chunkIndices.x, (r32)CHUNK_Y * chunkIndices.y, 0);
			return chunk->second->getLightLevel(blockPos);
		}
	}

	return 0xf;
}

bool World::generateChunk(glm::ivec2 indices)
{
	Chunk* newChunk = new Chunk;
	chunks[indices] = newChunk;
	chunks[indices]->create(this, indices);
	for (i32 x = -1; x < 2; ++x)
	{
		for (i32 y = -1; y < 2; ++y)
		{
			if (!(x == 0 && y == 0))
			{
				auto neighborChunk = chunks.find(indices + glm::ivec2(x, y));
				if (neighborChunk != chunks.end())
				{
					neighborChunk->second->constructMesh();
				}
			}
		}
	}
	return true;
}

Chunk* World::getChunk(const glm::ivec2& indices)
{
	auto chunk = chunks.find(indices);
	if (chunk != chunks.end())
	{
		return chunk->second;
	}

	return nullptr;
}

u8 World::getBlock(glm::ivec3 blockPos)
{
	if (blockPos.z >= 0 && blockPos.z < CHUNK_Z)
	{
		glm::ivec2 chunkIndices = blockPosToChunkIndices(blockPos);
		const auto& chunk = chunks.find(chunkIndices);
		if (chunk != chunks.end())
		{
			blockPos -= glm::vec3((r32)CHUNK_X * chunkIndices.x, (r32)CHUNK_Y * chunkIndices.y, 0);
			return chunk->second->getBlock(blockPos);
		}
	}

	return 0;
}

bool World::breakBlock(glm::ivec3 blockPos)
{
	glm::ivec2 chunkIndices = blockPosToChunkIndices(blockPos);
	auto chunk = chunks.find(chunkIndices);
	if (chunk != chunks.end())
	{
		for (i32 x = -1; x < 2; ++x)
		{
			for (i32 y = -1; y < 2; ++y)
			{
				glm::ivec3 chunkBlockPos = blockPos - glm::ivec3((i32)CHUNK_X * (chunkIndices.x + x), (i32)CHUNK_Y * (chunkIndices.y + y), 0);
				if (x != 0 || y != 0)
				{
					auto cascadeChunk = chunks.find(glm::vec2(chunkIndices.x + x, chunkIndices.y + y));
					if (cascadeChunk != chunks.end())
					{
						cascadeChunk->second->blockChange = { chunkBlockPos, true };
						cascadeChunk->second->lightChanges.push_back(chunkBlockPos);
						cascadeChunk->second->setMeshDirty();
					}
				}
				else
				{
					chunk->second->deleteBlock(chunkBlockPos);
				}
			}
		}
		return true;
	}

	return false;
}

bool World::setBlock(glm::ivec3 blockPos, u8 block)
{
	glm::ivec2 chunkIndices = blockPosToChunkIndices(blockPos);
	auto chunk = chunks.find(chunkIndices);
	if (chunk != chunks.end())
	{
		for (i32 x = -1; x < 2; ++x)
		{
			for (i32 y = -1; y < 2; ++y)
			{
				glm::ivec3 chunkBlockPos = blockPos - glm::ivec3((i32)CHUNK_X * (chunkIndices.x + x), (i32)CHUNK_Y * (chunkIndices.y + y), 0);
				if (x != 0 || y != 0)
				{
					auto cascadeChunk = chunks.find(glm::vec2(chunkIndices.x + x, chunkIndices.y + y));
					if (cascadeChunk != chunks.end())
					{
						cascadeChunk->second->blockChange = { chunkBlockPos, block == 0 };
						cascadeChunk->second->lightChanges.push_back(chunkBlockPos);
						cascadeChunk->second->setMeshDirty();
					}
				}
				else
				{
					chunk->second->setBlock(chunkBlockPos, block);
				}
			}
		}
		return true;
	}

	return false;
}

bool World::getFirstBlock(glm::vec3 rayOrigin, glm::vec3 rayDirection, r32 reach, glm::ivec3& outBlockPos, BlockFace& outBlockFace)
{
	glm::vec3 rayEnd = rayOrigin + reach * rayDirection;

	std::vector<r32> blocks;

	std::set<r32> dirTimes[3];

	for (i32 xIntercept = (i32)glm::floor(glm::min(rayOrigin.x, rayEnd.x)); xIntercept <= glm::ceil(glm::max(rayOrigin.x, rayEnd.x)); ++xIntercept)
	{
		r32 interceptTime = (xIntercept - rayOrigin.x) / rayDirection.x;
		if (interceptTime < reach && interceptTime > 0.0f)
		{
			dirTimes[0].insert(interceptTime);
		}
	}
	for (i32 yIntercept = (i32)glm::floor(glm::min(rayOrigin.y, rayEnd.y)); yIntercept <= glm::ceil(glm::max(rayOrigin.y, rayEnd.y)); ++yIntercept)
	{
		r32 interceptTime = (yIntercept - rayOrigin.y) / rayDirection.y;
		if (interceptTime < reach && interceptTime > 0.0f)
		{
			dirTimes[1].insert(interceptTime);
		}
	}
	for (i32 zIntercept = (i32)glm::floor(glm::min(rayOrigin.z, rayEnd.z)); zIntercept <= glm::ceil(glm::max(rayOrigin.z, rayEnd.z)); ++zIntercept)
	{
		r32 interceptTime = (zIntercept - rayOrigin.z) / rayDirection.z;
		if (interceptTime < reach && interceptTime > 0.0f)
		{
			dirTimes[2].insert(interceptTime);
		}
	}

	while (!dirTimes[0].empty() || !dirTimes[1].empty() || !dirTimes[2].empty())
	{
		i32 minIndex = -1;
		r32 minTime = reach;
		
		for (i32 dir = 0; dir < 3; ++dir)
		{
			if (!dirTimes[dir].empty() && *dirTimes[dir].begin() < minTime)
			{
				minIndex = dir;
				minTime = *dirTimes[dir].begin();
			}
		}

		if (minIndex == -1)
		{
			break;
		}

		glm::vec3 offset(0.0f);
		offset[minIndex] = rayOrigin[minIndex] > rayEnd[minIndex] ? -1.0f : 0.0f;
		glm::vec3 subVoxelPoint = rayOrigin + minTime * rayDirection + offset;
		glm::ivec3 voxelPoint = glm::ivec3(glm::floor(subVoxelPoint.x), glm::floor(subVoxelPoint.y), glm::floor(subVoxelPoint.z));
		if (getBlock(voxelPoint) == 0)
		{
			dirTimes[minIndex].erase(dirTimes[minIndex].begin());
		}
		else
		{
			outBlockPos = voxelPoint;
			switch (minIndex)
			{
			case 0:
			{
				outBlockFace = ((offset[0] == -1.0f) ? BF_POS_X : BF_NEG_X);
				break;
			}
			case 1:
			{
				outBlockFace = ((offset[1] == -1.0f) ? BF_POS_Y : BF_NEG_Y);
				break;
			}
			case 2:
			{
				outBlockFace = ((offset[2] == -1.0f) ? BF_POS_Z : BF_NEG_Z);
				break;
			}
			}
			return true;
		}
	}

	outBlockPos = glm::ivec3(0);
	outBlockFace = BF_POS_X;
	return false;
}

bool World::addLightNode(const glm::ivec3& blockPos)
{
	bool wasLightDirty = isLightDirty;
	lightNodes.push(blockPos);
	isLightDirty = true;
	return wasLightDirty;
}

void World::propogateLight()
{
	for (u8 lightLevel = 0xe; lightLevel != 0; --lightLevel)
	{
		for (st levelNodes = lightNodes.size(); levelNodes != 0; --levelNodes)
		{
			glm::ivec3 node = lightNodes.front();
			lightNodes.pop();

			i32 x = node.x;
			i32 y = node.y;
			i32 z = node.z;

			if (getBlock({ x - 1,y,z }) == 0 &&
				getLightLevel({ x - 1,y,z }) < lightLevel)
			{
				setLightLevel({ x - 1,y,z }, lightLevel);

				if (lightLevel != 1)
				{
					lightNodes.push({ x - 1, y, z });
				}
			}
			if (getBlock({ x + 1,y,z }) == 0 &&
				getLightLevel({ x + 1,y,z }) < lightLevel)
			{
				setLightLevel({ x + 1,y,z }, lightLevel);

				if (lightLevel != 1)
				{
					lightNodes.push({ x + 1, y, z });
				}
			}
			if (getBlock({ x,y - 1,z }) == 0 &&
				getLightLevel({ x,y - 1,z }) < lightLevel)
			{
				setLightLevel({ x,y - 1,z }, lightLevel);

				if (lightLevel != 1)
				{
					lightNodes.push({ x, y - 1, z });
				}
			}
			if (getBlock({ x,y + 1,z }) == 0 &&
				getLightLevel({ x,y + 1,z }) < lightLevel)
			{
				setLightLevel({ x,y + 1,z }, lightLevel);

				if (lightLevel != 1)
				{
					lightNodes.push({ x, y + 1, z });
				}
			}
			if (getBlock({ x,y,z - 1 }) == 0 &&
				getLightLevel({ x,y,z - 1 }) < lightLevel)
			{
				setLightLevel({ x,y,z - 1 }, lightLevel);

				if (lightLevel != 1)
				{
					lightNodes.push({ x, y, z - 1 });
				}
			}
			if (getBlock({ x,y,z + 1 }) == 0 &&
				getLightLevel({ x,y,z + 1 }) < lightLevel)
			{
				setLightLevel({ x,y,z + 1 }, lightLevel);

				if (lightLevel != 1)
				{
					lightNodes.push({ x, y, z + 1 });
				}
			}
		}
	}
}

void World::update()
{
	//This acts as a "propogate shadows"
	std::vector<glm::ivec3> lightChangePropogate;
	for (auto& chunk : chunks)
	{
		for (const auto& lightChange : chunk.second->lightChanges)
		{
			glm::ivec3 blockPos = chunk.second->chunkBlockPosToWorldBlockPos(lightChange);
			if (getBlock(blockPos + glm::ivec3({ 1,0,0 })) == 0 && getLightLevel(blockPos + glm::ivec3({ 1,0,0 })) != 0xf && std::find(lightChangePropogate.begin(), lightChangePropogate.end(), blockPos + glm::ivec3({ 1,0,0 })) == lightChangePropogate.end())
			{
				lightChangePropogate.push_back(blockPos + glm::ivec3({ 1,0,0 }));
			}
			if (getBlock(blockPos + glm::ivec3({ -1,0,0 })) == 0 && getLightLevel(blockPos + glm::ivec3({ -1,0,0 })) != 0xf && std::find(lightChangePropogate.begin(), lightChangePropogate.end(), blockPos + glm::ivec3({ -1,0,0 })) == lightChangePropogate.end())
			{
				lightChangePropogate.push_back(blockPos + glm::ivec3({ -1,0,0 }));
			}
			if (getBlock(blockPos + glm::ivec3({ 0,1,0 })) == 0 && getLightLevel(blockPos + glm::ivec3({ 0,1,0 })) != 0xf && std::find(lightChangePropogate.begin(), lightChangePropogate.end(), blockPos + glm::ivec3({ 0,1,0 })) == lightChangePropogate.end())
			{
				lightChangePropogate.push_back(blockPos + glm::ivec3({ 0,1,0 }));
			}
			if (getBlock(blockPos + glm::ivec3({ 0,-1,0 })) == 0 && getLightLevel(blockPos + glm::ivec3({ 0,-1,0 })) != 0xf && std::find(lightChangePropogate.begin(), lightChangePropogate.end(), blockPos + glm::ivec3({ 0,-1,0 })) == lightChangePropogate.end())
			{
				lightChangePropogate.push_back(blockPos + glm::ivec3({ 0,-1,0 }));
			}
			if (getBlock(blockPos + glm::ivec3({ 0,0,1 })) == 0 && getLightLevel(blockPos + glm::ivec3({ 0,0,1 })) != 0xf && std::find(lightChangePropogate.begin(), lightChangePropogate.end(), blockPos + glm::ivec3({ 0,0,1 })) == lightChangePropogate.end())
			{
				lightChangePropogate.push_back(blockPos + glm::ivec3({ 0,0,1 }));
			}
			if (getBlock(blockPos + glm::ivec3({ 0,0,-1 })) == 0 && std::find(lightChangePropogate.begin(), lightChangePropogate.end(), blockPos + glm::ivec3({ 0,0,-1 })) == lightChangePropogate.end())
			{
				lightChangePropogate.push_back(blockPos + glm::ivec3({ 0,0,-1 }));
			}
		}
	}

	for (st i = 0; i < lightChangePropogate.size(); ++i)
	{
		glm::ivec2 chunkIndices = blockPosToChunkIndices(lightChangePropogate[i]);
		auto chunk = chunks.find(chunkIndices);
		if (chunk != chunks.end())
		{
			glm::ivec3 blockPos = lightChangePropogate[i];
			for (i32 x = -1; x < 2; ++x)
			{
				for (i32 y = -1; y < 2; ++y)
				{
					auto cascadeChunk = chunks.find(glm::vec2(chunkIndices.x + x, chunkIndices.y + y));
					if (cascadeChunk != chunks.end())
					{
						cascadeChunk->second->lightChanges.push_back(blockPos - glm::ivec3((i32)CHUNK_X * (chunkIndices.x + x), (i32)CHUNK_Y * (chunkIndices.y + y), 0));
						cascadeChunk->second->setMeshDirty();
					}
				}
			}
			if (getBlock(blockPos + glm::ivec3({ 1,0,0 })) == 0 && getLightLevel(blockPos + glm::ivec3({ 1,0,0 })) != 0xf && std::find(lightChangePropogate.begin(), lightChangePropogate.end(), blockPos + glm::ivec3({ 1,0,0 })) == lightChangePropogate.end())
			{
				lightChangePropogate.push_back(blockPos + glm::ivec3({ 1,0,0 }));
			}
			if (getBlock(blockPos + glm::ivec3({ -1,0,0 })) == 0 && getLightLevel(blockPos + glm::ivec3({ -1,0,0 })) != 0xf && std::find(lightChangePropogate.begin(), lightChangePropogate.end(), blockPos + glm::ivec3({ -1,0,0 })) == lightChangePropogate.end())
			{
				lightChangePropogate.push_back(blockPos + glm::ivec3({ -1,0,0 }));
			}
			if (getBlock(blockPos + glm::ivec3({ 0,1,0 })) == 0 && getLightLevel(blockPos + glm::ivec3({ 0,1,0 })) != 0xf && std::find(lightChangePropogate.begin(), lightChangePropogate.end(), blockPos + glm::ivec3({ 0,1,0 })) == lightChangePropogate.end())
			{
				lightChangePropogate.push_back(blockPos + glm::ivec3({ 0,1,0 }));
			}
			if (getBlock(blockPos + glm::ivec3({ 0,-1,0 })) == 0 && getLightLevel(blockPos + glm::ivec3({ 0,-1,0 })) != 0xf && std::find(lightChangePropogate.begin(), lightChangePropogate.end(), blockPos + glm::ivec3({ 0,-1,0 })) == lightChangePropogate.end())
			{
				lightChangePropogate.push_back(blockPos + glm::ivec3({ 0,-1,0 }));
			}
			if (getBlock(blockPos + glm::ivec3({ 0,0,1 })) == 0 && getLightLevel(blockPos + glm::ivec3({ 0,0,1 })) != 0xf && std::find(lightChangePropogate.begin(), lightChangePropogate.end(), blockPos + glm::ivec3({ 0,0,1 })) == lightChangePropogate.end())
			{
				lightChangePropogate.push_back(blockPos + glm::ivec3({ 0,0,1 }));
			}
			if (getBlock(blockPos + glm::ivec3({ 0,0,-1 })) == 0 && std::find(lightChangePropogate.begin(), lightChangePropogate.end(), blockPos + glm::ivec3({ 0,0,-1 })) == lightChangePropogate.end())
			{
				lightChangePropogate.push_back(blockPos + glm::ivec3({ 0,0,-1 }));
			}
		}
	}

	for (auto chunk : chunks)
	{
		chunk.second->propogateLight();
	}

	propogateLight();

	for (auto chunk : chunks)
	{
		chunk.second->update();
	}
}

void World::destroy()
{
	for (auto chunk : chunks)
	{
		chunk.second->destroy();
		delete chunk.second;
	}

	chunks.clear();
}

void World::render()
{
	for (auto chunk : chunks)
	{
		SSE::Graphics::Renderer::gRenderer.renderObject(chunk.second->getObject());
	}
}
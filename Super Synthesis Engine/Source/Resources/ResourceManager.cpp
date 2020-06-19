#include "Resources/ResourceManager.h"

#include "Resources/Assets/TextureAssetUtils.h"
#include "Logging/Logger.h"

namespace SSE
{
	extern Logger gLogger;

	std::map<std::string, Graphics::Texture2D> ResourceManager::textures;

#pragma message("TODO: Handle failure to find texture")
	Graphics::Texture2D ResourceManager::getTexture(const std::string& name)
	{
		auto textureIter = textures.find(name);
		if (textureIter != textures.end())
		{
			return textureIter->second;
		}

		return {};
	}

#pragma message("TODO: Handle failure to load texture")
	Graphics::Texture2D ResourceManager::loadTexture(const std::string& name, const std::string& path)
	{
		Graphics::Texture2D result;

		glm::uvec2 dimensions;
		byte* imageData = Assets::TextureAssetUtils::loadTextureFromFile("Source\\Textures\\" + path, dimensions);
		if (imageData != nullptr)
		{
			result.create(imageData, dimensions, VK_FORMAT_B8G8R8A8_SRGB, VK_IMAGE_TILING_OPTIMAL);
			textures.emplace(name, result);
		}

		return result;
	}

	void ResourceManager::clear()
	{
		while(textures.size() != 0)
		{
			textures.begin()->second.destroy();
			textures.erase(textures.begin());
		}
	}
}
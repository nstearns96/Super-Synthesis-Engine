#include "Resources/ResourceManager.h"

#include "Logging/Logger.h"
#include "Resources/Assets/TextureAssetUtils.h"
#include "Resources/Assets/AudioAssetUtils.h"

#pragma message("TODO: Handle failure to load a resource")

namespace SSE
{
	extern Logger gLogger;

	std::map<std::string, Graphics::Texture2D> ResourceManager::textures;
	std::map<std::string, Graphics::Shader> ResourceManager::shaders;
	std::map<std::string, Audio::AudioSample> ResourceManager::audioSamples;

	Graphics::Texture2D ResourceManager::getTexture(const std::string& name)
	{
		auto textureIter = textures.find(name);
		if (textureIter != textures.end())
		{
			return textureIter->second;
		}

		return {};
	}

	Graphics::Texture2D ResourceManager::loadTexture(const std::string& name, const std::string& path)
	{
		Graphics::Texture2D result;

		glm::uvec2 dimensions;
		byte* imageData = Assets::TextureAssetUtils::loadTextureFromFile("Source\\Textures\\" + path, dimensions);
		if (imageData != nullptr && 
			result.create(imageData, dimensions, VK_FORMAT_B8G8R8A8_SRGB, VK_IMAGE_TILING_OPTIMAL))
		{
			textures.emplace(name, result);
		}

		return result;
	}

	Graphics::Shader ResourceManager::getShader(const std::string& name)
	{
		auto shaderIter = shaders.find(name);
		if (shaderIter != shaders.end())
		{
			return shaderIter->second;
		}

		return {};
	}

	Graphics::Shader ResourceManager::loadShader(const std::string& name, const std::vector<std::string>& sourceFiles, const std::vector<SSE::Vulkan::ShaderModuleType>& moduleTypes)
	{
		Graphics::Shader result;

		if (result.create(sourceFiles, moduleTypes))
		{
			shaders.emplace(name, result );
		}

		return result;
	}

	Audio::AudioSample ResourceManager::getAudio(const std::string& name)
	{
		auto audioIter = audioSamples.find(name);
		if (audioIter != audioSamples.end())
		{
			return audioIter->second;
		}
		else
		{
			return {};
		}
	}

	Audio::AudioSample ResourceManager::loadAudio(const std::string& path, const std::string& name)
	{
		Audio::AudioSample result = {};

		st dataSize;
		byte* data = Assets::AudioAssetUtils::loadAudioFromFile("Source\\Audio\\" + path, dataSize);

		if (data != nullptr && result.create(data, dataSize))
		{
			audioSamples[name] = result;
		}

		return result;
	}


	void ResourceManager::clear()
	{
		for (auto& texture : textures)
		{
			texture.second.destroy();
		}

		textures.clear();

		for (auto& shader : shaders)
		{
			shader.second.destroy();
		}

		shaders.clear();

		for (auto& audioSample : audioSamples)
		{
			audioSample.second.destroy();
		}

		audioSamples.clear();
	}
}
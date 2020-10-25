#include "Resources/ResourceManager.h"

#include "Logging/Logger.h"
#include "Resources/Assets/TextureAssetUtils.h"
#include "Resources/Assets/AudioAssetUtils.h"
#include "Resources/Assets/ModelAssetUtils.h"
#include "Graphics/Bitmap.h"
#include "Graphics/Font.h"

#pragma message("TODO: Handle failure to load a resource")
#pragma message("TODO: Handle failure to find a resource")

namespace SSE
{
	extern Logger gLogger;

	std::map<std::string, Graphics::Texture2D> ResourceManager::textures;
	std::map<std::string, Graphics::Shader> ResourceManager::shaders;
	std::map<std::string, Audio::AudioSample> ResourceManager::audioSamples;
	std::map<std::string, Graphics::Font> ResourceManager::fonts;
	std::map<std::string, Model> ResourceManager::models;

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
		Graphics::Texture2D result = {};

		glm::uvec2 dimensions;
		const byte* imageData = Assets::TextureAssetUtils::loadTextureFromFile("Source\\Textures\\" + path, dimensions);
		Graphics::Bitmap bitmap;
		bitmap.create(imageData, dimensions, VK_FORMAT_B8G8R8A8_UINT);
		if (imageData != nullptr && 
			result.create(bitmap, VK_IMAGE_TILING_OPTIMAL))
		{
			textures.emplace(name, result);
		}

		bitmap.destroy();
		return result;
	}

	Graphics::Texture2D ResourceManager::createTexture(const std::string& name, const Graphics::Texture2D& texture)
	{
		textures.emplace(name, texture);
		return textures.at(name);
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

	Graphics::Font ResourceManager::getFont(const std::string& name)
	{
		auto fontIter = fonts.find(name);
		if (fontIter != fonts.end())
		{
			return fontIter->second;
		}
		else
		{
			return {};
		}
	}

	Graphics::Font ResourceManager::loadFont(const std::string& path, const std::string& name)
	{
		Graphics::Font result = {};

		FileHandle fh;
		if (fh.create("Source\\Fonts\\" + path, FIOM_BINARY | FIOM_READ))
		{
			std::vector<byte> fontData = fh.readIntoVector();
			byte* data = new byte[fontData.size()];
			memcpy(data, fontData.data(), fontData.size());

			if (data != nullptr && result.create(data))
			{
				fonts[name] = result;
			}
		}

		return result;
	}

	Model ResourceManager::getModel(const std::string& name)
	{
		auto modelIter = models.find(name);
		if (modelIter != models.end())
		{
			return modelIter->second;
		}
		else
		{
			return {};
		}
	}

	Model ResourceManager::loadModel(const std::string& name, const std::string& path)
	{
		Model* result = Assets::ModelAssetUtils::loadModelFromFile("Source\\Models\\" + path);
		if (result != nullptr)
		{
			models.emplace(name, *result);
			return *result;
		}
		else
		{
			return {};
		}
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

		for (auto& font : fonts)
		{
			font.second.destroy();
		}

		fonts.clear();

		for (auto& model : models)
		{
			model.second.destroy();
		}

		models.clear();
	}
}
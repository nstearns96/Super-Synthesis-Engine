#include "Resources/ResourceManager.h"

#include "Graphics/Bitmap.h"
#include "Graphics/Font.h"

#include "Logging/Logger.h"

#include "Resources/Assets/AudioAssetUtils.h"
#include "Resources/Assets/ModelAssetUtils.h"
#include "Resources/Assets/TextureAssetUtils.h"

#pragma message("TODO: Handle failure to load a resource")
#pragma message("TODO: Handle failure to find a resource")

namespace SSE
{
	extern Logger gLogger;

	std::vector<Graphics::Texture2D> ResourceManager::textures(1024);
	std::vector<Graphics::Shader> ResourceManager::shaders(1024);
	std::vector<Audio::AudioSample> ResourceManager::audioSamples(1024);
	std::vector<Graphics::Font> ResourceManager::fonts(1024);
	std::vector<Model> ResourceManager::models(1024);
	ResourceHandleManager ResourceManager::textureHandles;
	ResourceHandleManager ResourceManager::shaderHandles;
	ResourceHandleManager ResourceManager::audioSampleHandles;
	ResourceHandleManager ResourceManager::fontHandles;
	ResourceHandleManager ResourceManager::modelHandles;

	const Graphics::Texture2D* ResourceManager::getTexture(ResourceHandle resourceHandle)
	{
		if (textureHandles.isHandleActive(resourceHandle))
		{
			return &textures[resourceHandle];
		}
		
		return nullptr;
	}

	ResourceHandle ResourceManager::loadTexture(const std::string& path)
	{
		glm::uvec2 dimensions;
		const byte* imageData = Assets::TextureAssetUtils::loadTextureFromFile("Source\\Textures\\" + path, dimensions);
		Graphics::Bitmap bitmap;
		bitmap.create(imageData, dimensions, VK_FORMAT_B8G8R8A8_UINT);
		Graphics::Texture2D newTexture;
		if (imageData != nullptr && 
			newTexture.create(bitmap, VK_IMAGE_TILING_OPTIMAL))
		{
			ResourceHandle resourceHandle = textureHandles.getHandle();
			textures[resourceHandle] = newTexture;
			return resourceHandle;
		}

		bitmap.destroy();
		return INVALID_HANDLE;
	}

	ResourceHandle ResourceManager::createTexture(const Graphics::Texture2D& texture)
	{
		ResourceHandle resourceHandle = textureHandles.getHandle();
		textures[resourceHandle] = texture;
		return resourceHandle;
	}

	const Graphics::Shader* ResourceManager::getShader(ResourceHandle resourceHandle)
	{
		if (shaderHandles.isHandleActive(resourceHandle))
		{
			return &shaders[resourceHandle];
		}

		return nullptr;
	}

	ResourceHandle ResourceManager::loadShader(const std::vector<std::string>& sourceFiles, const std::vector<Graphics::ShaderModuleType>& moduleTypes)
	{
		Graphics::Shader newShader;
		if (newShader.create(sourceFiles, moduleTypes))
		{
			ResourceHandle resourceHandle = shaderHandles.getHandle();
			shaders[resourceHandle] = newShader;
			return resourceHandle;
		}

		return INVALID_HANDLE;
	}

	const Audio::AudioSample* ResourceManager::getAudio(ResourceHandle resourceHandle)
	{
		if (audioSampleHandles.isHandleActive(resourceHandle))
		{
			return &audioSamples[resourceHandle];
		}

		return nullptr;
	}

	ResourceHandle ResourceManager::loadAudio(const std::string& path)
	{
		st dataSize;
		byte* data = Assets::AudioAssetUtils::loadAudioFromFile("Source\\Audio\\" + path, dataSize);

		Audio::AudioSample newAudio;
		if (data != nullptr && newAudio.create(data, dataSize))
		{
			ResourceHandle resourceHandle = audioSampleHandles.getHandle();
			audioSamples[resourceHandle] = newAudio;
			return resourceHandle;
		}

		return INVALID_HANDLE;
	}

	const Graphics::Font* ResourceManager::getFont(ResourceHandle resourceHandle)
	{
		if (fontHandles.isHandleActive(resourceHandle))
		{
			return &fonts[resourceHandle];
		}

		return nullptr;
	}

	ResourceHandle ResourceManager::loadFont(const std::string& path)
	{
		FileHandle fh;
		if (fh.create("Source\\Fonts\\" + path, FIOM_BINARY | FIOM_READ))
		{
			std::vector<byte> fontData = fh.readIntoVector();
			byte* data = new byte[fontData.size()];
			memcpy(data, fontData.data(), fontData.size());

			Graphics::Font newFont;
			if (data != nullptr && newFont.create(data))
			{
				ResourceHandle resourceHandle = shaderHandles.getHandle();
				fonts[resourceHandle] = newFont;
				return resourceHandle;
			}
		}

		return INVALID_HANDLE;
	}

	const Model* ResourceManager::getModel(ResourceHandle resourceHandle)
	{
		if (modelHandles.isHandleActive(resourceHandle))
		{
			return &models[resourceHandle];
		}

		return nullptr;
	}

	ResourceHandle ResourceManager::loadModel(const std::string& path)
	{
		Model newModel;
		if (Assets::ModelAssetUtils::loadModelFromFile(newModel, "Source\\Models\\" + path))
		{
			ResourceHandle resourceHandle = modelHandles.getHandle();
			models[resourceHandle] = newModel;
			return resourceHandle;
		}

		return INVALID_HANDLE;
	}

	void ResourceManager::clear()
	{
		for (auto& texture : textures)
		{
			texture.destroy();
		}
		
		for (auto& shader : shaders)
		{
			shader.destroy();
		}

		for (auto& audioSample : audioSamples)
		{
			audioSample.destroy();
		}

		for (auto& font : fonts)
		{
			font.destroy();
		}

		for (auto& model : models)
		{
			model.destroy();
		}

		textures.clear();
		shaders.clear();
		audioSamples.clear();
		fonts.clear();
		models.clear();
		textureHandles.clear();
		shaderHandles.clear();
		audioSampleHandles.clear();
		fontHandles.clear();
		modelHandles.clear();
	}
}
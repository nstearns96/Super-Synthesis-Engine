#ifndef _SSE_RESOURCE_MANAGER_H
#define _SSE_RESOURCE_MANAGER_H

#include "Audio/AudioSample.h"

#include "Graphics/Font.h"
#include "Graphics/Texture2D.h"

#include "Graphics/Shaders/Shader.h"

#include "Model/Model.h"

#include "Resources/ResourceHandleManager.h"

#pragma message("TODO: Add support for streamed audio resource")

namespace SSE
{
	class ResourceManager
	{
	private:
		ResourceManager() {};

		static std::vector<Graphics::Texture2D> textures;
		static ResourceHandleManager textureHandles;
		static std::vector<Graphics::Shader> shaders;
		static ResourceHandleManager shaderHandles;
		static std::vector<Audio::AudioSample> audioSamples;
		static ResourceHandleManager audioSampleHandles;
		static std::vector<Graphics::Font> fonts;
		static ResourceHandleManager fontHandles;
		static std::vector<Model> models;
		static ResourceHandleManager modelHandles;

	public:
		static const Graphics::Texture2D* getTexture(ResourceHandle resourceHandle);

		static ResourceHandle loadTexture(const std::string& path);
		static ResourceHandle createTexture(const Graphics::Texture2D& texture);

		static const Graphics::Shader* getShader(ResourceHandle resourceHandle);

		static ResourceHandle loadShader(const std::vector<std::string>& sourceFiles, const std::vector<Graphics::ShaderModuleType>& moduleTypes);
		
		static const Audio::AudioSample* getAudio(ResourceHandle resourceHandle);

		static ResourceHandle loadAudio(const std::string& path);

		static const Graphics::Font* getFont(ResourceHandle resourceHandle);

		static ResourceHandle loadFont(const std::string& path);

		static const Model* getModel(ResourceHandle resourceHandle);

		static ResourceHandle loadModel(const std::string& path);

		static void clear();
	};
}

#endif
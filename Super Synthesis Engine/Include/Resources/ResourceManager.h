#ifndef _SSE_RESOURCE_MANAGER_H
#define _SSE_RESOURCE_MANAGER_H

#include <map>

#include "Graphics/Texture2D.h"
#include "Graphics/Shader.h"
#include "Audio/AudioSample.h"
#include "Graphics/Font.h"
#include "Model/Model.h"

#pragma message("TODO: Add support for streamed audio resource")

namespace SSE
{
	class ResourceManager
	{
	private:
		ResourceManager() {};

		static std::map<std::string, Graphics::Texture2D> textures;
		static std::map<std::string, Graphics::Shader> shaders;
		static std::map<std::string, Audio::AudioSample> audioSamples;
		static std::map<std::string, Graphics::Font> fonts;
		static std::map<std::string, Model> models;

	public:
		static Graphics::Texture2D getTexture(const std::string& name);

		static Graphics::Texture2D loadTexture(const std::string& name, const std::string& path);
		static Graphics::Texture2D createTexture(const std::string& name, const Graphics::Texture2D& texture);

		static Graphics::Shader getShader(const std::string& name);

		static Graphics::Shader loadShader(const std::string& name, const std::vector<std::string>& sourceFiles, const std::vector<SSE::Vulkan::ShaderModuleType>& moduleTypes);
		
		static Audio::AudioSample getAudio(const std::string& name);

		static Audio::AudioSample loadAudio(const std::string& name, const std::string& path);

		static Graphics::Font getFont(const std::string& name);

		static Graphics::Font loadFont(const std::string& name, const std::string& path);

		static Model getModel(const std::string& name);

		static Model loadModel(const std::string& name, const std::string& path);

		static void clear();
	};
}

#endif
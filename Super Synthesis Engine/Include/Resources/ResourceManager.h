#ifndef _SSE_RESOURCE_MANAGER_H
#define _SSE_RESOURCE_MANAGER_H

#include <map>

#include "Graphics/Texture2D.h"
#include "Graphics/Shader.h"

namespace SSE
{
	class ResourceManager
	{
	private:
		ResourceManager() {};

		static std::map<std::string, Graphics::Texture2D> textures;

		static std::map<std::string, Graphics::Shader> shaders;

	public:
		static Graphics::Texture2D getTexture(const std::string& name);

		static Graphics::Texture2D loadTexture(const std::string& name, const std::string& path);

		static Graphics::Shader getShader(const std::string& name);

		static Graphics::Shader loadShader(const std::string& name, const std::vector<std::string>& sourceFiles, const std::vector<SSE::Vulkan::ShaderModuleType>& moduleTypes);

		static void clear();
	};
}

#endif
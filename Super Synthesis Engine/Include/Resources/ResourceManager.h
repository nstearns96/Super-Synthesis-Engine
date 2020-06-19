#ifndef _SSE_RESOURCE_MANAGER_H
#define _SSE_RESOURCE_MANAGER_H

#include <map>

#include "Graphics/Texture2D.h"

namespace SSE
{
	class ResourceManager
	{
	private:
		ResourceManager() {};

		static std::map<std::string, Graphics::Texture2D> textures;

	public:
		static Graphics::Texture2D getTexture(const std::string& name);

		static Graphics::Texture2D loadTexture(const std::string& name, const std::string& path);

		static void clear();
	};
}

#endif
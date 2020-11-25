#ifndef _SSE_RESOURCE_HANDLE_MANAGER_H
#define _SSE_RESOURCE_HANDLE_MANAGER_H

#include <vector>

#include "EngineTypeDefs.h"

namespace SSE
{
	class ResourceHandleManager
	{
	private:
		std::vector<ResourceHandle> handles;

	public:
		ResourceHandle getHandle();
		const std::vector<ResourceHandle>& getActiveHandles();
		bool freeHandle(ResourceHandle resourceHandle);
		bool isHandleActive(ResourceHandle resourceHandle);

		void clear();
	};
}

#endif
#include "Resources/ResourceHandleManager.h"

#include <glm/glm.hpp>

namespace SSE
{
	ResourceHandle ResourceHandleManager::getHandle()
	{
		if (handles.empty())
		{
			handles.push_back(0);
			return 0;
		}
		else if (*handles.rbegin() == handles.size() - 1)
		{
			handles.insert(handles.end(), handles.size());
			return handles.size() - 1;
		}
		else
		{
			const ResourceHandle* left = &handles[0];
			const ResourceHandle* right = &handles[handles.size()-1];
			u32 m = 0;

			while (left != right)
			{
				m = (u32)glm::ceil((r32)(((right - &handles[0]) + (left - &handles[0])) / 2.0f));
				if (handles[m] > m)
				{
					right = &handles[m - 1];
				}
				else
				{
					left = &handles[m];
				}
			}

			handles.insert(handles.begin() + m + 1, (*left) + 1);
			return m;
		}
	}

	const std::vector<ResourceHandle>& ResourceHandleManager::getActiveHandles()
	{
		return handles;
	}

	bool ResourceHandleManager::freeHandle(ResourceHandle resourceHandle)
	{
		if (handles.empty())
		{
			return false;
		}
		else
		{
			const ResourceHandle* left = &handles[0];
			const ResourceHandle* right = &handles[handles.size() - 1];
			u32 m = 0;

			while (left != right)
			{
				m = (u32)glm::ceil((r32)(((right - &handles[0]) + (left - &handles[0])) / 2.0f));
				if (handles[m] > resourceHandle)
				{
					right = &handles[m - 1];
				}
				else
				{
					left = &handles[m];
				}
			}

			if (*left == resourceHandle)
			{
				handles.erase(handles.begin() + m);
				return true;
			}
			else
			{
				return false;
			}
		}
	}

	bool ResourceHandleManager::isHandleActive(ResourceHandle resourceHandle)
	{
		if (handles.empty())
		{
			return false;
		}
		else
		{
			const ResourceHandle* left = &handles[0];
			const ResourceHandle* right = &handles[handles.size() - 1];
			u32 m = 0;

			while (left != right)
			{
				m = (u32)glm::ceil((r32)(((right - &handles[0]) + (left - &handles[0])) / 2.0f));
				if (handles[m] > resourceHandle)
				{
					right = &handles[m - 1];
				}
				else
				{
					left = &handles[m];
				}
			}

			return (*left == resourceHandle);
		}
	}

	void ResourceHandleManager::clear()
	{
		handles.clear();
	}
}
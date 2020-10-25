#include "Model/Model.h"

#include "Logging/Logger.h"

namespace SSE
{
	extern Logger gLogger;

	bool Model::create(void* data, st size, VertexFormat& format, const std::vector<u16>& _indices)
	{
		if (vertexData.create(data, size, format) &&
			vertexBuffer.create(vertexData, _indices))
		{
			indices = _indices;
			return true;
		}
		else
		{
			destroy();
			return false;
		}
	}

	void Model::destroy()
	{
		vertexData.destroy();
		vertexBuffer.destroy();
		indices.clear();
	}

	Vulkan::VulkanVertexBuffer Model::getVertexBuffer()
	{
		return vertexBuffer;
	}
	VertexData Model::getVertexData()
	{
		return vertexData;
	}
}
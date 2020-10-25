#ifndef _SSE_MODEL_ASSET_UTILS_H
#define _SSE_MODEL_ASSET_UTILS_H

#include <string>

#include "EngineTypeDefs.h"
#include "Model/Model.h"

namespace SSE::Assets::ModelAssetUtils
{
	Model* loadModelFromFile(const std::string& path);
}

#endif
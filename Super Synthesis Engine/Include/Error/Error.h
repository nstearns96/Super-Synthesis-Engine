//====================================================================================
// File: Error.h
// Description: Header file for window backend wrapping and engine-level window functions
//====================================================================================

#ifndef _SSE_ERROR_H
#define _SSE_ERROR_H

#include <string>

#include "EngineTypeDefs.h"

namespace SSE
{
	struct Error
	{
		std::string message;
		u32 level;
		u32 code;
	};
}

#endif
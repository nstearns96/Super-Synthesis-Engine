//====================================================================================
// File: Error.h
// Description: Header file for window backend wrapping and engine-level window functions
//====================================================================================

#ifndef _SSE_ERROR_H
#define _SSE_ERROR_H

#include <string>

namespace SSE
{
	struct Error
	{
		std::string message;
		unsigned int level;
		unsigned int code;
	};
}

#endif
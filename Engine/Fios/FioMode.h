#ifndef _SSE_FIO_MODE_H
#define _SSE_FIO_MODE_H

namespace SSE
{
	enum FioMode
	{
		FIOM_NONE = 0x0,
		FIOM_WRITE = 0x1,
		FIOM_READ = 0x2,
		FIOM_BINARY = 0x4,
		FIOM_OPEN = 0x10,
	};
}
#endif
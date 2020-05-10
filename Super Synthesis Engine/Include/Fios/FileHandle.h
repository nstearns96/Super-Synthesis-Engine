#ifndef _SSE_FILE_HANDLE_H
#define _SSE_fILE_HANDLE_H

#include <fstream>
#include <vector>
#include <string>

#include "FioMode.h"

namespace SSE
{
	class FileHandle
	{
	private:
		std::fstream handle;
		std::string filePath = "";
		unsigned int mode = FioMode::FIOM_NONE;

	public:
		FileHandle() {};
		FileHandle(const std::string& _filePath, unsigned int _mode);

		~FileHandle();

		bool open(const std::string& _filePath, unsigned int _mode);
		bool close();

		std::vector<char> readIntoVector();
		void writeToFile(const std::string& buffer);

		bool isValid() const;

	};
}

#endif
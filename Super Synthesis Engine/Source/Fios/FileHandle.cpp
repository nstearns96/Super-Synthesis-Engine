#include "Fios/FileHandle.h"

#include <cerrno>
#include <iostream>

namespace SSE
{
	FileHandle::FileHandle(const std::string& _filePath, bitfield _mode)
	{
		this->create(_filePath, _mode);
	}

	FileHandle::~FileHandle()
	{
		if (handle.is_open()) handle.close();
		filePath = "";
		mode = FioMode::FIOM_NONE;
	}

	bool FileHandle::destroy()
	{
		if (isValid())
		{
			handle.close();
			if (!handle.is_open())
			{
				filePath = "";
				mode = FioMode::FIOM_NONE;
				return true;
			}
			else
			{
				return false;
			}
		}
		else
		{
			return true;
		}
	}

	bool FileHandle::create(const std::string& _filePath, bitfield _mode)
	{
		bitfield fhMode = 0;
		if (_mode & FioMode::FIOM_READ)
		{
			fhMode |= std::fstream::in | std::fstream::ate;
		}
		else if (_mode & FioMode::FIOM_WRITE)
		{
			fhMode |= std::fstream::out;
		}
		else
		{
			return false;
		}

		if (this->destroy())
		{
			if (_mode & FioMode::FIOM_BINARY)
			{
				fhMode |= std::fstream::binary;
			}

			handle.open(_filePath, fhMode);
			if (handle.is_open())
			{
				filePath = _filePath;
				mode = (_mode | FioMode::FIOM_OPEN);

				return true;
			}
			else
			{
				return false;
			}
		}
		else
		{
			return false;
		}
	}

	std::vector<byte> FileHandle::readIntoVector()
	{
		std::vector<byte> result;
		if (isValid() && (mode & FioMode::FIOM_READ))
		{
			st fileSize = (st)handle.tellg();
			result.resize(fileSize);
			handle.seekg(0);
			handle.read((i8*)result.data(), fileSize);
		}

		destroy();

		return result;
	}

	void FileHandle::writeToFile(const std::string& buffer)
	{
		if (isValid() && (mode & FioMode::FIOM_WRITE))
		{
			handle.write(buffer.c_str(), buffer.size());
		}
	}

	bool FileHandle::isValid() const
	{
		return (handle.is_open() && (mode & FioMode::FIOM_OPEN) && (filePath.size()));
	}
}
//====================================================================================
// File: Logger.h
// Description: Header file for engine logging
//====================================================================================

#ifndef LOGGER_H
#define LOGGER_H

#include "FileHandle.h"
#include "Error.h"
#include "ErrorLevel.h"

namespace SSE
{
	class Logger
	{
	private:
		FileHandle outputFile;

		unsigned int logLevel;
		bool logConsole;

		static std::string getErrorLogPrefix(const Error& error);

	public:
		Logger();
		~Logger();

		void openLog(const std::string& logPath, ErrorLevel _logLevel = ErrorLevel::EL_ALL, bool _logConsole = true);
		void closeLog();

		void logError(const Error& error, bool console = true);
		void logError(ErrorLevel errorLevel, const std::string& message);
	};
}
#endif
//====================================================================================
// File: Logger.h
// Description: Header file for engine logging
//====================================================================================

#ifndef LOGGER_H
#define LOGGER_H

#include <string.h>

#include "Fios/FileHandle.h"
#include "Error/Error.h"
#include "Error/ErrorLevel.h"

#define ERROR_MESSAGE_END (std::string(" ") + __FILE__ + " (Line: " + std::to_string(__LINE__) + ")")
#define GLOG_ERROR(errorLevel, message) SSE::gLogger.logError( errorLevel , message + ERROR_MESSAGE_END )
#define GLOG_CRITICAL(message) GLOG_ERROR(ErrorLevel::EL_CRITICAL, std::string(message))
#define GLOG_WARNING(message) GLOG_ERROR(ErrorLevel::EL_WARNING, std::string(message))
#define GLOG_INFO(message) GLOG_ERROR(ErrorLevel::EL_INFO, std::string(message))


namespace SSE
{
	class Logger
	{
	private:
		FileHandle outputFile;

		u32 logLevel;
		bool logConsole;

		static std::string getErrorLogPrefix(const Error& error);

	public:
		Logger();
		~Logger();

		void openLog(const std::string& logPath, ErrorLevel _logLevel = ErrorLevel::EL_ALL, bool _logConsole = true);
		void closeLog();

		void logError(const Error& error);
		void logError(ErrorLevel errorLevel, const std::string& message);
	};
}
#endif
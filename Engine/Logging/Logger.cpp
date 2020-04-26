#include "Logger.h"

#include <assert.h>
#include <chrono>
#include <ctime>
#include <iostream>
#include <iomanip>
#include <sstream>

#include "FioMode.h"
#include "TimeUtils.h"

namespace SSE
{
	Logger gLogger;

	Logger::Logger()
	{
	}

	Logger::~Logger()
	{
	}

	void Logger::openLog(const std::string& logPath, ErrorLevel _logLevel, bool _logConsole)
	{
		assert(outputFile.open(logPath, FioMode::FIOM_WRITE));
		this->logLevel = _logLevel;
		this->logConsole = _logConsole;
	}

	void Logger::closeLog()
	{
		assert(outputFile.close());
	}

	void Logger::logError(const Error& error, bool console)
	{
		std::string logMessage =
			"[" + TimeUtils::getDate() + "]" + getErrorLogPrefix(error) + ": " + error.message + '\n';

		if ((logLevel != ErrorLevel::EL_NONE) && (error.level <= logLevel))
		{
			outputFile.writeToFile(logMessage);
		}

		if (logConsole)
		{
			std::cout << logMessage;
		}
	}

	void Logger::logError(ErrorLevel errorLevel, const std::string& message)
	{
		logError({ message, (unsigned int)errorLevel, NULL });
	}

	std::string Logger::getErrorLogPrefix(const Error& error)
	{
		switch (error.level)
		{
		case ErrorLevel::EL_CRITICAL:
		{
			return "[CRITICAL]";
		}
		case ErrorLevel::EL_WARNING:
		{
			return "[WARNING]";
		}
		default:
		{
			return "";
		}
		}
	}
}
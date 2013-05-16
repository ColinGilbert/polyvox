/*******************************************************************************
Copyright (c) 2005-2009 David Williams

This software is provided 'as-is', without any express or implied
warranty. In no event will the authors be held liable for any damages
arising from the use of this software.

Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute it
freely, subject to the following restrictions:

    1. The origin of this software must not be misrepresented; you must not
    claim that you wrote the original software. If you use this software
    in a product, an acknowledgment in the product documentation would be
    appreciated but is not required.

    2. Altered source versions must be plainly marked as such, and must not be
    misrepresented as being the original software.

    3. This notice may not be removed or altered from any source
    distribution. 	
*******************************************************************************/

#include "PolyVoxCore/Impl/ErrorHandling.h"

namespace PolyVox
{
	void defaultLogHandler(const std::string& message, LogLevel logLevel)
	{
		switch(logLevel)
		{
		case LogLevels::Debug:
			{
				// Debug messages are not output by this default log handler.
				// Provide a custom handler if you want to process them.
				break;
			}
		case LogLevels::Info:
			{
				std::cout << "Info:    " << message.c_str() << std::endl;
				break;
			}
		case LogLevels::Warning:
			{
				std::cerr << "Warning: " << message.c_str() << std::endl;
				break;
			}
		case LogLevels::Error:
			{
				std::cerr << "Error:   " << message.c_str() << std::endl;
				break;
			}
		case LogLevels::Fatal:
			{
				std::cerr << "Fatal:   " << message.c_str() << std::endl;
				break;
			}
		}
	}

	LogHandler& getLogHandlerInstance()
	{
		static LogHandler s_fLogHandler = &defaultLogHandler;
		return s_fLogHandler;
	}

	LogHandler getLogHandler()
	{
		return getLogHandlerInstance();
	}

	void setLogHandler(LogHandler fNewHandler)
	{
		getLogHandlerInstance() = fNewHandler;
	}

	void log(const std::string& message, LogLevel logLevel)
	{
		LogHandler logHandler = getLogHandler();
		if(logHandler)
		{
			logHandler(message, logLevel);
		}
	}

	// Some handy wrappers
	void logDebug(const std::string& message)
	{ 
		log(message, LogLevels::Debug  );
	}

	void logInfo(const std::string& message)
	{
		log(message, LogLevels::Info);
	}

	void logWarning(const std::string& message)
	{
		log(message, LogLevels::Warning);
	}

	void logError(const std::string& message)
	{
		log(message, LogLevels::Error);
	}

	void logFatal(const std::string& message)
	{
		log(message, LogLevels::Fatal);
	}

#ifndef POLYVOX_THROW_ENABLED
	void defaultThrowHandler(std::exception& e, const char* file, int line)
	{
		std::stringstream ss; \
		ss << std::endl << std::endl; \
		ss << "    PolyVox exception thrown!" << std::endl; \
		ss << "    =========================" << std::endl; \
		ss << "    PolyVox has tried to throw an exception but it was built without support" << std::endl; \
		ss << "    for exceptions. In this scenario PolyVox will call a 'throw handler'" << std::endl; \
		ss << "    and this message is being printed by the default throw handler." << std::endl << std::endl; \

		ss << "    If you don't want to enable exceptions then you should try to determine why" << std::endl; \
		ss << "    this exception was thrown and make sure it doesn't happen again. If it was" << std::endl; \
		ss << "    due to something like an invalid argument to a function then you should be" << std::endl; \
		ss << "    able to fix it quite easily by validating parameters as appropriate. More" << std::endl; \
		ss << "    complex exception scenarios (out of memory, etc) might be harder to fix and" << std::endl; \
		ss << "    you should replace this default handler with something which is more" << std::endl; \
		ss << "    meaningful to your users." << std::endl << std::endl; \

		ss << "    Exception details" << std::endl; \
		ss << "    -----------------" << std::endl; \
		ss << "    Line:    " << line << std::endl; \
		ss << "    File:    " << file << std::endl; \
		ss << "    Message: " << e.what() << std::endl << std::endl; \
		logFatal(ss.str()); \

		POLYVOX_HALT(); \
	}

	ThrowHandler& getThrowHandlerInstance()
	{
		static ThrowHandler s_fThrowHandler = &defaultThrowHandler;
		return s_fThrowHandler;
	}

	ThrowHandler getThrowHandler()
	{
		return getThrowHandlerInstance();
	}

	void setThrowHandler(ThrowHandler fNewHandler)
	{
		getThrowHandlerInstance() = fNewHandler;
	}
#endif
}

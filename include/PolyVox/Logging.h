/*******************************************************************************
Copyright (c) 2005-2009 David Williams and Matthew Williams

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

#ifndef __PolyVox_Logging_H__
#define __PolyVox_Logging_H__

#include <iostream>
#include <string>

// We expose the logger class to the user so that they can provide custom implementations
// to redirect PolyVox's log messages. However, it is not expected that user code will make 
// use of PolyVox's logging macros s these are part of the private implementation.
namespace PolyVox
{
	class Logger
	{
	public:
		Logger() {};
		virtual ~Logger() {};

		virtual void logTraceMessage(const std::string& message) = 0;
		virtual void logDebugMessage(const std::string& message) = 0;
		virtual void logInfoMessage(const std::string& message) = 0;
		virtual void logWarningMessage(const std::string& message) = 0;
		virtual void logErrorMessage(const std::string& message) = 0;
		virtual void logFatalMessage(const std::string& message) = 0;
	};

	class DefaultLogger : public Logger
	{
	public:
		DefaultLogger() : Logger() {}
		virtual ~DefaultLogger() {}

		// Appending the 'std::endl' forces the stream to be flushed.
		void logTraceMessage(const std::string& /*message*/) {  }
		void logDebugMessage(const std::string& /*message*/) {  }
		void logInfoMessage(const std::string& message) { std::cout << message << std::endl; }
		void logWarningMessage(const std::string& message) { std::cerr << message << std::endl; }
		void logErrorMessage(const std::string& message) { std::cerr << message << std::endl; }
		void logFatalMessage(const std::string& message) { std::cerr << message << std::endl; }
	};

	inline Logger*& getLoggerInstance()
	{
		static Logger* s_pLogger = new DefaultLogger;
		return s_pLogger;
	}

	inline void setLoggerInstance(Logger* pLogger)
	{
		getLoggerInstance() = pLogger;
	}
}

#endif //__PolyVox_Logging_H__

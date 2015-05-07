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

#include "PolyVox/Impl/Config.h"

#include <iostream>
#include <sstream>

/*
 * Logging
 * --------
 * PolyVox provides basic logging facilities which can be redirected by your application.
 */

namespace PolyVox
{
	class Logger
	{
	public:
		// Passing zero to the null stream constructor guarentees it will discard all input. See
		// here http://stackoverflow.com/a/8244052 and here http://stackoverflow.com/a/6240980
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

	namespace Impl
	{
		inline Logger*& getLoggerInstance()
		{
			static Logger* s_pLogger = new DefaultLogger;
			return s_pLogger;
		}
	}

	inline void setLogger(Logger* pLogger)
	{
		Impl::getLoggerInstance() = pLogger;
	}

	namespace Impl
	{
		// Used for building the log messages - convert a list of variables into a string.
		// Based on second approach here: http://stackoverflow.com/a/25386444/2337254
		template< typename ... Args >
		std::string argListToString(const Args& ... args)
		{
			std::ostringstream oss;
			int a[] = { 0, ((void)(oss << args), 0) ... };
			(void)a; // Prevent warnings about unused param
			return oss.str();
		}

		// Log trace message
		template< typename ... Args >
		void logTraceMessage(Args const& ... messageArgs)
		{
			std::string message = argListToString(messageArgs...);
			PolyVox::Impl::getLoggerInstance()->logTraceMessage(message);
		}

		template< typename ... Args >
		void logTraceMessageIf(bool condition, Args const& ... messageArgs)
		{
			if (condition) { logTraceMessage(messageArgs...); }
		}

		// Log debug message
		template< typename ... Args >
		void logDebugMessage(Args const& ... messageArgs)
		{
			std::string message = argListToString(messageArgs...);
			PolyVox::Impl::getLoggerInstance()->logDebugMessage(message);
		}

		template< typename ... Args >
		void logDebugMessageIf(bool condition, Args const& ... messageArgs)
		{
			if (condition) { logDebugMessage(messageArgs...); }
		}

		// Log info message
		template< typename ... Args >
		void logInfoMessage(Args const& ... messageArgs)
		{
			std::string message = argListToString(messageArgs...);
			PolyVox::Impl::getLoggerInstance()->logInfoMessage(message);
		}

		template< typename ... Args >
		void logInfoMessageIf(bool condition, Args const& ... messageArgs)
		{
			if (condition) { logInfoMessage(messageArgs...); }
		}

		// Log warning message
		template< typename ... Args >
		void logWarningMessage(Args const& ... messageArgs)
		{
			std::string message = argListToString(messageArgs...);
			PolyVox::Impl::getLoggerInstance()->logWarningMessage(message);
		}

		template< typename ... Args >
		void logWarningMessageIf(bool condition, Args const& ... messageArgs)
		{
			if (condition) { logWarningMessage(messageArgs...); }
		}

		// Log error message
		template< typename ... Args >
		void logErrorMessage(Args const& ... messageArgs)
		{
			std::string message = argListToString(messageArgs...);
			PolyVox::Impl::getLoggerInstance()->logErrorMessage(message);
		}

		template< typename ... Args >
		void logErrorMessageIf(bool condition, Args const& ... messageArgs)
		{
			if (condition) { logErrorMessage(messageArgs...); }
		}

		// Log fatal message
		template< typename ... Args >
		void logFatalMessage(Args const& ... messageArgs)
		{
			std::string message = argListToString(messageArgs...);
			PolyVox::Impl::getLoggerInstance()->logFatalMessage(message);
		}

		template< typename ... Args >
		void logFatalMessageIf(bool condition, Args const& ... messageArgs)
		{
			if (condition) { logFatalMessage(messageArgs...); }
		}
	}
}

#ifdef POLYVOX_LOG_TRACE_ENABLED
#define POLYVOX_LOG_TRACE(...) PolyVox::Impl::logTraceMessage(__VA_ARGS__)
	#define POLYVOX_LOG_TRACE_IF(condition, ...) PolyVox::Impl::logTraceMessageIf(condition, __VA_ARGS__)	
#else
	#define POLYVOX_LOG_TRACE(...)
	#define POLYVOX_LOG_TRACE_IF(condition, ...)
#endif

#ifdef POLYVOX_LOG_DEBUG_ENABLED
#define POLYVOX_LOG_DEBUG(...) PolyVox::Impl::logDebugMessage(__VA_ARGS__)
#define POLYVOX_LOG_DEBUG_IF(condition, ...) PolyVox::Impl::logDebugMessageIf(condition, __VA_ARGS__)	
#else
	#define POLYVOX_LOG_DEBUG(...)
	#define POLYVOX_LOG_DEBUG_IF(condition, ...)
#endif

#ifdef POLYVOX_LOG_INFO_ENABLED
#define POLYVOX_LOG_INFO(...) PolyVox::Impl::logInfoMessage(__VA_ARGS__)
#define POLYVOX_LOG_INFO_IF(condition, ...) PolyVox::Impl::logInfoMessageIf(condition, __VA_ARGS__)	
#else
	#define POLYVOX_LOG_INFO(...)
	#define POLYVOX_LOG_INFO_IF(condition, ...)
#endif

#ifdef POLYVOX_LOG_WARNING_ENABLED
#define POLYVOX_LOG_WARNING(...) PolyVox::Impl::logWarningMessage(__VA_ARGS__)
#define POLYVOX_LOG_WARNING_IF(condition, ...) PolyVox::Impl::logWarningMessageIf(condition, __VA_ARGS__)	
#else
	#define POLYVOX_LOG_WARNING(...)
	#define POLYVOX_LOG_WARNING_IF(condition, ...)
#endif

#ifdef POLYVOX_LOG_ERROR_ENABLED
#define POLYVOX_LOG_ERROR(...) PolyVox::Impl::logErrorMessage(__VA_ARGS__)
#define POLYVOX_LOG_ERROR_IF(condition, ...) PolyVox::Impl::logErrorMessageIf(condition, __VA_ARGS__)	
#else
	#define POLYVOX_LOG_ERROR(...)
	#define POLYVOX_LOG_ERROR_IF(condition, ...)
#endif

#ifdef POLYVOX_LOG_FATAL_ENABLED
#define POLYVOX_LOG_FATAL(...) PolyVox::Impl::logFatalMessage(__VA_ARGS__)
#define POLYVOX_LOG_FATAL_IF(condition, ...) PolyVox::Impl::logFatalMessageIf(condition, __VA_ARGS__)	
#else
	#define POLYVOX_LOG_FATAL(...)
	#define POLYVOX_LOG_FATAL_IF(condition, ...)
#endif

#endif //__PolyVox_Logging_H__
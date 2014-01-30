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

#include "PolyVoxCore/Impl/Config.h"

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
		Logger*& getLoggerInstance();
	}

	void setLogger(Logger* pLogger);
}

#ifdef POLYVOX_LOG_TRACE_ENABLED

	#define POLYVOX_LOG_TRACE(message) \
		/* We use the do...while(0) construct in our macros (for reasons see here: http://stackoverflow.com/a/154138) \
		   but Visual Studio gives unhelpful 'conditional expression is constant' warnings. The recommended solution \
		   (http://stackoverflow.com/a/1946485) is to disable these warnings. */ \
		POLYVOX_MSC_WARNING_PUSH \
		POLYVOX_DISABLE_MSC_WARNING(4127) \
		do \
		{ \
			std::stringstream ss; \
			ss << message; \
			PolyVox::Impl::getLoggerInstance()->logTraceMessage(ss.str()); \
		} while(0) \
		POLYVOX_MSC_WARNING_POP

	#define POLYVOX_LOG_TRACE_IF(condition, message) \
		/* We use the do...while(0) construct in our macros (for reasons see here: http://stackoverflow.com/a/154138) \
		   but Visual Studio gives unhelpful 'conditional expression is constant' warnings. The recommended solution \
		   (http://stackoverflow.com/a/1946485) is to disable these warnings. */ \
		POLYVOX_MSC_WARNING_PUSH \
		POLYVOX_DISABLE_MSC_WARNING(4127) \
		do \
		{ \
			if ((condition)) \
			{ \
				std::stringstream ss; \
				ss << message; \
				PolyVox::Impl::getLoggerInstance()->logTraceMessage(ss.str()); \
			} \
		} while(0) \
		POLYVOX_MSC_WARNING_POP

#else

	// We don't bother with the do...while(0) construct here as we're not executing multiple statements
	// We also don't bother with forcing variables to be 'used'. If this causes a problem then calling
	// code can just force them to be used itself in addition to calling the logging macro. Basically 
	// we just want to reduce the chance of extra code being generated.
	#define POLYVOX_LOG_TRACE(message)
	#define POLYVOX_LOG_TRACE_IF(condition, message)

#endif

#ifdef POLYVOX_LOG_DEBUG_ENABLED

	#define POLYVOX_LOG_DEBUG(message) \
		/* We use the do...while(0) construct in our macros (for reasons see here: http://stackoverflow.com/a/154138) \
		   but Visual Studio gives unhelpful 'conditional expression is constant' warnings. The recommended solution \
		   (http://stackoverflow.com/a/1946485) is to disable these warnings. */ \
		POLYVOX_MSC_WARNING_PUSH \
		POLYVOX_DISABLE_MSC_WARNING(4127) \
		do \
		{ \
			std::stringstream ss; \
			ss << message; \
			PolyVox::Impl::getLoggerInstance()->logDebugMessage(ss.str()); \
		} while(0) \
		POLYVOX_MSC_WARNING_POP

	#define POLYVOX_LOG_DEBUG_IF(condition, message) \
		/* We use the do...while(0) construct in our macros (for reasons see here: http://stackoverflow.com/a/154138) \
		   but Visual Studio gives unhelpful 'conditional expression is constant' warnings. The recommended solution \
		   (http://stackoverflow.com/a/1946485) is to disable these warnings. */ \
		POLYVOX_MSC_WARNING_PUSH \
		POLYVOX_DISABLE_MSC_WARNING(4127) \
		do \
		{ \
			if ((condition)) \
			{ \
				std::stringstream ss; \
				ss << message; \
				PolyVox::Impl::getLoggerInstance()->logDebugMessage(ss.str()); \
			} \
		} while(0) \
		POLYVOX_MSC_WARNING_POP

#else

	// We don't bother with the do...while(0) construct here as we're not executing multiple statements
	// We also don't bother with forcing variables to be 'used'. If this causes a problem then calling
	// code can just force them to be used itself in addition to calling the logging macro. Basically 
	// we just want to reduce the chance of extra code being generated.
	#define POLYVOX_LOG_DEBUG(message)
	#define POLYVOX_LOG_DEBUG_IF(condition, message)

#endif

#ifdef POLYVOX_LOG_INFO_ENABLED

	#define POLYVOX_LOG_INFO(message) \
		/* We use the do...while(0) construct in our macros (for reasons see here: http://stackoverflow.com/a/154138) \
		   but Visual Studio gives unhelpful 'conditional expression is constant' warnings. The recommended solution \
		   (http://stackoverflow.com/a/1946485) is to disable these warnings. */ \
		POLYVOX_MSC_WARNING_PUSH \
		POLYVOX_DISABLE_MSC_WARNING(4127) \
		do \
		{ \
			std::stringstream ss; \
			ss << message; \
			PolyVox::Impl::getLoggerInstance()->logInfoMessage(ss.str()); \
		} while(0) \
		POLYVOX_MSC_WARNING_POP

	#define POLYVOX_LOG_INFO_IF(condition, message) \
		/* We use the do...while(0) construct in our macros (for reasons see here: http://stackoverflow.com/a/154138) \
		   but Visual Studio gives unhelpful 'conditional expression is constant' warnings. The recommended solution \
		   (http://stackoverflow.com/a/1946485) is to disable these warnings. */ \
		POLYVOX_MSC_WARNING_PUSH \
		POLYVOX_DISABLE_MSC_WARNING(4127) \
		do \
		{ \
			if ((condition)) \
			{ \
				std::stringstream ss; \
				ss << message; \
				PolyVox::Impl::getLoggerInstance()->logInfoMessage(ss.str()); \
			} \
		} while(0) \
		POLYVOX_MSC_WARNING_POP

#else

	// We don't bother with the do...while(0) construct here as we're not executing multiple statements
	// We also don't bother with forcing variables to be 'used'. If this causes a problem then calling
	// code can just force them to be used itself in addition to calling the logging macro. Basically 
	// we just want to reduce the chance of extra code being generated.
	#define POLYVOX_LOG_INFO(message)
	#define POLYVOX_LOG_INFO_IF(condition, message)

#endif

#ifdef POLYVOX_LOG_WARNING_ENABLED

	#define POLYVOX_LOG_WARNING(message) \
		/* We use the do...while(0) construct in our macros (for reasons see here: http://stackoverflow.com/a/154138) \
		   but Visual Studio gives unhelpful 'conditional expression is constant' warnings. The recommended solution \
		   (http://stackoverflow.com/a/1946485) is to disable these warnings. */ \
		POLYVOX_MSC_WARNING_PUSH \
		POLYVOX_DISABLE_MSC_WARNING(4127) \
		do \
		{ \
			std::stringstream ss; \
			ss << message; \
			PolyVox::Impl::getLoggerInstance()->logWarningMessage(ss.str()); \
		} while(0) \
		POLYVOX_MSC_WARNING_POP

	#define POLYVOX_LOG_WARNING_IF(condition, message) \
		/* We use the do...while(0) construct in our macros (for reasons see here: http://stackoverflow.com/a/154138) \
		   but Visual Studio gives unhelpful 'conditional expression is constant' warnings. The recommended solution \
		   (http://stackoverflow.com/a/1946485) is to disable these warnings. */ \
		POLYVOX_MSC_WARNING_PUSH \
		POLYVOX_DISABLE_MSC_WARNING(4127) \
		do \
		{ \
			if ((condition)) \
			{ \
				std::stringstream ss; \
				ss << message; \
				PolyVox::Impl::getLoggerInstance()->logWarningMessage(ss.str()); \
			} \
		} while(0) \
		POLYVOX_MSC_WARNING_POP

#else

	// We don't bother with the do...while(0) construct here as we're not executing multiple statements
	// We also don't bother with forcing variables to be 'used'. If this causes a problem then calling
	// code can just force them to be used itself in addition to calling the logging macro. Basically 
	// we just want to reduce the chance of extra code being generated.
	#define POLYVOX_LOG_WARNING(message)
	#define POLYVOX_LOG_WARNING_IF(condition, message)

#endif

#ifdef POLYVOX_LOG_ERROR_ENABLED

	#define POLYVOX_LOG_ERROR(message) \
		/* We use the do...while(0) construct in our macros (for reasons see here: http://stackoverflow.com/a/154138) \
		   but Visual Studio gives unhelpful 'conditional expression is constant' warnings. The recommended solution \
		   (http://stackoverflow.com/a/1946485) is to disable these warnings. */ \
		POLYVOX_MSC_WARNING_PUSH \
		POLYVOX_DISABLE_MSC_WARNING(4127) \
		do \
		{ \
			std::stringstream ss; \
			ss << message; \
			PolyVox::Impl::getLoggerInstance()->logErrorMessage(ss.str()); \
		} while(0) \
		POLYVOX_MSC_WARNING_POP

	#define POLYVOX_LOG_ERROR_IF(condition, message) \
		/* We use the do...while(0) construct in our macros (for reasons see here: http://stackoverflow.com/a/154138) \
		   but Visual Studio gives unhelpful 'conditional expression is constant' warnings. The recommended solution \
		   (http://stackoverflow.com/a/1946485) is to disable these warnings. */ \
		POLYVOX_MSC_WARNING_PUSH \
		POLYVOX_DISABLE_MSC_WARNING(4127) \
		do \
		{ \
			if ((condition)) \
			{ \
				std::stringstream ss; \
				ss << message; \
				PolyVox::Impl::getLoggerInstance()->logErrorMessage(ss.str()); \
			} \
		} while(0) \
		POLYVOX_MSC_WARNING_POP

#else

	// We don't bother with the do...while(0) construct here as we're not executing multiple statements
	// We also don't bother with forcing variables to be 'used'. If this causes a problem then calling
	// code can just force them to be used itself in addition to calling the logging macro. Basically 
	// we just want to reduce the chance of extra code being generated.
	#define POLYVOX_LOG_ERROR(message)
	#define POLYVOX_LOG_ERROR_IF(condition, message)

#endif

#ifdef POLYVOX_LOG_FATAL_ENABLED

		#define POLYVOX_LOG_FATAL(message) \
		/* We use the do...while(0) construct in our macros (for reasons see here: http://stackoverflow.com/a/154138) \
		   but Visual Studio gives unhelpful 'conditional expression is constant' warnings. The recommended solution \
		   (http://stackoverflow.com/a/1946485) is to disable these warnings. */ \
		POLYVOX_MSC_WARNING_PUSH \
		POLYVOX_DISABLE_MSC_WARNING(4127) \
		do \
		{ \
			std::stringstream ss; \
			ss << message; \
			PolyVox::Impl::getLoggerInstance()->logFatalMessage(ss.str()); \
		} while(0) \
		POLYVOX_MSC_WARNING_POP

	#define POLYVOX_LOG_FATAL_IF(condition, message) \
		/* We use the do...while(0) construct in our macros (for reasons see here: http://stackoverflow.com/a/154138) \
		   but Visual Studio gives unhelpful 'conditional expression is constant' warnings. The recommended solution \
		   (http://stackoverflow.com/a/1946485) is to disable these warnings. */ \
		POLYVOX_MSC_WARNING_PUSH \
		POLYVOX_DISABLE_MSC_WARNING(4127) \
		do \
		{ \
			if ((condition)) \
			{ \
				std::stringstream ss; \
				ss << message; \
				PolyVox::Impl::getLoggerInstance()->logFatalMessage(ss.str()); \
			} \
		} while(0) \
		POLYVOX_MSC_WARNING_POP

#else

	// We don't bother with the do...while(0) construct here as we're not executing multiple statements
	// We also don't bother with forcing variables to be 'used'. If this causes a problem then calling
	// code can just force them to be used itself in addition to calling the logging macro. Basically 
	// we just want to reduce the chance of extra code being generated.
	#define POLYVOX_LOG_FATAL(message)
	#define POLYVOX_LOG_FATAL_IF(condition, message)

#endif

#endif //__PolyVox_Logging_H__
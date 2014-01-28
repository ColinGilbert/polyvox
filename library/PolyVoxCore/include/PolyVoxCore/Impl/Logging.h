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

#define LOG_DECLARATION(name) \
	std::ostream& log(name)(void); \
	void set(name)Stream(std::ostream& nameStream);

namespace PolyVox
{
	namespace Impl
	{
		std::ostream*& getTraceStreamInstance();
		std::ostream*& getDebugStreamInstance();
		std::ostream*& getInfoStreamInstance();
		std::ostream*& getWarningStreamInstance();
		std::ostream*& getErrorStreamInstance();
		std::ostream*& getFatalStreamInstance();
	}

	/// Get a stream which will consume all input without outputting anything.
	std::ostream* getNullStream(void);

	// These take pointers rather than references to emphasise that the 
	// user needs to keep the target alive as long as PolyVox is writing data.
	void setTraceStream(std::ostream* pStream);
	void setDebugStream(std::ostream* pStream);
	void setInfoStream(std::ostream* pStream);
	void setWarningStream(std::ostream* pStream);
	void setErrorStream(std::ostream* pStream);
	void setFatalStream(std::ostream* pStream);

	// Automatically appending 'std::endl' as described here: http://stackoverflow.com/a/2179782
	struct logTrace
	{
		logTrace(){}
		~logTrace(){*(Impl::getTraceStreamInstance()) << std::endl;}

		template<class T>
		logTrace &operator<<(const T &x)
		{
			*(Impl::getTraceStreamInstance()) << x;
			return *this;
		}
	};

	// Automatically appending 'std::endl' as described here: http://stackoverflow.com/a/2179782
	struct logDebug
	{
		logDebug(){}
		~logDebug(){*(Impl::getDebugStreamInstance()) << std::endl;}

		template<class T>
		logDebug &operator<<(const T &x)
		{
			*(Impl::getDebugStreamInstance()) << x;
			return *this;
		}
	};

	// Automatically appending 'std::endl' as described here: http://stackoverflow.com/a/2179782
	struct logInfo
	{
		logInfo(){}
		~logInfo(){*(Impl::getInfoStreamInstance()) << std::endl;}

		template<class T>
		logInfo &operator<<(const T &x)
		{
			*(Impl::getInfoStreamInstance()) << x;
			return *this;
		}
	};

	// Automatically appending 'std::endl' as described here: http://stackoverflow.com/a/2179782
	struct logWarning
	{
		logWarning(){}
		~logWarning(){*(Impl::getWarningStreamInstance()) << std::endl;}

		template<class T>
		logWarning &operator<<(const T &x)
		{
			*(Impl::getWarningStreamInstance()) << x;
			return *this;
		}
	};

	// Automatically appending 'std::endl' as described here: http://stackoverflow.com/a/2179782
	struct logError
	{
		logError(){}
		~logError(){*(Impl::getErrorStreamInstance()) << std::endl;}

		template<class T>
		logError &operator<<(const T &x)
		{
			*(Impl::getErrorStreamInstance()) << x;
			return *this;
		}
	};

	// Automatically appending 'std::endl' as described here: http://stackoverflow.com/a/2179782
	struct logFatal
	{
		logFatal(){}
		~logFatal(){*(Impl::getFatalStreamInstance()) << std::endl;}

		template<class T>
		logFatal &operator<<(const T &x)
		{
			*(Impl::getFatalStreamInstance()) << x;
			return *this;
		}
	};
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
			/* Appending the 'std::endl' forces the stream to be flushed. */ \
			*(PolyVox::Impl::getTraceStreamInstance()) << message << std::endl; \
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
				/* Appending the 'std::endl' forces the stream to be flushed. */ \
				*(PolyVox::Impl::getTraceStreamInstance()) << message << std::endl; \
			} \
		} while(0) \
		POLYVOX_MSC_WARNING_POP

#else

	#define POLYVOX_LOG_TRACE(message) \
		/* We use the do...while(0) construct in our macros (for reasons see here: http://stackoverflow.com/a/154138) \
		   but Visual Studio gives unhelpful 'conditional expression is constant' warnings. The recommended solution \
		   (http://stackoverflow.com/a/1946485) is to disable these warnings. */ \
		POLYVOX_MSC_WARNING_PUSH \
		POLYVOX_DISABLE_MSC_WARNING(4127) \
		do { POLYVOX_UNUSED(message); } while(0) \
		POLYVOX_MSC_WARNING_POP	

	#define POLYVOX_LOG_TRACE_IF(condition, message) \
		/* We use the do...while(0) construct in our macros (for reasons see here: http://stackoverflow.com/a/154138) \
		   but Visual Studio gives unhelpful 'conditional expression is constant' warnings. The recommended solution \
		   (http://stackoverflow.com/a/1946485) is to disable these warnings. */ \
		POLYVOX_MSC_WARNING_PUSH \
		POLYVOX_DISABLE_MSC_WARNING(4127) \
		do { POLYVOX_UNUSED(condition); POLYVOX_UNUSED(message); } while(0) \
		POLYVOX_MSC_WARNING_POP	

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
			/* Appending the 'std::endl' forces the stream to be flushed. */ \
			*(PolyVox::Impl::getDebugStreamInstance()) << message << std::endl; \
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
				/* Appending the 'std::endl' forces the stream to be flushed. */ \
				*(PolyVox::Impl::getDebugStreamInstance()) << message << std::endl; \
			} \
		} while(0) \
		POLYVOX_MSC_WARNING_POP

#else

	#define POLYVOX_LOG_DEBUG(message) \
		/* We use the do...while(0) construct in our macros (for reasons see here: http://stackoverflow.com/a/154138) \
		   but Visual Studio gives unhelpful 'conditional expression is constant' warnings. The recommended solution \
		   (http://stackoverflow.com/a/1946485) is to disable these warnings. */ \
		POLYVOX_MSC_WARNING_PUSH \
		POLYVOX_DISABLE_MSC_WARNING(4127) \
		do { POLYVOX_UNUSED(message); } while(0) \
		POLYVOX_MSC_WARNING_POP	

	#define POLYVOX_LOG_DEBUG_IF(condition, message) \
		/* We use the do...while(0) construct in our macros (for reasons see here: http://stackoverflow.com/a/154138) \
		   but Visual Studio gives unhelpful 'conditional expression is constant' warnings. The recommended solution \
		   (http://stackoverflow.com/a/1946485) is to disable these warnings. */ \
		POLYVOX_MSC_WARNING_PUSH \
		POLYVOX_DISABLE_MSC_WARNING(4127) \
		do { POLYVOX_UNUSED(condition); POLYVOX_UNUSED(message); } while(0) \
		POLYVOX_MSC_WARNING_POP	

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
			/* Appending the 'std::endl' forces the stream to be flushed. */ \
			*(PolyVox::Impl::getInfoStreamInstance()) << message << std::endl; \
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
				/* Appending the 'std::endl' forces the stream to be flushed. */ \
				*(PolyVox::Impl::getInfoStreamInstance()) << message << std::endl; \
			} \
		} while(0) \
		POLYVOX_MSC_WARNING_POP

#else

	#define POLYVOX_LOG_INFO(message) \
		/* We use the do...while(0) construct in our macros (for reasons see here: http://stackoverflow.com/a/154138) \
		   but Visual Studio gives unhelpful 'conditional expression is constant' warnings. The recommended solution \
		   (http://stackoverflow.com/a/1946485) is to disable these warnings. */ \
		POLYVOX_MSC_WARNING_PUSH \
		POLYVOX_DISABLE_MSC_WARNING(4127) \
		do { POLYVOX_UNUSED(message); } while(0) \
		POLYVOX_MSC_WARNING_POP	

	#define POLYVOX_LOG_INFO_IF(condition, message) \
		/* We use the do...while(0) construct in our macros (for reasons see here: http://stackoverflow.com/a/154138) \
		   but Visual Studio gives unhelpful 'conditional expression is constant' warnings. The recommended solution \
		   (http://stackoverflow.com/a/1946485) is to disable these warnings. */ \
		POLYVOX_MSC_WARNING_PUSH \
		POLYVOX_DISABLE_MSC_WARNING(4127) \
		do { POLYVOX_UNUSED(condition); POLYVOX_UNUSED(message); } while(0) \
		POLYVOX_MSC_WARNING_POP	

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
			/* Appending the 'std::endl' forces the stream to be flushed. */ \
			*(PolyVox::Impl::getWarningStreamInstance()) << message << std::endl; \
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
				/* Appending the 'std::endl' forces the stream to be flushed. */ \
				*(PolyVox::Impl::getWarningStreamInstance()) << message << std::endl; \
			} \
		} while(0) \
		POLYVOX_MSC_WARNING_POP

#else

	#define POLYVOX_LOG_WARNING(message) \
		/* We use the do...while(0) construct in our macros (for reasons see here: http://stackoverflow.com/a/154138) \
		   but Visual Studio gives unhelpful 'conditional expression is constant' warnings. The recommended solution \
		   (http://stackoverflow.com/a/1946485) is to disable these warnings. */ \
		POLYVOX_MSC_WARNING_PUSH \
		POLYVOX_DISABLE_MSC_WARNING(4127) \
		do { POLYVOX_UNUSED(message); } while(0) \
		POLYVOX_MSC_WARNING_POP	

	#define POLYVOX_LOG_WARNING_IF(condition, message) \
		/* We use the do...while(0) construct in our macros (for reasons see here: http://stackoverflow.com/a/154138) \
		   but Visual Studio gives unhelpful 'conditional expression is constant' warnings. The recommended solution \
		   (http://stackoverflow.com/a/1946485) is to disable these warnings. */ \
		POLYVOX_MSC_WARNING_PUSH \
		POLYVOX_DISABLE_MSC_WARNING(4127) \
		do { POLYVOX_UNUSED(condition); POLYVOX_UNUSED(message); } while(0) \
		POLYVOX_MSC_WARNING_POP	

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
			/* Appending the 'std::endl' forces the stream to be flushed. */ \
			*(PolyVox::Impl::getErrorStreamInstance()) << message << std::endl; \
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
				/* Appending the 'std::endl' forces the stream to be flushed. */ \
				*(PolyVox::Impl::getErrorStreamInstance()) << message << std::endl; \
			} \
		} while(0) \
		POLYVOX_MSC_WARNING_POP

#else

	#define POLYVOX_LOG_ERROR(message) \
		/* We use the do...while(0) construct in our macros (for reasons see here: http://stackoverflow.com/a/154138) \
		   but Visual Studio gives unhelpful 'conditional expression is constant' warnings. The recommended solution \
		   (http://stackoverflow.com/a/1946485) is to disable these warnings. */ \
		POLYVOX_MSC_WARNING_PUSH \
		POLYVOX_DISABLE_MSC_WARNING(4127) \
		do { POLYVOX_UNUSED(message); } while(0) \
		POLYVOX_MSC_WARNING_POP	

	#define POLYVOX_LOG_ERROR_IF(condition, message) \
		/* We use the do...while(0) construct in our macros (for reasons see here: http://stackoverflow.com/a/154138) \
		   but Visual Studio gives unhelpful 'conditional expression is constant' warnings. The recommended solution \
		   (http://stackoverflow.com/a/1946485) is to disable these warnings. */ \
		POLYVOX_MSC_WARNING_PUSH \
		POLYVOX_DISABLE_MSC_WARNING(4127) \
		do { POLYVOX_UNUSED(condition); POLYVOX_UNUSED(message); } while(0) \
		POLYVOX_MSC_WARNING_POP	

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
			/* Appending the 'std::endl' forces the stream to be flushed. */ \
			*(PolyVox::Impl::getFatalStreamInstance()) << message << std::endl; \
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
				/* Appending the 'std::endl' forces the stream to be flushed. */ \
				*(PolyVox::Impl::getFatalStreamInstance()) << message << std::endl; \
			} \
		} while(0) \
		POLYVOX_MSC_WARNING_POP

#else

	#define POLYVOX_LOG_FATAL(message) \
		/* We use the do...while(0) construct in our macros (for reasons see here: http://stackoverflow.com/a/154138) \
		   but Visual Studio gives unhelpful 'conditional expression is constant' warnings. The recommended solution \
		   (http://stackoverflow.com/a/1946485) is to disable these warnings. */ \
		POLYVOX_MSC_WARNING_PUSH \
		POLYVOX_DISABLE_MSC_WARNING(4127) \
		do { POLYVOX_UNUSED(message); } while(0) \
		POLYVOX_MSC_WARNING_POP	

	#define POLYVOX_LOG_FATAL_IF(condition, message) \
		/* We use the do...while(0) construct in our macros (for reasons see here: http://stackoverflow.com/a/154138) \
		   but Visual Studio gives unhelpful 'conditional expression is constant' warnings. The recommended solution \
		   (http://stackoverflow.com/a/1946485) is to disable these warnings. */ \
		POLYVOX_MSC_WARNING_PUSH \
		POLYVOX_DISABLE_MSC_WARNING(4127) \
		do { POLYVOX_UNUSED(condition); POLYVOX_UNUSED(message); } while(0) \
		POLYVOX_MSC_WARNING_POP	

#endif

#endif //__PolyVox_Logging_H__
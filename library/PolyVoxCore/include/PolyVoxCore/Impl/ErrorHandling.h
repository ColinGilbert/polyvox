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

#ifndef __PolyVox_ErrorHandling_H__
#define __PolyVox_ErrorHandling_H__

#include "PolyVoxCore/Impl/Config.h"

#include <cstdlib>  // For std::exit
#include <iostream> // For std::cerr
#include <stdexcept>
#include <sstream>
#include <string.h> // Exception constuctors take strings.

#if defined(_MSC_VER)
	// In Visual Studio we can use this function to go into the debugger.
    #define POLYVOX_HALT() __debugbreak()
#else
	// On other platforms we just halt by forcing a crash.
	// Hopefully this puts us in the debugger if one is running
    #define POLYVOX_HALT() *((unsigned int*)0) = 0xDEAD
#endif

// Macros cannot contain #ifdefs, but some of our macros need to disable warnings and such warning supression is
// platform specific. But macros can contain other macros, so we create macros to control the warnings and use
// those instead. This set of warning supression macros can be extended to GCC/Clang when required.
#if defined(_MSC_VER)
	#define POLYVOX_MSC_WARNING_PUSH __pragma(warning(push))
	#define POLYVOX_DISABLE_MSC_WARNING(x) __pragma(warning(disable:x))
	#define POLYVOX_MSC_WARNING_POP __pragma(warning(pop))
#else
	#define POLYVOX_MSC_WARNING_PUSH
	#define POLYVOX_DISABLE_MSC_WARNING(x)
	#define POLYVOX_MSC_WARNING_POP
#endif

#define POLYVOX_UNUSED(x) do { (void)sizeof(x); } while(0)

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

/*
 * Assertions
 * ----------
 * The code below implements a custom assert function called POLYVOX_ASSERT which has a number of advantages compared
 * to the standard C/C++ assert(). It is inspired by http://cnicholson.net/2009/02/stupid-c-tricks-adventures-in-assert/ 
 * which provides code under the MIT license.
 */

#ifdef POLYVOX_ASSERTS_ENABLED

	#define POLYVOX_ASSERT(condition, message) \
		/* We use the do...while(0) construct in our macros (for reasons see here: http://stackoverflow.com/a/154138) \
		   but Visual Studio gives unhelpful 'conditional expression is constant' warnings. The recommended solution \
		   (http://stackoverflow.com/a/1946485) is to disable these warnings. */ \
		POLYVOX_MSC_WARNING_PUSH \
		POLYVOX_DISABLE_MSC_WARNING(4127) \
		do \
		{ \
			if (!(condition)) \
			{ \
				PolyVox::logFatal() << "\n"; \
				PolyVox::logFatal() << "    PolyVox Assertion Failed!"; \
				PolyVox::logFatal() << "    ========================="; \
				PolyVox::logFatal() << "    Condition: " << #condition; \
				PolyVox::logFatal() << "    Message:   " << (message); \
				PolyVox::logFatal() << "    Location:  " << "Line " << __LINE__ << " of " << __FILE__; \
				PolyVox::logFatal() << "\n"; \
				POLYVOX_HALT(); \
			} \
		} while(0) \
		POLYVOX_MSC_WARNING_POP

#else
	
	#define POLYVOX_ASSERT(condition, message) \
		/* We use the do...while(0) construct in our macros (for reasons see here: http://stackoverflow.com/a/154138) \
		   but Visual Studio gives unhelpful 'conditional expression is constant' warnings. The recommended solution \
		   (http://stackoverflow.com/a/1946485) is to disable these warnings. */ \
		POLYVOX_MSC_WARNING_PUSH \
		POLYVOX_DISABLE_MSC_WARNING(4127) \
		do { POLYVOX_UNUSED(condition); POLYVOX_UNUSED(message); } while(0) \
		POLYVOX_MSC_WARNING_POP
	
#endif

/*
 * Static Assertions
 * -----------------
 * These map to C+11 static_assert if available or our own implentation otherwise.
 */

#if defined(HAS_CXX11_STATIC_ASSERT)
	//In this case we can just use static_assert
	#define POLYVOX_STATIC_ASSERT static_assert
#else
	namespace PolyVox
	{
		// empty default template
		template <bool b>
		struct StaticAssert {};

		// template specialized on true
		template <>
		struct StaticAssert<true>
		{
			// If the static assertion is failing then this function won't exist. It will then
			// appear in the error message which gives a clue to the user about what is wrong.
			static void ERROR_The_static_assertion_has_failed() {}
		};
	}

	#define POLYVOX_STATIC_ASSERT(condition, message) StaticAssert<(condition)>::ERROR_The_static_assertion_has_failed();
#endif

/*
 * Exceptions
 * ----------
 * ...
 */
#ifdef POLYVOX_THROW_ENABLED
	#define POLYVOX_THROW(type, message) \
		PolyVox::logError() << (message); \
		throw type((message))

	// Some fast functions (getVoxel(), etc) use exceptions for error handling but don't want the overhead of logging.
	// This overhead is present even if no exception is thrown, probably because the presence of the logging code prevents
	// some inlining. Therefore we provide this macro which doesn't log for such specialised circumstances.
	#define POLYVOX_THROW_DONT_LOG(type, message) \
		throw type((message))
#else
	namespace PolyVox
	{
		typedef void (*ThrowHandler)(std::exception& e, const char* file, int line);

		ThrowHandler getThrowHandler();
		void setThrowHandler(ThrowHandler newHandler);
	}

	#define POLYVOX_THROW(type, message) \
		PolyVox::logError() << (message); \
		type except = (type)((message)); \
		getThrowHandler()((except), __FILE__, __LINE__)

	// Some fast functions (getVoxel(), etc) use exceptions for error handling but don't want the overhead of logging.
	// This overhead is present even if no exception is thrown, probably because the presence of the logging code prevents
	// some inlining. Therefore we provide this macro which doesn't log for such specialised circumstances.
	#define POLYVOX_THROW_DONT_LOG(type, message) \
		type except = (type)((message)); \
		getThrowHandler()((except), __FILE__, __LINE__)
#endif

namespace PolyVox
{
	/// A general purpose exception to indicate that an operation cannot be peformed.
	class invalid_operation : public std::logic_error
	{
	public:
	explicit invalid_operation(const std::string& message)
		: logic_error(message.c_str()) {}

	explicit invalid_operation(const char *message)
		: logic_error(message) {}
	};

	/// Thrown to indicate that a function is deliberatly not implmented. For example, perhaps you called a function
	/// in a base class whereas you are supposed to use a derived class which implements the function, or perhaps the
	/// function is not defined for a particular template parameter. It may be that the function is required to
	/// compile sucessfully but it should not be called.
	class not_implemented : public std::logic_error
	{
	public:
	explicit not_implemented(const std::string& message)
		: logic_error(message.c_str()) {}

	explicit not_implemented(const char *message)
		: logic_error(message) {}
	};
}

#endif //__PolyVox_ErrorHandling_H__

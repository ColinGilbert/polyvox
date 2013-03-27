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

#include <cstdlib>  //For std::exit
#include <iostream> //For std::cerr
#include <stdexcept>

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
				std::cerr << std::endl << std::endl; \
				std::cerr << "    PolyVox Assertion Failed!" << std::endl; \
				std::cerr << "    =========================" << std::endl; \
				std::cerr << "    Condition: " << #condition << std::endl; \
				std::cerr << "    Message:   " << (message) << std::endl; \
				std::cerr << "    Location:  " << "Line " << __LINE__ << " of " << __FILE__ << std::endl << std::endl; \
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
	#define POLYVOX_THROW(type, message) throw type((message))
#else
	namespace PolyVox
	{
		typedef void (*ThrowHandler)(std::exception& e, const char* file, int line);

		ThrowHandler getThrowHandler();
		void setThrowHandler(ThrowHandler newHandler);
	}

	#define POLYVOX_THROW(type, message) \
		type except = (type)((message)); \
		getThrowHandler()((except), __FILE__, __LINE__)
#endif

#endif //__PolyVox_ErrorHandling_H__

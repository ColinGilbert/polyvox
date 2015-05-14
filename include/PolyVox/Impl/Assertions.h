/*******************************************************************************
Copyright (c) 2005-2015 David Williams and Matthew Williams

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

#ifndef __PolyVox_Assertions_H__
#define __PolyVox_Assertions_H__

#include "PolyVox/Config.h"

#include "PolyVox/Impl/LoggingImpl.h" // Asserts can log when they fire.
#include "PolyVox/Impl/PlatformDefinitions.h"

// The code below implements a custom assert function called POLYVOX_ASSERT which has a number of advantages compared
// to the standard C/C++ assert(). It is inspired by http://cnicholson.net/2009/02/stupid-c-tricks-adventures-in-assert/
// which provides code under the MIT license.

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
				std::stringstream ss; \
				ss << "\n"; \
				ss << "    PolyVox Assertion Failed!"; \
				ss << "    ========================="; \
				ss << "    Condition: " << #condition; \
				ss << "    Message:   " << (message); \
				ss << "    Location:  " << "Line " << __LINE__ << " of " << __FILE__; \
				ss << "\n"; \
				PolyVox::getLoggerInstance()->logFatalMessage(ss.str()); \
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

#endif //__PolyVox_Assertions_H__

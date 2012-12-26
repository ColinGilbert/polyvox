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

#define POLYVOX_ASSERTS_ENABLED

namespace PolyVox
{
	namespace Assert
	{
		enum FailBehavior
		{
			Halt,
			Continue,
		};

		typedef FailBehavior (*Handler)(const char* condition, const char* msg, const char* file, int line);

		Handler GetHandler();
		void SetHandler(Handler newHandler);

		FailBehavior ReportFailure(const char* condition, 
								   const char* file, 
								   int line, 
								   const char* msg, ...);
	}
}

#define POLYVOX_HALT() __debugbreak()
#define POLYVOX_UNUSED(x) do { (void)sizeof(x); } while(0)

#ifdef POLYVOX_ASSERTS_ENABLED

	#define POLYVOX_ASSERT(cond, msg, ...) \
		do \
		{ \
			if (!(cond)) \
			{ \
				if (PolyVox::Assert::ReportFailure(#cond, __FILE__, __LINE__, (msg), __VA_ARGS__) == \
					PolyVox::Assert::Halt) \
					POLYVOX_HALT(); \
			} \
		} while(0)

#else
	
	#define POLYVOX_ASSERT(condition, msg, ...) \
		do { POLYVOX_UNUSED(condition); POLYVOX_UNUSED(msg); } while(0)
	
#endif

#define POLYVOX_STATIC_ASSERT(x) \
	typedef char polyvoxStaticAssert[(x) ? 1 : -1];

#endif //__PolyVox_ErrorHandling_H__

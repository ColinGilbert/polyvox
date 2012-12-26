#include "PolyVoxCore/Impl/ErrorHandling.h"

#include <cstdio>
#include <cstdarg>

/*
 * Assertions
 * ----------
 * The code below implements a custom assert function called POLYVOX_ASSERT which has a number of advantages compared to
 * the standard C/C++ assert(). It is based on http://cnicholson.net/2009/02/stupid-c-tricks-adventures-in-assert/ which
 * provides code under the MIT license.
 */
namespace PolyVox
{
	namespace
	{
		Assert::FailBehavior DefaultHandler(const char* condition, const char* msg, const char* file, const int line)
		{
			std::printf("%s(%d): Assert Failure: ", file, line);

			if (condition != NULL)
				std::printf("'%s' ", condition);

			if (msg != NULL)
				std::printf("%s", msg);

			std::printf("\n");

			return Assert::Halt;
		}

		Assert::Handler& GetAssertHandlerInstance()
		{
			static Assert::Handler s_handler = &DefaultHandler;
			return s_handler;
		}

	}

	Assert::Handler Assert::GetHandler()
	{
		return GetAssertHandlerInstance();
	}

	void Assert::SetHandler(Assert::Handler newHandler)
	{
		GetAssertHandlerInstance() = newHandler;
	}

	Assert::FailBehavior Assert::ReportFailure(const char* condition, const char* file, const int line, const char* msg, ...)
	{
		const char* message = NULL;
		if (msg != NULL)
		{
			char messageBuffer[1024];
			{
				va_list args;
				va_start(args, msg);
#if defined (_MSC_VER)
				vsnprintf_s(messageBuffer, 1024, msg, args);
#else
				vsnprintf(messageBuffer, 1024, msg, args);
#endif
				va_end(args);
			}

			message = messageBuffer;
		}

		return GetAssertHandlerInstance()(condition, message, file, line);
	}
}

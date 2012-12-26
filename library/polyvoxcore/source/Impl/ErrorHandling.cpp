#include "PolyVoxCore/Impl/ErrorHandling.h"

#include <cstdio>
#include <cstdarg>

namespace PolyVox
{

namespace
{

Assert::FailBehavior DefaultHandler(const char* condition, 
									const char* msg, 
									const char* file, 
									const int line)
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

Assert::FailBehavior Assert::ReportFailure(const char* condition, 
										   const char* file, 
										   const int line, 
										   const char* msg, ...)
{
	const char* message = NULL;
	if (msg != NULL)
	{
		char messageBuffer[1024];
		{
			va_list args;
			va_start(args, msg);
			vsnprintf(messageBuffer, 1024, msg, args);
			va_end(args);
		}

		message = messageBuffer;
	}

	return GetAssertHandlerInstance()(condition, message, file, line);
}

}
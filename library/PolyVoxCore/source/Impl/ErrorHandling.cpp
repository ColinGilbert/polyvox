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

#include "PolyVoxCore/Impl/ErrorHandling.h"

namespace PolyVox
{
	// Error stream for logging
	std::ostream*& getErrorStreamInstance()
	{
		static std::ostream* s_pErrorStream = &(std::cerr);
		return s_pErrorStream;
	}

	void setErrorStream(std::ostream* errorStream)
	{
		getErrorStreamInstance() = errorStream;
	}

	std::ostream& logError(void)
	{
		return *(getErrorStreamInstance());
	}

	// Fatal stream for logging
	std::ostream*& getFatalStreamInstance()
	{
		static std::ostream* s_pFatalStream = &(std::cerr);
		return s_pFatalStream;
	}

	void setFatalStream(std::ostream* fatalStream)
	{
		getFatalStreamInstance() = fatalStream;
	}

	std::ostream& logFatal(void)
	{
		return *(getFatalStreamInstance());
	}

#ifndef POLYVOX_THROW_ENABLED
	void defaultThrowHandler(std::exception& e, const char* file, int line)
	{
		logFatal() << std::endl << std::endl; \
		logFatal() << "    PolyVox exception thrown!" << std::endl; \
		logFatal() << "    =========================" << std::endl; \
		logFatal() << "    PolyVox has tried to throw an exception but it was built without support" << std::endl; \
		logFatal() << "    for exceptions. In this scenario PolyVox will call a 'throw handler'" << std::endl; \
		logFatal() << "    and this message is being printed by the default throw handler." << std::endl << std::endl; \

		logFatal() << "    If you don't want to enable exceptions then you should try to determine why" << std::endl; \
		logFatal() << "    this exception was thrown and make sure it doesn't happen again. If it was" << std::endl; \
		logFatal() << "    due to something like an invalid argument to a function then you should be" << std::endl; \
		logFatal() << "    able to fix it quite easily by validating parameters as appropriate. More" << std::endl; \
		logFatal() << "    complex exception scenarios (out of memory, etc) might be harder to fix and" << std::endl; \
		logFatal() << "    you should replace this default handler with something which is more" << std::endl; \
		logFatal() << "    meaningful to your users." << std::endl << std::endl; \

		logFatal() << "    Exception details" << std::endl; \
		logFatal() << "    -----------------" << std::endl; \
		logFatal() << "    Line:    " << line << std::endl; \
		logFatal() << "    File:    " << file << std::endl; \
		logFatal() << "    Message: " << e.what() << std::endl << std::endl; \

		POLYVOX_HALT(); \
	}

	ThrowHandler& getThrowHandlerInstance()
	{
		static ThrowHandler s_fThrowHandler = &defaultThrowHandler;
		return s_fThrowHandler;
	}

	ThrowHandler getThrowHandler()
	{
		return getThrowHandlerInstance();
	}

	void setThrowHandler(ThrowHandler fNewHandler)
	{
		getThrowHandlerInstance() = fNewHandler;
	}
#endif
}

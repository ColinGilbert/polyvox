/*******************************************************************************
Copyright (c) 2005-20013 David Williams and Matthew Williams

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

#include "PolyVoxCore/Impl/Timer.h"

#include "PolyVoxCore/Impl/ErrorHandling.h"

#include <iostream>

namespace PolyVox
{
#if defined(_MSC_VER)
	Timer::Timer(bool bAutoStart)
		:m_fPCFreq(0.0)
		,m_iStartTime(0)
	{
		if(bAutoStart)
		{
			start();
		}
	}

	void Timer::start(void)
	{
		LARGE_INTEGER li;
		if(!QueryPerformanceFrequency(&li))
		{
			logWarning() << "QueryPerformanceFrequency failed!";
			m_fPCFreq = 1.0f;
		}

		m_fPCFreq = double(li.QuadPart);

		QueryPerformanceCounter(&li);
		m_iStartTime = li.QuadPart;
	}

	float Timer::elapsedTimeInSeconds(void)
	{
		LARGE_INTEGER li;
		QueryPerformanceCounter(&li);
		double fDifference = static_cast<double>(li.QuadPart-m_iStartTime);
		return static_cast<float>(fDifference / m_fPCFreq);
	}

	uint32_t Timer::elapsedTimeInMilliSeconds(void)
	{
		return static_cast<uint32_t>(elapsedTimeInSeconds() * 1000.0f);
	}
#else //_MSC_VER
	Timer::Timer(bool bAutoStart)
	{
		if(bAutoStart)
		{
			start();
		}
	}

	void Timer::start(void)
	{
	}

	float Timer::elapsedTimeInSeconds(void)
	{
		return 0.0f
	}

	uint32_t Timer::elapsedTimeInMilliSeconds(void)
	{
		return 0;
	}
#endif //_MSC_VER
}

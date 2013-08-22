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

#ifndef __PolyVox_Timer_H__
#define __PolyVox_Timer_H__

#include <cstdint>

#ifdef _MSC_VER // Don't worry about the exact version, as long as this is defied.
#include <Windows.h>
#else
#include <chrono>
#endif //_MSC_VER

namespace PolyVox
{
	class Timer
	{
	public:
		Timer(bool bAutoStart = true);
		
		void start(void);
		
		float elapsedTimeInSeconds(void);
		uint32_t elapsedTimeInMilliSeconds(void);
		
	private:
#if defined(_MSC_VER)
		double m_fPCFreq;
		__int64 m_iStartTime;
#else //_MSC_VER
		typedef std::chrono::system_clock clock;
		std::chrono::time_point<clock> m_start;
#endif //_MSC_VER
	};
}

#endif //__PolyVox_Timer_H__
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

#include <chrono>

namespace PolyVox
{
	class Timer
	{
	public:
		Timer(bool bAutoStart = true)
		{
			if (bAutoStart)
			{
				start();
			}
		}
		
		void start(void)
		{
			m_start = clock::now();
		}
		
		float elapsedTimeInSeconds(void)
		{
			std::chrono::duration<float> elapsed_seconds = clock::now() - m_start;
			return elapsed_seconds.count();
		}

		float elapsedTimeInMilliSeconds(void)
		{
			std::chrono::duration<float, std::milli> elapsed_milliseconds = clock::now() - m_start;
			return elapsed_milliseconds.count();
		}

		float elapsedTimeInMicroSeconds(void)
		{
			std::chrono::duration<float, std::micro> elapsed_microseconds = clock::now() - m_start;
			return elapsed_microseconds.count();
		}
		
	private:
		typedef std::chrono::system_clock clock;
		std::chrono::time_point<clock> m_start;
	};
}

#endif //__PolyVox_Timer_H__
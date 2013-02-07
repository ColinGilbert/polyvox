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

#ifndef __PolyVox_Utility_H__
#define __PolyVox_Utility_H__

#include "PolyVoxCore/Impl/TypeDef.h"

namespace PolyVox
{
	POLYVOX_API uint8_t logBase2(uint32_t uInput);
	POLYVOX_API bool isPowerOf2(uint32_t uInput);

	int32_t roundTowardsNegInf(float r);
	int32_t roundToInteger(float r);
	template <typename Type>
	Type clamp(const Type& value, const Type& low, const Type& high);
	uint32_t upperPowerOfTwo(uint32_t v);

	inline int32_t roundTowardsNegInf(float r)
	{
		return (r >= 0.0) ? static_cast<int32_t>(r) : static_cast<int32_t>(r - 1.0f);
	}

	inline int32_t roundToNearestInteger(float r)
	{
		return (r >= 0.0) ? static_cast<int32_t>(r + 0.5f) : static_cast<int32_t>(r - 0.5f);
	}

	template <typename Type>
	inline Type clamp(const Type& value, const Type& low, const Type& high)
	{
		return (std::min)(high, (std::max)(low, value));
	}

	float triangleFilter(float fInput);
}

#endif

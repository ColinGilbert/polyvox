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

#include "PolyVoxCore/Density.h"

#include <cfloat> //Can't use <limits> as we need compile time constants.

namespace PolyVox
{
	template<>
	const int8_t VoxelTypeTraits< Density<int8_t> >::MinDensity = -127;
	template<>
	const int8_t VoxelTypeTraits< Density<int8_t> >::MaxDensity = 127;
	
	template<>
	const uint8_t VoxelTypeTraits< Density<uint8_t> >::MinDensity = 0;
	template<>
	const uint8_t VoxelTypeTraits< Density<uint8_t> >::MaxDensity = 255;
	
	template<>
	const int16_t VoxelTypeTraits< Density<int16_t> >::MinDensity = -32767;
	template<>
	const int16_t VoxelTypeTraits< Density<int16_t> >::MaxDensity = 32767;
	
	template<>
	const uint16_t VoxelTypeTraits< Density<uint16_t> >::MinDensity = 0;
	template<>
	const uint16_t VoxelTypeTraits< Density<uint16_t> >::MaxDensity = 65535;
	
	template<>
	const float VoxelTypeTraits< Density<float> >::MinDensity = FLT_MIN;
	template<>
	const float VoxelTypeTraits< Density<float> >::MaxDensity = FLT_MAX;

	template<>
	const double VoxelTypeTraits< Density<double> >::MinDensity = DBL_MIN;
	template<>
	const double VoxelTypeTraits< Density<double> >::MaxDensity = DBL_MAX;
}
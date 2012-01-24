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
	//const typename Density8::DensityType VoxelTypeTraits< Density8 >::MinDensity = 0;
	//const typename Density8::DensityType VoxelTypeTraits< Density8 >::MaxDensity = 255;
	
	const typename DensityI8::DensityType VoxelTypeTraits< DensityI8 >::MinDensity = -127;
	const typename DensityI8::DensityType VoxelTypeTraits< DensityI8 >::MaxDensity = 127;
	
	const typename DensityU8::DensityType VoxelTypeTraits< DensityU8 >::MinDensity = 0;
	const typename DensityU8::DensityType VoxelTypeTraits< DensityU8 >::MaxDensity = 255;
	
	//const typename Density16::DensityType VoxelTypeTraits< Density16 >::MinDensity = 0;
	//const typename Density16::DensityType VoxelTypeTraits< Density16 >::MaxDensity = 65535;
	
	const typename DensityI16::DensityType VoxelTypeTraits< DensityI16 >::MinDensity = -32767;
	const typename DensityI16::DensityType VoxelTypeTraits< DensityI16 >::MaxDensity = 32767;
	
	const typename DensityU16::DensityType VoxelTypeTraits< DensityU16 >::MinDensity = 0;
	const typename DensityU16::DensityType VoxelTypeTraits< DensityU16 >::MaxDensity = 65535;
	
	const typename DensityFloat::DensityType VoxelTypeTraits< DensityFloat >::MinDensity = FLT_MIN;
	const typename DensityFloat::DensityType VoxelTypeTraits< DensityFloat >::MaxDensity = FLT_MAX;
	
	const typename DensityDouble::DensityType VoxelTypeTraits< DensityDouble >::MinDensity = DBL_MIN;
	const typename DensityDouble::DensityType VoxelTypeTraits< DensityDouble >::MaxDensity = DBL_MAX;
	
	//template<>
	//const typename DensityI8::DensityType VoxelTypeTraits< DensityI8 >::MinDensity = -127;
	//template<>
	//const typename DensityI8::DensityType VoxelTypeTraits< DensityI8 >::MaxDensity = 127;
	
	//template<>
	//const typename DensityU8::DensityType VoxelTypeTraits< DensityU8 >::MinDensity = 0;
	//template<>
	//const typename DensityU8::DensityType VoxelTypeTraits< DensityU8 >::MaxDensity = 255;
	
	/*template<>
	const uint8_t VoxelTypeTraits< Density8 >::MinDensity = 0;
	template<>
	const uint8_t VoxelTypeTraits< Density8 >::MaxDensity = 255;
	
	template<>
	const int16_t VoxelTypeTraits< DensityI16 >::MinDensity = -32767;
	template<>
	const int16_t VoxelTypeTraits< DensityI16 >::MaxDensity = 32767;
	
	template<>
	const uint16_t VoxelTypeTraits< Density16 >::MinDensity = 0;
	template<>
	const uint16_t VoxelTypeTraits< Density16 >::MaxDensity = 65535;
	
	template<>
	const float VoxelTypeTraits< DensityFloat >::MinDensity = FLT_MIN;
	template<>
	const float VoxelTypeTraits< DensityFloat >::MaxDensity = FLT_MAX;

	template<>
	const double VoxelTypeTraits< DensityDouble >::MinDensity = DBL_MIN;
	template<>
	const double VoxelTypeTraits< DensityDouble >::MaxDensity = DBL_MAX;*/
}
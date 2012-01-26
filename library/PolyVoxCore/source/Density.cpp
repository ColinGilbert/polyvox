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
	const bool VoxelTypeTraits< DensityI8 >::HasDensity = true;
	const bool VoxelTypeTraits< DensityI8 >::HasMaterial = false;
	const DensityI8::DensityType VoxelTypeTraits< DensityI8 >::MinDensity = -127;
	const DensityI8::DensityType VoxelTypeTraits< DensityI8 >::MaxDensity = 127;
	
	const bool VoxelTypeTraits< DensityU8 >::HasDensity = true;
	const bool VoxelTypeTraits< DensityU8 >::HasMaterial = false;
	const DensityU8::DensityType VoxelTypeTraits< DensityU8 >::MinDensity = 0;
	const DensityU8::DensityType VoxelTypeTraits< DensityU8 >::MaxDensity = 255;
	
	const bool VoxelTypeTraits< DensityI16 >::HasDensity = true;
	const bool VoxelTypeTraits< DensityI16 >::HasMaterial = false;
	const DensityI16::DensityType VoxelTypeTraits< DensityI16 >::MinDensity = -32767;
	const DensityI16::DensityType VoxelTypeTraits< DensityI16 >::MaxDensity = 32767;
	
	const bool VoxelTypeTraits< DensityU16 >::HasDensity = true;
	const bool VoxelTypeTraits< DensityU16 >::HasMaterial = false;
	const DensityU16::DensityType VoxelTypeTraits< DensityU16 >::MinDensity = 0;
	const DensityU16::DensityType VoxelTypeTraits< DensityU16 >::MaxDensity = 65535;
	
	const bool VoxelTypeTraits< DensityFloat >::HasDensity = true;
	const bool VoxelTypeTraits< DensityFloat >::HasMaterial = false;
	const DensityFloat::DensityType VoxelTypeTraits< DensityFloat >::MinDensity = FLT_MIN;
	const DensityFloat::DensityType VoxelTypeTraits< DensityFloat >::MaxDensity = FLT_MAX;
	
	const bool VoxelTypeTraits< DensityDouble >::HasDensity = true;
	const bool VoxelTypeTraits< DensityDouble >::HasMaterial = false;
	const DensityDouble::DensityType VoxelTypeTraits< DensityDouble >::MinDensity = DBL_MIN;
	const DensityDouble::DensityType VoxelTypeTraits< DensityDouble >::MaxDensity = DBL_MAX;
}
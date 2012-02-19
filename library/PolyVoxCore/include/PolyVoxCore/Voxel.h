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

#ifndef __PolyVox_Voxel_H__
#define __PolyVox_Voxel_H__

#include "PolyVoxImpl/TypeDef.h"

#include <cassert>
#include <limits>

namespace PolyVox
{	
	// Various properties of the voxel types can be expressed via types traits, similar to the way std::numeric_limits is implemented.
	// This has some advantages compared to storing the properties directly in the voxel class. For example, by using traits it is possible
	// to also apply these properties to primative types which might be desirable (the Volume classes do accept primative types as template
	// parameters). Also, properties such as minDensity() and maxDensity() would be difficult to represent though class members because they
	// depend ont the type (float has a very different range from int8_t for example).
	//
	// The properties are currently exposed as constants because we need access to them at compile time. Ideally we would like to make them
	// functions flagged with 'constexpr' as we could then make use of the max() and min() functions in std::numeric_limits, but this is not
	// widely supported by compilers yet. We may change this in the future.
	//
	// Syntax for templatised traits classes: http://stackoverflow.com/q/8606302/849083
	template <typename Type>
	class VoxelTypeTraits
	{
	public:
		typedef uint8_t DensityType;
		static const bool HasDensity = false;
		static const bool HasMaterial = false;

		static bool hasDensity() { return false; }
		static bool hasMaterial() { return false; }

		// These default implementations return an int32_t rather than void so that the result can be
		// assigned to a variable for all voxel types (even those without density components). Calls
		// to these functions should be protected by calls to hasDensity(), but the compiler still
		// needs to ensure the assignment is compilable even if hasDensity() returns false.
		static int32_t minDensity() { assert(false); return 0; } 
		static int32_t maxDensity() { assert(false); return 0; }
	};

	template<typename VoxelType>
	typename VoxelTypeTraits<VoxelType>::DensityType convertToDensity(VoxelType voxel)
	{
		return voxel;
	}

	template<>
	class VoxelTypeTraits< uint8_t >	
	{
	public:
		typedef uint8_t DensityType;
		static const bool HasDensity = true;
		static const bool HasMaterial = false;
		static bool hasDensity() { return true; }
		static bool hasMaterial() { return false; }
		static uint8_t minDensity() { return 0; }
		static uint8_t maxDensity() { return 255; }
	};
}

#endif //__PolyVox_Voxel_H__

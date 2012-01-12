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
	/// This class represents a voxel
	////////////////////////////////////////////////////////////////////////////////
	/// Detailed description...
	////////////////////////////////////////////////////////////////////////////////
	template <typename DenType, typename MatType>
	class Voxel
	{
	public:
		//We expose DensityType and MaterialType in this way so that, when code is
		//templatised on voxel type, it can determine the underlying storage type
		//using code such as 'VoxelType::DensityType value = voxel.getDensity()'
		//or 'VoxelType::MaterialType value = voxel.getMaterial()'.
		typedef DenType DensityType;
		typedef MatType MaterialType;
		
		DensityType getDensity() const throw() { assert(false); return 0; }
		MaterialType getMaterial() const throw() { assert(false); return 0; }

		void setDensity(DensityType uDensity) { assert(false); }
		void setMaterial(MaterialType /*uMaterial*/) { assert(false); } 
	};
}

#endif //__PolyVox_Voxel_H__

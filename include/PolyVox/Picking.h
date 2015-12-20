/*******************************************************************************
Copyright (c) 2013 Matt Williams

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

#ifndef __PolyVox_Picking_H__
#define __PolyVox_Picking_H__

#include "Vector.h"

namespace PolyVox
{
	/**
	 * A structure containing the information about a picking operation
	 */
	struct PickResult
	{
		PickResult() : didHit(false) {}
		bool didHit; ///< Did the picking operation hit anything
		Vector3DInt32 hitVoxel; ///< The location of the solid voxel it hit
		Vector3DInt32 previousVoxel; ///< The location of the voxel before the one it hit
	};
	
	/// Pick the first solid voxel along a vector
	template<typename VolumeType>
	PickResult pickVoxel(VolumeType* volData, const Vector3DFloat& v3dStart, const Vector3DFloat& v3dDirectionAndLength, const typename VolumeType::VoxelType& emptyVoxelExample);
}

#include "Picking.inl"

#endif //__PolyVox_Picking_H__

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

#include "Raycast.h"

namespace PolyVox
{
	namespace 
	{
		/**
		 * This is just an implementation class for the pickVoxel function
		 * 
		 * It makes note of the sort of empty voxel you're looking for in the constructor.
		 * 
		 * Each time the operator() is called:
		 *  * if it's hit a voxel it sets up the result and returns false
		 *  * otherwise it preps the result for the next iteration and returns true
		 */
		template <typename VolumeType>
		class RaycastPickingFunctor
		{
		public:
			RaycastPickingFunctor(const typename VolumeType::VoxelType& emptyVoxelExample)
				:m_emptyVoxelExample(emptyVoxelExample)
				,m_result()
			{
			}
		
			bool operator()(const typename VolumeType::Sampler& sampler)
			{
				if(sampler.getVoxel() != m_emptyVoxelExample) //If we've hit something
				{
					m_result.didHit = true;
					m_result.hitVoxel = sampler.getPosition();
					return false;
				}
				
				m_result.previousVoxel = sampler.getPosition();
				
				return true;
			}
			const typename VolumeType::VoxelType& m_emptyVoxelExample;
			PickResult m_result;
		};
	}
	
	/**
	 * \param volData The volume to pass the ray though
	 * \param v3dStart The start position in the volume
	 * \param v3dDirectionAndLength The direction and length of the ray
	 * \param emptyVoxelExample The value used to represent empty voxels in your volume
	 * 
	 * \return A PickResult containing the hit information
	 */
	template<typename VolumeType>
	PickResult pickVoxel(VolumeType* volData, const Vector3DFloat& v3dStart, const Vector3DFloat& v3dDirectionAndLength, const typename VolumeType::VoxelType& emptyVoxelExample)
	{
		RaycastPickingFunctor<VolumeType> functor(emptyVoxelExample);
		
		raycastWithDirection(volData, v3dStart, v3dDirectionAndLength, functor);
		
		return functor.m_result;
	}
}

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

#include "PolyVoxImpl/Block.h"
#include "PolyVoxCore/Volume.h"
#include "PolyVoxCore/Vector.h"
#include "PolyVoxCore/Region.h"

#include <limits>
namespace PolyVox
{
	template <typename VoxelType>
	int32_t Volume<VoxelType>::Sampler::getPosX(void) const
	{
		assert(false);
		return 0;
	}

	template <typename VoxelType>
	int32_t Volume<VoxelType>::Sampler::getPosY(void) const
	{
		assert(false);
		return 0;
	}

	template <typename VoxelType>
	int32_t Volume<VoxelType>::Sampler::getPosZ(void) const
	{
		assert(false);
		return 0;
	}

	template <typename VoxelType>
	const Volume<VoxelType>* Volume<VoxelType>::Sampler::getVolume(void) const
	{
		assert(false);
		return 0;
	}

	template <typename VoxelType>
	VoxelType Volume<VoxelType>::Sampler::getVoxel(void) const
	{
		assert(false);
		return VoxelType();
	}

	template <typename VoxelType>
	void Volume<VoxelType>::Sampler::setPosition(const Vector3DInt32& v3dNewPos)
	{
		assert(false);
	}

	template <typename VoxelType>
	void Volume<VoxelType>::Sampler::setPosition(int32_t xPos, int32_t yPos, int32_t zPos)
	{
		assert(false);
	}

	template <typename VoxelType>
	void Volume<VoxelType>::Sampler::movePositiveX(void)
	{
		assert(false);
	}

	template <typename VoxelType>
	void Volume<VoxelType>::Sampler::movePositiveY(void)
	{
		assert(false);
	}

	template <typename VoxelType>
	void Volume<VoxelType>::Sampler::movePositiveZ(void)
	{
		assert(false);
	}

	template <typename VoxelType>
	void Volume<VoxelType>::Sampler::moveNegativeX(void)
	{
		assert(false);
	}

	template <typename VoxelType>
	void Volume<VoxelType>::Sampler::moveNegativeY(void)
	{
		assert(false);
	}

	template <typename VoxelType>
	void Volume<VoxelType>::Sampler::moveNegativeZ(void)
	{
		assert(false);
	}

	template <typename VoxelType>
	VoxelType Volume<VoxelType>::Sampler::peekVoxel1nx1ny1nz(void) const
	{
		assert(false);
		return VoxelType();
	}

	template <typename VoxelType>
	VoxelType Volume<VoxelType>::Sampler::peekVoxel1nx1ny0pz(void) const
	{
		assert(false);
		return VoxelType();
	}

	template <typename VoxelType>
	VoxelType Volume<VoxelType>::Sampler::peekVoxel1nx1ny1pz(void) const
	{
		assert(false);
		return VoxelType();
	}

	template <typename VoxelType>
	VoxelType Volume<VoxelType>::Sampler::peekVoxel1nx0py1nz(void) const
	{
		assert(false);
		return VoxelType();
	}

	template <typename VoxelType>
	VoxelType Volume<VoxelType>::Sampler::peekVoxel1nx0py0pz(void) const
	{
		assert(false);
		return VoxelType();
	}

	template <typename VoxelType>
	VoxelType Volume<VoxelType>::Sampler::peekVoxel1nx0py1pz(void) const
	{
		assert(false);
		return VoxelType();
	}

	template <typename VoxelType>
	VoxelType Volume<VoxelType>::Sampler::peekVoxel1nx1py1nz(void) const
	{
		assert(false);
		return VoxelType();
	}

	template <typename VoxelType>
	VoxelType Volume<VoxelType>::Sampler::peekVoxel1nx1py0pz(void) const
	{
		assert(false);
		return VoxelType();
	}

	template <typename VoxelType>
	VoxelType Volume<VoxelType>::Sampler::peekVoxel1nx1py1pz(void) const
	{
		assert(false);
		return VoxelType();
	}

	//////////////////////////////////////////////////////////////////////////

	template <typename VoxelType>
	VoxelType Volume<VoxelType>::Sampler::peekVoxel0px1ny1nz(void) const
	{
		assert(false);
		return VoxelType();
	}

	template <typename VoxelType>
	VoxelType Volume<VoxelType>::Sampler::peekVoxel0px1ny0pz(void) const
	{
		assert(false);
		return VoxelType();
	}

	template <typename VoxelType>
	VoxelType Volume<VoxelType>::Sampler::peekVoxel0px1ny1pz(void) const
	{
		assert(false);
		return VoxelType();
	}

	template <typename VoxelType>
	VoxelType Volume<VoxelType>::Sampler::peekVoxel0px0py1nz(void) const
	{
		assert(false);
		return VoxelType();
	}

	template <typename VoxelType>
	VoxelType Volume<VoxelType>::Sampler::peekVoxel0px0py0pz(void) const
	{
			assert(false);
		return VoxelType();
	}

	template <typename VoxelType>
	VoxelType Volume<VoxelType>::Sampler::peekVoxel0px0py1pz(void) const
	{
		assert(false);
		return VoxelType();
	}

	template <typename VoxelType>
	VoxelType Volume<VoxelType>::Sampler::peekVoxel0px1py1nz(void) const
	{
		assert(false);
		return VoxelType();
	}

	template <typename VoxelType>
	VoxelType Volume<VoxelType>::Sampler::peekVoxel0px1py0pz(void) const
	{
		assert(false);
		return VoxelType();
	}

	template <typename VoxelType>
	VoxelType Volume<VoxelType>::Sampler::peekVoxel0px1py1pz(void) const
	{
		assert(false);
		return VoxelType();
	}

	//////////////////////////////////////////////////////////////////////////

	template <typename VoxelType>
	VoxelType Volume<VoxelType>::Sampler::peekVoxel1px1ny1nz(void) const
	{
		assert(false);
		return VoxelType();
	}

	template <typename VoxelType>
	VoxelType Volume<VoxelType>::Sampler::peekVoxel1px1ny0pz(void) const
	{
		assert(false);
		return VoxelType();
	}

	template <typename VoxelType>
	VoxelType Volume<VoxelType>::Sampler::peekVoxel1px1ny1pz(void) const
	{
		assert(false);
		return VoxelType();
	}

	template <typename VoxelType>
	VoxelType Volume<VoxelType>::Sampler::peekVoxel1px0py1nz(void) const
	{
		assert(false);
		return VoxelType();
	}

	template <typename VoxelType>
	VoxelType Volume<VoxelType>::Sampler::peekVoxel1px0py0pz(void) const
	{
		assert(false);
		return VoxelType();
	}

	template <typename VoxelType>
	VoxelType Volume<VoxelType>::Sampler::peekVoxel1px0py1pz(void) const
	{
		assert(false);
		return VoxelType();
	}

	template <typename VoxelType>
	VoxelType Volume<VoxelType>::Sampler::peekVoxel1px1py1nz(void) const
	{
		assert(false);
		return VoxelType();
	}

	template <typename VoxelType>
	VoxelType Volume<VoxelType>::Sampler::peekVoxel1px1py0pz(void) const
	{
		assert(false);
		return VoxelType();
	}

	template <typename VoxelType>
	VoxelType Volume<VoxelType>::Sampler::peekVoxel1px1py1pz(void) const
	{
		assert(false);
		return VoxelType();
	}
}

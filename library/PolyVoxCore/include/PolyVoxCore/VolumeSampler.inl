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
	template <typename DerivedVolumeType>
	int32_t Volume<VoxelType>::Sampler<DerivedVolumeType>::getPosX(void) const
	{
		return mXPos;
	}

	template <typename VoxelType>
	template <typename DerivedVolumeType>
	int32_t Volume<VoxelType>::Sampler<DerivedVolumeType>::getPosY(void) const
	{
		return mYPos;
	}

	template <typename VoxelType>
	template <typename DerivedVolumeType>
	int32_t Volume<VoxelType>::Sampler<DerivedVolumeType>::getPosZ(void) const
	{
		return mZPos;
	}

	template <typename VoxelType>
	template <typename DerivedVolumeType>
	VoxelType Volume<VoxelType>::Sampler<DerivedVolumeType>::getVoxel(void) const
	{
		mVolume->getVoxelAt(mXPos, mYPos, mZPos);
	}

	template <typename VoxelType>
	template <typename DerivedVolumeType>
	void Volume<VoxelType>::Sampler<DerivedVolumeType>::setPosition(const Vector3DInt32& v3dNewPos)
	{
		mXPos = v3dNewPos.getX();
		mYPos = v3dNewPos.getY();
		mZPos = v3dNewPos.getZ();
	}

	template <typename VoxelType>
	template <typename DerivedVolumeType>
	void Volume<VoxelType>::Sampler<DerivedVolumeType>::setPosition(int32_t xPos, int32_t yPos, int32_t zPos)
	{
		mXPos = xPos;
		mYPos = yPos;
		mZPos = zPos;
	}

	template <typename VoxelType>
	template <typename DerivedVolumeType>
	void Volume<VoxelType>::Sampler<DerivedVolumeType>::movePositiveX(void)
	{
		mXPos++;
	}

	template <typename VoxelType>
	template <typename DerivedVolumeType>
	void Volume<VoxelType>::Sampler<DerivedVolumeType>::movePositiveY(void)
	{
		mYPos++;
	}

	template <typename VoxelType>
	template <typename DerivedVolumeType>
	void Volume<VoxelType>::Sampler<DerivedVolumeType>::movePositiveZ(void)
	{
		mZPos++;
	}

	template <typename VoxelType>
	template <typename DerivedVolumeType>
	void Volume<VoxelType>::Sampler<DerivedVolumeType>::moveNegativeX(void)
	{
		mXPos--;
	}

	template <typename VoxelType>
	template <typename DerivedVolumeType>
	void Volume<VoxelType>::Sampler<DerivedVolumeType>::moveNegativeY(void)
	{
		mYPos--;
	}

	template <typename VoxelType>
	template <typename DerivedVolumeType>
	void Volume<VoxelType>::Sampler<DerivedVolumeType>::moveNegativeZ(void)
	{
		mZPos--;
	}

	template <typename VoxelType>
	template <typename DerivedVolumeType>
	VoxelType Volume<VoxelType>::Sampler<DerivedVolumeType>::peekVoxel1nx1ny1nz(void) const
	{
		mVolume->getVoxelAt(mXPos - 1, mYPos - 1, mZPos - 1);
	}

	template <typename VoxelType>
	template <typename DerivedVolumeType>
	VoxelType Volume<VoxelType>::Sampler<DerivedVolumeType>::peekVoxel1nx1ny0pz(void) const
	{
		mVolume->getVoxelAt(mXPos - 1, mYPos - 1, mZPos    );
	}

	template <typename VoxelType>
	template <typename DerivedVolumeType>
	VoxelType Volume<VoxelType>::Sampler<DerivedVolumeType>::peekVoxel1nx1ny1pz(void) const
	{
		mVolume->getVoxelAt(mXPos - 1, mYPos - 1, mZPos + 1);
	}

	template <typename VoxelType>
	template <typename DerivedVolumeType>
	VoxelType Volume<VoxelType>::Sampler<DerivedVolumeType>::peekVoxel1nx0py1nz(void) const
	{
		mVolume->getVoxelAt(mXPos - 1, mYPos    , mZPos - 1);
	}

	template <typename VoxelType>
	template <typename DerivedVolumeType>
	VoxelType Volume<VoxelType>::Sampler<DerivedVolumeType>::peekVoxel1nx0py0pz(void) const
	{
		mVolume->getVoxelAt(mXPos - 1, mYPos    , mZPos    );
	}

	template <typename VoxelType>
	template <typename DerivedVolumeType>
	VoxelType Volume<VoxelType>::Sampler<DerivedVolumeType>::peekVoxel1nx0py1pz(void) const
	{
		mVolume->getVoxelAt(mXPos - 1, mYPos    , mZPos + 1);
	}

	template <typename VoxelType>
	template <typename DerivedVolumeType>
	VoxelType Volume<VoxelType>::Sampler<DerivedVolumeType>::peekVoxel1nx1py1nz(void) const
	{
		mVolume->getVoxelAt(mXPos - 1, mYPos + 1, mZPos - 1);
	}

	template <typename VoxelType>
	template <typename DerivedVolumeType>
	VoxelType Volume<VoxelType>::Sampler<DerivedVolumeType>::peekVoxel1nx1py0pz(void) const
	{
		mVolume->getVoxelAt(mXPos - 1, mYPos + 1, mZPos    );
	}

	template <typename VoxelType>
	template <typename DerivedVolumeType>
	VoxelType Volume<VoxelType>::Sampler<DerivedVolumeType>::peekVoxel1nx1py1pz(void) const
	{
		mVolume->getVoxelAt(mXPos - 1, mYPos + 1, mZPos + 1);
	}

	//////////////////////////////////////////////////////////////////////////

	template <typename VoxelType>
	template <typename DerivedVolumeType>
	VoxelType Volume<VoxelType>::Sampler<DerivedVolumeType>::peekVoxel0px1ny1nz(void) const
	{
		mVolume->getVoxelAt(mXPos    , mYPos - 1, mZPos - 1);
	}

	template <typename VoxelType>
	template <typename DerivedVolumeType>
	VoxelType Volume<VoxelType>::Sampler<DerivedVolumeType>::peekVoxel0px1ny0pz(void) const
	{
		mVolume->getVoxelAt(mXPos    , mYPos - 1, mZPos    );
	}

	template <typename VoxelType>
	template <typename DerivedVolumeType>
	VoxelType Volume<VoxelType>::Sampler<DerivedVolumeType>::peekVoxel0px1ny1pz(void) const
	{
		mVolume->getVoxelAt(mXPos    , mYPos - 1, mZPos + 1);
	}

	template <typename VoxelType>
	template <typename DerivedVolumeType>
	VoxelType Volume<VoxelType>::Sampler<DerivedVolumeType>::peekVoxel0px0py1nz(void) const
	{
		mVolume->getVoxelAt(mXPos    , mYPos    , mZPos - 1);
	}

	template <typename VoxelType>
	template <typename DerivedVolumeType>
	VoxelType Volume<VoxelType>::Sampler<DerivedVolumeType>::peekVoxel0px0py0pz(void) const
	{
		mVolume->getVoxelAt(mXPos    , mYPos    , mZPos    );
	}

	template <typename VoxelType>
	template <typename DerivedVolumeType>
	VoxelType Volume<VoxelType>::Sampler<DerivedVolumeType>::peekVoxel0px0py1pz(void) const
	{
		mVolume->getVoxelAt(mXPos    , mYPos    , mZPos + 1);
	}

	template <typename VoxelType>
	template <typename DerivedVolumeType>
	VoxelType Volume<VoxelType>::Sampler<DerivedVolumeType>::peekVoxel0px1py1nz(void) const
	{
		mVolume->getVoxelAt(mXPos    , mYPos + 1, mZPos - 1);
	}

	template <typename VoxelType>
	template <typename DerivedVolumeType>
	VoxelType Volume<VoxelType>::Sampler<DerivedVolumeType>::peekVoxel0px1py0pz(void) const
	{
		mVolume->getVoxelAt(mXPos    , mYPos + 1, mZPos    );
	}

	template <typename VoxelType>
	template <typename DerivedVolumeType>
	VoxelType Volume<VoxelType>::Sampler<DerivedVolumeType>::peekVoxel0px1py1pz(void) const
	{
		mVolume->getVoxelAt(mXPos    , mYPos + 1, mZPos + 1);
	}

	//////////////////////////////////////////////////////////////////////////

	template <typename VoxelType>
	template <typename DerivedVolumeType>
	VoxelType Volume<VoxelType>::Sampler<DerivedVolumeType>::peekVoxel1px1ny1nz(void) const
	{
		mVolume->getVoxelAt(mXPos + 1, mYPos - 1, mZPos - 1);
	}

	template <typename VoxelType>
	template <typename DerivedVolumeType>
	VoxelType Volume<VoxelType>::Sampler<DerivedVolumeType>::peekVoxel1px1ny0pz(void) const
	{
		mVolume->getVoxelAt(mXPos + 1, mYPos - 1, mZPos    );
	}

	template <typename VoxelType>
	template <typename DerivedVolumeType>
	VoxelType Volume<VoxelType>::Sampler<DerivedVolumeType>::peekVoxel1px1ny1pz(void) const
	{
		mVolume->getVoxelAt(mXPos + 1, mYPos - 1, mZPos + 1);
	}

	template <typename VoxelType>
	template <typename DerivedVolumeType>
	VoxelType Volume<VoxelType>::Sampler<DerivedVolumeType>::peekVoxel1px0py1nz(void) const
	{
		mVolume->getVoxelAt(mXPos + 1, mYPos    , mZPos - 1);
	}

	template <typename VoxelType>
	template <typename DerivedVolumeType>
	VoxelType Volume<VoxelType>::Sampler<DerivedVolumeType>::peekVoxel1px0py0pz(void) const
	{
		mVolume->getVoxelAt(mXPos + 1, mYPos    , mZPos    );
	}

	template <typename VoxelType>
	template <typename DerivedVolumeType>
	VoxelType Volume<VoxelType>::Sampler<DerivedVolumeType>::peekVoxel1px0py1pz(void) const
	{
		mVolume->getVoxelAt(mXPos + 1, mYPos    , mZPos + 1);
	}

	template <typename VoxelType>
	template <typename DerivedVolumeType>
	VoxelType Volume<VoxelType>::Sampler<DerivedVolumeType>::peekVoxel1px1py1nz(void) const
	{
		mVolume->getVoxelAt(mXPos + 1, mYPos + 1, mZPos - 1);
	}

	template <typename VoxelType>
	template <typename DerivedVolumeType>
	VoxelType Volume<VoxelType>::Sampler<DerivedVolumeType>::peekVoxel1px1py0pz(void) const
	{
		mVolume->getVoxelAt(mXPos + 1, mYPos + 1, mZPos    );
	}

	template <typename VoxelType>
	template <typename DerivedVolumeType>
	VoxelType Volume<VoxelType>::Sampler<DerivedVolumeType>::peekVoxel1px1py1pz(void) const
	{
		mVolume->getVoxelAt(mXPos + 1, mYPos + 1, mZPos + 1);
	}
}

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

#include "PolyVoxCore/Impl/Utility.h"

namespace PolyVox
{
	template <typename VoxelType>
	template <typename DerivedVolumeType>
	BaseVolume<VoxelType>::Sampler<DerivedVolumeType>::Sampler(DerivedVolumeType* volume)
		:mVolume(volume)
		,mXPosInVolume(0)
		,mYPosInVolume(0)
		,mZPosInVolume(0)
		,m_eWrapMode(WrapModes::Border)
		,m_tBorder()
		,m_bIsCurrentPositionValidInX(false)
		,m_bIsCurrentPositionValidInY(false)
		,m_bIsCurrentPositionValidInZ(false)
	{
	}

	template <typename VoxelType>
	template <typename DerivedVolumeType>
	BaseVolume<VoxelType>::Sampler<DerivedVolumeType>::~Sampler()
	{
	}

	template <typename VoxelType>
	template <typename DerivedVolumeType>
	Vector3DInt32 BaseVolume<VoxelType>::Sampler<DerivedVolumeType>::getPosition(void) const
	{
		return Vector3DInt32(mXPosInVolume, mYPosInVolume, mZPosInVolume);
	}

	template <typename VoxelType>
	template <typename DerivedVolumeType>
	VoxelType BaseVolume<VoxelType>::Sampler<DerivedVolumeType>::getVoxel(void) const
	{
		return mVolume->getVoxel<WrapModes::None>(mXPosInVolume, mYPosInVolume, mZPosInVolume);
	}

	template <typename VoxelType>
	template <typename DerivedVolumeType>
	bool inline BaseVolume<VoxelType>::Sampler<DerivedVolumeType>::isCurrentPositionValid(void) const
	{
		return m_bIsCurrentPositionValidInX && m_bIsCurrentPositionValidInY && m_bIsCurrentPositionValidInZ;
	}

	template <typename VoxelType>
	template <typename DerivedVolumeType>
	void BaseVolume<VoxelType>::Sampler<DerivedVolumeType>::setPosition(const Vector3DInt32& v3dNewPos)
	{
		setPosition(v3dNewPos.getX(), v3dNewPos.getY(), v3dNewPos.getZ());
	}

	template <typename VoxelType>
	template <typename DerivedVolumeType>
	void BaseVolume<VoxelType>::Sampler<DerivedVolumeType>::setPosition(int32_t xPos, int32_t yPos, int32_t zPos)
	{
		mXPosInVolume = xPos;
		mYPosInVolume = yPos;
		mZPosInVolume = zPos;

		m_bIsCurrentPositionValidInX = mVolume->getEnclosingRegion().containsPointInX(xPos);
		m_bIsCurrentPositionValidInY = mVolume->getEnclosingRegion().containsPointInY(yPos);
		m_bIsCurrentPositionValidInZ = mVolume->getEnclosingRegion().containsPointInZ(zPos);
	}

	template <typename VoxelType>
	template <typename DerivedVolumeType>
	bool BaseVolume<VoxelType>::Sampler<DerivedVolumeType>::setVoxel(VoxelType tValue)
	{
		return mVolume->setVoxelAt(mXPosInVolume, mYPosInVolume, mZPosInVolume, tValue);
	}

	template <typename VoxelType>
	template <typename DerivedVolumeType>
	void BaseVolume<VoxelType>::Sampler<DerivedVolumeType>::setWrapMode(WrapMode eWrapMode, VoxelType tBorder)
	{
		m_eWrapMode = eWrapMode;
		m_tBorder = tBorder;
	}

	template <typename VoxelType>
	template <typename DerivedVolumeType>
	void BaseVolume<VoxelType>::Sampler<DerivedVolumeType>::movePositiveX(void)
	{
		mXPosInVolume++;
		m_bIsCurrentPositionValidInX = mVolume->getEnclosingRegion().containsPointInX(mXPosInVolume);
	}

	template <typename VoxelType>
	template <typename DerivedVolumeType>
	void BaseVolume<VoxelType>::Sampler<DerivedVolumeType>::movePositiveY(void)
	{
		mYPosInVolume++;
		m_bIsCurrentPositionValidInY = mVolume->getEnclosingRegion().containsPointInY(mYPosInVolume);
	}

	template <typename VoxelType>
	template <typename DerivedVolumeType>
	void BaseVolume<VoxelType>::Sampler<DerivedVolumeType>::movePositiveZ(void)
	{
		mZPosInVolume++;
		m_bIsCurrentPositionValidInZ = mVolume->getEnclosingRegion().containsPointInZ(mZPosInVolume);
	}

	template <typename VoxelType>
	template <typename DerivedVolumeType>
	void BaseVolume<VoxelType>::Sampler<DerivedVolumeType>::moveNegativeX(void)
	{
		mXPosInVolume--;
		m_bIsCurrentPositionValidInX = mVolume->getEnclosingRegion().containsPointInX(mXPosInVolume);
	}

	template <typename VoxelType>
	template <typename DerivedVolumeType>
	void BaseVolume<VoxelType>::Sampler<DerivedVolumeType>::moveNegativeY(void)
	{
		mYPosInVolume--;
		m_bIsCurrentPositionValidInY = mVolume->getEnclosingRegion().containsPointInY(mYPosInVolume);
	}

	template <typename VoxelType>
	template <typename DerivedVolumeType>
	void BaseVolume<VoxelType>::Sampler<DerivedVolumeType>::moveNegativeZ(void)
	{
		mZPosInVolume--;
		m_bIsCurrentPositionValidInZ = mVolume->getEnclosingRegion().containsPointInZ(mZPosInVolume);
	}

	template <typename VoxelType>
	template <typename DerivedVolumeType>
	VoxelType BaseVolume<VoxelType>::Sampler<DerivedVolumeType>::peekVoxel1nx1ny1nz(void) const
	{
		return getVoxelImpl(mXPosInVolume - 1, mYPosInVolume - 1, mZPosInVolume - 1);
	}

	template <typename VoxelType>
	template <typename DerivedVolumeType>
	VoxelType BaseVolume<VoxelType>::Sampler<DerivedVolumeType>::peekVoxel1nx1ny0pz(void) const
	{
		return getVoxelImpl(mXPosInVolume - 1, mYPosInVolume - 1, mZPosInVolume    );
	}

	template <typename VoxelType>
	template <typename DerivedVolumeType>
	VoxelType BaseVolume<VoxelType>::Sampler<DerivedVolumeType>::peekVoxel1nx1ny1pz(void) const
	{
		return getVoxelImpl(mXPosInVolume - 1, mYPosInVolume - 1, mZPosInVolume + 1);
	}

	template <typename VoxelType>
	template <typename DerivedVolumeType>
	VoxelType BaseVolume<VoxelType>::Sampler<DerivedVolumeType>::peekVoxel1nx0py1nz(void) const
	{
		return getVoxelImpl(mXPosInVolume - 1, mYPosInVolume    , mZPosInVolume - 1);
	}

	template <typename VoxelType>
	template <typename DerivedVolumeType>
	VoxelType BaseVolume<VoxelType>::Sampler<DerivedVolumeType>::peekVoxel1nx0py0pz(void) const
	{
		return getVoxelImpl(mXPosInVolume - 1, mYPosInVolume    , mZPosInVolume    );
	}

	template <typename VoxelType>
	template <typename DerivedVolumeType>
	VoxelType BaseVolume<VoxelType>::Sampler<DerivedVolumeType>::peekVoxel1nx0py1pz(void) const
	{
		return getVoxelImpl(mXPosInVolume - 1, mYPosInVolume    , mZPosInVolume + 1);
	}

	template <typename VoxelType>
	template <typename DerivedVolumeType>
	VoxelType BaseVolume<VoxelType>::Sampler<DerivedVolumeType>::peekVoxel1nx1py1nz(void) const
	{
		return getVoxelImpl(mXPosInVolume - 1, mYPosInVolume + 1, mZPosInVolume - 1);
	}

	template <typename VoxelType>
	template <typename DerivedVolumeType>
	VoxelType BaseVolume<VoxelType>::Sampler<DerivedVolumeType>::peekVoxel1nx1py0pz(void) const
	{
		return getVoxelImpl(mXPosInVolume - 1, mYPosInVolume + 1, mZPosInVolume    );
	}

	template <typename VoxelType>
	template <typename DerivedVolumeType>
	VoxelType BaseVolume<VoxelType>::Sampler<DerivedVolumeType>::peekVoxel1nx1py1pz(void) const
	{
		return getVoxelImpl(mXPosInVolume - 1, mYPosInVolume + 1, mZPosInVolume + 1);
	}

	//////////////////////////////////////////////////////////////////////////

	template <typename VoxelType>
	template <typename DerivedVolumeType>
	VoxelType BaseVolume<VoxelType>::Sampler<DerivedVolumeType>::peekVoxel0px1ny1nz(void) const
	{
		return getVoxelImpl(mXPosInVolume    , mYPosInVolume - 1, mZPosInVolume - 1);
	}

	template <typename VoxelType>
	template <typename DerivedVolumeType>
	VoxelType BaseVolume<VoxelType>::Sampler<DerivedVolumeType>::peekVoxel0px1ny0pz(void) const
	{
		return getVoxelImpl(mXPosInVolume    , mYPosInVolume - 1, mZPosInVolume    );
	}

	template <typename VoxelType>
	template <typename DerivedVolumeType>
	VoxelType BaseVolume<VoxelType>::Sampler<DerivedVolumeType>::peekVoxel0px1ny1pz(void) const
	{
		return getVoxelImpl(mXPosInVolume    , mYPosInVolume - 1, mZPosInVolume + 1);
	}

	template <typename VoxelType>
	template <typename DerivedVolumeType>
	VoxelType BaseVolume<VoxelType>::Sampler<DerivedVolumeType>::peekVoxel0px0py1nz(void) const
	{
		return getVoxelImpl(mXPosInVolume    , mYPosInVolume    , mZPosInVolume - 1);
	}

	template <typename VoxelType>
	template <typename DerivedVolumeType>
	VoxelType BaseVolume<VoxelType>::Sampler<DerivedVolumeType>::peekVoxel0px0py0pz(void) const
	{
		return getVoxelImpl(mXPosInVolume    , mYPosInVolume    , mZPosInVolume    );
	}

	template <typename VoxelType>
	template <typename DerivedVolumeType>
	VoxelType BaseVolume<VoxelType>::Sampler<DerivedVolumeType>::peekVoxel0px0py1pz(void) const
	{
		return getVoxelImpl(mXPosInVolume    , mYPosInVolume    , mZPosInVolume + 1);
	}

	template <typename VoxelType>
	template <typename DerivedVolumeType>
	VoxelType BaseVolume<VoxelType>::Sampler<DerivedVolumeType>::peekVoxel0px1py1nz(void) const
	{
		return getVoxelImpl(mXPosInVolume    , mYPosInVolume + 1, mZPosInVolume - 1);
	}

	template <typename VoxelType>
	template <typename DerivedVolumeType>
	VoxelType BaseVolume<VoxelType>::Sampler<DerivedVolumeType>::peekVoxel0px1py0pz(void) const
	{
		return getVoxelImpl(mXPosInVolume    , mYPosInVolume + 1, mZPosInVolume    );
	}

	template <typename VoxelType>
	template <typename DerivedVolumeType>
	VoxelType BaseVolume<VoxelType>::Sampler<DerivedVolumeType>::peekVoxel0px1py1pz(void) const
	{
		return getVoxelImpl(mXPosInVolume    , mYPosInVolume + 1, mZPosInVolume + 1);
	}

	//////////////////////////////////////////////////////////////////////////

	template <typename VoxelType>
	template <typename DerivedVolumeType>
	VoxelType BaseVolume<VoxelType>::Sampler<DerivedVolumeType>::peekVoxel1px1ny1nz(void) const
	{
		return getVoxelImpl(mXPosInVolume + 1, mYPosInVolume - 1, mZPosInVolume - 1);
	}

	template <typename VoxelType>
	template <typename DerivedVolumeType>
	VoxelType BaseVolume<VoxelType>::Sampler<DerivedVolumeType>::peekVoxel1px1ny0pz(void) const
	{
		return getVoxelImpl(mXPosInVolume + 1, mYPosInVolume - 1, mZPosInVolume    );
	}

	template <typename VoxelType>
	template <typename DerivedVolumeType>
	VoxelType BaseVolume<VoxelType>::Sampler<DerivedVolumeType>::peekVoxel1px1ny1pz(void) const
	{
		return getVoxelImpl(mXPosInVolume + 1, mYPosInVolume - 1, mZPosInVolume + 1);
	}

	template <typename VoxelType>
	template <typename DerivedVolumeType>
	VoxelType BaseVolume<VoxelType>::Sampler<DerivedVolumeType>::peekVoxel1px0py1nz(void) const
	{
		return getVoxelImpl(mXPosInVolume + 1, mYPosInVolume    , mZPosInVolume - 1);
	}

	template <typename VoxelType>
	template <typename DerivedVolumeType>
	VoxelType BaseVolume<VoxelType>::Sampler<DerivedVolumeType>::peekVoxel1px0py0pz(void) const
	{
		return getVoxelImpl(mXPosInVolume + 1, mYPosInVolume    , mZPosInVolume    );
	}

	template <typename VoxelType>
	template <typename DerivedVolumeType>
	VoxelType BaseVolume<VoxelType>::Sampler<DerivedVolumeType>::peekVoxel1px0py1pz(void) const
	{
		return getVoxelImpl(mXPosInVolume + 1, mYPosInVolume    , mZPosInVolume + 1);
	}

	template <typename VoxelType>
	template <typename DerivedVolumeType>
	VoxelType BaseVolume<VoxelType>::Sampler<DerivedVolumeType>::peekVoxel1px1py1nz(void) const
	{
		return getVoxelImpl(mXPosInVolume + 1, mYPosInVolume + 1, mZPosInVolume - 1);
	}

	template <typename VoxelType>
	template <typename DerivedVolumeType>
	VoxelType BaseVolume<VoxelType>::Sampler<DerivedVolumeType>::peekVoxel1px1py0pz(void) const
	{
		return getVoxelImpl(mXPosInVolume + 1, mYPosInVolume + 1, mZPosInVolume    );
	}

	template <typename VoxelType>
	template <typename DerivedVolumeType>
	VoxelType BaseVolume<VoxelType>::Sampler<DerivedVolumeType>::peekVoxel1px1py1pz(void) const
	{
		return getVoxelImpl(mXPosInVolume + 1, mYPosInVolume + 1, mZPosInVolume + 1);
	}

	template <typename VoxelType>
	template <typename DerivedVolumeType>
	VoxelType BaseVolume<VoxelType>::Sampler<DerivedVolumeType>::getVoxelImpl(int32_t uXPos, int32_t uYPos, int32_t uZPos) const
	{
		switch(m_eWrapMode)
		{
		case WrapModes::None:
			return mVolume->getVoxel<WrapModes::None>(uXPos, uYPos, uZPos, m_tBorder);
		case WrapModes::Clamp:
			return mVolume->getVoxel<WrapModes::Clamp>(uXPos, uYPos, uZPos, m_tBorder);
		case WrapModes::Border:
			return mVolume->getVoxel<WrapModes::Border>(uXPos, uYPos, uZPos, m_tBorder);
		case WrapModes::DontCheck:
			return mVolume->getVoxel<WrapModes::DontCheck>(uXPos, uYPos, uZPos, m_tBorder);
		default:
			// Should never happen
			POLYVOX_ASSERT(false, "Invalid wrap mode");
			return VoxelType();
		}
	}
}

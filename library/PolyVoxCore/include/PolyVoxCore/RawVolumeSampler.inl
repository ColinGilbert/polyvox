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
#include "PolyVoxCore/RawVolume.h"
#include "PolyVoxCore/Vector.h"
#include "PolyVoxCore/Region.h"

#include <limits>
namespace PolyVox
{
	template <typename VoxelType>
	RawVolume<VoxelType>::Sampler::Sampler(RawVolume<VoxelType>* volume)
		:mVolume(volume)
		,mXPosInVolume(0)
		,mYPosInVolume(0)
		,mZPosInVolume(0)
		,mCurrentVoxel(0)
		,m_bIsCurrentPositionValid(false)
	{
	}

	template <typename VoxelType>
	RawVolume<VoxelType>::Sampler::~Sampler()
	{
	}

	template <typename VoxelType>
	typename RawVolume<VoxelType>::Sampler& RawVolume<VoxelType>::Sampler::operator=(const typename RawVolume<VoxelType>::Sampler& rhs) throw()
	{
		if(this == &rhs)
		{
			return *this;
		}
        mVolume = rhs.mVolume;
		mXPosInVolume = rhs.mXPosInVolume;
		mYPosInVolume = rhs.mYPosInVolume;
		mZPosInVolume = rhs.mZPosInVolume;
		mCurrentVoxel = rhs.mCurrentVoxel;
        return *this;
	}

	template <typename VoxelType>
	int32_t RawVolume<VoxelType>::Sampler::getPosX(void) const
	{
		return mXPosInVolume;
	}

	template <typename VoxelType>
	int32_t RawVolume<VoxelType>::Sampler::getPosY(void) const
	{
		return mYPosInVolume;
	}

	template <typename VoxelType>
	int32_t RawVolume<VoxelType>::Sampler::getPosZ(void) const
	{
		return mZPosInVolume;
	}

	template <typename VoxelType>
	const RawVolume<VoxelType>* RawVolume<VoxelType>::Sampler::getVolume(void) const
	{
		return mVolume;
	}

	template <typename VoxelType>
	VoxelType RawVolume<VoxelType>::Sampler::getVoxel(void) const
	{
		return m_bIsCurrentPositionValid ? *mCurrentVoxel : mVolume->m_tBorderValue;
	}

	template <typename VoxelType>
	void RawVolume<VoxelType>::Sampler::setPosition(const Vector3DInt32& v3dNewPos)
	{
		setPosition(v3dNewPos.getX(), v3dNewPos.getY(), v3dNewPos.getZ());
	}

	template <typename VoxelType>
	void RawVolume<VoxelType>::Sampler::setPosition(int32_t xPos, int32_t yPos, int32_t zPos)
	{
		mXPosInVolume = xPos;
		mYPosInVolume = yPos;
		mZPosInVolume = zPos;

		const uint32_t uVoxelIndex = xPos + 
				yPos * mVolume->getWidth() + 
				zPos * mVolume->getWidth() * mVolume->getHeight();

		mCurrentVoxel = mVolume->m_pData + uVoxelIndex;

		m_bIsCurrentPositionValid = mVolume->m_regValidRegion.containsPoint(Vector3DInt32(xPos, yPos, zPos));
	}

	template <typename VoxelType>
	void RawVolume<VoxelType>::Sampler::movePositiveX(void)
	{
		mXPosInVolume++;
		++mCurrentVoxel;
		m_bIsCurrentPositionValid = mXPosInVolume <= mVolume->getEnclosingRegion().getUpperCorner().getX();
	}

	template <typename VoxelType>
	void RawVolume<VoxelType>::Sampler::movePositiveY(void)
	{
		mYPosInVolume++;
		mCurrentVoxel += mVolume->getWidth();
		m_bIsCurrentPositionValid = mYPosInVolume <= mVolume->getEnclosingRegion().getUpperCorner().getY();
	}

	template <typename VoxelType>
	void RawVolume<VoxelType>::Sampler::movePositiveZ(void)
	{
		mZPosInVolume--;
		mCurrentVoxel += mVolume->getWidth() * mVolume->getHeight();
		m_bIsCurrentPositionValid = mZPosInVolume <= mVolume->getEnclosingRegion().getUpperCorner().getZ();
	}

	template <typename VoxelType>
	void RawVolume<VoxelType>::Sampler::moveNegativeX(void)
	{
		mXPosInVolume--;
		--mCurrentVoxel;
		m_bIsCurrentPositionValid = mXPosInVolume >= mVolume->getEnclosingRegion().getLowerCorner().getX();
	}

	template <typename VoxelType>
	void RawVolume<VoxelType>::Sampler::moveNegativeY(void)
	{
		mYPosInVolume--;
		mCurrentVoxel -= mVolume->getWidth();
		m_bIsCurrentPositionValid = mYPosInVolume >= mVolume->getEnclosingRegion().getLowerCorner().getY();
	}

	template <typename VoxelType>
	void RawVolume<VoxelType>::Sampler::moveNegativeZ(void)
	{
		mZPosInVolume--;
		mCurrentVoxel -= mVolume->getWidth() * mVolume->getHeight();
		m_bIsCurrentPositionValid =mZPosInVolume >= mVolume->getEnclosingRegion().getLowerCorner().getZ();
	}

	template <typename VoxelType>
	VoxelType RawVolume<VoxelType>::Sampler::peekVoxel1nx1ny1nz(void) const
	{
		return mVolume->getVoxelAt(mXPosInVolume-1,mYPosInVolume-1,mZPosInVolume-1);
	}

	template <typename VoxelType>
	VoxelType RawVolume<VoxelType>::Sampler::peekVoxel1nx1ny0pz(void) const
	{
		return mVolume->getVoxelAt(mXPosInVolume-1,mYPosInVolume-1,mZPosInVolume);
	}

	template <typename VoxelType>
	VoxelType RawVolume<VoxelType>::Sampler::peekVoxel1nx1ny1pz(void) const
	{
		return mVolume->getVoxelAt(mXPosInVolume-1,mYPosInVolume-1,mZPosInVolume+1);
	}

	template <typename VoxelType>
	VoxelType RawVolume<VoxelType>::Sampler::peekVoxel1nx0py1nz(void) const
	{
		return mVolume->getVoxelAt(mXPosInVolume-1,mYPosInVolume,mZPosInVolume-1);
	}

	template <typename VoxelType>
	VoxelType RawVolume<VoxelType>::Sampler::peekVoxel1nx0py0pz(void) const
	{
		return mVolume->getVoxelAt(mXPosInVolume-1,mYPosInVolume,mZPosInVolume);
	}

	template <typename VoxelType>
	VoxelType RawVolume<VoxelType>::Sampler::peekVoxel1nx0py1pz(void) const
	{
		return mVolume->getVoxelAt(mXPosInVolume-1,mYPosInVolume,mZPosInVolume+1);
	}

	template <typename VoxelType>
	VoxelType RawVolume<VoxelType>::Sampler::peekVoxel1nx1py1nz(void) const
	{
		return mVolume->getVoxelAt(mXPosInVolume-1,mYPosInVolume+1,mZPosInVolume-1);
	}

	template <typename VoxelType>
	VoxelType RawVolume<VoxelType>::Sampler::peekVoxel1nx1py0pz(void) const
	{
		return mVolume->getVoxelAt(mXPosInVolume-1,mYPosInVolume+1,mZPosInVolume);
	}

	template <typename VoxelType>
	VoxelType RawVolume<VoxelType>::Sampler::peekVoxel1nx1py1pz(void) const
	{
		return mVolume->getVoxelAt(mXPosInVolume-1,mYPosInVolume+1,mZPosInVolume+1);
	}

	//////////////////////////////////////////////////////////////////////////

	template <typename VoxelType>
	VoxelType RawVolume<VoxelType>::Sampler::peekVoxel0px1ny1nz(void) const
	{
		return mVolume->getVoxelAt(mXPosInVolume,mYPosInVolume-1,mZPosInVolume-1);
	}

	template <typename VoxelType>
	VoxelType RawVolume<VoxelType>::Sampler::peekVoxel0px1ny0pz(void) const
	{
		return mVolume->getVoxelAt(mXPosInVolume,mYPosInVolume-1,mZPosInVolume);
	}

	template <typename VoxelType>
	VoxelType RawVolume<VoxelType>::Sampler::peekVoxel0px1ny1pz(void) const
	{
		return mVolume->getVoxelAt(mXPosInVolume,mYPosInVolume-1,mZPosInVolume+1);
	}

	template <typename VoxelType>
	VoxelType RawVolume<VoxelType>::Sampler::peekVoxel0px0py1nz(void) const
	{
		return mVolume->getVoxelAt(mXPosInVolume,mYPosInVolume,mZPosInVolume-1);
	}

	template <typename VoxelType>
	VoxelType RawVolume<VoxelType>::Sampler::peekVoxel0px0py0pz(void) const
	{
			return *mCurrentVoxel;
	}

	template <typename VoxelType>
	VoxelType RawVolume<VoxelType>::Sampler::peekVoxel0px0py1pz(void) const
	{
		return mVolume->getVoxelAt(mXPosInVolume,mYPosInVolume,mZPosInVolume+1);
	}

	template <typename VoxelType>
	VoxelType RawVolume<VoxelType>::Sampler::peekVoxel0px1py1nz(void) const
	{
		return mVolume->getVoxelAt(mXPosInVolume,mYPosInVolume+1,mZPosInVolume-1);
	}

	template <typename VoxelType>
	VoxelType RawVolume<VoxelType>::Sampler::peekVoxel0px1py0pz(void) const
	{
		return mVolume->getVoxelAt(mXPosInVolume,mYPosInVolume+1,mZPosInVolume);
	}

	template <typename VoxelType>
	VoxelType RawVolume<VoxelType>::Sampler::peekVoxel0px1py1pz(void) const
	{
		return mVolume->getVoxelAt(mXPosInVolume,mYPosInVolume+1,mZPosInVolume+1);
	}

	//////////////////////////////////////////////////////////////////////////

	template <typename VoxelType>
	VoxelType RawVolume<VoxelType>::Sampler::peekVoxel1px1ny1nz(void) const
	{
		return mVolume->getVoxelAt(mXPosInVolume+1,mYPosInVolume-1,mZPosInVolume-1);
	}

	template <typename VoxelType>
	VoxelType RawVolume<VoxelType>::Sampler::peekVoxel1px1ny0pz(void) const
	{
		return mVolume->getVoxelAt(mXPosInVolume+1,mYPosInVolume-1,mZPosInVolume);
	}

	template <typename VoxelType>
	VoxelType RawVolume<VoxelType>::Sampler::peekVoxel1px1ny1pz(void) const
	{
		return mVolume->getVoxelAt(mXPosInVolume+1,mYPosInVolume-1,mZPosInVolume+1);
	}

	template <typename VoxelType>
	VoxelType RawVolume<VoxelType>::Sampler::peekVoxel1px0py1nz(void) const
	{
		return mVolume->getVoxelAt(mXPosInVolume+1,mYPosInVolume,mZPosInVolume-1);
	}

	template <typename VoxelType>
	VoxelType RawVolume<VoxelType>::Sampler::peekVoxel1px0py0pz(void) const
	{
		return mVolume->getVoxelAt(mXPosInVolume+1,mYPosInVolume,mZPosInVolume);
	}

	template <typename VoxelType>
	VoxelType RawVolume<VoxelType>::Sampler::peekVoxel1px0py1pz(void) const
	{
		return mVolume->getVoxelAt(mXPosInVolume+1,mYPosInVolume,mZPosInVolume+1);
	}

	template <typename VoxelType>
	VoxelType RawVolume<VoxelType>::Sampler::peekVoxel1px1py1nz(void) const
	{
		return mVolume->getVoxelAt(mXPosInVolume+1,mYPosInVolume+1,mZPosInVolume-1);
	}

	template <typename VoxelType>
	VoxelType RawVolume<VoxelType>::Sampler::peekVoxel1px1py0pz(void) const
	{
		return mVolume->getVoxelAt(mXPosInVolume+1,mYPosInVolume+1,mZPosInVolume);
	}

	template <typename VoxelType>
	VoxelType RawVolume<VoxelType>::Sampler::peekVoxel1px1py1pz(void) const
	{
		return mVolume->getVoxelAt(mXPosInVolume+1,mYPosInVolume+1,mZPosInVolume+1);
	}
}

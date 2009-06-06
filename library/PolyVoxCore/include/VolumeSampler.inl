#pragma region License
/******************************************************************************
This file is part of the PolyVox library
Copyright (C) 2006  David Williams

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
******************************************************************************/
#pragma endregion

#pragma region Headers
#include "PolyVoxImpl/Block.h"
#include "Volume.h"
#include "Vector.h"
#include "Region.h"

#include <limits>
#pragma endregion

namespace PolyVox
{
	#pragma region Constructors/Destructors
	template <typename VoxelType>
	VolumeSampler<VoxelType>::VolumeSampler(Volume<VoxelType>& volume)
		:mVolume(volume)
	{
	}

	template <typename VoxelType>
	VolumeSampler<VoxelType>::~VolumeSampler()
	{
	}
	#pragma endregion

	#pragma region Operators
	template <typename VoxelType>
	bool VolumeSampler<VoxelType>::operator==(const VolumeSampler<VoxelType>& rhs)
	{
		//We could just check whether the two mCurrentVoxel pointers are equal, but this may not
		//be safe in the future if we decide to allow blocks to be shared between volumes
		//So we really check whether the positions are the same.
		//NOTE: With all iterator comparisons it is the users job to ensure they at least point
		//to the same volume. Otherwise they are not comparible.
		assert(&mVolume == &rhs.mVolume);
		return
		(
			(mXPosInVolume == rhs.mXPosInVolume) &&
			(mYPosInVolume == rhs.mYPosInVolume) &&
			(mZPosInVolume == rhs.mZPosInVolume)
		);
	}

	template <typename VoxelType>
	bool VolumeSampler<VoxelType>::operator<(const VolumeSampler<VoxelType>& rhs)
	{
		assert(&mVolume == &rhs.mVolume);

		if(mZPosInVolume < rhs.mZPosInVolume)
			return true;
		if(mZPosInVolume > rhs.mZPosInVolume)
			return false;

		if(mYPosInVolume < rhs.mYPosInVolume)
			return true;
		if(mYPosInVolume > rhs.mYPosInVolume)
			return false;

		if(mXPosInVolume < rhs.mXPosInVolume)
			return true;
		if(mXPosInVolume > rhs.mXPosInVolume)
			return false;

		return false;
	}

	template <typename VoxelType>
	bool VolumeSampler<VoxelType>::operator>(const VolumeSampler<VoxelType>& rhs)
	{
		assert(&mVolume == &rhs.mVolume);
		return (rhs < *this);
	}

	template <typename VoxelType>
	bool VolumeSampler<VoxelType>::operator<=(const VolumeSampler<VoxelType>& rhs)
	{
		assert(&mVolume == &rhs.mVolume);
		return (rhs > *this);
	}

	template <typename VoxelType>
	bool VolumeSampler<VoxelType>::operator>=(const VolumeSampler<VoxelType>& rhs)
	{
		assert(&mVolume == &rhs.mVolume);
		return (rhs < *this);
	}
	#pragma endregion

	#pragma region Getters
	template <typename VoxelType>
	uint16_t VolumeSampler<VoxelType>::getPosX(void) const
	{
		return mXPosInVolume;
	}

	template <typename VoxelType>
	uint16_t VolumeSampler<VoxelType>::getPosY(void) const
	{
		return mYPosInVolume;
	}

	template <typename VoxelType>
	uint16_t VolumeSampler<VoxelType>::getPosZ(void) const
	{
		return mZPosInVolume;
	}

	template <typename VoxelType>
	VoxelType VolumeSampler<VoxelType>::getSubSampledVoxel(uint8_t uLevel) const
	{		
		if(uLevel == 0)
		{
			return getVoxel();
		}
		else if(uLevel == 1)
		{
			VoxelType tValue = getVoxel();
			tValue = (std::min)(tValue, peekVoxel1px0py0pz());
			tValue = (std::min)(tValue, peekVoxel0px1py0pz());
			tValue = (std::min)(tValue, peekVoxel1px1py0pz());
			tValue = (std::min)(tValue, peekVoxel0px0py1pz());
			tValue = (std::min)(tValue, peekVoxel1px0py1pz());
			tValue = (std::min)(tValue, peekVoxel0px1py1pz());
			tValue = (std::min)(tValue, peekVoxel1px1py1pz());
			return tValue;
		}
		else
		{
			const uint8_t uSize = 1 << uLevel;

			VoxelType tValue = (std::numeric_limits<VoxelType>::max)();
			for(uint8_t z = 0; z < uSize; ++z)
			{
				for(uint8_t y = 0; y < uSize; ++y)
				{
					for(uint8_t x = 0; x < uSize; ++x)
					{
						tValue = (std::min)(tValue, mVolume.getVoxelAt(mXPosInVolume + x, mYPosInVolume + y, mZPosInVolume + z));
					}
				}
			}
			return tValue;
		}
	}

	template <typename VoxelType>
	const Volume<VoxelType>& VolumeSampler<VoxelType>::getVolume(void) const
	{
		return mVolume;
	}

	template <typename VoxelType>
	VoxelType VolumeSampler<VoxelType>::getVoxel(void) const
	{
		return *mCurrentVoxel;
	}
	#pragma endregion

	#pragma region Setters
	template <typename VoxelType>
	void VolumeSampler<VoxelType>::setPosition(const Vector3DInt16& v3dNewPos)
	{
		setPosition(v3dNewPos.getX(), v3dNewPos.getY(), v3dNewPos.getZ());
	}

	template <typename VoxelType>
	void VolumeSampler<VoxelType>::setPosition(uint16_t xPos, uint16_t yPos, uint16_t zPos)
	{
		mXPosInVolume = xPos;
		mYPosInVolume = yPos;
		mZPosInVolume = zPos;

		const uint16_t uXBlock = mXPosInVolume >> mVolume.m_uBlockSideLengthPower;
		const uint16_t uYBlock = mYPosInVolume >> mVolume.m_uBlockSideLengthPower;
		const uint16_t uZBlock = mZPosInVolume >> mVolume.m_uBlockSideLengthPower;

		const uint16_t uXPosInBlock = mXPosInVolume - (uXBlock << mVolume.m_uBlockSideLengthPower);
		const uint16_t uYPosInBlock = mYPosInVolume - (uYBlock << mVolume.m_uBlockSideLengthPower);
		const uint16_t uZPosInBlock = mZPosInVolume - (uZBlock << mVolume.m_uBlockSideLengthPower);

		const uint32_t uBlockIndexInVolume = uXBlock + 
			uYBlock * mVolume.m_uWidthInBlocks + 
			uZBlock * mVolume.m_uWidthInBlocks * mVolume.m_uHeightInBlocks;
		const POLYVOX_SHARED_PTR< Block<VoxelType> >& currentBlock = mVolume.m_pBlocks[uBlockIndexInVolume];

		const uint32_t uVoxelIndexInBlock = uXPosInBlock + 
			uYPosInBlock * mVolume.m_uBlockSideLength + 
			uZPosInBlock * mVolume.m_uBlockSideLength * mVolume.m_uBlockSideLength;
		mCurrentVoxel = currentBlock->m_tData + uVoxelIndexInBlock;
	}
	#pragma endregion

	#pragma region Other
	template <typename VoxelType>
	void VolumeSampler<VoxelType>::movePositiveX(void)
	{
		assert(mXPosInVolume < mVolume.m_uWidth - 1);

		//Note the *post* increament here
		if((mXPosInVolume++) % mVolume.m_uBlockSideLength == 0)
		{
			//No need to compute new block.
			++mCurrentVoxel;			
		}
		else
		{
			//We've hit the block boundary. Just calling setPosition() is the easiest way to resolve this.
			setPosition(mXPosInVolume, mYPosInVolume, mZPosInVolume);
		}
	}

	template <typename VoxelType>
	void VolumeSampler<VoxelType>::movePositiveY(void)
	{
		assert(mYPosInVolume < mVolume.m_uHeight - 1);

		//Note the *post* increament here
		if((mYPosInVolume++) % mVolume.m_uBlockSideLength == 0)
		{
			//No need to compute new block.
			mCurrentVoxel += mVolume.m_uBlockSideLength;
		}
		else
		{
			//We've hit the block boundary. Just calling setPosition() is the easiest way to resolve this.
			setPosition(mXPosInVolume, mYPosInVolume, mZPosInVolume);
		}
	}

	template <typename VoxelType>
	void VolumeSampler<VoxelType>::movePositiveZ(void)
	{
		assert(mZPosInVolume < mVolume.m_uDepth - 1);

		//Note the *post* increament here
		if((mZPosInVolume++) % mVolume.m_uBlockSideLength == 0)
		{
			//No need to compute new block.
			mCurrentVoxel += mVolume.m_uBlockSideLength * mVolume.m_uBlockSideLength;
		}
		else
		{
			//We've hit the block boundary. Just calling setPosition() is the easiest way to resolve this.
			setPosition(mXPosInVolume, mYPosInVolume, mZPosInVolume);
		}
	}

	template <typename VoxelType>
	void VolumeSampler<VoxelType>::moveNegativeX(void)
	{
		assert(mXPosInVolume > 0);

		//Note the *pre* increament here
		if((--mXPosInVolume) % mVolume.m_uBlockSideLength == 0)
		{
			//No need to compute new block.
			++mCurrentVoxel;			
		}
		else
		{
			//We've hit the block boundary. Just calling setPosition() is the easiest way to resolve this.
			setPosition(mXPosInVolume, mYPosInVolume, mZPosInVolume);
		}
	}

	template <typename VoxelType>
	void VolumeSampler<VoxelType>::moveNegativeY(void)
	{
		assert(mYPosInVolume > 0);

		//Note the *pre* increament here
		if((--mYPosInVolume) % mVolume.m_uBlockSideLength == 0)
		{
			//No need to compute new block.
			mCurrentVoxel += mVolume.m_uBlockSideLength;
		}
		else
		{
			//We've hit the block boundary. Just calling setPosition() is the easiest way to resolve this.
			setPosition(mXPosInVolume, mYPosInVolume, mZPosInVolume);
		}
	}

	template <typename VoxelType>
	void VolumeSampler<VoxelType>::moveNegativeZ(void)
	{
		assert(mZPosInVolume > 0);

		//Note the *pre* increament here
		if((--mZPosInVolume) % mVolume.m_uBlockSideLength == 0)
		{
			//No need to compute new block.
			mCurrentVoxel += mVolume.m_uBlockSideLength * mVolume.m_uBlockSideLength;
		}
		else
		{
			//We've hit the block boundary. Just calling setPosition() is the easiest way to resolve this.
			setPosition(mXPosInVolume, mYPosInVolume, mZPosInVolume);
		}
	}
	#pragma endregion

	#pragma region Peekers
	template <typename VoxelType>
	VoxelType VolumeSampler<VoxelType>::peekVoxel1nx1ny1nz(void) const
	{
		if((mXPosInVolume%mVolume.m_uBlockSideLength != 0) && (mYPosInVolume%mVolume.m_uBlockSideLength != 0) && (mZPosInVolume%mVolume.m_uBlockSideLength != 0))
		{
			return *(mCurrentVoxel - 1 - mVolume.m_uBlockSideLength - mVolume.m_uBlockSideLength*mVolume.m_uBlockSideLength);
		}
		return mVolume.getVoxelAt(mXPosInVolume-1,mYPosInVolume-1,mZPosInVolume-1);
	}

	template <typename VoxelType>
	VoxelType VolumeSampler<VoxelType>::peekVoxel1nx1ny0pz(void) const
	{
		if((mXPosInVolume%mVolume.m_uBlockSideLength != 0) && (mYPosInVolume%mVolume.m_uBlockSideLength != 0))
		{
			return *(mCurrentVoxel - 1 - mVolume.m_uBlockSideLength);
		}
		return mVolume.getVoxelAt(mXPosInVolume-1,mYPosInVolume-1,mZPosInVolume);
	}

	template <typename VoxelType>
	VoxelType VolumeSampler<VoxelType>::peekVoxel1nx1ny1pz(void) const
	{
		if((mXPosInVolume%mVolume.m_uBlockSideLength != 0) && (mYPosInVolume%mVolume.m_uBlockSideLength != 0) && (mZPosInVolume%mVolume.m_uBlockSideLength != mVolume.m_uBlockSideLength-1))
		{
			return *(mCurrentVoxel - 1 - mVolume.m_uBlockSideLength + mVolume.m_uBlockSideLength*mVolume.m_uBlockSideLength);
		}
		return mVolume.getVoxelAt(mXPosInVolume-1,mYPosInVolume-1,mZPosInVolume+1);
	}

	template <typename VoxelType>
	VoxelType VolumeSampler<VoxelType>::peekVoxel1nx0py1nz(void) const
	{
		if((mXPosInVolume%mVolume.m_uBlockSideLength != 0) && (mZPosInVolume%mVolume.m_uBlockSideLength != 0))
		{
			return *(mCurrentVoxel - 1 - mVolume.m_uBlockSideLength*mVolume.m_uBlockSideLength);
		}
		return mVolume.getVoxelAt(mXPosInVolume-1,mYPosInVolume,mZPosInVolume-1);
	}

	template <typename VoxelType>
	VoxelType VolumeSampler<VoxelType>::peekVoxel1nx0py0pz(void) const
	{
		if((mXPosInVolume%mVolume.m_uBlockSideLength != 0))
		{
			return *(mCurrentVoxel - 1);
		}
		return mVolume.getVoxelAt(mXPosInVolume-1,mYPosInVolume,mZPosInVolume);
	}

	template <typename VoxelType>
	VoxelType VolumeSampler<VoxelType>::peekVoxel1nx0py1pz(void) const
	{
		if((mXPosInVolume%mVolume.m_uBlockSideLength != 0) && (mZPosInVolume%mVolume.m_uBlockSideLength != mVolume.m_uBlockSideLength-1))
		{
			return *(mCurrentVoxel - 1 + mVolume.m_uBlockSideLength*mVolume.m_uBlockSideLength);
		}
		return mVolume.getVoxelAt(mXPosInVolume-1,mYPosInVolume,mZPosInVolume+1);
	}

	template <typename VoxelType>
	VoxelType VolumeSampler<VoxelType>::peekVoxel1nx1py1nz(void) const
	{
		if((mXPosInVolume%mVolume.m_uBlockSideLength != 0) && (mYPosInVolume%mVolume.m_uBlockSideLength != mVolume.m_uBlockSideLength-1) && (mZPosInVolume%mVolume.m_uBlockSideLength != 0))
		{
			return *(mCurrentVoxel - 1 + mVolume.m_uBlockSideLength - mVolume.m_uBlockSideLength*mVolume.m_uBlockSideLength);
		}
		return mVolume.getVoxelAt(mXPosInVolume-1,mYPosInVolume+1,mZPosInVolume-1);
	}

	template <typename VoxelType>
	VoxelType VolumeSampler<VoxelType>::peekVoxel1nx1py0pz(void) const
	{
		if((mXPosInVolume%mVolume.m_uBlockSideLength != 0) && (mYPosInVolume%mVolume.m_uBlockSideLength != mVolume.m_uBlockSideLength-1))
		{
			return *(mCurrentVoxel - 1 + mVolume.m_uBlockSideLength);
		}
		return mVolume.getVoxelAt(mXPosInVolume-1,mYPosInVolume+1,mZPosInVolume);
	}

	template <typename VoxelType>
	VoxelType VolumeSampler<VoxelType>::peekVoxel1nx1py1pz(void) const
	{
		if((mXPosInVolume%mVolume.m_uBlockSideLength != 0) && (mYPosInVolume%mVolume.m_uBlockSideLength != mVolume.m_uBlockSideLength-1) && (mZPosInVolume%mVolume.m_uBlockSideLength != mVolume.m_uBlockSideLength-1))
		{
			return *(mCurrentVoxel - 1 + mVolume.m_uBlockSideLength + mVolume.m_uBlockSideLength*mVolume.m_uBlockSideLength);
		}
		return mVolume.getVoxelAt(mXPosInVolume-1,mYPosInVolume+1,mZPosInVolume+1);
	}

	//////////////////////////////////////////////////////////////////////////

	template <typename VoxelType>
	VoxelType VolumeSampler<VoxelType>::peekVoxel0px1ny1nz(void) const
	{
		if((mYPosInVolume%mVolume.m_uBlockSideLength != 0) && (mZPosInVolume%mVolume.m_uBlockSideLength != 0))
		{
			return *(mCurrentVoxel - mVolume.m_uBlockSideLength - mVolume.m_uBlockSideLength*mVolume.m_uBlockSideLength);
		}
		return mVolume.getVoxelAt(mXPosInVolume,mYPosInVolume-1,mZPosInVolume-1);
	}

	template <typename VoxelType>
	VoxelType VolumeSampler<VoxelType>::peekVoxel0px1ny0pz(void) const
	{
		if((mYPosInVolume%mVolume.m_uBlockSideLength != 0))
		{
			return *(mCurrentVoxel - mVolume.m_uBlockSideLength);
		}
		return mVolume.getVoxelAt(mXPosInVolume,mYPosInVolume-1,mZPosInVolume);
	}

	template <typename VoxelType>
	VoxelType VolumeSampler<VoxelType>::peekVoxel0px1ny1pz(void) const
	{
		if((mYPosInVolume%mVolume.m_uBlockSideLength != 0) && (mZPosInVolume%mVolume.m_uBlockSideLength != mVolume.m_uBlockSideLength-1))
		{
			return *(mCurrentVoxel - mVolume.m_uBlockSideLength + mVolume.m_uBlockSideLength*mVolume.m_uBlockSideLength);
		}
		return mVolume.getVoxelAt(mXPosInVolume,mYPosInVolume-1,mZPosInVolume+1);
	}

	template <typename VoxelType>
	VoxelType VolumeSampler<VoxelType>::peekVoxel0px0py1nz(void) const
	{
		if((mZPosInVolume%mVolume.m_uBlockSideLength != 0))
		{
			return *(mCurrentVoxel - mVolume.m_uBlockSideLength*mVolume.m_uBlockSideLength);
		}
		return mVolume.getVoxelAt(mXPosInVolume,mYPosInVolume,mZPosInVolume-1);
	}

	template <typename VoxelType>
	VoxelType VolumeSampler<VoxelType>::peekVoxel0px0py0pz(void) const
	{
			return *mCurrentVoxel;
	}

	template <typename VoxelType>
	VoxelType VolumeSampler<VoxelType>::peekVoxel0px0py1pz(void) const
	{
		if((mZPosInVolume%mVolume.m_uBlockSideLength != mVolume.m_uBlockSideLength-1))
		{
			return *(mCurrentVoxel + mVolume.m_uBlockSideLength*mVolume.m_uBlockSideLength);
		}
		return mVolume.getVoxelAt(mXPosInVolume,mYPosInVolume,mZPosInVolume+1);
	}

	template <typename VoxelType>
	VoxelType VolumeSampler<VoxelType>::peekVoxel0px1py1nz(void) const
	{
		if((mYPosInVolume%mVolume.m_uBlockSideLength != mVolume.m_uBlockSideLength-1) && (mZPosInVolume%mVolume.m_uBlockSideLength != 0))
		{
			return *(mCurrentVoxel + mVolume.m_uBlockSideLength - mVolume.m_uBlockSideLength*mVolume.m_uBlockSideLength);
		}
		return mVolume.getVoxelAt(mXPosInVolume,mYPosInVolume+1,mZPosInVolume-1);
	}

	template <typename VoxelType>
	VoxelType VolumeSampler<VoxelType>::peekVoxel0px1py0pz(void) const
	{
		if((mYPosInVolume%mVolume.m_uBlockSideLength != mVolume.m_uBlockSideLength-1))
		{
			return *(mCurrentVoxel + mVolume.m_uBlockSideLength);
		}
		return mVolume.getVoxelAt(mXPosInVolume,mYPosInVolume+1,mZPosInVolume);
	}

	template <typename VoxelType>
	VoxelType VolumeSampler<VoxelType>::peekVoxel0px1py1pz(void) const
	{
		if((mYPosInVolume%mVolume.m_uBlockSideLength != mVolume.m_uBlockSideLength-1) && (mZPosInVolume%mVolume.m_uBlockSideLength != mVolume.m_uBlockSideLength-1))
		{
			return *(mCurrentVoxel + mVolume.m_uBlockSideLength + mVolume.m_uBlockSideLength*mVolume.m_uBlockSideLength);
		}
		return mVolume.getVoxelAt(mXPosInVolume,mYPosInVolume+1,mZPosInVolume+1);
	}

	//////////////////////////////////////////////////////////////////////////

	template <typename VoxelType>
	VoxelType VolumeSampler<VoxelType>::peekVoxel1px1ny1nz(void) const
	{
		if((mXPosInVolume%mVolume.m_uBlockSideLength != mVolume.m_uBlockSideLength-1) && (mYPosInVolume%mVolume.m_uBlockSideLength != 0) && (mZPosInVolume%mVolume.m_uBlockSideLength != 0))
		{
			return *(mCurrentVoxel + 1 - mVolume.m_uBlockSideLength - mVolume.m_uBlockSideLength*mVolume.m_uBlockSideLength);
		}
		return mVolume.getVoxelAt(mXPosInVolume+1,mYPosInVolume-1,mZPosInVolume-1);
	}

	template <typename VoxelType>
	VoxelType VolumeSampler<VoxelType>::peekVoxel1px1ny0pz(void) const
	{
		if((mXPosInVolume%mVolume.m_uBlockSideLength != mVolume.m_uBlockSideLength-1) && (mYPosInVolume%mVolume.m_uBlockSideLength != 0))
		{
			return *(mCurrentVoxel + 1 - mVolume.m_uBlockSideLength);
		}
		return mVolume.getVoxelAt(mXPosInVolume+1,mYPosInVolume-1,mZPosInVolume);
	}

	template <typename VoxelType>
	VoxelType VolumeSampler<VoxelType>::peekVoxel1px1ny1pz(void) const
	{
		if((mXPosInVolume%mVolume.m_uBlockSideLength != mVolume.m_uBlockSideLength-1) && (mYPosInVolume%mVolume.m_uBlockSideLength != 0) && (mZPosInVolume%mVolume.m_uBlockSideLength != mVolume.m_uBlockSideLength-1))
		{
			return *(mCurrentVoxel + 1 - mVolume.m_uBlockSideLength + mVolume.m_uBlockSideLength*mVolume.m_uBlockSideLength);
		}
		return mVolume.getVoxelAt(mXPosInVolume+1,mYPosInVolume-1,mZPosInVolume+1);
	}

	template <typename VoxelType>
	VoxelType VolumeSampler<VoxelType>::peekVoxel1px0py1nz(void) const
	{
		if((mXPosInVolume%mVolume.m_uBlockSideLength != mVolume.m_uBlockSideLength-1) && (mZPosInVolume%mVolume.m_uBlockSideLength != 0))
		{
			return *(mCurrentVoxel + 1 - mVolume.m_uBlockSideLength*mVolume.m_uBlockSideLength);
		}
		return mVolume.getVoxelAt(mXPosInVolume+1,mYPosInVolume,mZPosInVolume-1);
	}

	template <typename VoxelType>
	VoxelType VolumeSampler<VoxelType>::peekVoxel1px0py0pz(void) const
	{
		if((mXPosInVolume%mVolume.m_uBlockSideLength != mVolume.m_uBlockSideLength-1))
		{
			return *(mCurrentVoxel + 1);
		}
		return mVolume.getVoxelAt(mXPosInVolume+1,mYPosInVolume,mZPosInVolume);
	}

	template <typename VoxelType>
	VoxelType VolumeSampler<VoxelType>::peekVoxel1px0py1pz(void) const
	{
		if((mXPosInVolume%mVolume.m_uBlockSideLength != mVolume.m_uBlockSideLength-1) && (mZPosInVolume%mVolume.m_uBlockSideLength != mVolume.m_uBlockSideLength-1))
		{
			return *(mCurrentVoxel + 1 + mVolume.m_uBlockSideLength*mVolume.m_uBlockSideLength);
		}
		return mVolume.getVoxelAt(mXPosInVolume+1,mYPosInVolume,mZPosInVolume+1);
	}

	template <typename VoxelType>
	VoxelType VolumeSampler<VoxelType>::peekVoxel1px1py1nz(void) const
	{
		if((mXPosInVolume%mVolume.m_uBlockSideLength != mVolume.m_uBlockSideLength-1) && (mYPosInVolume%mVolume.m_uBlockSideLength != mVolume.m_uBlockSideLength-1) && (mZPosInVolume%mVolume.m_uBlockSideLength != 0))
		{
			return *(mCurrentVoxel + 1 + mVolume.m_uBlockSideLength - mVolume.m_uBlockSideLength*mVolume.m_uBlockSideLength);
		}
		return mVolume.getVoxelAt(mXPosInVolume+1,mYPosInVolume+1,mZPosInVolume-1);
	}

	template <typename VoxelType>
	VoxelType VolumeSampler<VoxelType>::peekVoxel1px1py0pz(void) const
	{
		if((mXPosInVolume%mVolume.m_uBlockSideLength != mVolume.m_uBlockSideLength-1) && (mYPosInVolume%mVolume.m_uBlockSideLength != mVolume.m_uBlockSideLength-1))
		{
			return *(mCurrentVoxel + 1 + mVolume.m_uBlockSideLength);
		}
		return mVolume.getVoxelAt(mXPosInVolume+1,mYPosInVolume+1,mZPosInVolume);
	}

	template <typename VoxelType>
	VoxelType VolumeSampler<VoxelType>::peekVoxel1px1py1pz(void) const
	{
		if((mXPosInVolume%mVolume.m_uBlockSideLength != mVolume.m_uBlockSideLength-1) && (mYPosInVolume%mVolume.m_uBlockSideLength != mVolume.m_uBlockSideLength-1) && (mZPosInVolume%mVolume.m_uBlockSideLength != mVolume.m_uBlockSideLength-1))
		{
			return *(mCurrentVoxel + 1 + mVolume.m_uBlockSideLength + mVolume.m_uBlockSideLength*mVolume.m_uBlockSideLength);
		}
		return mVolume.getVoxelAt(mXPosInVolume+1,mYPosInVolume+1,mZPosInVolume+1);
	}
	#pragma endregion
}

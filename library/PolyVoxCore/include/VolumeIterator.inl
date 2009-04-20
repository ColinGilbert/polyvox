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
#include "Block.h"
#include "Volume.h"
#include "Vector.h"
#include "Region.h"

#include <limits>
#pragma endregion

namespace PolyVox
{
	#pragma region Constructors/Destructors
	template <typename VoxelType>
	VolumeIterator<VoxelType>::VolumeIterator(Volume<VoxelType>& volume)
		:mVolume(volume)
	{
	}

	template <typename VoxelType>
	VolumeIterator<VoxelType>::~VolumeIterator()
	{
	}
	#pragma endregion

	#pragma region Operators
	template <typename VoxelType>
	bool VolumeIterator<VoxelType>::operator==(const VolumeIterator<VoxelType>& rhs)
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
	bool VolumeIterator<VoxelType>::operator<(const VolumeIterator<VoxelType>& rhs)
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
	bool VolumeIterator<VoxelType>::operator>(const VolumeIterator<VoxelType>& rhs)
	{
		assert(&mVolume == &rhs.mVolume);
		return (rhs < *this);
	}

	template <typename VoxelType>
	bool VolumeIterator<VoxelType>::operator<=(const VolumeIterator<VoxelType>& rhs)
	{
		assert(&mVolume == &rhs.mVolume);
		return (rhs > *this);
	}

	template <typename VoxelType>
	bool VolumeIterator<VoxelType>::operator>=(const VolumeIterator<VoxelType>& rhs)
	{
		assert(&mVolume == &rhs.mVolume);
		return (rhs < *this);
	}
	#pragma endregion

	#pragma region Getters
	template <typename VoxelType>
	uint16_t VolumeIterator<VoxelType>::getPosX(void) const
	{
		return mXPosInVolume;
	}

	template <typename VoxelType>
	uint16_t VolumeIterator<VoxelType>::getPosY(void) const
	{
		return mYPosInVolume;
	}

	template <typename VoxelType>
	uint16_t VolumeIterator<VoxelType>::getPosZ(void) const
	{
		return mZPosInVolume;
	}

	template <typename VoxelType>
	VoxelType VolumeIterator<VoxelType>::getSubSampledVoxel(uint8_t uLevel) const
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
	const Volume<VoxelType>& VolumeIterator<VoxelType>::getVolume(void) const
	{
		return mVolume;
	}

	template <typename VoxelType>
	VoxelType VolumeIterator<VoxelType>::getVoxel(void) const
	{
		return *mCurrentVoxel;
	}
	#pragma endregion

	#pragma region Setters
	template <typename VoxelType>
	void VolumeIterator<VoxelType>::setPosition(const Vector3DInt16& v3dNewPos)
	{
		setPosition(v3dNewPos.getX(), v3dNewPos.getY(), v3dNewPos.getZ());
	}

	template <typename VoxelType>
	void VolumeIterator<VoxelType>::setPosition(uint16_t xPos, uint16_t yPos, uint16_t zPos)
	{
		mXPosInVolume = xPos;
		mYPosInVolume = yPos;
		mZPosInVolume = zPos;

		mXBlock = mXPosInVolume >> mVolume.m_uBlockSideLengthPower;
		mYBlock = mYPosInVolume >> mVolume.m_uBlockSideLengthPower;
		mZBlock = mZPosInVolume >> mVolume.m_uBlockSideLengthPower;

		mXPosInBlock = mXPosInVolume - (mXBlock << mVolume.m_uBlockSideLengthPower);
		mYPosInBlock = mYPosInVolume - (mYBlock << mVolume.m_uBlockSideLengthPower);
		mZPosInBlock = mZPosInVolume - (mZBlock << mVolume.m_uBlockSideLengthPower);

		mBlockIndexInVolume = mXBlock + 
			mYBlock * mVolume.m_uWidthInBlocks + 
			mZBlock * mVolume.m_uWidthInBlocks * mVolume.m_uHeightInBlocks;
		POLYVOX_SHARED_PTR< BlockData<VoxelType> > currentBlock = mVolume.m_pBlocks[mBlockIndexInVolume];

		mVoxelIndexInBlock = mXPosInBlock + 
			mYPosInBlock * mVolume.m_uBlockSideLength + 
			mZPosInBlock * mVolume.m_uBlockSideLength * mVolume.m_uBlockSideLength;

		mCurrentVoxel = currentBlock->m_tData + mVoxelIndexInBlock;
	}

	template <typename VoxelType>
	void VolumeIterator<VoxelType>::setValidRegion(const Region& region)
	{
		setValidRegion(region.getLowerCorner().getX(),region.getLowerCorner().getY(),region.getLowerCorner().getZ(),region.getUpperCorner().getX(),region.getUpperCorner().getY(),region.getUpperCorner().getZ());
	}

	template <typename VoxelType>
	void VolumeIterator<VoxelType>::setValidRegion(uint16_t xFirst, uint16_t yFirst, uint16_t zFirst, uint16_t xLast, uint16_t yLast, uint16_t zLast)
	{
		mXRegionFirst = xFirst;
		mYRegionFirst = yFirst;
		mZRegionFirst = zFirst;

		mXRegionLast = xLast;
		mYRegionLast = yLast;
		mZRegionLast = zLast;

		mXRegionFirstBlock = mXRegionFirst >> mVolume.m_uBlockSideLengthPower;
		mYRegionFirstBlock = mYRegionFirst >> mVolume.m_uBlockSideLengthPower;
		mZRegionFirstBlock = mZRegionFirst >> mVolume.m_uBlockSideLengthPower;

		mXRegionLastBlock = mXRegionLast >> mVolume.m_uBlockSideLengthPower;
		mYRegionLastBlock = mYRegionLast >> mVolume.m_uBlockSideLengthPower;
		mZRegionLastBlock = mZRegionLast >> mVolume.m_uBlockSideLengthPower;
	}
	#pragma endregion

	#pragma region Other
	template <typename VoxelType>
	bool VolumeIterator<VoxelType>::isValidForRegion(void) const
	{
		return mIsValidForRegion;
	}

	template <typename VoxelType>
	void VolumeIterator<VoxelType>::moveForwardInRegionXYZFast(void)
	{
		mXPosInBlock++;
		mCurrentVoxel++;
		mXPosInVolume++;
		if((mXPosInBlock == mVolume.m_uBlockSideLength) || (mXPosInVolume > mXRegionLast))
		{
			mXPosInVolume = (std::max)(mXRegionFirst,uint16_t(mXBlock * mVolume.m_uBlockSideLength));
			mXPosInBlock = mXPosInVolume - (mXBlock << mVolume.m_uBlockSideLengthPower);
			mVoxelIndexInBlock = mXPosInBlock + 
				mYPosInBlock * mVolume.m_uBlockSideLength + 
				mZPosInBlock * mVolume.m_uBlockSideLength * mVolume.m_uBlockSideLength;
			boost::shared_ptr< BlockData<VoxelType> > currentBlock = mVolume.m_pBlocks[mBlockIndexInVolume];
			mCurrentVoxel = currentBlock->m_tData + mVoxelIndexInBlock;

			mYPosInBlock++;
			mYPosInVolume++;
			mCurrentVoxel += mVolume.m_uBlockSideLength;
			if((mYPosInBlock == mVolume.m_uBlockSideLength) || (mYPosInVolume > mYRegionLast))
			{
				mYPosInVolume = (std::max)(mYRegionFirst,uint16_t(mYBlock * mVolume.m_uBlockSideLength));
				mYPosInBlock = mYPosInVolume - (mYBlock << mVolume.m_uBlockSideLengthPower);
				mVoxelIndexInBlock = mXPosInBlock + 
					mYPosInBlock * mVolume.m_uBlockSideLength + 
					mZPosInBlock * mVolume.m_uBlockSideLength * mVolume.m_uBlockSideLength;
				BlockData<VoxelType>* currentBlock = mVolume.m_pBlocks[mBlockIndexInVolume];
				mCurrentVoxel = currentBlock->m_tData + mVoxelIndexInBlock;

				mZPosInBlock++;
				mZPosInVolume++;
				mCurrentVoxel += mVolume.m_uBlockSideLength * mVolume.m_uBlockSideLength;

				if((mZPosInBlock == mVolume.m_uBlockSideLength) || (mZPosInVolume > mZRegionLast))
				{
					//At this point we've left the current block. Find a new one...

					++mXBlock;
					++mBlockIndexInVolume;
					if(mXBlock > mXRegionLastBlock)
					{
						mXBlock = mXRegionFirstBlock;
						mBlockIndexInVolume = mXBlock + 
							mYBlock * mVolume.m_uWidthInBlocks + 
							mZBlock * mVolume.m_uWidthInBlocks * mVolume.m_uHeightInBlocks;

						++mYBlock;
						mBlockIndexInVolume += mVolume.m_uWidthInBlocks;
						if(mYBlock > mYRegionLastBlock)
						{
							mYBlock = mYRegionFirstBlock;
							mBlockIndexInVolume = mXBlock + 
								mYBlock * mVolume.m_uSideLengthInBlocks + 
								mZBlock * mVolume.m_uSideLengthInBlocks * mVolume.m_uSideLengthInBlocks;

							++mZBlock;
							mBlockIndexInVolume += mVolume.m_uWidthInBlocks * mVolume.m_uHeightInBlocks;
							if(mZBlock > mZRegionLastBlock)
							{
								mIsValidForRegion = false;
								return;			
							}
						}
					}

					BlockData<VoxelType>* currentBlock = mVolume.m_pBlocks[mBlockIndexInVolume];
					//mCurrentBlock = mVolume->m_pBlocks[mBlockIndexInVolume];					

					mXPosInVolume = (std::max)(mXRegionFirst,uint16_t(mXBlock * mVolume.m_uBlockSideLength));					
					mYPosInVolume = (std::max)(mYRegionFirst,uint16_t(mYBlock * mVolume.m_uBlockSideLength));					
					mZPosInVolume = (std::max)(mZRegionFirst,uint16_t(mZBlock * mVolume.m_uBlockSideLength));					

					mXPosInBlock = mXPosInVolume - (mXBlock << mVolume.m_uBlockSideLengthPower);
					mYPosInBlock = mYPosInVolume - (mYBlock << mVolume.m_uBlockSideLengthPower);
					mZPosInBlock = mZPosInVolume - (mZBlock << mVolume.m_uBlockSideLengthPower);

					mVoxelIndexInBlock = mXPosInBlock + 
						mYPosInBlock * mVolume.m_uBlockSideLength + 
						mZPosInBlock * mVolume.m_uBlockSideLength * mVolume.m_uBlockSideLength;

					mCurrentVoxel = currentBlock->m_tData + mVoxelIndexInBlock;
				}
			}
		}		
	}

	template <typename VoxelType>
	bool VolumeIterator<VoxelType>::moveForwardInRegionXYZ(void)
	{
		if(mXPosInVolume < mXRegionLast)
		{
			++mXPosInVolume;
			if(mXPosInVolume % mVolume.m_uBlockSideLength != 0)
			{
				//No need to compute new block.
				++mVoxelIndexInBlock;
				++mCurrentVoxel;
			}
			else
			{
				//A more complex situation. Just call setPosition().
				setPosition(mXPosInVolume, mYPosInVolume, mZPosInVolume);
			}
		}
		else
		{
			mXPosInVolume = mXRegionFirst;
			if(mYPosInVolume < mYRegionLast)
			{
				++mYPosInVolume;
				//In the case of 'X' we used a trick to avoid calling this evey time. It's hard to use the same
				//trick here because the x position has been reset and so is likely to be in a different block.
				setPosition(mXPosInVolume, mYPosInVolume, mZPosInVolume);
			}
			else
			{
				mYPosInVolume = mYRegionFirst;
				if(mZPosInVolume < mZRegionLast)
				{
					++mZPosInVolume;
					//In the case of 'X' we used a trick to avoid calling this evey time. It's hard to use the same
					//trick here because the x position has been reset and so is likely to be in a different block.
					setPosition(mXPosInVolume, mYPosInVolume, mZPosInVolume);
				}
				else
				{
					//We've hit the end of the region. Reset x and y positions to where they were.
					mXPosInVolume = mXRegionLast;
					mYPosInVolume = mYRegionLast;

					//Return false to indicate we failed to move forward.
					return false;
				}
			}
		}

		return true;
	}
	#pragma endregion

	#pragma region Peekers
	template <typename VoxelType>
	VoxelType VolumeIterator<VoxelType>::peekVoxel1nx1ny1nz(void) const
	{
		if((mXPosInVolume%mVolume.m_uBlockSideLength != 0) && (mYPosInVolume%mVolume.m_uBlockSideLength != 0) && (mZPosInVolume%mVolume.m_uBlockSideLength != 0))
		{
			return *(mCurrentVoxel - 1 - mVolume.m_uBlockSideLength - mVolume.m_uBlockSideLength*mVolume.m_uBlockSideLength);
		}
		return mVolume.getVoxelAt(mXPosInVolume-1,mYPosInVolume-1,mZPosInVolume-1);
	}

	template <typename VoxelType>
	VoxelType VolumeIterator<VoxelType>::peekVoxel1nx1ny0pz(void) const
	{
		if((mXPosInVolume%mVolume.m_uBlockSideLength != 0) && (mYPosInVolume%mVolume.m_uBlockSideLength != 0))
		{
			return *(mCurrentVoxel - 1 - mVolume.m_uBlockSideLength);
		}
		return mVolume.getVoxelAt(mXPosInVolume-1,mYPosInVolume-1,mZPosInVolume);
	}

	template <typename VoxelType>
	VoxelType VolumeIterator<VoxelType>::peekVoxel1nx1ny1pz(void) const
	{
		if((mXPosInVolume%mVolume.m_uBlockSideLength != 0) && (mYPosInVolume%mVolume.m_uBlockSideLength != 0) && (mZPosInVolume%mVolume.m_uBlockSideLength != mVolume.m_uBlockSideLength-1))
		{
			return *(mCurrentVoxel - 1 - mVolume.m_uBlockSideLength + mVolume.m_uBlockSideLength*mVolume.m_uBlockSideLength);
		}
		return mVolume.getVoxelAt(mXPosInVolume-1,mYPosInVolume-1,mZPosInVolume+1);
	}

	template <typename VoxelType>
	VoxelType VolumeIterator<VoxelType>::peekVoxel1nx0py1nz(void) const
	{
		if((mXPosInVolume%mVolume.m_uBlockSideLength != 0) && (mZPosInVolume%mVolume.m_uBlockSideLength != 0))
		{
			return *(mCurrentVoxel - 1 - mVolume.m_uBlockSideLength*mVolume.m_uBlockSideLength);
		}
		return mVolume.getVoxelAt(mXPosInVolume-1,mYPosInVolume,mZPosInVolume-1);
	}

	template <typename VoxelType>
	VoxelType VolumeIterator<VoxelType>::peekVoxel1nx0py0pz(void) const
	{
		if((mXPosInVolume%mVolume.m_uBlockSideLength != 0))
		{
			return *(mCurrentVoxel - 1);
		}
		return mVolume.getVoxelAt(mXPosInVolume-1,mYPosInVolume,mZPosInVolume);
	}

	template <typename VoxelType>
	VoxelType VolumeIterator<VoxelType>::peekVoxel1nx0py1pz(void) const
	{
		if((mXPosInVolume%mVolume.m_uBlockSideLength != 0) && (mZPosInVolume%mVolume.m_uBlockSideLength != mVolume.m_uBlockSideLength-1))
		{
			return *(mCurrentVoxel - 1 + mVolume.m_uBlockSideLength*mVolume.m_uBlockSideLength);
		}
		return mVolume.getVoxelAt(mXPosInVolume-1,mYPosInVolume,mZPosInVolume+1);
	}

	template <typename VoxelType>
	VoxelType VolumeIterator<VoxelType>::peekVoxel1nx1py1nz(void) const
	{
		if((mXPosInVolume%mVolume.m_uBlockSideLength != 0) && (mYPosInVolume%mVolume.m_uBlockSideLength != mVolume.m_uBlockSideLength-1) && (mZPosInVolume%mVolume.m_uBlockSideLength != 0))
		{
			return *(mCurrentVoxel - 1 + mVolume.m_uBlockSideLength - mVolume.m_uBlockSideLength*mVolume.m_uBlockSideLength);
		}
		return mVolume.getVoxelAt(mXPosInVolume-1,mYPosInVolume+1,mZPosInVolume-1);
	}

	template <typename VoxelType>
	VoxelType VolumeIterator<VoxelType>::peekVoxel1nx1py0pz(void) const
	{
		if((mXPosInVolume%mVolume.m_uBlockSideLength != 0) && (mYPosInVolume%mVolume.m_uBlockSideLength != mVolume.m_uBlockSideLength-1))
		{
			return *(mCurrentVoxel - 1 + mVolume.m_uBlockSideLength);
		}
		return mVolume.getVoxelAt(mXPosInVolume-1,mYPosInVolume+1,mZPosInVolume);
	}

	template <typename VoxelType>
	VoxelType VolumeIterator<VoxelType>::peekVoxel1nx1py1pz(void) const
	{
		if((mXPosInVolume%mVolume.m_uBlockSideLength != 0) && (mYPosInVolume%mVolume.m_uBlockSideLength != mVolume.m_uBlockSideLength-1) && (mZPosInVolume%mVolume.m_uBlockSideLength != mVolume.m_uBlockSideLength-1))
		{
			return *(mCurrentVoxel - 1 + mVolume.m_uBlockSideLength + mVolume.m_uBlockSideLength*mVolume.m_uBlockSideLength);
		}
		return mVolume.getVoxelAt(mXPosInVolume-1,mYPosInVolume+1,mZPosInVolume+1);
	}

	//////////////////////////////////////////////////////////////////////////

	template <typename VoxelType>
	VoxelType VolumeIterator<VoxelType>::peekVoxel0px1ny1nz(void) const
	{
		if((mYPosInVolume%mVolume.m_uBlockSideLength != 0) && (mZPosInVolume%mVolume.m_uBlockSideLength != 0))
		{
			return *(mCurrentVoxel - mVolume.m_uBlockSideLength - mVolume.m_uBlockSideLength*mVolume.m_uBlockSideLength);
		}
		return mVolume.getVoxelAt(mXPosInVolume,mYPosInVolume-1,mZPosInVolume-1);
	}

	template <typename VoxelType>
	VoxelType VolumeIterator<VoxelType>::peekVoxel0px1ny0pz(void) const
	{
		if((mYPosInVolume%mVolume.m_uBlockSideLength != 0))
		{
			return *(mCurrentVoxel - mVolume.m_uBlockSideLength);
		}
		return mVolume.getVoxelAt(mXPosInVolume,mYPosInVolume-1,mZPosInVolume);
	}

	template <typename VoxelType>
	VoxelType VolumeIterator<VoxelType>::peekVoxel0px1ny1pz(void) const
	{
		if((mYPosInVolume%mVolume.m_uBlockSideLength != 0) && (mZPosInVolume%mVolume.m_uBlockSideLength != mVolume.m_uBlockSideLength-1))
		{
			return *(mCurrentVoxel - mVolume.m_uBlockSideLength + mVolume.m_uBlockSideLength*mVolume.m_uBlockSideLength);
		}
		return mVolume.getVoxelAt(mXPosInVolume,mYPosInVolume-1,mZPosInVolume+1);
	}

	template <typename VoxelType>
	VoxelType VolumeIterator<VoxelType>::peekVoxel0px0py1nz(void) const
	{
		if((mZPosInVolume%mVolume.m_uBlockSideLength != 0))
		{
			return *(mCurrentVoxel - mVolume.m_uBlockSideLength*mVolume.m_uBlockSideLength);
		}
		return mVolume.getVoxelAt(mXPosInVolume,mYPosInVolume,mZPosInVolume-1);
	}

	template <typename VoxelType>
	VoxelType VolumeIterator<VoxelType>::peekVoxel0px0py0pz(void) const
	{
			return *mCurrentVoxel;
	}

	template <typename VoxelType>
	VoxelType VolumeIterator<VoxelType>::peekVoxel0px0py1pz(void) const
	{
		if((mZPosInVolume%mVolume.m_uBlockSideLength != mVolume.m_uBlockSideLength-1))
		{
			return *(mCurrentVoxel + mVolume.m_uBlockSideLength*mVolume.m_uBlockSideLength);
		}
		return mVolume.getVoxelAt(mXPosInVolume,mYPosInVolume,mZPosInVolume+1);
	}

	template <typename VoxelType>
	VoxelType VolumeIterator<VoxelType>::peekVoxel0px1py1nz(void) const
	{
		if((mYPosInVolume%mVolume.m_uBlockSideLength != mVolume.m_uBlockSideLength-1) && (mZPosInVolume%mVolume.m_uBlockSideLength != 0))
		{
			return *(mCurrentVoxel + mVolume.m_uBlockSideLength - mVolume.m_uBlockSideLength*mVolume.m_uBlockSideLength);
		}
		return mVolume.getVoxelAt(mXPosInVolume,mYPosInVolume+1,mZPosInVolume-1);
	}

	template <typename VoxelType>
	VoxelType VolumeIterator<VoxelType>::peekVoxel0px1py0pz(void) const
	{
		if((mYPosInVolume%mVolume.m_uBlockSideLength != mVolume.m_uBlockSideLength-1))
		{
			return *(mCurrentVoxel + mVolume.m_uBlockSideLength);
		}
		return mVolume.getVoxelAt(mXPosInVolume,mYPosInVolume+1,mZPosInVolume);
	}

	template <typename VoxelType>
	VoxelType VolumeIterator<VoxelType>::peekVoxel0px1py1pz(void) const
	{
		if((mYPosInVolume%mVolume.m_uBlockSideLength != mVolume.m_uBlockSideLength-1) && (mZPosInVolume%mVolume.m_uBlockSideLength != mVolume.m_uBlockSideLength-1))
		{
			return *(mCurrentVoxel + mVolume.m_uBlockSideLength + mVolume.m_uBlockSideLength*mVolume.m_uBlockSideLength);
		}
		return mVolume.getVoxelAt(mXPosInVolume,mYPosInVolume+1,mZPosInVolume+1);
	}

	//////////////////////////////////////////////////////////////////////////

	template <typename VoxelType>
	VoxelType VolumeIterator<VoxelType>::peekVoxel1px1ny1nz(void) const
	{
		if((mXPosInVolume%mVolume.m_uBlockSideLength != mVolume.m_uBlockSideLength-1) && (mYPosInVolume%mVolume.m_uBlockSideLength != 0) && (mZPosInVolume%mVolume.m_uBlockSideLength != 0))
		{
			return *(mCurrentVoxel + 1 - mVolume.m_uBlockSideLength - mVolume.m_uBlockSideLength*mVolume.m_uBlockSideLength);
		}
		return mVolume.getVoxelAt(mXPosInVolume+1,mYPosInVolume-1,mZPosInVolume-1);
	}

	template <typename VoxelType>
	VoxelType VolumeIterator<VoxelType>::peekVoxel1px1ny0pz(void) const
	{
		if((mXPosInVolume%mVolume.m_uBlockSideLength != mVolume.m_uBlockSideLength-1) && (mYPosInVolume%mVolume.m_uBlockSideLength != 0))
		{
			return *(mCurrentVoxel + 1 - mVolume.m_uBlockSideLength);
		}
		return mVolume.getVoxelAt(mXPosInVolume+1,mYPosInVolume-1,mZPosInVolume);
	}

	template <typename VoxelType>
	VoxelType VolumeIterator<VoxelType>::peekVoxel1px1ny1pz(void) const
	{
		if((mXPosInVolume%mVolume.m_uBlockSideLength != mVolume.m_uBlockSideLength-1) && (mYPosInVolume%mVolume.m_uBlockSideLength != 0) && (mZPosInVolume%mVolume.m_uBlockSideLength != mVolume.m_uBlockSideLength-1))
		{
			return *(mCurrentVoxel + 1 - mVolume.m_uBlockSideLength + mVolume.m_uBlockSideLength*mVolume.m_uBlockSideLength);
		}
		return mVolume.getVoxelAt(mXPosInVolume+1,mYPosInVolume-1,mZPosInVolume+1);
	}

	template <typename VoxelType>
	VoxelType VolumeIterator<VoxelType>::peekVoxel1px0py1nz(void) const
	{
		if((mXPosInVolume%mVolume.m_uBlockSideLength != mVolume.m_uBlockSideLength-1) && (mZPosInVolume%mVolume.m_uBlockSideLength != 0))
		{
			return *(mCurrentVoxel + 1 - mVolume.m_uBlockSideLength*mVolume.m_uBlockSideLength);
		}
		return mVolume.getVoxelAt(mXPosInVolume+1,mYPosInVolume,mZPosInVolume-1);
	}

	template <typename VoxelType>
	VoxelType VolumeIterator<VoxelType>::peekVoxel1px0py0pz(void) const
	{
		if((mXPosInVolume%mVolume.m_uBlockSideLength != mVolume.m_uBlockSideLength-1))
		{
			return *(mCurrentVoxel + 1);
		}
		return mVolume.getVoxelAt(mXPosInVolume+1,mYPosInVolume,mZPosInVolume);
	}

	template <typename VoxelType>
	VoxelType VolumeIterator<VoxelType>::peekVoxel1px0py1pz(void) const
	{
		if((mXPosInVolume%mVolume.m_uBlockSideLength != mVolume.m_uBlockSideLength-1) && (mZPosInVolume%mVolume.m_uBlockSideLength != mVolume.m_uBlockSideLength-1))
		{
			return *(mCurrentVoxel + 1 + mVolume.m_uBlockSideLength*mVolume.m_uBlockSideLength);
		}
		return mVolume.getVoxelAt(mXPosInVolume+1,mYPosInVolume,mZPosInVolume+1);
	}

	template <typename VoxelType>
	VoxelType VolumeIterator<VoxelType>::peekVoxel1px1py1nz(void) const
	{
		if((mXPosInVolume%mVolume.m_uBlockSideLength != mVolume.m_uBlockSideLength-1) && (mYPosInVolume%mVolume.m_uBlockSideLength != mVolume.m_uBlockSideLength-1) && (mZPosInVolume%mVolume.m_uBlockSideLength != 0))
		{
			return *(mCurrentVoxel + 1 + mVolume.m_uBlockSideLength - mVolume.m_uBlockSideLength*mVolume.m_uBlockSideLength);
		}
		return mVolume.getVoxelAt(mXPosInVolume+1,mYPosInVolume+1,mZPosInVolume-1);
	}

	template <typename VoxelType>
	VoxelType VolumeIterator<VoxelType>::peekVoxel1px1py0pz(void) const
	{
		if((mXPosInVolume%mVolume.m_uBlockSideLength != mVolume.m_uBlockSideLength-1) && (mYPosInVolume%mVolume.m_uBlockSideLength != mVolume.m_uBlockSideLength-1))
		{
			return *(mCurrentVoxel + 1 + mVolume.m_uBlockSideLength);
		}
		return mVolume.getVoxelAt(mXPosInVolume+1,mYPosInVolume+1,mZPosInVolume);
	}

	template <typename VoxelType>
	VoxelType VolumeIterator<VoxelType>::peekVoxel1px1py1pz(void) const
	{
		if((mXPosInVolume%mVolume.m_uBlockSideLength != mVolume.m_uBlockSideLength-1) && (mYPosInVolume%mVolume.m_uBlockSideLength != mVolume.m_uBlockSideLength-1) && (mZPosInVolume%mVolume.m_uBlockSideLength != mVolume.m_uBlockSideLength-1))
		{
			return *(mCurrentVoxel + 1 + mVolume.m_uBlockSideLength + mVolume.m_uBlockSideLength*mVolume.m_uBlockSideLength);
		}
		return mVolume.getVoxelAt(mXPosInVolume+1,mYPosInVolume+1,mZPosInVolume+1);
	}
	#pragma endregion
}

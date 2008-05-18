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

#include "Block.h"
#include "Volume.h"

using namespace boost;

namespace PolyVox
{
	template <typename VoxelType>
	VolumeIterator<VoxelType>::VolumeIterator(Volume<VoxelType>& volume)
		:mVolume(volume)
		,mXRegionFirst(0)
		,mYRegionFirst(0)
		,mZRegionFirst(0)
		,mXRegionLast(volume.getSideLength()-1)
		,mYRegionLast(volume.getSideLength()-1)
		,mZRegionLast(volume.getSideLength()-1)
		,mXRegionFirstBlock(0)
		,mYRegionFirstBlock(0)
		,mZRegionFirstBlock(0)
		,mXRegionLastBlock(volume.m_uSideLengthInBlocks-1)
		,mYRegionLastBlock(volume.m_uSideLengthInBlocks-1)
		,mZRegionLastBlock(volume.m_uSideLengthInBlocks-1)
		,mXPosInVolume(0)
		,mYPosInVolume(0)
		,mZPosInVolume(0)
		,mXBlock(0)
		,mYBlock(0)
		,mZBlock(0)
		,mXPosInBlock(0)
		,mYPosInBlock(0)
		,mZPosInBlock(0)
		,mIsValidForRegion(true)
		,mCurrentVoxel(volume.mBlocks[0]->m_tData)
		//,mCurrentBlock(volume->mBlocks[0])
		,mVoxelIndexInBlock(0)
		,mBlockIndexInVolume(0)
	{
	}

	template <typename VoxelType>
	VolumeIterator<VoxelType>::~VolumeIterator()
	{
	}	

	template <typename VoxelType>
	void VolumeIterator<VoxelType>::setVoxel(VoxelType value)
	{
		Block<VoxelType>* currentBlock = mVolume.mBlocks[mBlockIndexInVolume];

		/*if(!currentBlock.unique())
		{
			Block* copy(new Block(*currentBlock));
			currentBlock = copy;

			mCurrentVoxel = currentBlock->mData + mVoxelIndexInBlock;
		}*/

		*mCurrentVoxel = value;
	}

	template <typename VoxelType>
	VoxelType VolumeIterator<VoxelType>::getVoxel(void)
	{
		//return getVoxelAt(mXPosInVolume,mYPosInVolume,mZPosInVolume);
		return *mCurrentVoxel;
	}

	/*template <typename VoxelType>
	VoxelType VolumeIterator<VoxelType>::getVoxelAt(const uint16_t xPosition, const uint16_t yPosition, const uint16_t zPosition) const
	{
		assert(xPosition < mVolume.getSideLength());
		assert(yPosition < mVolume.getSideLength());
		assert(zPosition < mVolume.getSideLength());

		const uint16_t blockX = xPosition >> mVolume.m_uBlockSideLengthPower;
		const uint16_t blockY = yPosition >> mVolume.m_uBlockSideLengthPower;
		const uint16_t blockZ = zPosition >> mVolume.m_uBlockSideLengthPower;

		const uint16_t xOffset = xPosition - (blockX << mVolume.m_uBlockSideLengthPower);
		const uint16_t yOffset = yPosition - (blockY << mVolume.m_uBlockSideLengthPower);
		const uint16_t zOffset = zPosition - (blockZ << mVolume.m_uBlockSideLengthPower);

		const Block<VoxelType>* block = mVolume.mBlocks
			[
				blockX + 
				blockY * mVolume.m_uSideLengthInBlocks + 
				blockZ * mVolume.m_uSideLengthInBlocks * mVolume.m_uSideLengthInBlocks
			];

		return block->getVoxelAt(xOffset,yOffset,zOffset);
	}*/

	template <typename VoxelType>
	float VolumeIterator<VoxelType>::getAveragedVoxel(uint16_t size) const
	{
		assert(mXPosInVolume >= size);
		assert(mYPosInVolume >= size);
		assert(mZPosInVolume >= size);
		assert(mXPosInVolume < mVolume.getSideLength() - (size + 1));
		assert(mYPosInVolume < mVolume.getSideLength() - (size + 1));
		assert(mZPosInVolume < mVolume.getSideLength() - (size + 1));

		float sum = 0.0;
		for(uint16_t z = mZPosInVolume-size; z <= mZPosInVolume+size; ++z)
		{
			for(uint16_t y = mYPosInVolume-size; y <= mYPosInVolume+size; ++y)
			{
				for(uint16_t x = mXPosInVolume-size; x <= mXPosInVolume+size; ++x)
				{
					if(mVolume.getVoxelAt(x,y,z) != 0)
					{
						sum += 1.0;
					}
				}
			}
		}

		uint16_t kernelSideLength = size * 2 + 1;
		uint16_t kernelVolume = kernelSideLength * kernelSideLength * kernelSideLength;

		sum /= static_cast<float>(kernelVolume);
		return sum;
	}

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
			mYBlock * mVolume.m_uSideLengthInBlocks + 
			mZBlock * mVolume.m_uSideLengthInBlocks * mVolume.m_uSideLengthInBlocks;
		Block<VoxelType>* currentBlock = mVolume.mBlocks[mBlockIndexInVolume];

		mVoxelIndexInBlock = mXPosInBlock + 
			mYPosInBlock * mVolume.m_uBlockSideLength + 
			mZPosInBlock * mVolume.m_uBlockSideLength * mVolume.m_uBlockSideLength;

		mCurrentVoxel = currentBlock->m_tData + mVoxelIndexInBlock;
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

	template <typename VoxelType>
	void VolumeIterator<VoxelType>::moveForwardInRegion(void)
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
			Block<VoxelType>* currentBlock = mVolume.mBlocks[mBlockIndexInVolume];
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
				Block<VoxelType>* currentBlock = mVolume.mBlocks[mBlockIndexInVolume];
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
							mYBlock * mVolume.m_uSideLengthInBlocks + 
							mZBlock * mVolume.m_uSideLengthInBlocks * mVolume.m_uSideLengthInBlocks;

						++mYBlock;
						mBlockIndexInVolume += mVolume.m_uSideLengthInBlocks;
						if(mYBlock > mYRegionLastBlock)
						{
							mYBlock = mYRegionFirstBlock;
							mBlockIndexInVolume = mXBlock + 
								mYBlock * mVolume.m_uSideLengthInBlocks + 
								mZBlock * mVolume.m_uSideLengthInBlocks * mVolume.m_uSideLengthInBlocks;

							++mZBlock;
							mBlockIndexInVolume += mVolume.m_uSideLengthInBlocks * mVolume.m_uSideLengthInBlocks;
							if(mZBlock > mZRegionLastBlock)
							{
								mIsValidForRegion = false;
								return;			
							}
						}
					}

					Block<VoxelType>* currentBlock = mVolume.mBlocks[mBlockIndexInVolume];
					//mCurrentBlock = mVolume->mBlocks[mBlockIndexInVolume];					

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
	bool VolumeIterator<VoxelType>::isValidForRegion(void)
	{
		return mIsValidForRegion;
	}

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
}

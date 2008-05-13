/******************************************************************************
This file is part of a voxel plugin for OGRE
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

#include "block.h"
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
		,mXRegionLastBlock(volume.getSideLengthInBlocks()-1)
		,mYRegionLastBlock(volume.getSideLengthInBlocks()-1)
		,mZRegionLastBlock(volume.getSideLengthInBlocks()-1)
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

	template <typename VoxelType>
	VoxelType VolumeIterator<VoxelType>::getVoxelAt(const uint16_t xPosition, const uint16_t yPosition, const uint16_t zPosition) const
	{
		const uint16_t blockX = xPosition >> mVolume.getBlockSideLengthPower();
		const uint16_t blockY = yPosition >> mVolume.getBlockSideLengthPower();
		const uint16_t blockZ = zPosition >> mVolume.getBlockSideLengthPower();

		const uint16_t xOffset = xPosition - (blockX << mVolume.getBlockSideLengthPower());
		const uint16_t yOffset = yPosition - (blockY << mVolume.getBlockSideLengthPower());
		const uint16_t zOffset = zPosition - (blockZ << mVolume.getBlockSideLengthPower());

		const Block<VoxelType>* block = mVolume.mBlocks
			[
				blockX + 
				blockY * mVolume.getSideLengthInBlocks() + 
				blockZ * mVolume.getSideLengthInBlocks() * mVolume.getSideLengthInBlocks()
			];

		return block->getVoxelAt(xOffset,yOffset,zOffset);
	}

	template <typename VoxelType>
	float VolumeIterator<VoxelType>::getAveragedVoxelAt(const uint16_t xPosition, const uint16_t yPosition, const uint16_t zPosition, uint16_t size) const
	{
		float sum = 0.0;
		for(uint16_t z = zPosition-size; z <= zPosition+size; ++z)
		{
			for(uint16_t y = yPosition-size; y <= yPosition+size; ++y)
			{
				for(uint16_t x = xPosition-size; x <= xPosition+size; ++x)
				{
					if(getVoxelAt(x,y,z) != 0)
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

		mXBlock = mXPosInVolume >> mVolume.getBlockSideLengthPower();
		mYBlock = mYPosInVolume >> mVolume.getBlockSideLengthPower();
		mZBlock = mZPosInVolume >> mVolume.getBlockSideLengthPower();

		mXPosInBlock = mXPosInVolume - (mXBlock << mVolume.getBlockSideLengthPower());
		mYPosInBlock = mYPosInVolume - (mYBlock << mVolume.getBlockSideLengthPower());
		mZPosInBlock = mZPosInVolume - (mZBlock << mVolume.getBlockSideLengthPower());

		mBlockIndexInVolume = mXBlock + 
			mYBlock * mVolume.getSideLengthInBlocks() + 
			mZBlock * mVolume.getSideLengthInBlocks() * mVolume.getSideLengthInBlocks();
		Block<VoxelType>* currentBlock = mVolume.mBlocks[mBlockIndexInVolume];

		mVoxelIndexInBlock = mXPosInBlock + 
			mYPosInBlock * mVolume.getBlockSideLength() + 
			mZPosInBlock * mVolume.getBlockSideLength() * mVolume.getBlockSideLength();

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

		mXRegionFirstBlock = mXRegionFirst >> mVolume.getBlockSideLengthPower();
		mYRegionFirstBlock = mYRegionFirst >> mVolume.getBlockSideLengthPower();
		mZRegionFirstBlock = mZRegionFirst >> mVolume.getBlockSideLengthPower();

		mXRegionLastBlock = mXRegionLast >> mVolume.getBlockSideLengthPower();
		mYRegionLastBlock = mYRegionLast >> mVolume.getBlockSideLengthPower();
		mZRegionLastBlock = mZRegionLast >> mVolume.getBlockSideLengthPower();
	}

	template <typename VoxelType>
	void VolumeIterator<VoxelType>::moveForwardInRegion(void)
	{
		mXPosInBlock++;
		mCurrentVoxel++;
		mXPosInVolume++;
		if((mXPosInBlock == mVolume.getBlockSideLength()) || (mXPosInVolume > mXRegionLast))
		{
			mXPosInVolume = (std::max)(mXRegionFirst,uint16_t(mXBlock * mVolume.getBlockSideLength()));
			mXPosInBlock = mXPosInVolume - (mXBlock << mVolume.getBlockSideLengthPower());
			mVoxelIndexInBlock = mXPosInBlock + 
				mYPosInBlock * mVolume.getBlockSideLength() + 
				mZPosInBlock * mVolume.getBlockSideLength() * mVolume.getBlockSideLength();
			Block<VoxelType>* currentBlock = mVolume.mBlocks[mBlockIndexInVolume];
			mCurrentVoxel = currentBlock->m_tData + mVoxelIndexInBlock;

			mYPosInBlock++;
			mYPosInVolume++;
			mCurrentVoxel += mVolume.getBlockSideLength();
			if((mYPosInBlock == mVolume.getBlockSideLength()) || (mYPosInVolume > mYRegionLast))
			{
				mYPosInVolume = (std::max)(mYRegionFirst,uint16_t(mYBlock * mVolume.getBlockSideLength()));
				mYPosInBlock = mYPosInVolume - (mYBlock << mVolume.getBlockSideLengthPower());
				mVoxelIndexInBlock = mXPosInBlock + 
					mYPosInBlock * mVolume.getBlockSideLength() + 
					mZPosInBlock * mVolume.getBlockSideLength() * mVolume.getBlockSideLength();
				Block<VoxelType>* currentBlock = mVolume.mBlocks[mBlockIndexInVolume];
				mCurrentVoxel = currentBlock->m_tData + mVoxelIndexInBlock;

				mZPosInBlock++;
				mZPosInVolume++;
				mCurrentVoxel += mVolume.getBlockSideLength() * mVolume.getBlockSideLength();

				if((mZPosInBlock == mVolume.getBlockSideLength()) || (mZPosInVolume > mZRegionLast))
				{
					//At this point we've left the current block. Find a new one...

					++mXBlock;
					++mBlockIndexInVolume;
					if(mXBlock > mXRegionLastBlock)
					{
						mXBlock = mXRegionFirstBlock;
						mBlockIndexInVolume = mXBlock + 
							mYBlock * mVolume.getSideLengthInBlocks() + 
							mZBlock * mVolume.getSideLengthInBlocks() * mVolume.getSideLengthInBlocks();

						++mYBlock;
						mBlockIndexInVolume += mVolume.getSideLengthInBlocks();
						if(mYBlock > mYRegionLastBlock)
						{
							mYBlock = mYRegionFirstBlock;
							mBlockIndexInVolume = mXBlock + 
								mYBlock * mVolume.getSideLengthInBlocks() + 
								mZBlock * mVolume.getSideLengthInBlocks() * mVolume.getSideLengthInBlocks();

							++mZBlock;
							mBlockIndexInVolume += mVolume.getSideLengthInBlocks() * mVolume.getSideLengthInBlocks();
							if(mZBlock > mZRegionLastBlock)
							{
								mIsValidForRegion = false;
								return;			
							}
						}
					}

					Block<VoxelType>* currentBlock = mVolume.mBlocks[mBlockIndexInVolume];
					//mCurrentBlock = mVolume->mBlocks[mBlockIndexInVolume];					

					mXPosInVolume = (std::max)(mXRegionFirst,uint16_t(mXBlock * mVolume.getBlockSideLength()));					
					mYPosInVolume = (std::max)(mYRegionFirst,uint16_t(mYBlock * mVolume.getBlockSideLength()));					
					mZPosInVolume = (std::max)(mZRegionFirst,uint16_t(mZBlock * mVolume.getBlockSideLength()));					

					mXPosInBlock = mXPosInVolume - (mXBlock << mVolume.getBlockSideLengthPower());
					mYPosInBlock = mYPosInVolume - (mYBlock << mVolume.getBlockSideLengthPower());
					mZPosInBlock = mZPosInVolume - (mZBlock << mVolume.getBlockSideLengthPower());

					mVoxelIndexInBlock = mXPosInBlock + 
						mYPosInBlock * mVolume.getBlockSideLength() + 
						mZPosInBlock * mVolume.getBlockSideLength() * mVolume.getBlockSideLength();

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
		if((mXPosInVolume%mVolume.getBlockSideLength() != 0) && (mYPosInVolume%mVolume.getBlockSideLength() != 0) && (mZPosInVolume%mVolume.getBlockSideLength() != 0))
		{
			return *(mCurrentVoxel - 1 - mVolume.getBlockSideLength() - mVolume.getBlockSideLength()*mVolume.getBlockSideLength());
		}
		return getVoxelAt(mXPosInVolume-1,mYPosInVolume-1,mZPosInVolume-1);
	}

	template <typename VoxelType>
	VoxelType VolumeIterator<VoxelType>::peekVoxel1nx1ny0pz(void) const
	{
		if((mXPosInVolume%mVolume.getBlockSideLength() != 0) && (mYPosInVolume%mVolume.getBlockSideLength() != 0))
		{
			return *(mCurrentVoxel - 1 - mVolume.getBlockSideLength());
		}
		return getVoxelAt(mXPosInVolume-1,mYPosInVolume-1,mZPosInVolume);
	}

	template <typename VoxelType>
	VoxelType VolumeIterator<VoxelType>::peekVoxel1nx1ny1pz(void) const
	{
		if((mXPosInVolume%mVolume.getBlockSideLength() != 0) && (mYPosInVolume%mVolume.getBlockSideLength() != 0) && (mZPosInVolume%mVolume.getBlockSideLength() != mVolume.getBlockSideLength()-1))
		{
			return *(mCurrentVoxel - 1 - mVolume.getBlockSideLength() + mVolume.getBlockSideLength()*mVolume.getBlockSideLength());
		}
		return getVoxelAt(mXPosInVolume-1,mYPosInVolume-1,mZPosInVolume+1);
	}

	template <typename VoxelType>
	VoxelType VolumeIterator<VoxelType>::peekVoxel1nx0py1nz(void) const
	{
		if((mXPosInVolume%mVolume.getBlockSideLength() != 0) && (mZPosInVolume%mVolume.getBlockSideLength() != 0))
		{
			return *(mCurrentVoxel - 1 - mVolume.getBlockSideLength()*mVolume.getBlockSideLength());
		}
		return getVoxelAt(mXPosInVolume-1,mYPosInVolume,mZPosInVolume-1);
	}

	template <typename VoxelType>
	VoxelType VolumeIterator<VoxelType>::peekVoxel1nx0py0pz(void) const
	{
		if((mXPosInVolume%mVolume.getBlockSideLength() != 0))
		{
			return *(mCurrentVoxel - 1);
		}
		return getVoxelAt(mXPosInVolume-1,mYPosInVolume,mZPosInVolume);
	}

	template <typename VoxelType>
	VoxelType VolumeIterator<VoxelType>::peekVoxel1nx0py1pz(void) const
	{
		if((mXPosInVolume%mVolume.getBlockSideLength() != 0) && (mZPosInVolume%mVolume.getBlockSideLength() != mVolume.getBlockSideLength()-1))
		{
			return *(mCurrentVoxel - 1 + mVolume.getBlockSideLength()*mVolume.getBlockSideLength());
		}
		return getVoxelAt(mXPosInVolume-1,mYPosInVolume,mZPosInVolume+1);
	}

	template <typename VoxelType>
	VoxelType VolumeIterator<VoxelType>::peekVoxel1nx1py1nz(void) const
	{
		if((mXPosInVolume%mVolume.getBlockSideLength() != 0) && (mYPosInVolume%mVolume.getBlockSideLength() != mVolume.getBlockSideLength()-1) && (mZPosInVolume%mVolume.getBlockSideLength() != 0))
		{
			return *(mCurrentVoxel - 1 + mVolume.getBlockSideLength() - mVolume.getBlockSideLength()*mVolume.getBlockSideLength());
		}
		return getVoxelAt(mXPosInVolume-1,mYPosInVolume+1,mZPosInVolume-1);
	}

	template <typename VoxelType>
	VoxelType VolumeIterator<VoxelType>::peekVoxel1nx1py0pz(void) const
	{
		if((mXPosInVolume%mVolume.getBlockSideLength() != 0) && (mYPosInVolume%mVolume.getBlockSideLength() != mVolume.getBlockSideLength()-1))
		{
			return *(mCurrentVoxel - 1 + mVolume.getBlockSideLength());
		}
		return getVoxelAt(mXPosInVolume-1,mYPosInVolume+1,mZPosInVolume);
	}

	template <typename VoxelType>
	VoxelType VolumeIterator<VoxelType>::peekVoxel1nx1py1pz(void) const
	{
		if((mXPosInVolume%mVolume.getBlockSideLength() != 0) && (mYPosInVolume%mVolume.getBlockSideLength() != mVolume.getBlockSideLength()-1) && (mZPosInVolume%mVolume.getBlockSideLength() != mVolume.getBlockSideLength()-1))
		{
			return *(mCurrentVoxel - 1 + mVolume.getBlockSideLength() + mVolume.getBlockSideLength()*mVolume.getBlockSideLength());
		}
		return getVoxelAt(mXPosInVolume-1,mYPosInVolume+1,mZPosInVolume+1);
	}

	//////////////////////////////////////////////////////////////////////////

	template <typename VoxelType>
	VoxelType VolumeIterator<VoxelType>::peekVoxel0px1ny1nz(void) const
	{
		if((mYPosInVolume%mVolume.getBlockSideLength() != 0) && (mZPosInVolume%mVolume.getBlockSideLength() != 0))
		{
			return *(mCurrentVoxel - mVolume.getBlockSideLength() - mVolume.getBlockSideLength()*mVolume.getBlockSideLength());
		}
		return getVoxelAt(mXPosInVolume,mYPosInVolume-1,mZPosInVolume-1);
	}

	template <typename VoxelType>
	VoxelType VolumeIterator<VoxelType>::peekVoxel0px1ny0pz(void) const
	{
		if((mYPosInVolume%mVolume.getBlockSideLength() != 0))
		{
			return *(mCurrentVoxel - mVolume.getBlockSideLength());
		}
		return getVoxelAt(mXPosInVolume,mYPosInVolume-1,mZPosInVolume);
	}

	template <typename VoxelType>
	VoxelType VolumeIterator<VoxelType>::peekVoxel0px1ny1pz(void) const
	{
		if((mYPosInVolume%mVolume.getBlockSideLength() != 0) && (mZPosInVolume%mVolume.getBlockSideLength() != mVolume.getBlockSideLength()-1))
		{
			return *(mCurrentVoxel - mVolume.getBlockSideLength() + mVolume.getBlockSideLength()*mVolume.getBlockSideLength());
		}
		return getVoxelAt(mXPosInVolume,mYPosInVolume-1,mZPosInVolume+1);
	}

	template <typename VoxelType>
	VoxelType VolumeIterator<VoxelType>::peekVoxel0px0py1nz(void) const
	{
		if((mZPosInVolume%mVolume.getBlockSideLength() != 0))
		{
			return *(mCurrentVoxel - mVolume.getBlockSideLength()*mVolume.getBlockSideLength());
		}
		return getVoxelAt(mXPosInVolume,mYPosInVolume,mZPosInVolume-1);
	}

	template <typename VoxelType>
	VoxelType VolumeIterator<VoxelType>::peekVoxel0px0py0pz(void) const
	{
			return *mCurrentVoxel;
	}

	template <typename VoxelType>
	VoxelType VolumeIterator<VoxelType>::peekVoxel0px0py1pz(void) const
	{
		if((mZPosInVolume%mVolume.getBlockSideLength() != mVolume.getBlockSideLength()-1))
		{
			return *(mCurrentVoxel + mVolume.getBlockSideLength()*mVolume.getBlockSideLength());
		}
		return getVoxelAt(mXPosInVolume,mYPosInVolume,mZPosInVolume+1);
	}

	template <typename VoxelType>
	VoxelType VolumeIterator<VoxelType>::peekVoxel0px1py1nz(void) const
	{
		if((mYPosInVolume%mVolume.getBlockSideLength() != mVolume.getBlockSideLength()-1) && (mZPosInVolume%mVolume.getBlockSideLength() != 0))
		{
			return *(mCurrentVoxel + mVolume.getBlockSideLength() - mVolume.getBlockSideLength()*mVolume.getBlockSideLength());
		}
		return getVoxelAt(mXPosInVolume,mYPosInVolume+1,mZPosInVolume-1);
	}

	template <typename VoxelType>
	VoxelType VolumeIterator<VoxelType>::peekVoxel0px1py0pz(void) const
	{
		if((mYPosInVolume%mVolume.getBlockSideLength() != mVolume.getBlockSideLength()-1))
		{
			return *(mCurrentVoxel + mVolume.getBlockSideLength());
		}
		return getVoxelAt(mXPosInVolume,mYPosInVolume+1,mZPosInVolume);
	}

	template <typename VoxelType>
	VoxelType VolumeIterator<VoxelType>::peekVoxel0px1py1pz(void) const
	{
		if((mYPosInVolume%mVolume.getBlockSideLength() != mVolume.getBlockSideLength()-1) && (mZPosInVolume%mVolume.getBlockSideLength() != mVolume.getBlockSideLength()-1))
		{
			return *(mCurrentVoxel + mVolume.getBlockSideLength() + mVolume.getBlockSideLength()*mVolume.getBlockSideLength());
		}
		return getVoxelAt(mXPosInVolume,mYPosInVolume+1,mZPosInVolume+1);
	}

	//////////////////////////////////////////////////////////////////////////

	template <typename VoxelType>
	VoxelType VolumeIterator<VoxelType>::peekVoxel1px1ny1nz(void) const
	{
		if((mXPosInVolume%mVolume.getBlockSideLength() != mVolume.getBlockSideLength()-1) && (mYPosInVolume%mVolume.getBlockSideLength() != 0) && (mZPosInVolume%mVolume.getBlockSideLength() != 0))
		{
			return *(mCurrentVoxel + 1 - mVolume.getBlockSideLength() - mVolume.getBlockSideLength()*mVolume.getBlockSideLength());
		}
		return getVoxelAt(mXPosInVolume+1,mYPosInVolume-1,mZPosInVolume-1);
	}

	template <typename VoxelType>
	VoxelType VolumeIterator<VoxelType>::peekVoxel1px1ny0pz(void) const
	{
		if((mXPosInVolume%mVolume.getBlockSideLength() != mVolume.getBlockSideLength()-1) && (mYPosInVolume%mVolume.getBlockSideLength() != 0))
		{
			return *(mCurrentVoxel + 1 - mVolume.getBlockSideLength());
		}
		return getVoxelAt(mXPosInVolume+1,mYPosInVolume-1,mZPosInVolume);
	}

	template <typename VoxelType>
	VoxelType VolumeIterator<VoxelType>::peekVoxel1px1ny1pz(void) const
	{
		if((mXPosInVolume%mVolume.getBlockSideLength() != mVolume.getBlockSideLength()-1) && (mYPosInVolume%mVolume.getBlockSideLength() != 0) && (mZPosInVolume%mVolume.getBlockSideLength() != mVolume.getBlockSideLength()-1))
		{
			return *(mCurrentVoxel + 1 - mVolume.getBlockSideLength() + mVolume.getBlockSideLength()*mVolume.getBlockSideLength());
		}
		return getVoxelAt(mXPosInVolume+1,mYPosInVolume-1,mZPosInVolume+1);
	}

	template <typename VoxelType>
	VoxelType VolumeIterator<VoxelType>::peekVoxel1px0py1nz(void) const
	{
		if((mXPosInVolume%mVolume.getBlockSideLength() != mVolume.getBlockSideLength()-1) && (mZPosInVolume%mVolume.getBlockSideLength() != 0))
		{
			return *(mCurrentVoxel + 1 - mVolume.getBlockSideLength()*mVolume.getBlockSideLength());
		}
		return getVoxelAt(mXPosInVolume+1,mYPosInVolume,mZPosInVolume-1);
	}

	template <typename VoxelType>
	VoxelType VolumeIterator<VoxelType>::peekVoxel1px0py0pz(void) const
	{
		if((mXPosInVolume%mVolume.getBlockSideLength() != mVolume.getBlockSideLength()-1))
		{
			return *(mCurrentVoxel + 1);
		}
		return getVoxelAt(mXPosInVolume+1,mYPosInVolume,mZPosInVolume);
	}

	template <typename VoxelType>
	VoxelType VolumeIterator<VoxelType>::peekVoxel1px0py1pz(void) const
	{
		if((mXPosInVolume%mVolume.getBlockSideLength() != mVolume.getBlockSideLength()-1) && (mZPosInVolume%mVolume.getBlockSideLength() != mVolume.getBlockSideLength()-1))
		{
			return *(mCurrentVoxel + 1 + mVolume.getBlockSideLength()*mVolume.getBlockSideLength());
		}
		return getVoxelAt(mXPosInVolume+1,mYPosInVolume,mZPosInVolume+1);
	}

	template <typename VoxelType>
	VoxelType VolumeIterator<VoxelType>::peekVoxel1px1py1nz(void) const
	{
		if((mXPosInVolume%mVolume.getBlockSideLength() != mVolume.getBlockSideLength()-1) && (mYPosInVolume%mVolume.getBlockSideLength() != mVolume.getBlockSideLength()-1) && (mZPosInVolume%mVolume.getBlockSideLength() != 0))
		{
			return *(mCurrentVoxel + 1 + mVolume.getBlockSideLength() - mVolume.getBlockSideLength()*mVolume.getBlockSideLength());
		}
		return getVoxelAt(mXPosInVolume+1,mYPosInVolume+1,mZPosInVolume-1);
	}

	template <typename VoxelType>
	VoxelType VolumeIterator<VoxelType>::peekVoxel1px1py0pz(void) const
	{
		if((mXPosInVolume%mVolume.getBlockSideLength() != mVolume.getBlockSideLength()-1) && (mYPosInVolume%mVolume.getBlockSideLength() != mVolume.getBlockSideLength()-1))
		{
			return *(mCurrentVoxel + 1 + mVolume.getBlockSideLength());
		}
		return getVoxelAt(mXPosInVolume+1,mYPosInVolume+1,mZPosInVolume);
	}

	template <typename VoxelType>
	VoxelType VolumeIterator<VoxelType>::peekVoxel1px1py1pz(void) const
	{
		if((mXPosInVolume%mVolume.getBlockSideLength() != mVolume.getBlockSideLength()-1) && (mYPosInVolume%mVolume.getBlockSideLength() != mVolume.getBlockSideLength()-1) && (mZPosInVolume%mVolume.getBlockSideLength() != mVolume.getBlockSideLength()-1))
		{
			return *(mCurrentVoxel + 1 + mVolume.getBlockSideLength() + mVolume.getBlockSideLength()*mVolume.getBlockSideLength());
		}
		return getVoxelAt(mXPosInVolume+1,mYPosInVolume+1,mZPosInVolume+1);
	}
}

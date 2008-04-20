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
		,mXRegionLast(POLYVOX_VOLUME_SIDE_LENGTH-1)
		,mYRegionLast(POLYVOX_VOLUME_SIDE_LENGTH-1)
		,mZRegionLast(POLYVOX_VOLUME_SIDE_LENGTH-1)
		,mXRegionFirstBlock(0)
		,mYRegionFirstBlock(0)
		,mZRegionFirstBlock(0)
		,mXRegionLastBlock(POLYVOX_VOLUME_SIDE_LENGTH_IN_BLOCKS-1)
		,mYRegionLastBlock(POLYVOX_VOLUME_SIDE_LENGTH_IN_BLOCKS-1)
		,mZRegionLastBlock(POLYVOX_VOLUME_SIDE_LENGTH_IN_BLOCKS-1)
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
		,mCurrentVoxel(volume.mBlocks[0]->mData)
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
		const uint16_t blockX = xPosition >> POLYVOX_BLOCK_SIDE_LENGTH_POWER;
		const uint16_t blockY = yPosition >> POLYVOX_BLOCK_SIDE_LENGTH_POWER;
		const uint16_t blockZ = zPosition >> POLYVOX_BLOCK_SIDE_LENGTH_POWER;

		const uint16_t xOffset = xPosition - (blockX << POLYVOX_BLOCK_SIDE_LENGTH_POWER);
		const uint16_t yOffset = yPosition - (blockY << POLYVOX_BLOCK_SIDE_LENGTH_POWER);
		const uint16_t zOffset = zPosition - (blockZ << POLYVOX_BLOCK_SIDE_LENGTH_POWER);

		const Block<VoxelType>* block = mVolume.mBlocks
			[
				blockX + 
				blockY * POLYVOX_VOLUME_SIDE_LENGTH_IN_BLOCKS + 
				blockZ * POLYVOX_VOLUME_SIDE_LENGTH_IN_BLOCKS * POLYVOX_VOLUME_SIDE_LENGTH_IN_BLOCKS
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
	void VolumeIterator<VoxelType>::setVoxelAt(const uint16_t xPosition, const uint16_t yPosition, const uint16_t zPosition, const VoxelType value)
		{
		const uint16_t blockX = xPosition >> POLYVOX_BLOCK_SIDE_LENGTH_POWER;
		const uint16_t blockY = yPosition >> POLYVOX_BLOCK_SIDE_LENGTH_POWER;
		const uint16_t blockZ = zPosition >> POLYVOX_BLOCK_SIDE_LENGTH_POWER;

		const uint16_t xOffset = xPosition - (blockX << POLYVOX_BLOCK_SIDE_LENGTH_POWER);
		const uint16_t yOffset = yPosition - (blockY << POLYVOX_BLOCK_SIDE_LENGTH_POWER);
		const uint16_t zOffset = zPosition - (blockZ << POLYVOX_BLOCK_SIDE_LENGTH_POWER);

		Block<VoxelType>* block = mVolume.mBlocks
			[
				blockX + 
				blockY * POLYVOX_VOLUME_SIDE_LENGTH_IN_BLOCKS + 
				blockZ * POLYVOX_VOLUME_SIDE_LENGTH_IN_BLOCKS * POLYVOX_VOLUME_SIDE_LENGTH_IN_BLOCKS
			];

		/*if(!block.unique())
		{
			Block* copy(new Block(*block));
			block = copy;

			mCurrentVoxel = block->mData + mVoxelIndexInBlock;
		}*/

		block->setVoxelAt(xOffset,yOffset,zOffset, value);
	}

	template <typename VoxelType>
	Vector3DFloat VolumeIterator<VoxelType>::getCentralDifferenceGradient(void) const
	{
		//FIXME - should this test be here?
		if((mXPosInVolume < 1) || (mXPosInVolume > POLYVOX_VOLUME_SIDE_LENGTH-2) ||
			(mYPosInVolume < 1) || (mYPosInVolume > POLYVOX_VOLUME_SIDE_LENGTH-2) ||
			(mZPosInVolume < 1) || (mZPosInVolume > POLYVOX_VOLUME_SIDE_LENGTH-2))
		{
			//LogManager::getSingleton().logMessage("Out of range");
			return Vector3DFloat(0.0,0.0,0.0);
		}

		//FIXME - bitwise way of doing this?
		VoxelType voxel1nx = peekVoxel1nx0py0pz() > 0 ? 1: 0;
		VoxelType voxel1px = peekVoxel1px0py0pz() > 0 ? 1: 0;

		VoxelType voxel1ny = peekVoxel0px1ny0pz() > 0 ? 1: 0;
		VoxelType voxel1py = peekVoxel0px1py0pz() > 0 ? 1: 0;

		VoxelType voxel1nz = peekVoxel0px0py1nz() > 0 ? 1: 0;
		VoxelType voxel1pz = peekVoxel0px0py1pz() > 0 ? 1: 0;

		return Vector3DFloat(int(voxel1px) - int(voxel1nx),int(voxel1py) - int(voxel1ny),int(voxel1pz) - int(voxel1nz));
	}

	template <typename VoxelType>
	Vector3DFloat VolumeIterator<VoxelType>::getAveragedCentralDifferenceGradient(void) const
	{
		//FIXME - should this test be here?
		if((mXPosInVolume < 2) || (mXPosInVolume > POLYVOX_VOLUME_SIDE_LENGTH-3) ||
			(mYPosInVolume < 2) || (mYPosInVolume > POLYVOX_VOLUME_SIDE_LENGTH-3) ||
			(mZPosInVolume < 2) || (mZPosInVolume > POLYVOX_VOLUME_SIDE_LENGTH-3))
		{
			//LogManager::getSingleton().logMessage("Out of range");
			return Vector3DFloat(0.0,0.0,0.0);
		}

		//FIXME - bitwise way of doing this?
		float voxel1nx = getAveragedVoxelAt(mXPosInVolume-1,mYPosInVolume  ,mZPosInVolume,  2);
		float voxel1px = getAveragedVoxelAt(mXPosInVolume+1,mYPosInVolume  ,mZPosInVolume,  2);

		float voxel1ny = getAveragedVoxelAt(mXPosInVolume  ,mYPosInVolume-1,mZPosInVolume,  2);
		float voxel1py = getAveragedVoxelAt(mXPosInVolume  ,mYPosInVolume+1,mZPosInVolume,  2);

		float voxel1nz = getAveragedVoxelAt(mXPosInVolume  ,mYPosInVolume  ,mZPosInVolume-1,2);
		float voxel1pz = getAveragedVoxelAt(mXPosInVolume  ,mYPosInVolume  ,mZPosInVolume+1,2);

		return Vector3DFloat(voxel1px - voxel1nx,voxel1py - voxel1ny,voxel1pz - voxel1nz);
	}

	template <typename VoxelType>
	Vector3DFloat VolumeIterator<VoxelType>::getSobelGradient(void) const
	{
		//FIXME - should this test be here?
		if((mXPosInVolume < 1) || (mXPosInVolume > POLYVOX_VOLUME_SIDE_LENGTH-2) ||
			(mYPosInVolume < 1) || (mYPosInVolume > POLYVOX_VOLUME_SIDE_LENGTH-2) ||
			(mZPosInVolume < 1) || (mZPosInVolume > POLYVOX_VOLUME_SIDE_LENGTH-2))
		{
			//LogManager::getSingleton().logMessage("Out of range");
			return Vector3DFloat(0.0,0.0,0.0);
		}

		static const int weights[3][3][3] = {  {  {2,3,2}, {3,6,3}, {2,3,2}  },  {
			{3,6,3},  {6,0,6},  {3,6,3} },  { {2,3,2},  {3,6,3},  {2,3,2} } };

			const VoxelType pVoxel1nx1ny1nz = peekVoxel1nx1ny1nz() > 0 ? 1: 0;
			const VoxelType pVoxel1nx1ny0pz = peekVoxel1nx1ny0pz() > 0 ? 1: 0;
			const VoxelType pVoxel1nx1ny1pz = peekVoxel1nx1ny1pz() > 0 ? 1: 0;
			const VoxelType pVoxel1nx0py1nz = peekVoxel1nx0py1nz() > 0 ? 1: 0;
			const VoxelType pVoxel1nx0py0pz = peekVoxel1nx0py0pz() > 0 ? 1: 0;
			const VoxelType pVoxel1nx0py1pz = peekVoxel1nx0py1pz() > 0 ? 1: 0;
			const VoxelType pVoxel1nx1py1nz = peekVoxel1nx1py1nz() > 0 ? 1: 0;
			const VoxelType pVoxel1nx1py0pz = peekVoxel1nx1py0pz() > 0 ? 1: 0;
			const VoxelType pVoxel1nx1py1pz = peekVoxel1nx1py1pz() > 0 ? 1: 0;

			const VoxelType pVoxel0px1ny1nz = peekVoxel0px1ny1nz() > 0 ? 1: 0;
			const VoxelType pVoxel0px1ny0pz = peekVoxel0px1ny0pz() > 0 ? 1: 0;
			const VoxelType pVoxel0px1ny1pz = peekVoxel0px1ny1pz() > 0 ? 1: 0;
			const VoxelType pVoxel0px0py1nz = peekVoxel0px0py1nz() > 0 ? 1: 0;
			//const VoxelType pVoxel0px0py0pz = peekVoxel0px0py0pz() > 0 ? 1: 0;
			const VoxelType pVoxel0px0py1pz = peekVoxel0px0py1pz() > 0 ? 1: 0;
			const VoxelType pVoxel0px1py1nz = peekVoxel0px1py1nz() > 0 ? 1: 0;
			const VoxelType pVoxel0px1py0pz = peekVoxel0px1py0pz() > 0 ? 1: 0;
			const VoxelType pVoxel0px1py1pz = peekVoxel0px1py1pz() > 0 ? 1: 0;

			const VoxelType pVoxel1px1ny1nz = peekVoxel1px1ny1nz() > 0 ? 1: 0;
			const VoxelType pVoxel1px1ny0pz = peekVoxel1px1ny0pz() > 0 ? 1: 0;
			const VoxelType pVoxel1px1ny1pz = peekVoxel1px1ny1pz() > 0 ? 1: 0;
			const VoxelType pVoxel1px0py1nz = peekVoxel1px0py1nz() > 0 ? 1: 0;
			const VoxelType pVoxel1px0py0pz = peekVoxel1px0py0pz() > 0 ? 1: 0;
			const VoxelType pVoxel1px0py1pz = peekVoxel1px0py1pz() > 0 ? 1: 0;
			const VoxelType pVoxel1px1py1nz = peekVoxel1px1py1nz() > 0 ? 1: 0;
			const VoxelType pVoxel1px1py0pz = peekVoxel1px1py0pz() > 0 ? 1: 0;
			const VoxelType pVoxel1px1py1pz = peekVoxel1px1py1pz() > 0 ? 1: 0;



			const int xGrad(- weights[0][0][0] * ( pVoxel1nx1ny1nz) -
				weights[1][0][0] * ( pVoxel1nx1ny0pz) - weights[2][0][0] *
				( pVoxel1nx1ny1pz) - weights[0][1][0] * ( pVoxel1nx0py1nz) -
				weights[1][1][0] * ( pVoxel1nx0py0pz) - weights[2][1][0] *
				( pVoxel1nx0py1pz) - weights[0][2][0] * ( pVoxel1nx1py1nz) -
				weights[1][2][0] * ( pVoxel1nx1py0pz) - weights[2][2][0] *
				( pVoxel1nx1py1pz) + weights[0][0][2] * ( pVoxel1px1ny1nz) +
				weights[1][0][2] * ( pVoxel1px1ny0pz) + weights[2][0][2] *
				( pVoxel1px1ny1pz) + weights[0][1][2] * ( pVoxel1px0py1nz) +
				weights[1][1][2] * ( pVoxel1px0py0pz) + weights[2][1][2] *
				( pVoxel1px0py1pz) + weights[0][2][2] * ( pVoxel1px1py1nz) +
				weights[1][2][2] * ( pVoxel1px1py0pz) + weights[2][2][2] *
				( pVoxel1px1py1pz));

			const int yGrad(- weights[0][0][0] * ( pVoxel1nx1ny1nz) -
				weights[1][0][0] * ( pVoxel1nx1ny0pz) - weights[2][0][0] *
				( pVoxel1nx1ny1pz) + weights[0][2][0] * ( pVoxel1nx1py1nz) +
				weights[1][2][0] * ( pVoxel1nx1py0pz) + weights[2][2][0] *
				( pVoxel1nx1py1pz) - weights[0][0][1] * ( pVoxel0px1ny1nz) -
				weights[1][0][1] * ( pVoxel0px1ny0pz) - weights[2][0][1] *
				( pVoxel0px1ny1pz) + weights[0][2][1] * ( pVoxel0px1py1nz) +
				weights[1][2][1] * ( pVoxel0px1py0pz) + weights[2][2][1] *
				( pVoxel0px1py1pz) - weights[0][0][2] * ( pVoxel1px1ny1nz) -
				weights[1][0][2] * ( pVoxel1px1ny0pz) - weights[2][0][2] *
				( pVoxel1px1ny1pz) + weights[0][2][2] * ( pVoxel1px1py1nz) +
				weights[1][2][2] * ( pVoxel1px1py0pz) + weights[2][2][2] *
				( pVoxel1px1py1pz));

			const int zGrad(- weights[0][0][0] * ( pVoxel1nx1ny1nz) +
				weights[2][0][0] * ( pVoxel1nx1ny1pz) - weights[0][1][0] *
				( pVoxel1nx0py1nz) + weights[2][1][0] * ( pVoxel1nx0py1pz) -
				weights[0][2][0] * ( pVoxel1nx1py1nz) + weights[2][2][0] *
				( pVoxel1nx1py1pz) - weights[0][0][1] * ( pVoxel0px1ny1nz) +
				weights[2][0][1] * ( pVoxel0px1ny1pz) - weights[0][1][1] *
				( pVoxel0px0py1nz) + weights[2][1][1] * ( pVoxel0px0py1pz) -
				weights[0][2][1] * ( pVoxel0px1py1nz) + weights[2][2][1] *
				( pVoxel0px1py1pz) - weights[0][0][2] * ( pVoxel1px1ny1nz) +
				weights[2][0][2] * ( pVoxel1px1ny1pz) - weights[0][1][2] *
				( pVoxel1px0py1nz) + weights[2][1][2] * ( pVoxel1px0py1pz) -
				weights[0][2][2] * ( pVoxel1px1py1nz) + weights[2][2][2] *
				( pVoxel1px1py1pz));

			return Vector3DFloat(xGrad,yGrad,zGrad);
	}

	template <typename VoxelType>
	uint16_t VolumeIterator<VoxelType>::getPosX(void)
	{
		return mXPosInVolume;
	}

	template <typename VoxelType>
	uint16_t VolumeIterator<VoxelType>::getPosY(void)
	{
		return mYPosInVolume;
	}

	template <typename VoxelType>
	uint16_t VolumeIterator<VoxelType>::getPosZ(void)
	{
		return mZPosInVolume;
	}

	template <typename VoxelType>
	void VolumeIterator<VoxelType>::setPosition(uint16_t xPos, uint16_t yPos, uint16_t zPos)
	{
		mXPosInVolume = xPos;
		mYPosInVolume = yPos;
		mZPosInVolume = zPos;

		mXBlock = mXPosInVolume >> POLYVOX_BLOCK_SIDE_LENGTH_POWER;
		mYBlock = mYPosInVolume >> POLYVOX_BLOCK_SIDE_LENGTH_POWER;
		mZBlock = mZPosInVolume >> POLYVOX_BLOCK_SIDE_LENGTH_POWER;

		mXPosInBlock = mXPosInVolume - (mXBlock << POLYVOX_BLOCK_SIDE_LENGTH_POWER);
		mYPosInBlock = mYPosInVolume - (mYBlock << POLYVOX_BLOCK_SIDE_LENGTH_POWER);
		mZPosInBlock = mZPosInVolume - (mZBlock << POLYVOX_BLOCK_SIDE_LENGTH_POWER);

		mBlockIndexInVolume = mXBlock + 
			mYBlock * POLYVOX_VOLUME_SIDE_LENGTH_IN_BLOCKS + 
			mZBlock * POLYVOX_VOLUME_SIDE_LENGTH_IN_BLOCKS * POLYVOX_VOLUME_SIDE_LENGTH_IN_BLOCKS;
		Block<VoxelType>* currentBlock = mVolume.mBlocks[mBlockIndexInVolume];

		mVoxelIndexInBlock = mXPosInBlock + 
			mYPosInBlock * POLYVOX_BLOCK_SIDE_LENGTH + 
			mZPosInBlock * POLYVOX_BLOCK_SIDE_LENGTH * POLYVOX_BLOCK_SIDE_LENGTH;

		mCurrentVoxel = currentBlock->mData + mVoxelIndexInBlock;
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

		/*mXRegionFirst = (std::max)(0,xFirst);
		mYRegionFirst = (std::max)(0,yFirst);
		mZRegionFirst = (std::max)(0,zFirst);

		mXRegionLast = (std::min)(POLYVOX_VOLUME_SIDE_LENGTH-1, xLast);
		mYRegionLast = (std::min)(POLYVOX_VOLUME_SIDE_LENGTH-1, yLast);
		mZRegionLast = (std::min)(POLYVOX_VOLUME_SIDE_LENGTH-1, zLast);*/

		mXRegionFirstBlock = mXRegionFirst >> POLYVOX_BLOCK_SIDE_LENGTH_POWER;
		mYRegionFirstBlock = mYRegionFirst >> POLYVOX_BLOCK_SIDE_LENGTH_POWER;
		mZRegionFirstBlock = mZRegionFirst >> POLYVOX_BLOCK_SIDE_LENGTH_POWER;

		mXRegionLastBlock = mXRegionLast >> POLYVOX_BLOCK_SIDE_LENGTH_POWER;
		mYRegionLastBlock = mYRegionLast >> POLYVOX_BLOCK_SIDE_LENGTH_POWER;
		mZRegionLastBlock = mZRegionLast >> POLYVOX_BLOCK_SIDE_LENGTH_POWER;
	}

	template <typename VoxelType>
	void VolumeIterator<VoxelType>::moveForwardInRegion(void)
	{
		mXPosInBlock++;
		mCurrentVoxel++;
		mXPosInVolume++;
		if((mXPosInBlock == POLYVOX_BLOCK_SIDE_LENGTH) || (mXPosInVolume > mXRegionLast))
		{
			mXPosInVolume = (std::max)(mXRegionFirst,uint16_t(mXBlock * POLYVOX_BLOCK_SIDE_LENGTH));
			mXPosInBlock = mXPosInVolume - (mXBlock << POLYVOX_BLOCK_SIDE_LENGTH_POWER);
			mVoxelIndexInBlock = mXPosInBlock + 
				mYPosInBlock * POLYVOX_BLOCK_SIDE_LENGTH + 
				mZPosInBlock * POLYVOX_BLOCK_SIDE_LENGTH * POLYVOX_BLOCK_SIDE_LENGTH;
			Block<VoxelType>* currentBlock = mVolume.mBlocks[mBlockIndexInVolume];
			mCurrentVoxel = currentBlock->mData + mVoxelIndexInBlock;

			mYPosInBlock++;
			mYPosInVolume++;
			mCurrentVoxel += POLYVOX_BLOCK_SIDE_LENGTH;
			if((mYPosInBlock == POLYVOX_BLOCK_SIDE_LENGTH) || (mYPosInVolume > mYRegionLast))
			{
				mYPosInVolume = (std::max)(mYRegionFirst,uint16_t(mYBlock * POLYVOX_BLOCK_SIDE_LENGTH));
				mYPosInBlock = mYPosInVolume - (mYBlock << POLYVOX_BLOCK_SIDE_LENGTH_POWER);
				mVoxelIndexInBlock = mXPosInBlock + 
					mYPosInBlock * POLYVOX_BLOCK_SIDE_LENGTH + 
					mZPosInBlock * POLYVOX_BLOCK_SIDE_LENGTH * POLYVOX_BLOCK_SIDE_LENGTH;
				Block<VoxelType>* currentBlock = mVolume.mBlocks[mBlockIndexInVolume];
				mCurrentVoxel = currentBlock->mData + mVoxelIndexInBlock;

				mZPosInBlock++;
				mZPosInVolume++;
				mCurrentVoxel += POLYVOX_BLOCK_SIDE_LENGTH * POLYVOX_BLOCK_SIDE_LENGTH;

				if((mZPosInBlock == POLYVOX_BLOCK_SIDE_LENGTH) || (mZPosInVolume > mZRegionLast))
				{
					//At this point we've left the current block. Find a new one...

					++mXBlock;
					++mBlockIndexInVolume;
					if(mXBlock > mXRegionLastBlock)
					{
						mXBlock = mXRegionFirstBlock;
						mBlockIndexInVolume = mXBlock + 
							mYBlock * POLYVOX_VOLUME_SIDE_LENGTH_IN_BLOCKS + 
							mZBlock * POLYVOX_VOLUME_SIDE_LENGTH_IN_BLOCKS * POLYVOX_VOLUME_SIDE_LENGTH_IN_BLOCKS;

						++mYBlock;
						mBlockIndexInVolume += POLYVOX_VOLUME_SIDE_LENGTH_IN_BLOCKS;
						if(mYBlock > mYRegionLastBlock)
						{
							mYBlock = mYRegionFirstBlock;
							mBlockIndexInVolume = mXBlock + 
								mYBlock * POLYVOX_VOLUME_SIDE_LENGTH_IN_BLOCKS + 
								mZBlock * POLYVOX_VOLUME_SIDE_LENGTH_IN_BLOCKS * POLYVOX_VOLUME_SIDE_LENGTH_IN_BLOCKS;

							++mZBlock;
							mBlockIndexInVolume += POLYVOX_VOLUME_SIDE_LENGTH_IN_BLOCKS * POLYVOX_VOLUME_SIDE_LENGTH_IN_BLOCKS;
							if(mZBlock > mZRegionLastBlock)
							{
								mIsValidForRegion = false;
								return;			
							}
						}
					}

					Block<VoxelType>* currentBlock = mVolume.mBlocks[mBlockIndexInVolume];
					//mCurrentBlock = mVolume->mBlocks[mBlockIndexInVolume];					

					mXPosInVolume = (std::max)(mXRegionFirst,uint16_t(mXBlock * POLYVOX_BLOCK_SIDE_LENGTH));					
					mYPosInVolume = (std::max)(mYRegionFirst,uint16_t(mYBlock * POLYVOX_BLOCK_SIDE_LENGTH));					
					mZPosInVolume = (std::max)(mZRegionFirst,uint16_t(mZBlock * POLYVOX_BLOCK_SIDE_LENGTH));					

					mXPosInBlock = mXPosInVolume - (mXBlock << POLYVOX_BLOCK_SIDE_LENGTH_POWER);
					mYPosInBlock = mYPosInVolume - (mYBlock << POLYVOX_BLOCK_SIDE_LENGTH_POWER);
					mZPosInBlock = mZPosInVolume - (mZBlock << POLYVOX_BLOCK_SIDE_LENGTH_POWER);

					mVoxelIndexInBlock = mXPosInBlock + 
						mYPosInBlock * POLYVOX_BLOCK_SIDE_LENGTH + 
						mZPosInBlock * POLYVOX_BLOCK_SIDE_LENGTH * POLYVOX_BLOCK_SIDE_LENGTH;

					mCurrentVoxel = currentBlock->mData + mVoxelIndexInBlock;
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
		if((mXPosInVolume%POLYVOX_BLOCK_SIDE_LENGTH != 0) && (mYPosInVolume%POLYVOX_BLOCK_SIDE_LENGTH != 0) && (mZPosInVolume%POLYVOX_BLOCK_SIDE_LENGTH != 0))
		{
			return *(mCurrentVoxel - 1 - POLYVOX_BLOCK_SIDE_LENGTH - POLYVOX_BLOCK_SIDE_LENGTH*POLYVOX_BLOCK_SIDE_LENGTH);
		}
		return getVoxelAt(mXPosInVolume-1,mYPosInVolume-1,mZPosInVolume-1);
	}

	template <typename VoxelType>
	VoxelType VolumeIterator<VoxelType>::peekVoxel1nx1ny0pz(void) const
	{
		if((mXPosInVolume%POLYVOX_BLOCK_SIDE_LENGTH != 0) && (mYPosInVolume%POLYVOX_BLOCK_SIDE_LENGTH != 0))
		{
			return *(mCurrentVoxel - 1 - POLYVOX_BLOCK_SIDE_LENGTH);
		}
		return getVoxelAt(mXPosInVolume-1,mYPosInVolume-1,mZPosInVolume);
	}

	template <typename VoxelType>
	VoxelType VolumeIterator<VoxelType>::peekVoxel1nx1ny1pz(void) const
	{
		if((mXPosInVolume%POLYVOX_BLOCK_SIDE_LENGTH != 0) && (mYPosInVolume%POLYVOX_BLOCK_SIDE_LENGTH != 0) && (mZPosInVolume%POLYVOX_BLOCK_SIDE_LENGTH != POLYVOX_BLOCK_SIDE_LENGTH-1))
		{
			return *(mCurrentVoxel - 1 - POLYVOX_BLOCK_SIDE_LENGTH + POLYVOX_BLOCK_SIDE_LENGTH*POLYVOX_BLOCK_SIDE_LENGTH);
		}
		return getVoxelAt(mXPosInVolume-1,mYPosInVolume-1,mZPosInVolume+1);
	}

	template <typename VoxelType>
	VoxelType VolumeIterator<VoxelType>::peekVoxel1nx0py1nz(void) const
	{
		if((mXPosInVolume%POLYVOX_BLOCK_SIDE_LENGTH != 0) && (mZPosInVolume%POLYVOX_BLOCK_SIDE_LENGTH != 0))
		{
			return *(mCurrentVoxel - 1 - POLYVOX_BLOCK_SIDE_LENGTH*POLYVOX_BLOCK_SIDE_LENGTH);
		}
		return getVoxelAt(mXPosInVolume-1,mYPosInVolume,mZPosInVolume-1);
	}

	template <typename VoxelType>
	VoxelType VolumeIterator<VoxelType>::peekVoxel1nx0py0pz(void) const
	{
		if((mXPosInVolume%POLYVOX_BLOCK_SIDE_LENGTH != 0))
		{
			return *(mCurrentVoxel - 1);
		}
		return getVoxelAt(mXPosInVolume-1,mYPosInVolume,mZPosInVolume);
	}

	template <typename VoxelType>
	VoxelType VolumeIterator<VoxelType>::peekVoxel1nx0py1pz(void) const
	{
		if((mXPosInVolume%POLYVOX_BLOCK_SIDE_LENGTH != 0) && (mZPosInVolume%POLYVOX_BLOCK_SIDE_LENGTH != POLYVOX_BLOCK_SIDE_LENGTH-1))
		{
			return *(mCurrentVoxel - 1 + POLYVOX_BLOCK_SIDE_LENGTH*POLYVOX_BLOCK_SIDE_LENGTH);
		}
		return getVoxelAt(mXPosInVolume-1,mYPosInVolume,mZPosInVolume+1);
	}

	template <typename VoxelType>
	VoxelType VolumeIterator<VoxelType>::peekVoxel1nx1py1nz(void) const
	{
		if((mXPosInVolume%POLYVOX_BLOCK_SIDE_LENGTH != 0) && (mYPosInVolume%POLYVOX_BLOCK_SIDE_LENGTH != POLYVOX_BLOCK_SIDE_LENGTH-1) && (mZPosInVolume%POLYVOX_BLOCK_SIDE_LENGTH != 0))
		{
			return *(mCurrentVoxel - 1 + POLYVOX_BLOCK_SIDE_LENGTH - POLYVOX_BLOCK_SIDE_LENGTH*POLYVOX_BLOCK_SIDE_LENGTH);
		}
		return getVoxelAt(mXPosInVolume-1,mYPosInVolume+1,mZPosInVolume-1);
	}

	template <typename VoxelType>
	VoxelType VolumeIterator<VoxelType>::peekVoxel1nx1py0pz(void) const
	{
		if((mXPosInVolume%POLYVOX_BLOCK_SIDE_LENGTH != 0) && (mYPosInVolume%POLYVOX_BLOCK_SIDE_LENGTH != POLYVOX_BLOCK_SIDE_LENGTH-1))
		{
			return *(mCurrentVoxel - 1 + POLYVOX_BLOCK_SIDE_LENGTH);
		}
		return getVoxelAt(mXPosInVolume-1,mYPosInVolume+1,mZPosInVolume);
	}

	template <typename VoxelType>
	VoxelType VolumeIterator<VoxelType>::peekVoxel1nx1py1pz(void) const
	{
		if((mXPosInVolume%POLYVOX_BLOCK_SIDE_LENGTH != 0) && (mYPosInVolume%POLYVOX_BLOCK_SIDE_LENGTH != POLYVOX_BLOCK_SIDE_LENGTH-1) && (mZPosInVolume%POLYVOX_BLOCK_SIDE_LENGTH != POLYVOX_BLOCK_SIDE_LENGTH-1))
		{
			return *(mCurrentVoxel - 1 + POLYVOX_BLOCK_SIDE_LENGTH + POLYVOX_BLOCK_SIDE_LENGTH*POLYVOX_BLOCK_SIDE_LENGTH);
		}
		return getVoxelAt(mXPosInVolume-1,mYPosInVolume+1,mZPosInVolume+1);
	}

	//////////////////////////////////////////////////////////////////////////

	template <typename VoxelType>
	VoxelType VolumeIterator<VoxelType>::peekVoxel0px1ny1nz(void) const
	{
		if((mYPosInVolume%POLYVOX_BLOCK_SIDE_LENGTH != 0) && (mZPosInVolume%POLYVOX_BLOCK_SIDE_LENGTH != 0))
		{
			return *(mCurrentVoxel - POLYVOX_BLOCK_SIDE_LENGTH - POLYVOX_BLOCK_SIDE_LENGTH*POLYVOX_BLOCK_SIDE_LENGTH);
		}
		return getVoxelAt(mXPosInVolume,mYPosInVolume-1,mZPosInVolume-1);
	}

	template <typename VoxelType>
	VoxelType VolumeIterator<VoxelType>::peekVoxel0px1ny0pz(void) const
	{
		if((mYPosInVolume%POLYVOX_BLOCK_SIDE_LENGTH != 0))
		{
			return *(mCurrentVoxel - POLYVOX_BLOCK_SIDE_LENGTH);
		}
		return getVoxelAt(mXPosInVolume,mYPosInVolume-1,mZPosInVolume);
	}

	template <typename VoxelType>
	VoxelType VolumeIterator<VoxelType>::peekVoxel0px1ny1pz(void) const
	{
		if((mYPosInVolume%POLYVOX_BLOCK_SIDE_LENGTH != 0) && (mZPosInVolume%POLYVOX_BLOCK_SIDE_LENGTH != POLYVOX_BLOCK_SIDE_LENGTH-1))
		{
			return *(mCurrentVoxel - POLYVOX_BLOCK_SIDE_LENGTH + POLYVOX_BLOCK_SIDE_LENGTH*POLYVOX_BLOCK_SIDE_LENGTH);
		}
		return getVoxelAt(mXPosInVolume,mYPosInVolume-1,mZPosInVolume+1);
	}

	template <typename VoxelType>
	VoxelType VolumeIterator<VoxelType>::peekVoxel0px0py1nz(void) const
	{
		if((mZPosInVolume%POLYVOX_BLOCK_SIDE_LENGTH != 0))
		{
			return *(mCurrentVoxel - POLYVOX_BLOCK_SIDE_LENGTH*POLYVOX_BLOCK_SIDE_LENGTH);
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
		if((mZPosInVolume%POLYVOX_BLOCK_SIDE_LENGTH != POLYVOX_BLOCK_SIDE_LENGTH-1))
		{
			return *(mCurrentVoxel + POLYVOX_BLOCK_SIDE_LENGTH*POLYVOX_BLOCK_SIDE_LENGTH);
		}
		return getVoxelAt(mXPosInVolume,mYPosInVolume,mZPosInVolume+1);
	}

	template <typename VoxelType>
	VoxelType VolumeIterator<VoxelType>::peekVoxel0px1py1nz(void) const
	{
		if((mYPosInVolume%POLYVOX_BLOCK_SIDE_LENGTH != POLYVOX_BLOCK_SIDE_LENGTH-1) && (mZPosInVolume%POLYVOX_BLOCK_SIDE_LENGTH != 0))
		{
			return *(mCurrentVoxel + POLYVOX_BLOCK_SIDE_LENGTH - POLYVOX_BLOCK_SIDE_LENGTH*POLYVOX_BLOCK_SIDE_LENGTH);
		}
		return getVoxelAt(mXPosInVolume,mYPosInVolume+1,mZPosInVolume-1);
	}

	template <typename VoxelType>
	VoxelType VolumeIterator<VoxelType>::peekVoxel0px1py0pz(void) const
	{
		if((mYPosInVolume%POLYVOX_BLOCK_SIDE_LENGTH != POLYVOX_BLOCK_SIDE_LENGTH-1))
		{
			return *(mCurrentVoxel + POLYVOX_BLOCK_SIDE_LENGTH);
		}
		return getVoxelAt(mXPosInVolume,mYPosInVolume+1,mZPosInVolume);
	}

	template <typename VoxelType>
	VoxelType VolumeIterator<VoxelType>::peekVoxel0px1py1pz(void) const
	{
		if((mYPosInVolume%POLYVOX_BLOCK_SIDE_LENGTH != POLYVOX_BLOCK_SIDE_LENGTH-1) && (mZPosInVolume%POLYVOX_BLOCK_SIDE_LENGTH != POLYVOX_BLOCK_SIDE_LENGTH-1))
		{
			return *(mCurrentVoxel + POLYVOX_BLOCK_SIDE_LENGTH + POLYVOX_BLOCK_SIDE_LENGTH*POLYVOX_BLOCK_SIDE_LENGTH);
		}
		return getVoxelAt(mXPosInVolume,mYPosInVolume+1,mZPosInVolume+1);
	}

	//////////////////////////////////////////////////////////////////////////

	template <typename VoxelType>
	VoxelType VolumeIterator<VoxelType>::peekVoxel1px1ny1nz(void) const
	{
		if((mXPosInVolume%POLYVOX_BLOCK_SIDE_LENGTH != POLYVOX_BLOCK_SIDE_LENGTH-1) && (mYPosInVolume%POLYVOX_BLOCK_SIDE_LENGTH != 0) && (mZPosInVolume%POLYVOX_BLOCK_SIDE_LENGTH != 0))
		{
			return *(mCurrentVoxel + 1 - POLYVOX_BLOCK_SIDE_LENGTH - POLYVOX_BLOCK_SIDE_LENGTH*POLYVOX_BLOCK_SIDE_LENGTH);
		}
		return getVoxelAt(mXPosInVolume+1,mYPosInVolume-1,mZPosInVolume-1);
	}

	template <typename VoxelType>
	VoxelType VolumeIterator<VoxelType>::peekVoxel1px1ny0pz(void) const
	{
		if((mXPosInVolume%POLYVOX_BLOCK_SIDE_LENGTH != POLYVOX_BLOCK_SIDE_LENGTH-1) && (mYPosInVolume%POLYVOX_BLOCK_SIDE_LENGTH != 0))
		{
			return *(mCurrentVoxel + 1 - POLYVOX_BLOCK_SIDE_LENGTH);
		}
		return getVoxelAt(mXPosInVolume+1,mYPosInVolume-1,mZPosInVolume);
	}

	template <typename VoxelType>
	VoxelType VolumeIterator<VoxelType>::peekVoxel1px1ny1pz(void) const
	{
		if((mXPosInVolume%POLYVOX_BLOCK_SIDE_LENGTH != POLYVOX_BLOCK_SIDE_LENGTH-1) && (mYPosInVolume%POLYVOX_BLOCK_SIDE_LENGTH != 0) && (mZPosInVolume%POLYVOX_BLOCK_SIDE_LENGTH != POLYVOX_BLOCK_SIDE_LENGTH-1))
		{
			return *(mCurrentVoxel + 1 - POLYVOX_BLOCK_SIDE_LENGTH + POLYVOX_BLOCK_SIDE_LENGTH*POLYVOX_BLOCK_SIDE_LENGTH);
		}
		return getVoxelAt(mXPosInVolume+1,mYPosInVolume-1,mZPosInVolume+1);
	}

	template <typename VoxelType>
	VoxelType VolumeIterator<VoxelType>::peekVoxel1px0py1nz(void) const
	{
		if((mXPosInVolume%POLYVOX_BLOCK_SIDE_LENGTH != POLYVOX_BLOCK_SIDE_LENGTH-1) && (mZPosInVolume%POLYVOX_BLOCK_SIDE_LENGTH != 0))
		{
			return *(mCurrentVoxel + 1 - POLYVOX_BLOCK_SIDE_LENGTH*POLYVOX_BLOCK_SIDE_LENGTH);
		}
		return getVoxelAt(mXPosInVolume+1,mYPosInVolume,mZPosInVolume-1);
	}

	template <typename VoxelType>
	VoxelType VolumeIterator<VoxelType>::peekVoxel1px0py0pz(void) const
	{
		if((mXPosInVolume%POLYVOX_BLOCK_SIDE_LENGTH != POLYVOX_BLOCK_SIDE_LENGTH-1))
		{
			return *(mCurrentVoxel + 1);
		}
		return getVoxelAt(mXPosInVolume+1,mYPosInVolume,mZPosInVolume);
	}

	template <typename VoxelType>
	VoxelType VolumeIterator<VoxelType>::peekVoxel1px0py1pz(void) const
	{
		if((mXPosInVolume%POLYVOX_BLOCK_SIDE_LENGTH != POLYVOX_BLOCK_SIDE_LENGTH-1) && (mZPosInVolume%POLYVOX_BLOCK_SIDE_LENGTH != POLYVOX_BLOCK_SIDE_LENGTH-1))
		{
			return *(mCurrentVoxel + 1 + POLYVOX_BLOCK_SIDE_LENGTH*POLYVOX_BLOCK_SIDE_LENGTH);
		}
		return getVoxelAt(mXPosInVolume+1,mYPosInVolume,mZPosInVolume+1);
	}

	template <typename VoxelType>
	VoxelType VolumeIterator<VoxelType>::peekVoxel1px1py1nz(void) const
	{
		if((mXPosInVolume%POLYVOX_BLOCK_SIDE_LENGTH != POLYVOX_BLOCK_SIDE_LENGTH-1) && (mYPosInVolume%POLYVOX_BLOCK_SIDE_LENGTH != POLYVOX_BLOCK_SIDE_LENGTH-1) && (mZPosInVolume%POLYVOX_BLOCK_SIDE_LENGTH != 0))
		{
			return *(mCurrentVoxel + 1 + POLYVOX_BLOCK_SIDE_LENGTH - POLYVOX_BLOCK_SIDE_LENGTH*POLYVOX_BLOCK_SIDE_LENGTH);
		}
		return getVoxelAt(mXPosInVolume+1,mYPosInVolume+1,mZPosInVolume-1);
	}

	template <typename VoxelType>
	VoxelType VolumeIterator<VoxelType>::peekVoxel1px1py0pz(void) const
	{
		if((mXPosInVolume%POLYVOX_BLOCK_SIDE_LENGTH != POLYVOX_BLOCK_SIDE_LENGTH-1) && (mYPosInVolume%POLYVOX_BLOCK_SIDE_LENGTH != POLYVOX_BLOCK_SIDE_LENGTH-1))
		{
			return *(mCurrentVoxel + 1 + POLYVOX_BLOCK_SIDE_LENGTH);
		}
		return getVoxelAt(mXPosInVolume+1,mYPosInVolume+1,mZPosInVolume);
	}

	template <typename VoxelType>
	VoxelType VolumeIterator<VoxelType>::peekVoxel1px1py1pz(void) const
	{
		if((mXPosInVolume%POLYVOX_BLOCK_SIDE_LENGTH != POLYVOX_BLOCK_SIDE_LENGTH-1) && (mYPosInVolume%POLYVOX_BLOCK_SIDE_LENGTH != POLYVOX_BLOCK_SIDE_LENGTH-1) && (mZPosInVolume%POLYVOX_BLOCK_SIDE_LENGTH != POLYVOX_BLOCK_SIDE_LENGTH-1))
		{
			return *(mCurrentVoxel + 1 + POLYVOX_BLOCK_SIDE_LENGTH + POLYVOX_BLOCK_SIDE_LENGTH*POLYVOX_BLOCK_SIDE_LENGTH);
		}
		return getVoxelAt(mXPosInVolume+1,mYPosInVolume+1,mZPosInVolume+1);
	}
}

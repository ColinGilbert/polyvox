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
#include "Log.h"
#include "VolumeIterator.h"
#include "Region.h"
#include "Vector.h"

#include <cassert>
#include <cstring> //For memcpy
#include <list>
#include <stdexcept> //For invalid_argument
#pragma endregion

namespace PolyVox
{
	#pragma region Constructors/Destructors
	template <typename VoxelType>
	Volume<VoxelType>::Volume(uint16_t uWidth, uint16_t uHeight, uint16_t uDepth, uint16_t uBlockSideLength)
		:m_pBlocks(0)
		,m_uCurrentBlockForTidying(0)
	{
		//Debug mode validation
		assert(isPowerOf2(uWidth));
		assert(isPowerOf2(uHeight));
		assert(isPowerOf2(uDepth));
		assert(isPowerOf2(uBlockSideLength));
		assert(uBlockSideLength <= uWidth);
		assert(uBlockSideLength <= uHeight);
		assert(uBlockSideLength <= uDepth);

		//Release mode validation
		if(!(isPowerOf2(uWidth) && isPowerOf2(uHeight) && isPowerOf2(uDepth)))
		{
			throw std::invalid_argument("Volume width, height, and depth must all be a power of two.");
		}
		if(!isPowerOf2(uBlockSideLength))
		{
			throw std::invalid_argument("Block side length must be a power of two.");
		}
		if(uBlockSideLength > uWidth)
		{
			throw std::invalid_argument("Block side length cannot be greater than volume width.");
		}
		if(uBlockSideLength > uHeight)
		{
			throw std::invalid_argument("Block side length cannot be greater than volume height.");
		}
		if(uBlockSideLength > uDepth)
		{
			throw std::invalid_argument("Block side length cannot be greater than volume depth.");
		}

		//Compute the volume side lengths
		m_uWidth = uWidth;
		m_uWidthPower = logBase2(m_uWidth);

		m_uHeight = uHeight;
		m_uHeightPower = logBase2(m_uHeight);

		m_uDepth = uDepth;
		m_uDepthPower = logBase2(m_uDepth);

		//Compute the block side length
		m_uBlockSideLength = uBlockSideLength;
		m_uBlockSideLengthPower = logBase2(m_uBlockSideLength);

		//Compute the side length in blocks
		//m_uSideLengthInBlocks = m_uSideLength / m_uBlockSideLength;
		m_uWidthInBlocks = m_uWidth / m_uBlockSideLength;
		m_uHeightInBlocks = m_uHeight / m_uBlockSideLength;
		m_uDepthInBlocks = m_uDepth / m_uBlockSideLength;

		//Compute number of blocks in the volume
		m_uNoOfBlocksInVolume = m_uWidthInBlocks * m_uHeightInBlocks * m_uDepthInBlocks;

		//Create the blocks
		m_pBlocks.resize(m_uNoOfBlocksInVolume);
		m_vecBlockIsPotentiallyHomogenous.resize(m_uNoOfBlocksInVolume);
		for(uint32_t i = 0; i < m_uNoOfBlocksInVolume; ++i)
		{
			m_pBlocks[i] = getHomogenousBlockData(0);
			m_vecBlockIsPotentiallyHomogenous[i] = false;
		}

		//Other properties we might find useful later
		m_uLongestSideLength = (std::max)((std::max)(m_uWidth,m_uHeight),m_uDepth);
		m_uShortestSideLength = (std::min)((std::min)(m_uWidth,m_uHeight),m_uDepth);
		m_fDiagonalLength = sqrtf(static_cast<float>(m_uWidth * m_uWidth + m_uHeight * m_uHeight + m_uDepth * m_uDepth));
	}

	/*template <typename VoxelType>
	Volume<VoxelType>::Volume(const Volume<VoxelType>& rhs)
	{
		*this = rhs;
	}*/

	template <typename VoxelType>
	Volume<VoxelType>::~Volume()
	{
	}
	#pragma endregion

	#pragma region Operators
	/*template <typename VoxelType>
	Volume<VoxelType>& Volume<VoxelType>::operator=(const Volume& rhs)
	{
		
	}*/
	#pragma endregion		

	#pragma region Getters
	template <typename VoxelType>
	Region Volume<VoxelType>::getEnclosingRegion(void) const
	{
		return Region(Vector3DInt32(0,0,0), Vector3DInt32(m_uWidth-1,m_uHeight-1,m_uDepth-1));
	}

	template <typename VoxelType>
	uint16_t Volume<VoxelType>::getDepth(void) const
	{
		return m_uDepth;
	}

	template <typename VoxelType>
	float Volume<VoxelType>::getDiagonalLength(void) const
	{
		return m_fDiagonalLength;
	}

	template <typename VoxelType>
	uint16_t Volume<VoxelType>::getHeight(void) const
	{
		return m_uHeight;
	}

	template <typename VoxelType>
	uint16_t Volume<VoxelType>::getLongestSideLength(void) const
	{
		return m_uLongestSideLength;
	}

	template <typename VoxelType>
	uint16_t Volume<VoxelType>::getShortestSideLength(void) const
	{
		return m_uShortestSideLength;
	}

	template <typename VoxelType>
	uint16_t Volume<VoxelType>::getWidth(void) const
	{
		return m_uWidth;
	}		

	template <typename VoxelType>
	VoxelType Volume<VoxelType>::getVoxelAt(uint16_t uXPos, uint16_t uYPos, uint16_t uZPos) const
	{
		assert(uXPos < getWidth());
		assert(uYPos < getHeight());
		assert(uZPos < getDepth());

		const uint16_t blockX = uXPos >> m_uBlockSideLengthPower;
		const uint16_t blockY = uYPos >> m_uBlockSideLengthPower;
		const uint16_t blockZ = uZPos >> m_uBlockSideLengthPower;

		const uint16_t xOffset = uXPos - (blockX << m_uBlockSideLengthPower);
		const uint16_t yOffset = uYPos - (blockY << m_uBlockSideLengthPower);
		const uint16_t zOffset = uZPos - (blockZ << m_uBlockSideLengthPower);

		const POLYVOX_SHARED_PTR< BlockData< VoxelType > >& block = m_pBlocks
			[
				blockX + 
				blockY * m_uWidthInBlocks + 
				blockZ * m_uWidthInBlocks * m_uHeightInBlocks
			];

		return block->getVoxelAt(xOffset,yOffset,zOffset);
	}

	template <typename VoxelType>
	VoxelType Volume<VoxelType>::getVoxelAt(const Vector3DUint16& v3dPos) const
	{
		assert(v3dPos.getX() < m_uWidth);
		assert(v3dPos.getY() < m_uHeight);
		assert(v3dPos.getZ() < m_uDepth);

		return getVoxelAt(v3dPos.getX(), v3dPos.getY(), v3dPos.getZ());
	}
	#pragma endregion	

	#pragma region Setters
	template <typename VoxelType>
	void Volume<VoxelType>::setVoxelAt(uint16_t uXPos, uint16_t uYPos, uint16_t uZPos, VoxelType tValue)
	{
		const uint16_t blockX = uXPos >> m_uBlockSideLengthPower;
		const uint16_t blockY = uYPos >> m_uBlockSideLengthPower;
		const uint16_t blockZ = uZPos >> m_uBlockSideLengthPower;

		const uint16_t xOffset = uXPos - (blockX << m_uBlockSideLengthPower);
		const uint16_t yOffset = uYPos - (blockY << m_uBlockSideLengthPower);
		const uint16_t zOffset = uZPos - (blockZ << m_uBlockSideLengthPower);

		uint32_t uBlockIndex =
			blockX + 
			blockY * m_uWidthInBlocks + 
			blockZ * m_uWidthInBlocks * m_uHeightInBlocks;

		POLYVOX_SHARED_PTR< BlockData<VoxelType> >& block = m_pBlocks[uBlockIndex];

		//It's quite possible that the user might attempt to set a voxel to it's current value.
		//We test for this case firstly because it could help performance, but more importantly
		//because it lets us avoid unsharing blocks unnecessarily.
		if(block->getVoxelAt(xOffset, yOffset, zOffset) != tValue)
		{
			if(block.unique())
			{
				block->setVoxelAt(xOffset,yOffset,zOffset, tValue);
				//There is a chance that setting this voxel makes the block homogenous and therefore shareable.
				//But checking this will take some time, so for now just set a flag.
				m_vecBlockIsPotentiallyHomogenous[uBlockIndex] = true;
			}
			else
			{			
				POLYVOX_SHARED_PTR< BlockData<VoxelType> > pNewBlockData(new BlockData<VoxelType>(*(block)));
				block = pNewBlockData;
				m_vecBlockIsPotentiallyHomogenous[uBlockIndex] = false;
				block->setVoxelAt(xOffset,yOffset,zOffset, tValue);
			}
		}
	}

	template <typename VoxelType>
	void Volume<VoxelType>::setVoxelAt(const Vector3DUint16& v3dPos, VoxelType tValue)
	{
		setVoxelAt(v3dPos.getX(), v3dPos.getY(), v3dPos.getZ(), tValue);
	}
	#pragma endregion

	#pragma region Other
	template <typename VoxelType>
	void Volume<VoxelType>::tidyUpMemory(uint32_t uNoOfBlocksToProcess)
	{
		//Track the number of blocks we have processed.
		uint32_t m_uNoOfProcessedBlocks = 0;

		//We will loop around, and finish if we get back to our start position
		uint32_t uFinishBlock = m_uCurrentBlockForTidying;

		//Increment the current block, looping around if necessary
		++m_uCurrentBlockForTidying;
		m_uCurrentBlockForTidying %= m_uNoOfBlocksInVolume;

		//While we have not reached the user specified limit and there are more blocks to process...
		while((m_uNoOfProcessedBlocks < uNoOfBlocksToProcess) && (m_uCurrentBlockForTidying != uFinishBlock))
		{
			//We only do any work if the block is flagged as potentially homogeneous.
			if(m_vecBlockIsPotentiallyHomogenous[m_uCurrentBlockForTidying])
			{
				//Check if it's really homogeneous (this can be slow).
				if(m_pBlocks[m_uCurrentBlockForTidying]->isHomogeneous())
				{
					//If so, replace is with a block from out homogeneous collection.
					VoxelType homogeneousValue = m_pBlocks[m_uCurrentBlockForTidying]->getVoxelAt(0,0,0);
					m_pBlocks[m_uCurrentBlockForTidying] = getHomogenousBlockData(homogeneousValue);
				}

				//Either way, we have now determined whether the block was sharable. So it's not *potentially* sharable.
				m_vecBlockIsPotentiallyHomogenous[m_uCurrentBlockForTidying] = false;

				//We've processed a block. This is inside the 'if' because the path outside the 'if' is trivially fast.
				++m_uNoOfProcessedBlocks;
			}

			//Increment the current block, looping around if necessary
			++m_uCurrentBlockForTidying;
			m_uCurrentBlockForTidying %= m_uNoOfBlocksInVolume;
		}

		//Identify and remove any homogeneous blocks which are not actually in use.
		typename std::map<VoxelType, POLYVOX_SHARED_PTR< BlockData<VoxelType> > >::iterator iter = m_pHomogenousBlockData.begin();
		while(iter != m_pHomogenousBlockData.end())
		{
			if(iter->second.unique())
			{
				m_pHomogenousBlockData.erase(iter++); //Increments the iterator and returns the previous position to be erased.
			}
			else
			{
				++iter; //Just increments the iterator.
			}
		}
	}	

	template <typename VoxelType>
	bool Volume<VoxelType>::isRegionHomogenous(const Region& region)
	{
		VolumeIterator<VoxelType> iter(*this);
		iter.setValidRegion(region);
		iter.setPosition(static_cast<Vector3DInt16>(region.getLowerCorner()));

		VoxelType tFirst = iter.getVoxel();
		iter.moveForwardInRegionXYZ();

		do
		{
			VoxelType tCurrent = iter.getVoxel();
			if(tCurrent != tFirst)
			{
				return false;
			}
		}while(iter.moveForwardInRegionXYZ());

		return true;
	}
	#pragma endregion

	#pragma region Private Implementation
	template <typename VoxelType>
	POLYVOX_SHARED_PTR< BlockData<VoxelType> > Volume<VoxelType>::getHomogenousBlockData(VoxelType tHomogenousValue) const
	{
		typename std::map<VoxelType, POLYVOX_SHARED_PTR< BlockData<VoxelType> > >::iterator iterResult = m_pHomogenousBlockData.find(tHomogenousValue);
		if(iterResult == m_pHomogenousBlockData.end())
		{
			//Block<VoxelType> block;
			POLYVOX_SHARED_PTR< BlockData<VoxelType> > pHomogeneousBlock(new BlockData<VoxelType>(m_uBlockSideLength));
			//block.m_pBlockData = temp;
			//block.m_uReferenceCount++;
			pHomogeneousBlock->fill(tHomogenousValue);
			m_pHomogenousBlockData.insert(std::make_pair(tHomogenousValue, pHomogeneousBlock));
			return pHomogeneousBlock;
		}
		else
		{
			//iterResult->second.m_uReferenceCount++;
			//POLYVOX_SHARED_PTR< BlockData<VoxelType> > result(iterResult->second);
			return iterResult->second;
		}
	}
	#pragma endregion
}

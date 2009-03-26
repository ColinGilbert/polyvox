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
#include "BlockData.h"
#include "VolumeIterator.h"
#include "Region.h"
#include "Vector.h"

#include <cassert>
#include <cstring> //For memcpy
#pragma endregion

namespace PolyVox
{
	#pragma region Constructors/Destructors
	template <typename VoxelType>
	Volume<VoxelType>::Volume(uint16 uSideLength, uint16 uBlockSideLength)
		:m_pBlocks(0)
	{
		//Debug mode validation
		assert(isPowerOf2(uSideLength));
		assert(isPowerOf2(uBlockSideLength));
		assert(uBlockSideLength <= uSideLength);

		//Release mode validation
		if(!isPowerOf2(uSideLength))
		{
			throw std::invalid_argument("Volume side length must be a power of two.");
		}
		if(!isPowerOf2(uBlockSideLength))
		{
			throw std::invalid_argument("Block side length must be a power of two.");
		}
		if(uBlockSideLength > uSideLength)
		{
			throw std::invalid_argument("Block side length cannot be less than volume side length.");
		}

		//Compute the volume side length
		m_uSideLength = uSideLength;
		m_uSideLengthPower = logBase2(m_uSideLength);

		//Compute the block side length
		m_uBlockSideLength = uBlockSideLength;
		m_uBlockSideLengthPower = logBase2(m_uBlockSideLength);

		//Compute the side length in blocks
		m_uSideLengthInBlocks = m_uSideLength / m_uBlockSideLength;

		//Compute number of blocks in the volume
		m_uNoOfBlocksInVolume = m_uSideLengthInBlocks * m_uSideLengthInBlocks * m_uSideLengthInBlocks;

		//Create the blocks
		/*m_pBlocks = new Block<VoxelType>*[m_uNoOfBlocksInVolume];
		m_pIsShared = new bool[m_uNoOfBlocksInVolume];
		m_pIsPotentiallySharable = new bool[m_uNoOfBlocksInVolume];
		m_pHomogenousValue = new VoxelType[m_uNoOfBlocksInVolume];
		for(uint32 i = 0; i < m_uNoOfBlocksInVolume; ++i)
		{
			m_pBlocks[i] = getHomogenousBlock(0);
			m_pIsShared[i] = true;
			m_pIsPotentiallySharable[i] = false;
			m_pHomogenousValue[i] = 0;
		}*/

		m_pBlocks = new Block<VoxelType>[m_uNoOfBlocksInVolume];
		for(uint32 i = 0; i < m_uNoOfBlocksInVolume; ++i)
		{
			m_pBlocks[i].m_pBlockData = getHomogenousBlock(0);
			m_pBlocks[i].m_pIsShared = true;
			m_pBlocks[i].m_pIsPotentiallySharable = false;
			m_pBlocks[i].m_pHomogenousValue = 0;
		}
	}

	template <typename VoxelType>
	Volume<VoxelType>::Volume(const Volume<VoxelType>& rhs)
	{
		*this = rhs;
	}

	template <typename VoxelType>
	Volume<VoxelType>::~Volume()
	{
		for(uint32 i = 0; i < m_uNoOfBlocksInVolume; ++i)
		{
			if(m_pBlocks[i].m_pIsShared == false)
			{
				delete m_pBlocks[i].m_pBlockData;
				m_pBlocks[i].m_pBlockData = 0;
			}
		}
		delete[] m_pBlocks;
		/*delete[] m_pIsShared;
		delete[] m_pIsPotentiallySharable;
		delete[] m_pHomogenousValue;*/
	}
	#pragma endregion

	#pragma region Operators
	template <typename VoxelType>
	Volume<VoxelType>& Volume<VoxelType>::operator=(const Volume& rhs)
	{
		
	}
	#pragma endregion		

	#pragma region Getters
	template <typename VoxelType>
	Region Volume<VoxelType>::getEnclosingRegion(void) const
	{
		return Region(Vector3DInt32(0,0,0), Vector3DInt32(m_uSideLength-1,m_uSideLength-1,m_uSideLength-1));
	}

	template <typename VoxelType>
	uint16 Volume<VoxelType>::getSideLength(void) const
	{
		return m_uSideLength;
	}

	template <typename VoxelType>
	VoxelType Volume<VoxelType>::getVoxelAt(uint16 uXPos, uint16 uYPos, uint16 uZPos) const
	{
		assert(uXPos < getSideLength());
		assert(uYPos < getSideLength());
		assert(uZPos < getSideLength());

		const uint16 blockX = uXPos >> m_uBlockSideLengthPower;
		const uint16 blockY = uYPos >> m_uBlockSideLengthPower;
		const uint16 blockZ = uZPos >> m_uBlockSideLengthPower;

		const uint16 xOffset = uXPos - (blockX << m_uBlockSideLengthPower);
		const uint16 yOffset = uYPos - (blockY << m_uBlockSideLengthPower);
		const uint16 zOffset = uZPos - (blockZ << m_uBlockSideLengthPower);

		const BlockData<VoxelType>* block = m_pBlocks
			[
				blockX + 
				blockY * m_uSideLengthInBlocks + 
				blockZ * m_uSideLengthInBlocks * m_uSideLengthInBlocks
			].m_pBlockData;

		return block->getVoxelAt(xOffset,yOffset,zOffset);
	}

	template <typename VoxelType>
	VoxelType Volume<VoxelType>::getVoxelAt(const Vector3DUint16& v3dPos) const
	{
		assert(v3dPos.getX() < m_uSideLength);
		assert(v3dPos.getY() < m_uSideLength);
		assert(v3dPos.getZ() < m_uSideLength);

		return getVoxelAt(v3dPos.getX(), v3dPos.getY(), v3dPos.getZ());
	}
	#pragma endregion	

	#pragma region Setters
	template <typename VoxelType>
	void Volume<VoxelType>::setVoxelAt(uint16 uXPos, uint16 uYPos, uint16 uZPos, VoxelType tValue)
	{
		const uint16 blockX = uXPos >> m_uBlockSideLengthPower;
		const uint16 blockY = uYPos >> m_uBlockSideLengthPower;
		const uint16 blockZ = uZPos >> m_uBlockSideLengthPower;

		const uint16 xOffset = uXPos - (blockX << m_uBlockSideLengthPower);
		const uint16 yOffset = uYPos - (blockY << m_uBlockSideLengthPower);
		const uint16 zOffset = uZPos - (blockZ << m_uBlockSideLengthPower);

		const uint32 uBlockIndex = 
				blockX + 
				blockY * m_uSideLengthInBlocks + 
				blockZ * m_uSideLengthInBlocks * m_uSideLengthInBlocks;

		const bool bIsShared = m_pBlocks[uBlockIndex].m_pIsShared;
		if(bIsShared)
		{
			const VoxelType tHomogenousValue = m_pBlocks[uBlockIndex].m_pHomogenousValue;
			if(tHomogenousValue != tValue)
			{
				m_pBlocks[uBlockIndex].m_pBlockData = new BlockData<VoxelType>(m_uBlockSideLength);
				m_pBlocks[uBlockIndex].m_pIsShared = false;
				m_pBlocks[uBlockIndex].m_pBlockData->fill(tHomogenousValue);
				m_pBlocks[uBlockIndex].m_pBlockData->setVoxelAt(xOffset,yOffset,zOffset, tValue);
			}
		}
		else
		{			
			m_pBlocks[uBlockIndex].m_pBlockData->setVoxelAt(xOffset,yOffset,zOffset, tValue);
			//There is a chance that setting this voxel makes the block homogenous and therefore shareable. But checking
			//this will take some time, so for now just set a flag.
			m_pBlocks[uBlockIndex].m_pIsPotentiallySharable = true;
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
	VolumeIterator<VoxelType> Volume<VoxelType>::firstVoxel(void)
	{
		VolumeIterator<VoxelType> iter(*this);
		iter.setPosition(0,0,0);
		return iter;
	}

	template <typename VoxelType>
	void Volume<VoxelType>::idle(uint32 uAmount)
	{
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

	template <typename VoxelType>
	VolumeIterator<VoxelType> Volume<VoxelType>::lastVoxel(void)
	{
		VolumeIterator<VoxelType> iter(*this);
		iter.setPosition(m_uSideLength-1,m_uSideLength-1,m_uSideLength-1);
		return iter;
	}
	#pragma endregion

	#pragma region Private Implementation
	template <typename VoxelType>
	BlockData<VoxelType>* Volume<VoxelType>::getHomogenousBlock(VoxelType tHomogenousValue) const
	{
		typename std::map<VoxelType, BlockData<VoxelType>*>::iterator iterResult = m_pHomogenousBlocks.find(tHomogenousValue);
		if(iterResult == m_pHomogenousBlocks.end())
		{
			BlockData<VoxelType>* pBlock = new BlockData<VoxelType>(m_uBlockSideLength);
			pBlock->fill(tHomogenousValue);
			m_pHomogenousBlocks.insert(std::make_pair(tHomogenousValue, pBlock));
			return pBlock;
		}
		return iterResult->second;
	}
	#pragma endregion
}

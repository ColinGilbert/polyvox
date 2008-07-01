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
#include "Region.h"
#include "Vector.h"

#include <cassert>
#include <cstring> //For memcpy
#pragma endregion

namespace PolyVox
{
	#pragma region Constructors/Destructors
	template <typename VoxelType>
	BlockVolume<VoxelType>::BlockVolume(uint8 uSideLengthPower, uint8 uBlockSideLengthPower)
		:m_pBlocks(0)
	{
		//Check the volume size is sensible. This corresponds to a side length of 65536 voxels
		if(uSideLengthPower > 16)
		{
			throw std::invalid_argument("Volume side length power must be less than or equal to 16");
		}

		//Compute the volume side length
		m_uSideLengthPower = uSideLengthPower;
		m_uSideLength = 0x01 << uSideLengthPower;

		//Compute the block side length
		m_uBlockSideLengthPower = uBlockSideLengthPower;
		m_uBlockSideLength = 0x01 << uBlockSideLengthPower;

		//Compute the side length in blocks
		m_uSideLengthInBlocks = m_uSideLength / m_uBlockSideLength;

		//Compute number of blocks in the volume
		m_uNoOfBlocksInVolume = m_uSideLengthInBlocks * m_uSideLengthInBlocks * m_uSideLengthInBlocks;

		//Create the blocks
		m_pBlocks = new Block<VoxelType>*[m_uNoOfBlocksInVolume];
		m_pIsShared = new bool[m_uNoOfBlocksInVolume];
		m_pIsPotentiallySharable = new bool[m_uNoOfBlocksInVolume];
		m_pHomogenousValue = new VoxelType[m_uNoOfBlocksInVolume];
		for(uint32 i = 0; i < m_uNoOfBlocksInVolume; ++i)
		{
			m_pBlocks[i] = getHomogenousBlock(0); //new Block<VoxelType>(uBlockSideLengthPower);
			m_pIsShared[i] = true;
			m_pIsPotentiallySharable[i] = false;
			m_pHomogenousValue[i] = 0;
		}
	}

	template <typename VoxelType>
	BlockVolume<VoxelType>::BlockVolume(const BlockVolume<VoxelType>& rhs)
	{
		*this = rhs;
	}

	template <typename VoxelType>
	BlockVolume<VoxelType>::~BlockVolume()
	{
		for(uint32 i = 0; i < m_uNoOfBlocksInVolume; ++i)
		{
			delete m_pBlocks[i];
		}
	}
	#pragma endregion

	#pragma region Operators
	template <typename VoxelType>
	BlockVolume<VoxelType>& BlockVolume<VoxelType>::operator=(const BlockVolume& rhs)
	{
		if (this == &rhs)
		{
			return *this;
		}

		/*for(uint16 i = 0; i < POLYVOX_NO_OF_BLOCKS_IN_VOLUME; ++i)
		{
			//FIXME - Add checking...
			m_pBlocks[i] = SharedPtr<Block>(new Block);
		}*/

		for(uint32 i = 0; i < m_uNoOfBlocksInVolume; ++i)
		{
			//I think this is OK... If a block is in the homogeneous array it's ref count will be greater
			//than 1 as there will be the pointer in the volume and the pointer in the static homogeneous array.
			/*if(rhs.m_pBlocks[i].unique())
			{
				m_pBlocks[i] = SharedPtr<Block>(new Block(*(rhs.m_pBlocks[i])));
			}
			else
			{*/
				//we have a block in the homogeneous array - just copy the pointer.
				m_pBlocks[i] = rhs.m_pBlocks[i];
			//}
		}

		return *this;
	}
	#pragma endregion		

	#pragma region Getters
	template <typename VoxelType>
	Region BlockVolume<VoxelType>::getEnclosingRegion(void) const
	{
		return Region(Vector3DInt32(0,0,0), Vector3DInt32(m_uSideLength-1,m_uSideLength-1,m_uSideLength-1));
	}

	template <typename VoxelType>
	uint16 BlockVolume<VoxelType>::getSideLength(void) const
	{
		return m_uSideLength;
	}

	template <typename VoxelType>
	VoxelType BlockVolume<VoxelType>::getVoxelAt(uint16 uXPos, uint16 uYPos, uint16 uZPos) const
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

		const Block<VoxelType>* block = m_pBlocks
			[
				blockX + 
				blockY * m_uSideLengthInBlocks + 
				blockZ * m_uSideLengthInBlocks * m_uSideLengthInBlocks
			];

		return block->getVoxelAt(xOffset,yOffset,zOffset);
	}

	template <typename VoxelType>
	VoxelType BlockVolume<VoxelType>::getVoxelAt(const Vector3DUint16& v3dPos) const
	{
		assert(v3dPos.getX() < m_uSideLength);
		assert(v3dPos.getY() < m_uSideLength);
		assert(v3dPos.getZ() < m_uSideLength);

		return getVoxelAt(v3dPos.getX(), v3dPos.getY(), v3dPos.getZ());
	}
	#pragma endregion	

	#pragma region Other
	template <typename VoxelType>
	bool BlockVolume<VoxelType>::containsPoint(const Vector3DFloat& pos, float boundary) const
	{
		return (pos.getX() <= m_uSideLength - 1 - boundary)
			&& (pos.getY() <= m_uSideLength - 1 - boundary) 
			&& (pos.getZ() <= m_uSideLength - 1 - boundary)
			&& (pos.getX() >= boundary)
			&& (pos.getY() >= boundary)
			&& (pos.getZ() >= boundary);
	}

	template <typename VoxelType>
	bool BlockVolume<VoxelType>::containsPoint(const Vector3DInt32& pos, uint16 boundary) const
	{
		return (pos.getX() <= m_uSideLength - 1 - boundary)
			&& (pos.getY() <= m_uSideLength - 1 - boundary) 
			&& (pos.getZ() <= m_uSideLength - 1 - boundary)
			&& (pos.getX() >= boundary)
			&& (pos.getY() >= boundary)
			&& (pos.getZ() >= boundary);
	}

	template <typename VoxelType>
	BlockVolumeIterator<VoxelType> BlockVolume<VoxelType>::firstVoxel(void)
	{
		BlockVolumeIterator<VoxelType> iter(*this);
		iter.setPosition(0,0,0);
		return iter;
	}

	template <typename VoxelType>
	void BlockVolume<VoxelType>::idle(uint32 uAmount)
	{
	}	

	template <typename VoxelType>
	BlockVolumeIterator<VoxelType> BlockVolume<VoxelType>::lastVoxel(void)
	{
		BlockVolumeIterator<VoxelType> iter(*this);
		iter.setPosition(m_uSideLength-1,m_uSideLength-1,m_uSideLength-1);
		return iter;
	}
	#pragma endregion

	#pragma region Private Implementation
	template <typename VoxelType>
	Block<VoxelType>* BlockVolume<VoxelType>::getHomogenousBlock(VoxelType tHomogenousValue) const
	{
		typename std::map<VoxelType, Block<VoxelType>*>::iterator iterResult = m_pHomogenousBlocks.find(tHomogenousValue);
		if(iterResult == m_pHomogenousBlocks.end())
		{
			Block<VoxelType>* pBlock = new Block<VoxelType>(m_uBlockSideLengthPower);
			pBlock->fill(tHomogenousValue);
			m_pHomogenousBlocks.insert(std::make_pair(tHomogenousValue, pBlock));
			return pBlock;
		}
		return  iterResult->second;
	}
	#pragma endregion
}

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
	BlockVolume<VoxelType>::BlockVolume(boost::uint8_t uSideLengthPower, boost::uint8_t uBlockSideLengthPower)
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
		for(boost::uint32_t i = 0; i < m_uNoOfBlocksInVolume; ++i)
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
		for(boost::uint32_t i = 0; i < m_uNoOfBlocksInVolume; ++i)
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

		/*for(uint16_t i = 0; i < POLYVOX_NO_OF_BLOCKS_IN_VOLUME; ++i)
		{
			//FIXME - Add checking...
			m_pBlocks[i] = SharedPtr<Block>(new Block);
		}*/

		for(uint32_t i = 0; i < m_uNoOfBlocksInVolume; ++i)
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
	boost::uint16_t BlockVolume<VoxelType>::getSideLength(void) const
	{
		return m_uSideLength;
	}

	template <typename VoxelType>
	VoxelType BlockVolume<VoxelType>::getVoxelAt(boost::uint16_t uXPos, boost::uint16_t uYPos, boost::uint16_t uZPos) const
	{
		assert(uXPos < getSideLength());
		assert(uYPos < getSideLength());
		assert(uZPos < getSideLength());

		const uint16_t blockX = uXPos >> m_uBlockSideLengthPower;
		const uint16_t blockY = uYPos >> m_uBlockSideLengthPower;
		const uint16_t blockZ = uZPos >> m_uBlockSideLengthPower;

		const uint16_t xOffset = uXPos - (blockX << m_uBlockSideLengthPower);
		const uint16_t yOffset = uYPos - (blockY << m_uBlockSideLengthPower);
		const uint16_t zOffset = uZPos - (blockZ << m_uBlockSideLengthPower);

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
		assert(v3dPos.x() < m_uSideLength);
		assert(v3dPos.y() < m_uSideLength);
		assert(v3dPos.z() < m_uSideLength);

		return getVoxelAt(v3dPos.x(), v3dPos.y(), v3dPos.z());
	}
	#pragma endregion	

	#pragma region Other
	template <typename VoxelType>
	bool BlockVolume<VoxelType>::containsPoint(const Vector3DFloat& pos, float boundary) const
	{
		return (pos.x() <= m_uSideLength - 1 - boundary)
			&& (pos.y() <= m_uSideLength - 1 - boundary) 
			&& (pos.z() <= m_uSideLength - 1 - boundary)
			&& (pos.x() >= boundary)
			&& (pos.y() >= boundary)
			&& (pos.z() >= boundary);
	}

	template <typename VoxelType>
	bool BlockVolume<VoxelType>::containsPoint(const Vector3DInt32& pos, boost::uint16_t boundary) const
	{
		return (pos.x() <= m_uSideLength - 1 - boundary)
			&& (pos.y() <= m_uSideLength - 1 - boundary) 
			&& (pos.z() <= m_uSideLength - 1 - boundary)
			&& (pos.x() >= boundary)
			&& (pos.y() >= boundary)
			&& (pos.z() >= boundary);
	}

	template <typename VoxelType>
	VolumeIterator<VoxelType> BlockVolume<VoxelType>::firstVoxel(void)
	{
		VolumeIterator<VoxelType> iter(*this);
		iter.setPosition(0,0,0);
		return iter;
	}

	template <typename VoxelType>
	void BlockVolume<VoxelType>::idle(boost::uint32_t uAmount)
	{
	}	

	template <typename VoxelType>
	VolumeIterator<VoxelType> BlockVolume<VoxelType>::lastVoxel(void)
	{
		VolumeIterator<VoxelType> iter(*this);
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

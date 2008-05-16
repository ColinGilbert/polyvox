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

#include <cassert>
#include <cstring> //For memcpy
#pragma endregion

namespace PolyVox
{
	#pragma region Constructors/Destructors
	template <typename VoxelType>
	Volume<VoxelType>::Volume(boost::uint8_t uSideLengthPower, boost::uint8_t uBlockSideLengthPower)
		:mBlocks(0)
	{
		//Check the volume size is sensible. This corresponds to a side length of 65536 voxels
		if(uSideLengthPower > 16)
		{
			throw std::invalid_argument("Block side length must be less than or equal to 65536");
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

		mBlocks = new Block<VoxelType>*[m_uNoOfBlocksInVolume];
		for(boost::uint32_t i = 0; i < m_uNoOfBlocksInVolume; ++i)
		{
			mBlocks[i] = 0;
		}

		for(boost::uint32_t i = 0; i < m_uNoOfBlocksInVolume; ++i)
		{
			mBlocks[i] = new Block<VoxelType>(uBlockSideLengthPower);
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
		for(boost::uint32_t i = 0; i < m_uNoOfBlocksInVolume; ++i)
		{
			delete mBlocks[i];
		}
	}
	#pragma endregion

	#pragma region Operators
	template <typename VoxelType>
	Volume<VoxelType>& Volume<VoxelType>::operator=(const Volume& rhs)
	{
		if (this == &rhs)
		{
			return *this;
		}

		/*for(uint16_t i = 0; i < POLYVOX_NO_OF_BLOCKS_IN_VOLUME; ++i)
		{
			//FIXME - Add checking...
			mBlocks[i] = SharedPtr<Block>(new Block);
		}*/

		for(uint32_t i = 0; i < m_uNoOfBlocksInVolume; ++i)
		{
			//I think this is OK... If a block is in the homogeneous array it's ref count will be greater
			//than 1 as there will be the pointer in the volume and the pointer in the static homogeneous array.
			/*if(rhs.mBlocks[i].unique())
			{
				mBlocks[i] = SharedPtr<Block>(new Block(*(rhs.mBlocks[i])));
			}
			else
			{*/
				//we have a block in the homogeneous array - just copy the pointer.
				mBlocks[i] = rhs.mBlocks[i];
			//}
		}

		return *this;
	}
	#pragma endregion		

	#pragma region Getters
	template <typename VoxelType>
	boost::uint16_t Volume<VoxelType>::getSideLength(void)
	{
		return m_uSideLength;
	}

	template <typename VoxelType>
	VoxelType Volume<VoxelType>::getVoxelAt(boost::uint16_t uXPos, boost::uint16_t uYPos, boost::uint16_t uZPos) const
	{
		assert(uXPos < mVolume.getSideLength());
		assert(uYPos < mVolume.getSideLength());
		assert(uZPos < mVolume.getSideLength());

		const uint16_t blockX = uXPos >> m_uBlockSideLengthPower;
		const uint16_t blockY = uYPos >> m_uBlockSideLengthPower;
		const uint16_t blockZ = uZPos >> m_uBlockSideLengthPower;

		const uint16_t xOffset = uXPos - (blockX << m_uBlockSideLengthPower);
		const uint16_t yOffset = uYPos - (blockY << m_uBlockSideLengthPower);
		const uint16_t zOffset = uZPos - (blockZ << m_uBlockSideLengthPower);

		const Block<VoxelType>* block = mBlocks
			[
				blockX + 
				blockY * m_uSideLengthInBlocks + 
				blockZ * m_uSideLengthInBlocks * m_uSideLengthInBlocks
			];

		return block->getVoxelAt(xOffset,yOffset,zOffset);
	}

	template <typename VoxelType>
	VoxelType Volume<VoxelType>::getVoxelAt(const Vector3DUint16& v3dPos) const
	{
		assert(v3dPos.x() < m_uSideLength);
		assert(v3dPos.y() < m_uSideLength);
		assert(v3dPos.z() < m_uSideLength);

		return getVoxelAt(v3dPos.x(), v3dPos.y(), v3dPos.z());
	}
	#pragma endregion

	#pragma region Setters
	template <typename VoxelType>
	void Volume<VoxelType>::setVoxelAt(boost::uint16_t uXPos, boost::uint16_t uYPos, boost::uint16_t uZPos, VoxelType tValue)
	{
		assert(uXPos < mVolume.getSideLength());
		assert(uYPos < mVolume.getSideLength());
		assert(uZPos < mVolume.getSideLength());

		const uint16_t blockX = uXPos >> m_uBlockSideLengthPower;
		const uint16_t blockY = uYPos >> m_uBlockSideLengthPower;
		const uint16_t blockZ = uZPos >> m_uBlockSideLengthPower;

		const uint16_t xOffset = uXPos - (blockX << m_uBlockSideLengthPower);
		const uint16_t yOffset = uYPos - (blockY << m_uBlockSideLengthPower);
		const uint16_t zOffset = uZPos - (blockZ << m_uBlockSideLengthPower);

		const Block<VoxelType>* block = mBlocks
			[
				blockX + 
				blockY * m_uSideLengthInBlocks + 
				blockZ * m_uSideLengthInBlocks * m_uSideLengthInBlocks
			];

		return block->setVoxelAt(xOffset,yOffset,zOffset, tValue);
	}

	template <typename VoxelType>
	void Volume<VoxelType>::setVoxelAt(const Vector3DUint16& v3dPos, VoxelType tValue)
	{
		assert(v3dPos.x() < m_uSideLength);
		assert(v3dPos.y() < m_uSideLength);
		assert(v3dPos.z() < m_uSideLength);

		setVoxelAt(v3dPos.x(), v3dPos.y(), v3dPos.z(), tValue);
	}
	#pragma endregion

	#pragma region Other
	template <typename VoxelType>
	bool Volume<VoxelType>::containsPoint(Vector3DFloat pos, float boundary) const
	{
		return (pos.x() <= m_uSideLength - 1 - boundary)
			&& (pos.y() <= m_uSideLength - 1 - boundary) 
			&& (pos.z() <= m_uSideLength - 1 - boundary)
			&& (pos.x() >= boundary)
			&& (pos.y() >= boundary)
			&& (pos.z() >= boundary);
	}

	template <typename VoxelType>
	bool Volume<VoxelType>::containsPoint(Vector3DInt32 pos, boost::uint16_t boundary) const
	{
		return (pos.x() <= m_uSideLength - 1 - boundary)
			&& (pos.y() <= m_uSideLength - 1 - boundary) 
			&& (pos.z() <= m_uSideLength - 1 - boundary)
			&& (pos.x() >= boundary)
			&& (pos.y() >= boundary)
			&& (pos.z() >= boundary);
	}
	#pragma endregion
}

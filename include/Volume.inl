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

#include <fstream>
#include <iostream> //FIXME - remove this...
#include <queue>

#include "Block.h"
#include "VolumeIterator.h" //Maybe this shouldn't be here?

namespace PolyVox
{

	template <typename VoxelType>
	Volume<VoxelType>::Volume(boost::uint8_t uSideLengthPower, boost::uint8_t uBlockSideLengthPower)
		:mBlocks(0)
	{
		//Check the volume size is sensible. This corresponds to a side length of 65536 voxels
		assert(uSideLengthPower <= 16);

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
		std::cout << "Warning - Copying Volume" << std::endl;
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

	template <typename VoxelType>
	Volume<VoxelType>& Volume<VoxelType>::operator=(const Volume& rhs)
	{
		std::cout << "Warning - Assigning Volume" << std::endl;
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

	template <typename VoxelType>
	Block<VoxelType>* Volume<VoxelType>::getBlock(boost::uint16_t index)
	{
		return mBlocks[index];
	}

	template <typename VoxelType>
	bool Volume<VoxelType>::containsPoint(Vector3DFloat pos, float boundary)
	{
		return (pos.x() < m_uSideLength - 1 - boundary)
			&& (pos.y() < m_uSideLength - 1 - boundary) 
			&& (pos.z() < m_uSideLength - 1 - boundary)
			&& (pos.x() > boundary)
			&& (pos.y() > boundary)
			&& (pos.z() > boundary);
	}

	template <typename VoxelType>
	bool Volume<VoxelType>::containsPoint(Vector3DInt32 pos, boost::uint16_t boundary)
	{
		return (pos.x() < m_uSideLength - 1 - boundary)
			&& (pos.y() < m_uSideLength - 1 - boundary) 
			&& (pos.z() < m_uSideLength - 1 - boundary)
			&& (pos.x() > boundary)
			&& (pos.y() > boundary)
			&& (pos.z() > boundary);
	}	

	template <typename VoxelType>
	void Volume<VoxelType>::tidy(void)
	{
		//Check for homogeneous blocks
		/*for(uint32_t ct = 0; ct < POLYVOX_NO_OF_BLOCKS_IN_VOLUME; ++ct)
		{
			if(mBlocks[ct]->isHomogeneous())
			{
				//LogManager::getSingleton().logMessage("Got homogeneous block with value " + stringConverter::tostring(mBlocks[ct]->getVoxelAt(0,0,0)));

				const VoxelType homogeneousValue = mBlocks[ct]->getVoxelAt(0,0,0);
				SharedPtr<Block>& homogeneousBlock = mHomogeneousBlocks[homogeneousValue];
				if(homogeneousBlock.isNull())
				{
					homogeneousBlock = SharedPtr<Block>(new Block);
					homogeneousBlock->fillWithValue(homogeneousValue);
				}
				mBlocks[ct] = homogeneousBlock;
			}
		}*/
	}

	template <typename VoxelType>
	boost::uint16_t Volume<VoxelType>::getSideLength(void)
	{
		return m_uSideLength;
	}

	template <typename VoxelType>
	boost::uint16_t Volume<VoxelType>::getSideLengthInBlocks(void)
	{
		return m_uSideLengthInBlocks;
	}

	template <typename VoxelType>
	boost::uint16_t Volume<VoxelType>::getBlockSideLength(void)
	{
		return m_uBlockSideLength;
	}

	template <typename VoxelType>
	boost::uint16_t Volume<VoxelType>::getBlockSideLengthPower(void)
	{
		return m_uBlockSideLengthPower;
	}
}

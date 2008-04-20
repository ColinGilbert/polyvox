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

#include <cstring>

namespace PolyVox
{

	template <typename VoxelType>
	Block<VoxelType>::Block()
	{
	}

	template <typename VoxelType>
	Block<VoxelType>::Block(const Block<VoxelType>& rhs)
	{
		*this = rhs;
	}

	template <typename VoxelType>
	Block<VoxelType>::~Block()
	{
	}

	template <typename VoxelType>
	Block<VoxelType>& Block<VoxelType>::operator=(const Block<VoxelType>& rhs)
	{
		if (this == &rhs)
		{
			return *this;
		}

		memcpy(mData,rhs.mData,POLYVOX_NO_OF_VOXELS_IN_BLOCK);

		return *this;
	}

	template <typename VoxelType>
	VoxelType Block<VoxelType>::getVoxelAt(const boost::uint16_t xPosition, const boost::uint16_t yPosition, const boost::uint16_t zPosition) const
	{
		return mData
			[
				xPosition + 
				yPosition * POLYVOX_BLOCK_SIDE_LENGTH + 
				zPosition * POLYVOX_BLOCK_SIDE_LENGTH * POLYVOX_BLOCK_SIDE_LENGTH
			];
	}

	template <typename VoxelType>
	void Block<VoxelType>::setVoxelAt(const boost::uint16_t xPosition, const boost::uint16_t yPosition, const boost::uint16_t zPosition, const VoxelType value)
	{
		mData
			[
				xPosition + 
				yPosition * POLYVOX_BLOCK_SIDE_LENGTH + 
				zPosition * POLYVOX_BLOCK_SIDE_LENGTH * POLYVOX_BLOCK_SIDE_LENGTH
			] = value;
	}

	/*void Block::fillWithValue(const uint8_t value)
	{
		memset(mData,value,POLYVOX_NO_OF_VOXELS_IN_BLOCK);
	}*/

	/*bool Block::isHomogeneous(void)
	{
		uint8_t uFirstVoxel = mData[0];
		for(uint32_t ct = 1; ct < POLYVOX_NO_OF_VOXELS_IN_BLOCK; ++ct)
		{
			if(mData[ct] != uFirstVoxel)
			{
				return false;
			}
		}
		return true;
	}*/
}

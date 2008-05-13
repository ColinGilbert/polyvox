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
	LinearVolume<VoxelType>::LinearVolume(boost::uint8_t uSideLengthPower)
		:m_tData(0)
	{
		//Check the block size is sensible. This corresponds to a side length of 256 voxels
		assert(uSideLengthPower <= 8);

		//Compute the side length
		m_uSideLengthPower = uSideLengthPower;
		m_uSideLength = 0x01 << uSideLengthPower;

		//If this fails an exception will be thrown. Memory is not   
		//allocated and there is nothing else in this class to clean up
		m_tData = new VoxelType[getNoOfVoxels()];
	}

	template <typename VoxelType>
	LinearVolume<VoxelType>::LinearVolume(const LinearVolume<VoxelType>& rhs)
	{
		*this = rhs;
	}

	template <typename VoxelType>
	LinearVolume<VoxelType>::~LinearVolume()
	{
		delete[] m_tData;
		m_tData = 0;
	}

	template <typename VoxelType>
	LinearVolume<VoxelType>& LinearVolume<VoxelType>::operator=(const LinearVolume<VoxelType>& rhs)
	{
		if (this == &rhs)
		{
			return *this;
		}

		memcpy(m_tData,rhs.m_tData,getNoOfVoxels());

		return *this;
	}

	template <typename VoxelType>
	VoxelType LinearVolume<VoxelType>::getVoxelAt(const boost::uint16_t xPosition, const boost::uint16_t yPosition, const boost::uint16_t zPosition) const
	{
		return m_tData
			[
				xPosition + 
				yPosition * m_uSideLength + 
				zPosition * m_uSideLength * m_uSideLength
			];
	}

	template <typename VoxelType>
	void LinearVolume<VoxelType>::setVoxelAt(const boost::uint16_t xPosition, const boost::uint16_t yPosition, const boost::uint16_t zPosition, const VoxelType value)
	{
		m_tData
			[
				xPosition + 
				yPosition * m_uSideLength + 
				zPosition * m_uSideLength * m_uSideLength
			] = value;
	}

	template <typename VoxelType>
	boost::uint16_t LinearVolume<VoxelType>::getSideLength(void)
	{
		return m_uSideLength;
	}

	template <typename VoxelType>
	boost::uint32_t LinearVolume<VoxelType>::getNoOfVoxels(void)
	{
		return m_uSideLength * m_uSideLength * m_uSideLength;
	}
}

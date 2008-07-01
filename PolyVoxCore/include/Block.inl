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
#include <cassert>
#include <cstring> //For memcpy
#include <stdexcept> //for std::invalid_argument
#pragma endregion

namespace PolyVox
{
	#pragma region Constructors/Destructors
	template <typename VoxelType>
	Block<VoxelType>::Block(uint8 uSideLengthPower)
		:m_tData(0)
	{
		//Check the block size is sensible. This corresponds to a side length of 256 voxels
		if(uSideLengthPower > 8)
		{
			throw std::invalid_argument("Block side length power must be less than or equal to eight");
		}

		//Compute the side length
		m_uSideLengthPower = uSideLengthPower;
		m_uSideLength = 0x01 << uSideLengthPower;

		//If this fails an exception will be thrown. Memory is not   
		//allocated and there is nothing else in this class to clean up
		m_tData = new VoxelType[m_uSideLength * m_uSideLength * m_uSideLength];
	}

	template <typename VoxelType>
	Block<VoxelType>::Block(const Block<VoxelType>& rhs)
	{
		*this = rhs;
	}

	template <typename VoxelType>
	Block<VoxelType>::~Block()
	{
		delete[] m_tData;
		m_tData = 0;
	}
	#pragma endregion

	#pragma region Operators
	template <typename VoxelType>
	Block<VoxelType>& Block<VoxelType>::operator=(const Block<VoxelType>& rhs)
	{
		if (this == &rhs)
		{
			return *this;
		}
		memcpy(m_tData, rhs.m_tData, m_uSideLength * m_uSideLength * m_uSideLength);
		return *this;
	}
	#pragma endregion

	#pragma region Getters
	template <typename VoxelType>
	uint16 Block<VoxelType>::getSideLength(void) const
	{
		return m_uSideLength;
	}

	template <typename VoxelType>
	VoxelType Block<VoxelType>::getVoxelAt(uint16 uXPos, uint16 uYPos, uint16 uZPos) const
	{
		assert(uXPos < m_uSideLength);
		assert(uYPos < m_uSideLength);
		assert(uZPos < m_uSideLength);

		return m_tData
			[
				uXPos + 
				uYPos * m_uSideLength + 
				uZPos * m_uSideLength * m_uSideLength
			];
	}
	#pragma endregion

	#pragma region Setters
	template <typename VoxelType>
	void Block<VoxelType>::setVoxelAt(uint16 uXPos, uint16 uYPos, uint16 uZPos, VoxelType tValue)
	{
		assert(uXPos < m_uSideLength);
		assert(uYPos < m_uSideLength);
		assert(uZPos < m_uSideLength);

		m_tData
		[
			uXPos + 
			uYPos * m_uSideLength + 
			uZPos * m_uSideLength * m_uSideLength
		] = tValue;
	}
	#pragma endregion

	#pragma region Other
	template <typename VoxelType>
	void Block<VoxelType>::fill(VoxelType tValue)
	{
		memset(m_tData, tValue, m_uSideLength * m_uSideLength * m_uSideLength * sizeof(VoxelType));
	}
	#pragma endregion
}

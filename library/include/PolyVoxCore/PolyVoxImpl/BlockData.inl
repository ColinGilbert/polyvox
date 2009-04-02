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
#include "Utility.h"
#include "Vector.h"
#include "Volume.h"

#include <cassert>
#include <cstring> //For memcpy
#include <stdexcept> //for std::invalid_argument
#pragma endregion

namespace PolyVox
{
	#pragma region Constructors/Destructors
	template <typename VoxelType>
	BlockData<VoxelType>::BlockData(uint16_t uSideLength)
		:m_tData(0)
	{
		//Debug mode validation
		assert(isPowerOf2(uSideLength));

		//Release mode validation
		if(!isPowerOf2(uSideLength))
		{
			throw std::invalid_argument("BlockData side length must be a power of two.");
		}

		//Compute the side length		
		m_uSideLength = uSideLength;
		m_uSideLengthPower = logBase2(uSideLength);

		//If this fails an exception will be thrown. Memory is not   
		//allocated and there is nothing else in this class to clean up
		m_tData = new VoxelType[m_uSideLength * m_uSideLength * m_uSideLength];
	}

	template <typename VoxelType>
	BlockData<VoxelType>::BlockData(const BlockData<VoxelType>& rhs)
	{
		*this = rhs;
	}

	template <typename VoxelType>
	BlockData<VoxelType>::~BlockData()
	{
		delete[] m_tData;
		m_tData = 0;
	}
	#pragma endregion

	#pragma region Operators
	template <typename VoxelType>
	BlockData<VoxelType>& BlockData<VoxelType>::operator=(const BlockData<VoxelType>& rhs)
	{
		if (this == &rhs)
		{
			return *this;
		}

		m_uSideLength = rhs.m_uSideLength;
		m_uSideLengthPower = rhs.m_uSideLengthPower;
		memcpy(m_tData, rhs.m_tData, m_uSideLength * m_uSideLength * m_uSideLength);

		return *this;
	}
	#pragma endregion

	#pragma region Getters
	template <typename VoxelType>
	uint16_t BlockData<VoxelType>::getSideLength(void) const
	{
		return m_uSideLength;
	}

	template <typename VoxelType>
	VoxelType BlockData<VoxelType>::getVoxelAt(uint16_t uXPos, uint16_t uYPos, uint16_t uZPos) const
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

	template <typename VoxelType>
	VoxelType BlockData<VoxelType>::getVoxelAt(const Vector3DUint16& v3dPos) const
	{
		return getVoxelAt(v3dPos.getX(), v3dPos.getY(), v3dPos.getZ());
	}
	#pragma endregion

	#pragma region Setters
	template <typename VoxelType>
	void BlockData<VoxelType>::setVoxelAt(uint16_t uXPos, uint16_t uYPos, uint16_t uZPos, VoxelType tValue)
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

	template <typename VoxelType>
	void BlockData<VoxelType>::setVoxelAt(const Vector3DUint16& v3dPos, VoxelType tValue)
	{
		setVoxelAt(v3dPos.getX(), v3dPos.getY(), v3dPos.getZ(), tValue);
	}
	#pragma endregion

	#pragma region Other
	template <typename VoxelType>
	void BlockData<VoxelType>::fill(VoxelType tValue)
	{
		memset(m_tData, tValue, m_uSideLength * m_uSideLength * m_uSideLength * sizeof(VoxelType));
	}

	template <typename VoxelType>
	bool BlockData<VoxelType>::isHomogeneous(void)
	{
		VoxelType currentVoxel = m_tData;
		VoxelType firstVal = *currentVoxel;

		uint32_t uNoOfVoxels = m_uSideLength * m_uSideLength * m_uSideLength;
		for(uint32_t ct = 1; ct < uNoOfVoxels; ++ct)
		{
			++currentVoxel;
			if(*currentVoxel != firstVal)
			{
				return false;
			}
		}
		return true;
	}
	#pragma endregion
}

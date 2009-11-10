#pragma region License
/*******************************************************************************
Copyright (c) 2005-2009 David Williams

This software is provided 'as-is', without any express or implied
warranty. In no event will the authors be held liable for any damages
arising from the use of this software.

Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute it
freely, subject to the following restrictions:

    1. The origin of this software must not be misrepresented; you must not
    claim that you wrote the original software. If you use this software
    in a product, an acknowledgment in the product documentation would be
    appreciated but is not required.

    2. Altered source versions must be plainly marked as such, and must not be
    misrepresented as being the original software.

    3. This notice may not be removed or altered from any source
    distribution. 	
*******************************************************************************/
#pragma endregion

#pragma region Headers
#include "PolyVoxImpl/Utility.h"
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
	Block<VoxelType>::Block(uint16_t uSideLength)
		:m_tData(0)
	{
		//Debug mode validation
		assert(isPowerOf2(uSideLength));

		//Release mode validation
		if(!isPowerOf2(uSideLength))
		{
			throw std::invalid_argument("Block side length must be a power of two.");
		}

		//Compute the side length		
		m_uSideLength = uSideLength;
		m_uSideLengthPower = logBase2(uSideLength);

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

		//If this fails an exception will be thrown. Memory is not   
		//allocated and there is nothing else in this class to clean up
		m_tData = new VoxelType[rhs.m_uSideLength * rhs.m_uSideLength * rhs.m_uSideLength];

		//Copy the data
		m_uSideLength = rhs.m_uSideLength;
		m_uSideLengthPower = rhs.m_uSideLengthPower;		
		memcpy(m_tData, rhs.m_tData, m_uSideLength * m_uSideLength * m_uSideLength * sizeof(VoxelType));

		return *this;
	}
	#pragma endregion

	#pragma region Getters
	template <typename VoxelType>
	uint16_t Block<VoxelType>::getSideLength(void) const
	{
		return m_uSideLength;
	}

	template <typename VoxelType>
	VoxelType Block<VoxelType>::getVoxelAt(uint16_t uXPos, uint16_t uYPos, uint16_t uZPos) const
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
	VoxelType Block<VoxelType>::getVoxelAt(const Vector3DUint16& v3dPos) const
	{
		return getVoxelAt(v3dPos.getX(), v3dPos.getY(), v3dPos.getZ());
	}
	#pragma endregion

	#pragma region Setters
	template <typename VoxelType>
	void Block<VoxelType>::setVoxelAt(uint16_t uXPos, uint16_t uYPos, uint16_t uZPos, VoxelType tValue)
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
	void Block<VoxelType>::setVoxelAt(const Vector3DUint16& v3dPos, VoxelType tValue)
	{
		setVoxelAt(v3dPos.getX(), v3dPos.getY(), v3dPos.getZ(), tValue);
	}
	#pragma endregion

	#pragma region Other
	template <typename VoxelType>
	void Block<VoxelType>::fill(VoxelType tValue)
	{
		//The memset *may* be faster than the std::fill(), but it doesn't compile nicely
		//in 64-bit mode as casting the pointer to an int causes a loss of precision.

		//memset(m_tData, (int)tValue, m_uSideLength * m_uSideLength * m_uSideLength * sizeof(VoxelType));
		const uint32_t uNoOfVoxels = m_uSideLength * m_uSideLength * m_uSideLength;
		std::fill(m_tData, m_tData + uNoOfVoxels, tValue);
	}

	template <typename VoxelType>
	bool Block<VoxelType>::isHomogeneous(void)
	{
		const VoxelType tFirstVoxel = m_tData[0];
		const uint32_t uNoOfVoxels = m_uSideLength * m_uSideLength * m_uSideLength;

		for(uint32_t ct = 1; ct < uNoOfVoxels; ++ct)
		{
			if(m_tData[ct] != tFirstVoxel)
			{
				return false;
			}
		}
		return true;
	}

	template <typename VoxelType>
	uint32_t Block<VoxelType>::sizeInChars(void)
	{
		uint32_t uSizeInChars = sizeof(Block<VoxelType>);

		if(m_tData != 0)
		{
			const uint32_t uNoOfVoxels = m_uSideLength * m_uSideLength * m_uSideLength;
			uSizeInChars += uNoOfVoxels * sizeof(VoxelType);
		}

		return  uSizeInChars;
	}
	#pragma endregion
}

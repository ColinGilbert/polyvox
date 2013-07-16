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

#include "PolyVoxCore/Impl/ErrorHandling.h"
#include "PolyVoxCore/Impl/Utility.h"

#include "PolyVoxCore/Compressor.h"
#include "PolyVoxCore/Vector.h"

#include "PolyVoxCore/Impl/ErrorHandling.h"

#include <cstring> //For memcpy
#include <limits>
#include <stdexcept> //for std::invalid_argument

namespace PolyVox
{
	////////////////////////////////////////////////////////////////////////////////

	template <typename VoxelType>
	CompressedBlock<VoxelType>::CompressedBlock()
		:m_pData(0)
		,m_uDataSizeInBytes(0)
	{
	}

	template <typename VoxelType>
	CompressedBlock<VoxelType>::~CompressedBlock()
	{
		delete[] m_pData;
		m_pData = 0;
	}

	template <typename VoxelType>
	const uint8_t* CompressedBlock<VoxelType>::getData(void) const
	{
		return m_pData;
	}

	template <typename VoxelType>
	uint32_t CompressedBlock<VoxelType>::getDataSizeInBytes(void) const
	{
		return m_uDataSizeInBytes;
	}

	template <typename VoxelType>
	void CompressedBlock<VoxelType>::setData(const uint8_t* const pData, uint32_t uDataSizeInBytes)
	{
		POLYVOX_THROW_IF(pData == 0, std::invalid_argument, "Pointer to data cannot be null");
		POLYVOX_THROW_IF(m_pData == pData, std::invalid_argument, "Attempting to copy data onto itself");

		// Delete any existing data
		delete[] m_pData;

		// Allocate new data
		m_uDataSizeInBytes = uDataSizeInBytes;
		m_pData = new uint8_t[uDataSizeInBytes];

		// Copy the data across
		memcpy(m_pData, pData, uDataSizeInBytes);

		// Flag as modified
		m_bDataModified = true;
	}

	template <typename VoxelType>
	uint32_t CompressedBlock<VoxelType>::calculateSizeInBytes(void)
	{
		// Returns the size of this class plus the size of the compressed data. It
		// doesn't include the uncompressed data cache as that is owned by the volume.
		uint32_t uSizeInBytes = sizeof(CompressedBlock<VoxelType>) + m_uDataSizeInBytes;
		return  uSizeInBytes;
	}

	////////////////////////////////////////////////////////////////////////////////

	template <typename VoxelType>
	UncompressedBlock<VoxelType>::UncompressedBlock(uint16_t uSideLength)
		:m_tUncompressedData(0)
		,m_uSideLength(0)
		,m_uSideLengthPower(0)
	{
		// Compute the side length               
		m_uSideLength = uSideLength;
		m_uSideLengthPower = logBase2(uSideLength);

		// Allocate the data
		const uint32_t uNoOfVoxels = m_uSideLength * m_uSideLength * m_uSideLength;
		m_tUncompressedData = new VoxelType[uNoOfVoxels];               
	}

	template <typename VoxelType>
	UncompressedBlock<VoxelType>::~UncompressedBlock()
	{
		delete m_tUncompressedData;
		m_tUncompressedData = 0;
	}

	template <typename VoxelType>
	uint16_t UncompressedBlock<VoxelType>::getSideLength(void) const
	{
		return m_uSideLength;
	}

	template <typename VoxelType>
	VoxelType UncompressedBlock<VoxelType>::getVoxel(uint16_t uXPos, uint16_t uYPos, uint16_t uZPos) const
	{
		// This is internal code not directly called by the user. For efficiency we assert rather than throwing.
		POLYVOX_ASSERT(uXPos < m_uSideLength, "Supplied position is outside of the block");
		POLYVOX_ASSERT(uYPos < m_uSideLength, "Supplied position is outside of the block");
		POLYVOX_ASSERT(uZPos < m_uSideLength, "Supplied position is outside of the block");
		POLYVOX_ASSERT(m_tUncompressedData, "No uncompressed data - block must be decompressed before accessing voxels.");

		return m_tUncompressedData
			[
				uXPos + 
				uYPos * m_uSideLength + 
				uZPos * m_uSideLength * m_uSideLength
			];
	}

	template <typename VoxelType>
	VoxelType UncompressedBlock<VoxelType>::getVoxel(const Vector3DUint16& v3dPos) const
	{
		return getVoxel(v3dPos.getX(), v3dPos.getY(), v3dPos.getZ());
	}

	template <typename VoxelType>
	void UncompressedBlock<VoxelType>::setVoxelAt(uint16_t uXPos, uint16_t uYPos, uint16_t uZPos, VoxelType tValue)
	{
		// This is internal code not directly called by the user. For efficiency we assert rather than throwing.
		POLYVOX_ASSERT(uXPos < m_uSideLength, "Supplied position is outside of the block");
		POLYVOX_ASSERT(uYPos < m_uSideLength, "Supplied position is outside of the block");
		POLYVOX_ASSERT(uZPos < m_uSideLength, "Supplied position is outside of the block");
		POLYVOX_ASSERT(m_tUncompressedData, "No uncompressed data - block must be decompressed before accessing voxels.");

		m_tUncompressedData
			[
				uXPos + 
				uYPos * m_uSideLength + 
				uZPos * m_uSideLength * m_uSideLength
			] = tValue;

		m_bDataModified = true;
	}

	template <typename VoxelType>
    void UncompressedBlock<VoxelType>::setVoxelAt(const Vector3DUint16& v3dPos, VoxelType tValue)
    {
		setVoxelAt(v3dPos.getX(), v3dPos.getY(), v3dPos.getZ(), tValue);
    }
}

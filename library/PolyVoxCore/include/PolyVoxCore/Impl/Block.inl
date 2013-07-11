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
	template <typename VoxelType>
	Block<VoxelType>::Block(uint16_t uSideLength, Compressor* pCompressor)
		:m_pCompressedData(0)
		,m_uCompressedDataLength(0)
	{
		if(uSideLength == 0)
		{
			POLYVOX_THROW(std::invalid_argument, "Block side length cannot be zero.");
		}

		if(!isPowerOf2(uSideLength))
		{
			POLYVOX_THROW(std::invalid_argument, "Block side length must be a power of two.");
		}

		if(pCompressor == 0)
		{
			POLYVOX_THROW(std::invalid_argument, "Block must be provided with a valid compressor.");
		}

	}

	template <typename VoxelType>
	const uint8_t* Block<VoxelType>::getCompressedData(void) const
	{
		POLYVOX_ASSERT(m_pCompressedData, "Compressed data is NULL");
		return m_pCompressedData;
	}
	
	template <typename VoxelType>
	uint32_t Block<VoxelType>::getCompressedDataLength(void) const
	{
		POLYVOX_ASSERT(m_pCompressedData, "Compressed data is NULL");
		return m_uCompressedDataLength;
	}

	template <typename VoxelType>
	void Block<VoxelType>::setCompressedData(const uint8_t* const data, uint32_t dataLength)
	{
		//POLYVOX_ASSERT(m_pCompressedData, "Compressed data is NULL");
		POLYVOX_ASSERT(m_pCompressedData != data, "Attempting to copy data onto itself");

		delete[] m_pCompressedData;

		m_uCompressedDataLength = dataLength;
		m_pCompressedData = new uint8_t[dataLength];
		memcpy(m_pCompressedData, data, dataLength);
	}

	template <typename VoxelType>
	uint32_t Block<VoxelType>::calculateSizeInBytes(void)
	{
		// Returns the size of this class plus the size of the compressed data. It
		// doesn't include the uncompressed data cache as that is owned by the volume.
		uint32_t uSizeInBytes = sizeof(Block<VoxelType>) + m_uCompressedDataLength;
		return  uSizeInBytes;
	}
}

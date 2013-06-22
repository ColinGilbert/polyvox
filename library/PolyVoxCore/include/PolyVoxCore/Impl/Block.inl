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
	Block<VoxelType>::Block(uint16_t uSideLength)
		:m_pCompressedData(0)
		,m_uCompressedDataLength(0)
		,m_tUncompressedData(0)
		,m_uSideLength(0)
		,m_uSideLengthPower(0)
		,m_bIsCompressed(false)
		,m_bIsUncompressedDataModified(true)
	{
		if(uSideLength != 0)
		{
			initialise(uSideLength);
		}
	}

	template <typename VoxelType>
	const uint8_t* Block<VoxelType>::getCompressedData(void) const
	{
		POLYVOX_ASSERT(m_bIsCompressed, "You cannot call getCompressedData() when the block is not compressed");
		POLYVOX_ASSERT(m_pCompressedData, "Compressed data is NULL");

		return m_pCompressedData;
	}
	
	template <typename VoxelType>
	const uint32_t Block<VoxelType>::getCompressedDataLength(void) const
	{
		POLYVOX_ASSERT(m_bIsCompressed, "You cannot call getCompressedData() when the block is not compressed");
		POLYVOX_ASSERT(m_pCompressedData, "Compressed data is NULL");

		return m_uCompressedDataLength;
	}

	template <typename VoxelType>
	uint16_t Block<VoxelType>::getSideLength(void) const
	{
		return m_uSideLength;
	}

	template <typename VoxelType>
	VoxelType Block<VoxelType>::getVoxel(uint16_t uXPos, uint16_t uYPos, uint16_t uZPos) const
	{
		// This is internal code not directly called by the user. For efficiency we assert rather than throwing.
		POLYVOX_ASSERT(uXPos < m_uSideLength, "Supplied position is outside of the block");
		POLYVOX_ASSERT(uYPos < m_uSideLength, "Supplied position is outside of the block");
		POLYVOX_ASSERT(uZPos < m_uSideLength, "Supplied position is outside of the block");
		POLYVOX_ASSERT(!m_bIsCompressed, "You cannot call getVoxel() when a block is compressed");
		POLYVOX_ASSERT(m_tUncompressedData, "No uncompressed data - block must be decompressed before accessing voxels.");

		return m_tUncompressedData
			[
				uXPos + 
				uYPos * m_uSideLength + 
				uZPos * m_uSideLength * m_uSideLength
			];
	}

	template <typename VoxelType>
	VoxelType Block<VoxelType>::getVoxel(const Vector3DUint16& v3dPos) const
	{
		return getVoxel(v3dPos.getX(), v3dPos.getY(), v3dPos.getZ());
	}

	template <typename VoxelType>
	void Block<VoxelType>::setCompressedData(const uint8_t* const data, uint32_t dataLength)
	{
		POLYVOX_ASSERT(m_bIsCompressed, "You cannot call setCompressedData() when the block is not compressed");
		POLYVOX_ASSERT(m_pCompressedData, "Compressed data is NULL");
		POLYVOX_ASSERT(m_pCompressedData != data, "Attempting to copy data onto itself");

		delete[] m_pCompressedData;

		m_uCompressedDataLength = dataLength;
		m_pCompressedData = new uint8_t[dataLength];
		memcpy(m_pCompressedData, data, dataLength);
	}

	template <typename VoxelType>
	void Block<VoxelType>::setVoxelAt(uint16_t uXPos, uint16_t uYPos, uint16_t uZPos, VoxelType tValue)
	{
		// This is internal code not directly called by the user. For efficiency we assert rather than throwing.
		POLYVOX_ASSERT(uXPos < m_uSideLength, "Supplied position is outside of the block");
		POLYVOX_ASSERT(uYPos < m_uSideLength, "Supplied position is outside of the block");
		POLYVOX_ASSERT(uZPos < m_uSideLength, "Supplied position is outside of the block");
		POLYVOX_ASSERT(!m_bIsCompressed, "You cannot call setVoxelAt() when a block is compressed");
		POLYVOX_ASSERT(m_tUncompressedData, "No uncompressed data - block must be decompressed before accessing voxels.");

		m_tUncompressedData
		[
			uXPos + 
			uYPos * m_uSideLength + 
			uZPos * m_uSideLength * m_uSideLength
		] = tValue;

		m_bIsUncompressedDataModified = true;
	}

	template <typename VoxelType>
	void Block<VoxelType>::setVoxelAt(const Vector3DUint16& v3dPos, VoxelType tValue)
	{
		setVoxelAt(v3dPos.getX(), v3dPos.getY(), v3dPos.getZ(), tValue);
	}

	template <typename VoxelType>
	void Block<VoxelType>::initialise(uint16_t uSideLength)
	{
		//Release mode validation
		if(!isPowerOf2(uSideLength))
		{
			POLYVOX_THROW(std::invalid_argument, "Block side length must be a power of two.");
		}

		//Compute the side length		
		m_uSideLength = uSideLength;
		m_uSideLengthPower = logBase2(uSideLength);

		//Create the block data
		m_tUncompressedData = new VoxelType[m_uSideLength * m_uSideLength * m_uSideLength];

		//Clear it (should we bother?)
		const uint32_t uNoOfVoxels = m_uSideLength * m_uSideLength * m_uSideLength;
		std::fill(m_tUncompressedData, m_tUncompressedData + uNoOfVoxels, VoxelType());
		m_bIsUncompressedDataModified = true;
	}

	template <typename VoxelType>
	uint32_t Block<VoxelType>::calculateSizeInBytes(void)
	{
		//FIXME - This function is incomplete.
		uint32_t uSizeInBytes = sizeof(Block<VoxelType>);
		return  uSizeInBytes;
	}

	template <typename VoxelType>
	void Block<VoxelType>::compress(Compressor* pCompressor)
	{
		if(m_bIsCompressed)
		{ 
			POLYVOX_THROW(invalid_operation, "Attempted to compress block which is already flagged as compressed.");
		}

		if(!pCompressor)
		{
			POLYVOX_THROW(std::invalid_argument, "A valid compressor must be provided");
		}

		POLYVOX_ASSERT(m_tUncompressedData != 0, "No uncompressed data is present.");

		//If the uncompressed data hasn't actually been
		//modified then we don't need to redo the compression.
		if(m_bIsUncompressedDataModified)
		{
			// Delete the old compressed data as we'll create a new one
			delete[] m_pCompressedData;
			m_pCompressedData = 0;

			void* pSrcData = reinterpret_cast<void*>(m_tUncompressedData);
			uint32_t uSrcLength = m_uSideLength * m_uSideLength * m_uSideLength * sizeof(VoxelType);

			uint8_t tempBuffer[10000];
			void* pDstData = reinterpret_cast<void*>( tempBuffer );				
			uint32_t uDstLength = 10000;

			uint32_t uCompressedLength = 0;

			try
			{
				uCompressedLength = pCompressor->compress(pSrcData, uSrcLength, pDstData, uDstLength);

				// Create new compressed data and copy across
				m_pCompressedData = new uint8_t[uCompressedLength];
				memcpy(m_pCompressedData, pDstData, uCompressedLength);
				m_uCompressedDataLength = uCompressedLength;
			}
			catch(std::exception&)
			{
				// It is possible for the compression to fail. A common cause for this would be if the destination
				// buffer is not big enough. So now we try again using a buffer that is definitely big enough.
				// Note that ideally we will choose our earlier buffer size so that this almost never happens.
				uint32_t uMaxCompressedSize = pCompressor->getMaxCompressedSize(uSrcLength);
				uint8_t* buffer = new uint8_t[ uMaxCompressedSize ];

				pDstData = reinterpret_cast<void*>( buffer );				
				uDstLength = uMaxCompressedSize;

				try
				{		
					uCompressedLength = pCompressor->compress(pSrcData, uSrcLength, pDstData, uDstLength);

					// Create new compressed data and copy across
					m_pCompressedData = new uint8_t[uCompressedLength];
					memcpy(m_pCompressedData, pDstData, uCompressedLength);
					m_uCompressedDataLength = uCompressedLength;
				}
				catch(std::exception&)
				{
					// At this point it didn't work even with a bigger buffer.
					// Not much more we can do so just rethrow the exception.
					delete[] buffer;
					POLYVOX_THROW(std::runtime_error, "Failed to compress block data");
				}

				delete[] buffer;
			}			
		}

		//Flag the uncompressed data as no longer being used.
		delete[] m_tUncompressedData;
		m_tUncompressedData = 0;
		m_bIsCompressed = true;
	}

	template <typename VoxelType>
	void Block<VoxelType>::uncompress(Compressor* pCompressor)
	{
		if(!m_bIsCompressed)
		{
			POLYVOX_THROW(invalid_operation, "Attempted to uncompress block which is not flagged as compressed.");
		}

		if(!pCompressor)
		{
			POLYVOX_THROW(std::invalid_argument, "A valid compressor must be provided");
		}

		POLYVOX_ASSERT(m_tUncompressedData == 0, "Uncompressed data already exists.");

		m_tUncompressedData = new VoxelType[m_uSideLength * m_uSideLength * m_uSideLength];

		void* pSrcData = reinterpret_cast<void*>(m_pCompressedData);
		void* pDstData = reinterpret_cast<void*>(m_tUncompressedData);
		uint32_t uSrcLength = m_uCompressedDataLength;
		uint32_t uDstLength = m_uSideLength * m_uSideLength * m_uSideLength * sizeof(VoxelType);

		//MinizCompressor compressor;
		//RLECompressor<VoxelType, uint16_t> compressor;
		uint32_t uUncompressedLength = pCompressor->decompress(pSrcData, uSrcLength, pDstData, uDstLength);

		POLYVOX_ASSERT(uUncompressedLength == m_uSideLength * m_uSideLength * m_uSideLength * sizeof(VoxelType), "Destination length has changed.");

		//m_tUncompressedData = reinterpret_cast<VoxelType*>(uncompressedResult.ptr);

		m_bIsCompressed = false;
		m_bIsUncompressedDataModified = false;
	}
}

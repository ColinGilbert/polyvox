/*******************************************************************************
Copyright (c) 2005-2013 David Williams and Matthew Williams

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

namespace PolyVox
{
	template <typename VoxelType>
	MinizBlockCompressor<VoxelType>::MinizBlockCompressor()
	{
		m_pCompressor = new MinizCompressor;
	}

	template <typename VoxelType>
	MinizBlockCompressor<VoxelType>::~MinizBlockCompressor()
	{
		delete m_pCompressor;
	}

	template <typename VoxelType>
	void MinizBlockCompressor<VoxelType>::compress(UncompressedBlock<VoxelType>* pSrcBlock, CompressedBlock<VoxelType>* pDstBlock)
	{
		void* pSrcData = reinterpret_cast<void*>(pSrcBlock->getData());
		uint32_t uSrcLength = pSrcBlock->getDataSizeInBytes();

		uint8_t tempBuffer[10000];
		uint8_t* pDstData = reinterpret_cast<uint8_t*>( tempBuffer );				
		uint32_t uDstLength = 10000;

		uint32_t uCompressedLength = 0;

		try
		{
			// Perform the compression
			uCompressedLength = m_pCompressor->compress(pSrcData, uSrcLength, pDstData, uDstLength);

			// Copy the resulting compressed data into the compressed block
			pDstBlock->setData(pDstData, uDstLength);			
		}
		catch(std::exception&)
		{
			// It is possible for the compression to fail. A common cause for this would be if the destination
			// buffer is not big enough. So now we try again using a buffer that is definitely big enough.
			// Note that ideally we will choose our earlier buffer size so that this almost never happens.
			logWarning() << "The compressor failed to compress the block, probabaly due to the buffer being too small.";
			logWarning() << "The compression will be tried again with a larger buffer";
			uint32_t uMaxCompressedSize = m_pCompressor->getMaxCompressedSize(uSrcLength);
			uint8_t* buffer = new uint8_t[ uMaxCompressedSize ];

			pDstData = reinterpret_cast<uint8_t*>( buffer );				
			uDstLength = uMaxCompressedSize;

			try
			{		
				// Perform the compression
				uCompressedLength = m_pCompressor->compress(pSrcData, uSrcLength, pDstData, uDstLength);

				// Copy the resulting compressed data into the compressed block
				pDstBlock->setData(pDstData, uDstLength);
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

	template <typename VoxelType>
	void MinizBlockCompressor<VoxelType>::decompress(CompressedBlock<VoxelType>* pSrcBlock, UncompressedBlock<VoxelType>* pDstBlock)
	{
		const void* pSrcData = reinterpret_cast<const void*>(pSrcBlock->getData());
		void* pDstData = reinterpret_cast<void*>(pDstBlock->getData());
		uint32_t uSrcLength = pSrcBlock->getDataSizeInBytes();
		uint32_t uDstLength = pDstBlock->getDataSizeInBytes();

		
		//RLECompressor<VoxelType, uint16_t> compressor;
		uint32_t uUncompressedLength = m_pCompressor->decompress(pSrcData, uSrcLength, pDstData, uDstLength);

		POLYVOX_ASSERT(uUncompressedLength == pDstBlock->getDataSizeInBytes(), "Destination length has changed.");
	}
}
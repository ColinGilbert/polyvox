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

#include <sstream>

namespace PolyVox
{
	/**
	 * You can specify a compression level when constructing this compressor. This controls the tradeoff between speed and compression
	 * rate. Levels 0-9 are the standard zlib-style levels, 10 is best possible compression (not zlib compatible, and may be very slow).
	 * \param iCompressionLevel The desired compression level.
	 */
	template <typename VoxelType>
	MinizBlockCompressor<VoxelType>::MinizBlockCompressor(int iCompressionLevel)
		:m_pDeflator(0)
	{
		// Create and store the deflator.
		m_pDeflator = new tdefl_compressor;

		// The number of dictionary probes to use at each compression level (0-10). 0=implies fastest/minimal possible probing.
		// The discontinuity is unsettling but may be explained by the 'iCompressionLevel <= 3' check later?
		static const mz_uint s_tdefl_num_probes[11] = { 0, 1, 6, 32,  16, 32, 128, 256,  512, 768, 1500 };

		// Create tdefl() compatible flags (we have to compose the low-level flags ourselves, or use tdefl_create_comp_flags_from_zip_params() but that means MINIZ_NO_ZLIB_APIS can't be defined).
		m_uCompressionFlags = TDEFL_WRITE_ZLIB_HEADER | s_tdefl_num_probes[(std::min)(10, iCompressionLevel)] | ((iCompressionLevel <= 3) ? TDEFL_GREEDY_PARSING_FLAG : 0);
		if (!iCompressionLevel)
		{
			m_uCompressionFlags |= TDEFL_FORCE_ALL_RAW_BLOCKS;
		}
	}

	template <typename VoxelType>
	MinizBlockCompressor<VoxelType>::~MinizBlockCompressor()
	{
		// Delete the deflator
		delete m_pDeflator;
	}

	template <typename VoxelType>
	void MinizBlockCompressor<VoxelType>::compress(UncompressedBlock<VoxelType>* pSrcBlock, CompressedBlock<VoxelType>* pDstBlock)
	{
		// The uncompressed data will be read straight out of the block
		void* pSrcData = reinterpret_cast<void*>(pSrcBlock->getData());
		size_t uSrcLength = pSrcBlock->getDataSizeInBytes();

		// This compressor is expected to be used many times to compress a large number of blocks, but they are all
		// expected to have the same size. Therefore the resize() function below should only perform allocation once.
		uint32_t expectedCompressedSize = getExpectedCompressedSize(uSrcLength);
		if(m_vecTempBuffer.size() != expectedCompressedSize)
		{
			logInfo() << "Resizing temp buffer to " << expectedCompressedSize << "bytes. "
				<< "This should only happen the first time the MinizBlockCompressor is used";
			m_vecTempBuffer.resize(expectedCompressedSize);
		}

		// The compressed data will be written into this temporary buffer.
		uint8_t* pDstData = &(m_vecTempBuffer[0]);
		size_t uDstLength = m_vecTempBuffer.size();

		try
		{
			// Perform the compression
			uint32_t uCompressedLength = compressWithMiniz(pSrcData, uSrcLength, pDstData, uDstLength);

			// Copy the resulting compressed data into the compressed block
			pDstBlock->setData(pDstData, uCompressedLength);			
		}
		catch(std::exception&)
		{
			// It is possible for the compression to fail. A common cause for this would be if the destination
			// buffer is not big enough. So now we try again using a buffer that is definitely big enough.
			// Note that ideally we will choose our earlier buffer size so that this almost never happens.
			logWarning() << "The compressor failed to compress the block, probabaly due to the buffer being too small.";
			logWarning() << "The compression will be tried again with a larger buffer.";

			std::vector<uint8_t> vecExtraBigBuffer;
			vecExtraBigBuffer.resize(getMaxCompressedSize(uSrcLength));

			uint8_t* pDstData = &(vecExtraBigBuffer[0]);
			size_t uDstLength = vecExtraBigBuffer.size();

			try
			{		
				// Perform the compression
				uint32_t uCompressedLength = compressWithMiniz(pSrcData, uSrcLength, pDstData, uDstLength);

				// Copy the resulting compressed data into the compressed block
				pDstBlock->setData(pDstData, uCompressedLength);
			}
			catch(std::exception&)
			{
				// At this point it didn't work even with a bigger buffer.
				// Not much more we can do so just rethrow the exception.
				POLYVOX_THROW(std::runtime_error, "Failed to compress block data");
			}
		}
	}

	template <typename VoxelType>
	void MinizBlockCompressor<VoxelType>::decompress(CompressedBlock<VoxelType>* pSrcBlock, UncompressedBlock<VoxelType>* pDstBlock)
	{
		// Get raw pointers so that the data can be decompressed directly into the destination block.
		const void* pSrcData = reinterpret_cast<const void*>(pSrcBlock->getData());
		void* pDstData = reinterpret_cast<void*>(pDstBlock->getData());
		uint32_t uSrcLength = pSrcBlock->getDataSizeInBytes();
		uint32_t uDstLength = pDstBlock->getDataSizeInBytes();

		// Perform the decompression
		uint32_t uUncompressedLength = decompressWithMiniz(pSrcData, uSrcLength, pDstData, uDstLength);

		// We know we should have received exactly one block's worth of data. If not then something went wrong.
		POLYVOX_THROW_IF(uUncompressedLength != pDstBlock->getDataSizeInBytes(), std::runtime_error, "Decompressed data does not have the expected length");
	}

	template <typename VoxelType>
	uint32_t MinizBlockCompressor<VoxelType>::getExpectedCompressedSize(uint32_t uUncompressedInputSize)
	{
		// We expect this block compressor will be used for smoothly changing volume data such as density fields and so
		// the compression rate might not be great. The value beloew is basically a guess based on previous experience.
		uint32_t uExpectedCompressionRate = 4;
		return uUncompressedInputSize / uExpectedCompressionRate;
	}

	template <typename VoxelType>
	uint32_t MinizBlockCompressor<VoxelType>::getMaxCompressedSize(uint32_t uUncompressedInputSize)
	{
		// The contents of this function are copied from miniz's 'mz_deflateBound()'
		// (which we can't use because it is part of the zlib-style higher level API).
		unsigned long source_len = uUncompressedInputSize;

		// This is really over conservative. (And lame, but it's actually pretty tricky to compute a true upper bound given the way tdefl's blocking works.)
		return (std::max)(128 + (source_len * 110) / 100, 128 + source_len + ((source_len / (31 * 1024)) + 1) * 5);
	}

	template <typename VoxelType>
	uint32_t MinizBlockCompressor<VoxelType>::compressWithMiniz(const void* pSrcData, size_t uSrcLength, void* pDstData, size_t uDstLength)
	{
		// It seems we have to reinitialise the deflator for each fresh dataset (it's probably intended for streaming, which we're not doing here)
		tdefl_status status = tdefl_init(m_pDeflator, NULL, NULL, m_uCompressionFlags);
		if (status != TDEFL_STATUS_OKAY)
		{
			std::stringstream ss;
			ss << "tdefl_init() failed with return code '" << status << "'";
			POLYVOX_THROW(std::runtime_error, ss.str());
		}

		// Compress as much of the input as possible (or all of it) to the output buffer.
		status = tdefl_compress(m_pDeflator, pSrcData, &uSrcLength, pDstData, &uDstLength, TDEFL_FINISH);

		//Check whether the compression was successful.
		if (status != TDEFL_STATUS_DONE)
		{
			std::stringstream ss;
			ss << "tdefl_compress() failed with return code '" << status << "'";
			POLYVOX_THROW(std::runtime_error, ss.str());
		}

		// The compression modifies 'ulDstLength' to hold the new length.
		return uDstLength;
	}

	template <typename VoxelType>
	uint32_t MinizBlockCompressor<VoxelType>::decompressWithMiniz(const void* pSrcData, uint32_t uSrcLength, void* pDstData, uint32_t uDstLength)
	{
		// I don't know exactly why this limitation exists but it's an implementation detail of miniz. It shouldn't matter for our purposes 
		// as our destination is a Block and those are always a power of two. If you need to use this code for other purposes then you'll
		// probably have to scale up your dst buffer to the nearest appropriate size. Alternatively you can use the mz_uncompress function,
		// but that means enabling parts of the miniz API which are #defined out at the top of this file.
		POLYVOX_THROW_IF(isPowerOf2(uDstLength) == false, std::invalid_argument, "Miniz decompressor requires the destination buffer to have a size which is a power of two.");

		// Change the type to avoid compiler warnings
		size_t ulSrcLength = uSrcLength;
		size_t ulDstLength = uDstLength;

		// Create and initialise the decompressor (I believe this is much small than the compressor).
		tinfl_decompressor inflator;
		tinfl_init(&inflator);

		// Do the decompression. In some scenarios 'tinfl_decompress' would be called multiple times with the same dest buffer but
		// different locations within it. In our scenario it's only called once so the start and the location are the same (both pDstData).
		tinfl_status status = tinfl_decompress(&inflator, (const mz_uint8 *)pSrcData, &ulSrcLength, (mz_uint8 *)pDstData, (mz_uint8 *)pDstData, &ulDstLength, TINFL_FLAG_PARSE_ZLIB_HEADER);

		//Check whether the decompression was successful.
		if (status != TINFL_STATUS_DONE)
		{
			std::stringstream ss;
			ss << "tinfl_decompress() failed with return code '" << status << "'";
			POLYVOX_THROW(std::runtime_error, ss.str());
		}

		// The decompression modifies 'ulDstLength' to hold the new length.
		return ulDstLength;
	}
}
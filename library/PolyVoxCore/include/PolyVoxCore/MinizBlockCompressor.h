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

#ifndef __PolyVox_MinizBlockCompressor_H__
#define __PolyVox_MinizBlockCompressor_H__

#include "PolyVoxCore/BlockCompressor.h"

#include "PolyVoxCore/Impl/MinizWrapper.h"

namespace PolyVox
{
	/**
	 * Provides an interface for performing paging of data.
	 */
	template <typename VoxelType>
	class MinizBlockCompressor : public BlockCompressor<VoxelType>
	{
	public:
		MinizBlockCompressor(int iCompressionLevel = 6); // Miniz defines MZ_DEFAULT_LEVEL = 6 so we use the same here
		~MinizBlockCompressor();

		void compress(UncompressedBlock<VoxelType>* pSrcBlock, CompressedBlock<VoxelType>* pDstBlock);
		void decompress(CompressedBlock<VoxelType>* pSrcBlock, UncompressedBlock<VoxelType>* pDstBlock);

	private:
		uint32_t getMaxCompressedSize(uint32_t uUncompressedInputSize);
		uint32_t compress(const void* pSrcData, uint32_t uSrcLength, void* pDstData, uint32_t uDstLength);
		uint32_t decompress(const void* pSrcData, uint32_t uSrcLength, void* pDstData, uint32_t uDstLength);

		unsigned int m_uCompressionFlags;

		// tdefl_compressor contains all the state needed by the low-level compressor so it's a pretty big struct (~300k).
		tdefl_compressor* m_pDeflator;
	};
}

#include "PolyVoxCore/MinizBlockCompressor.inl"

#endif //__PolyVox_MinizBlockCompressor_H__

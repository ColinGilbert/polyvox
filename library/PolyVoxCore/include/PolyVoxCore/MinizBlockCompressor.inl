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
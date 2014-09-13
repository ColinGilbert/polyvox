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

#ifndef __PolyVox_RLEBlockCompressor_H__
#define __PolyVox_RLEBlockCompressor_H__

#include "PolyVoxCore/BlockCompressor.h"

namespace PolyVox
{
	template <typename VoxelType>
	class Run
	{
	public:
		typedef uint16_t LengthType;
		VoxelType value;
		LengthType length;
	};

	/**
	 * Provides an interface for performing paging of data.
	 */
	template <typename VoxelType>
	class RLEBlockCompressor : public BlockCompressor<VoxelType>
	{
		

	public:
		RLEBlockCompressor();
		~RLEBlockCompressor();

		void compressData(UncompressedBlock<VoxelType>* pSrcBlock, CompressedBlock<VoxelType>* pDstBlock);
		void decompress(CompressedBlock<VoxelType>* pSrcBlock, UncompressedBlock<VoxelType>* pDstBlock);
	};
}

#include "PolyVoxCore/RLEBlockCompressor.inl"

#endif //__PolyVox_RLEBlockCompressor_H__

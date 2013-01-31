#ifndef __PolyVox_MinizCompressor_H__
#define __PolyVox_MinizCompressor_H__

#include "PolyVoxCore/Compressor.h"

namespace PolyVox
{
	class MinizCompressor : public Compressor
	{
	public:
		MinizCompressor();
		~MinizCompressor();

		uint32_t compress(void* pSrcData, uint32_t uSrcLength, void* pDstData, uint32_t uDstLength);
		uint32_t decompress(void* pSrcData, uint32_t uSrcLength, void* pDstData, uint32_t uDstLength);
	};
}

#endif //__PolyVox_MinizCompressor_H__

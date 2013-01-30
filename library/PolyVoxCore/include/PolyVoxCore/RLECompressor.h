#ifndef __PolyVox_RLECompressor_H__
#define __PolyVox_RLECompressor_H__

#include "PolyVoxCore/Impl/TypeDef.h"

template<typename ValueType, typename LengthType>
class RLECompressor
{
	struct Run
	{
		ValueType value;
		LengthType length;
	};
public:
	RLECompressor();
	~RLECompressor();

	uint32_t compress(void* pSrcData, uint32_t uSrcLength, void* pDstData, uint32_t uDstLength);
	uint32_t decompress(void* pSrcData, uint32_t uSrcLength, void* pDstData, uint32_t uDstLength);
};

#include "RLECompressor.inl"

#endif //__PolyVox_RLECompressor_H__

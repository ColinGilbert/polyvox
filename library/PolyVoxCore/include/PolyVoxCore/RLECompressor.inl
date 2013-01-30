#include <algorithm>
#include <cassert>

template<typename ValueType, typename LengthType>
RLECompressor<ValueType, LengthType>::RLECompressor()
{
}

template<typename ValueType, typename LengthType>
RLECompressor<ValueType, LengthType>::~RLECompressor()
{
}

template<typename ValueType, typename LengthType>
uint32_t RLECompressor<ValueType, LengthType>::compress(void* pSrcData, uint32_t uSrcLength, void* pDstData, uint32_t uDstLength)
{
	assert(uSrcLength % sizeof(ValueType) == 0);

	uSrcLength /= sizeof(ValueType);
	uDstLength /= sizeof(Run);

	ValueType* pSrcDataAsInt = reinterpret_cast<ValueType*>(pSrcData);
	Run* pDstDataAsRun = reinterpret_cast<Run*>(pDstData);

	ValueType* pSrcDataEnd = pSrcDataAsInt + uSrcLength;
	Run* pDstDataEnd = pDstDataAsRun + uDstLength;

	uint32_t uDstLengthInBytes = 0;


	pDstDataAsRun->value = *pSrcDataAsInt;
	pSrcDataAsInt++;
	pDstDataAsRun->length = 1;
	uDstLengthInBytes += sizeof(Run);

	while(pSrcDataAsInt < pSrcDataEnd)
	{
		if((*pSrcDataAsInt == pDstDataAsRun->value) && (pDstDataAsRun->length < std::numeric_limits<LengthType>::max()))
		{
			pDstDataAsRun->length++;
		}
		else
		{
			pDstDataAsRun++;
			assert(pDstDataAsRun < pDstDataEnd);
			pDstDataAsRun->value = *pSrcDataAsInt;
			pDstDataAsRun->length = 1;
			uDstLengthInBytes += sizeof(Run);
		}
		
		pSrcDataAsInt++;
	}

	return uDstLengthInBytes;
}

template<typename ValueType, typename LengthType>
uint32_t RLECompressor<ValueType, LengthType>::decompress(void* pSrcData, uint32_t uSrcLength, void* pDstData, uint32_t uDstLength)
{
	assert(uSrcLength % sizeof(Run) == 0);

	uSrcLength /= sizeof(Run);
	uDstLength /= sizeof(ValueType);

	Run* pSrcDataAsRun = reinterpret_cast<Run*>(pSrcData);
	ValueType* pDstDataAsInt = reinterpret_cast<ValueType*>(pDstData);

	Run* pSrcDataEnd = pSrcDataAsRun + uSrcLength;
	ValueType* pDstDataEnd = pDstDataAsInt + uDstLength;

	uint32_t uDstLengthInBytes = 0;

	while(pSrcDataAsRun < pSrcDataEnd)
	{
		std::fill(pDstDataAsInt, pDstDataAsInt + pSrcDataAsRun->length, pSrcDataAsRun->value);
		pDstDataAsInt += pSrcDataAsRun->length;

		uDstLengthInBytes += pSrcDataAsRun->length * sizeof(ValueType);
		
		pSrcDataAsRun++;
	}

	return uDstLengthInBytes;
}


#include <algorithm>
#include <cassert>
#include <limits>

namespace PolyVox
{
	template<typename ValueType, typename LengthType>
	RLECompressor<ValueType, LengthType>::RLECompressor()
	{
	}

	template<typename ValueType, typename LengthType>
	RLECompressor<ValueType, LengthType>::~RLECompressor()
	{
	}

	template<typename ValueType, typename LengthType>
	uint32_t RLECompressor<ValueType, LengthType>::getMaxCompressedSize(uint32_t uUncompressedInputSize)
	{
		// In the worst case we will have a seperate Run (of length one) for each element of the input data.
		return uUncompressedInputSize * sizeof(Run);
	}

	template<typename ValueType, typename LengthType>
	uint32_t RLECompressor<ValueType, LengthType>::compress(const void* pSrcData, uint32_t uSrcLength, void* pDstData, uint32_t uDstLength)
	{
		if(uSrcLength % sizeof(ValueType) != 0)
		{
			POLYVOX_THROW(std::length_error, "Source length must be a integer multiple of the ValueType size");
		}

		// Lengths provided are in bytes, so convert them to be in terms of our types.
		uSrcLength /= sizeof(ValueType);
		uDstLength /= sizeof(Run);

		// Get data pointers in the appropriate type
		const ValueType* pSrcDataAsType = reinterpret_cast<const ValueType*>(pSrcData);
		Run* pDstDataAsRun = reinterpret_cast<Run*>(pDstData);

		// Pointers to just past the end of the data
		const ValueType* pSrcDataEnd = pSrcDataAsType + uSrcLength;
		Run* pDstDataEnd = pDstDataAsRun + uDstLength;

		//Counter for the output length
		uint32_t uDstLengthInBytes = 0;

		// Read the first element of the source and set up the first run based on it.
		pDstDataAsRun->value = *pSrcDataAsType;
		pSrcDataAsType++;
		pDstDataAsRun->length = 1;
		uDstLengthInBytes += sizeof(Run);

		//Now process all remaining elements of the source.
		while(pSrcDataAsType < pSrcDataEnd)
		{
			// If the value is the same as the current run (and we have not
			// reached the maximum run length) then extend the current run.
			if((*pSrcDataAsType == pDstDataAsRun->value) && (pDstDataAsRun->length < (std::numeric_limits<LengthType>::max)()))
			{
				pDstDataAsRun->length++;
			}
			// Otherwise we need to start a new Run.
			else
			{
				pDstDataAsRun++;

				// Check if we have enough space in the destination buffer.
				if(pDstDataAsRun >= pDstDataEnd)
				{
					POLYVOX_THROW(std::runtime_error, "Insufficient space in destination buffer.");
				}

				// Create the new run.
				pDstDataAsRun->value = *pSrcDataAsType;
				pDstDataAsRun->length = 1;
				uDstLengthInBytes += sizeof(Run);
			}
			
			pSrcDataAsType++;
		}

		return uDstLengthInBytes;
	}

	template<typename ValueType, typename LengthType>
	uint32_t RLECompressor<ValueType, LengthType>::decompress(const void* pSrcData, uint32_t uSrcLength, void* pDstData, uint32_t uDstLength)
	{
		if(uSrcLength % sizeof(Run) != 0)
		{
			POLYVOX_THROW(std::length_error, "Source length must be a integer multiple of the Run size");
		}

		// Lengths provided are in bytes, so convert them to be in terms of our types.
		uSrcLength /= sizeof(Run);
		uDstLength /= sizeof(ValueType);

		// Get data pointers in the appropriate type
		const Run* pSrcDataAsRun = reinterpret_cast<const Run*>(pSrcData);
		ValueType* pDstDataAsType = reinterpret_cast<ValueType*>(pDstData);

		// Pointers to just past the end of the data
		const Run* pSrcDataEnd = pSrcDataAsRun + uSrcLength;
		ValueType* pDstDataEnd = pDstDataAsType + uDstLength;

		//Counter for the output length
		uint32_t uDstLengthInBytes = 0;

		while(pSrcDataAsRun < pSrcDataEnd)
		{
			// Check if we have enough space in the destination buffer.
			if(pDstDataAsType + pSrcDataAsRun->length > pDstDataEnd)
			{
				POLYVOX_THROW(std::runtime_error, "Insufficient space in destination buffer.");
			}

			// Write the run into the destination
			std::fill(pDstDataAsType, pDstDataAsType + pSrcDataAsRun->length, pSrcDataAsRun->value);
			pDstDataAsType += pSrcDataAsRun->length;

			uDstLengthInBytes += pSrcDataAsRun->length * sizeof(ValueType);			
			pSrcDataAsRun++;
		}

		return uDstLengthInBytes;
	}
}

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

#include <vector>

namespace PolyVox
{
	template <typename VoxelType>
	RLEBlockCompressor<VoxelType>::RLEBlockCompressor()
	{
	}

	template <typename VoxelType>
	RLEBlockCompressor<VoxelType>::~RLEBlockCompressor()
	{
	}

	template <typename VoxelType>
	void RLEBlockCompressor<VoxelType>::compress(UncompressedBlock<VoxelType>* pSrcBlock, CompressedBlock<VoxelType>* pDstBlock)
	{
		void* pSrcData = reinterpret_cast<void*>(pSrcBlock->getData());
		uint32_t uSrcLength = pSrcBlock->getDataSizeInBytes();

		if(uSrcLength % sizeof(VoxelType) != 0)
		{
			POLYVOX_THROW(std::length_error, "Source length must be a integer multiple of the ValueType size");
		}

		// Lengths provided are in bytes, so convert them to be in terms of our types.
		uSrcLength /= sizeof(VoxelType);
		//uDstLength /= sizeof(Run);

		// Get data pointers in the appropriate type
		const VoxelType* pSrcDataAsType = reinterpret_cast<const VoxelType*>(pSrcData);
		//Run* pDstDataAsRun = reinterpret_cast<Run*>(pDstData);
		std::vector< Run<VoxelType> > vecDstDataAsRuns;

		// Pointers to just past the end of the data
		const VoxelType* pSrcDataEnd = pSrcDataAsType + uSrcLength;
		//Run* pDstDataEnd = pDstDataAsRun + uDstLength;

		//Counter for the output length
		//uint32_t uDstLengthInBytes = 0;

		// Read the first element of the source and set up the first run based on it.
		/*pDstDataAsRun->value = *pSrcDataAsType;
		pSrcDataAsType++;
		pDstDataAsRun->length = 1;
		uDstLengthInBytes += sizeof(Run);*/
		Run<VoxelType> firstRun;
		firstRun.value = *pSrcDataAsType;
		firstRun.length = 1;
		vecDstDataAsRuns.push_back(firstRun);

		pSrcDataAsType++;

		//Now process all remaining elements of the source.
		while(pSrcDataAsType < pSrcDataEnd)
		{
			// If the value is the same as the current run (and we have not
			// reached the maximum run length) then extend the current run.
			std::vector< Run<VoxelType> >::iterator currentRun = (vecDstDataAsRuns.end() - 1);
			if((*pSrcDataAsType == currentRun->value) && (currentRun->length < (std::numeric_limits<Run<VoxelType>::LengthType>::max)()))
			{
				currentRun->length++;
			}
			// Otherwise we need to start a new Run.
			else
			{
				/*pDstDataAsRun++;

				// Check if we have enough space in the destination buffer.
				if(pDstDataAsRun >= pDstDataEnd)
				{
					POLYVOX_THROW(std::runtime_error, "Insufficient space in destination buffer.");
				}*/

				// Create the new run.
				/*pDstDataAsRun->value = *pSrcDataAsType;
				pDstDataAsRun->length = 1;
				uDstLengthInBytes += sizeof(Run);*/

				Run<VoxelType> newRun;
				newRun.value = *pSrcDataAsType;
				newRun.length = 1;
				vecDstDataAsRuns.push_back(newRun);
			}
			
			pSrcDataAsType++;
		}

		//Now copy the data into the compressed block.

		pDstBlock->setData(reinterpret_cast<uint8_t*>(&(vecDstDataAsRuns[0])), vecDstDataAsRuns.size() * sizeof(Run<VoxelType>));
	}

	template <typename VoxelType>
	void RLEBlockCompressor<VoxelType>::decompress(CompressedBlock<VoxelType>* pSrcBlock, UncompressedBlock<VoxelType>* pDstBlock)
	{
		const void* pSrcData = reinterpret_cast<const void*>(pSrcBlock->getData());
		uint32_t uSrcLength = pSrcBlock->getDataSizeInBytes();

		void* pDstData = pDstBlock->getData();
		uint32_t uDstLength = pDstBlock->getDataSizeInBytes();

		if(uSrcLength % sizeof(Run<VoxelType>) != 0)
		{
			POLYVOX_THROW(std::length_error, "Source length must be a integer multiple of the Run size");
		}

		// Lengths provided are in bytes, so convert them to be in terms of our types.
		uSrcLength /= sizeof(Run<VoxelType>);
		uDstLength /= sizeof(VoxelType);

		// Get data pointers in the appropriate type
		const Run<VoxelType>* pSrcDataAsRun = reinterpret_cast<const Run<VoxelType>*>(pSrcData);
		VoxelType* pDstDataAsType = reinterpret_cast<VoxelType*>(pDstData);

		// Pointers to just past the end of the data
		const Run<VoxelType>* pSrcDataEnd = pSrcDataAsRun + uSrcLength;
		VoxelType* pDstDataEnd = pDstDataAsType + uDstLength;

		//Counter for the output length
		uint32_t uDstLengthInBytes = 0;

		while(pSrcDataAsRun < pSrcDataEnd)
		{
			// Check if we have enough space in the destination buffer.
			if(pDstDataAsType + pSrcDataAsRun->length > pDstDataEnd)
			{
				POLYVOX_THROW(std::runtime_error, "Insufficient space in destination buffer.");
			}
			else
			{

			// Write the run into the destination
			std::fill(pDstDataAsType, pDstDataAsType + pSrcDataAsRun->length, pSrcDataAsRun->value);
			pDstDataAsType += pSrcDataAsRun->length;

			uDstLengthInBytes += pSrcDataAsRun->length * sizeof(VoxelType);	
			}
			pSrcDataAsRun++;
		}

		POLYVOX_ASSERT(uDstLengthInBytes == pDstBlock->getDataSizeInBytes(), "Uncompressed data does not have the correct length.");
	}
}
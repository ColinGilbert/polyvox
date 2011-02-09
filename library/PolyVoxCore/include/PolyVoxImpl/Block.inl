/*******************************************************************************
Copyright (c) 2005-2009 David Williams

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

#include "PolyVoxImpl/Utility.h"
#include "Vector.h"
#include "Volume.h"

#include <cassert>
#include <cstring> //For memcpy
#include <limits>
#include <stdexcept> //for std::invalid_argument

namespace PolyVox
{
	template <typename VoxelType>
	Block<VoxelType>::Block(uint16_t uSideLength)
		:m_uSideLength(0)
		,m_uSideLengthPower(0)
		,m_tUncompressedData(0)
		,m_bIsCompressed(true)
		,m_bIsUncompressedDataModified(true)
		,m_uTimestamp(0)
	{
		if(uSideLength != 0)
		{
			resize(uSideLength);
		}
	}

	template <typename VoxelType>
	Block<VoxelType>::Block(const Block<VoxelType>& rhs)
	{
		assert(false);
	}

	template <typename VoxelType>
	Block<VoxelType>::~Block()
	{
		delete[] m_tUncompressedData;
		m_tUncompressedData = 0;
	}

	template <typename VoxelType>
	Block<VoxelType>& Block<VoxelType>::operator=(const Block<VoxelType>& rhs)
	{
		assert(false);
		return 0;
	}

	template <typename VoxelType>
	uint16_t Block<VoxelType>::getSideLength(void) const
	{
		return m_uSideLength;
	}

	template <typename VoxelType>
	VoxelType Block<VoxelType>::getVoxelAt(uint16_t uXPos, uint16_t uYPos, uint16_t uZPos) const
	{
		assert(uXPos < m_uSideLength);
		assert(uYPos < m_uSideLength);
		assert(uZPos < m_uSideLength);

		assert(m_tUncompressedData);

		return m_tUncompressedData
			[
				uXPos + 
				uYPos * m_uSideLength + 
				uZPos * m_uSideLength * m_uSideLength
			];
	}

	template <typename VoxelType>
	VoxelType Block<VoxelType>::getVoxelAt(const Vector3DUint16& v3dPos) const
	{
		return getVoxelAt(v3dPos.getX(), v3dPos.getY(), v3dPos.getZ());
	}

	template <typename VoxelType>
	void Block<VoxelType>::setVoxelAt(uint16_t uXPos, uint16_t uYPos, uint16_t uZPos, VoxelType tValue)
	{
		assert(uXPos < m_uSideLength);
		assert(uYPos < m_uSideLength);
		assert(uZPos < m_uSideLength);

		assert(m_tUncompressedData);

		m_tUncompressedData
		[
			uXPos + 
			uYPos * m_uSideLength + 
			uZPos * m_uSideLength * m_uSideLength
		] = tValue;

		m_bIsUncompressedDataModified = true;
	}

	template <typename VoxelType>
	void Block<VoxelType>::setVoxelAt(const Vector3DUint16& v3dPos, VoxelType tValue)
	{
		setVoxelAt(v3dPos.getX(), v3dPos.getY(), v3dPos.getZ(), tValue);
	}

	template <typename VoxelType>
	void Block<VoxelType>::fill(VoxelType tValue)
	{
		//The memset *may* be faster than the std::fill(), but it doesn't compile nicely
		//in 64-bit mode as casting the pointer to an int causes a loss of precision.

		assert(m_tUncompressedData);

		const uint32_t uNoOfVoxels = m_uSideLength * m_uSideLength * m_uSideLength;
		std::fill(m_tUncompressedData, m_tUncompressedData + uNoOfVoxels, tValue);

		m_bIsUncompressedDataModified = true;
	}

	template <typename VoxelType>
	void Block<VoxelType>::resize(uint16_t uSideLength)
	{
		//Debug mode validation
		assert(isPowerOf2(uSideLength));

		//Release mode validation
		if(!isPowerOf2(uSideLength))
		{
			throw std::invalid_argument("Block side length must be a power of two.");
		}

		//Compute the side length		
		m_uSideLength = uSideLength;
		m_uSideLengthPower = logBase2(uSideLength);


		if(m_bIsCompressed == false)
		{
			//Delete the old data
			delete[] m_tUncompressedData;
			m_tUncompressedData = 0;

			//If this fails an exception will be thrown. Memory is not   
			//allocated and there is nothing else in this class to clean up
			m_tUncompressedData = new VoxelType[m_uSideLength * m_uSideLength * m_uSideLength];

			m_bIsUncompressedDataModified = true;
		}
	}

	template <typename VoxelType>
	uint32_t Block<VoxelType>::sizeInChars(void)
	{
		uint32_t uSizeInChars = 0; //sizeof(Block<VoxelType>);

		if(m_tUncompressedData != 0)
		{
			const uint32_t uNoOfVoxels = m_uSideLength * m_uSideLength * m_uSideLength;
			uSizeInChars += uNoOfVoxels * sizeof(VoxelType);
		}

		uSizeInChars += values.size() * sizeof(VoxelType);
		uSizeInChars += runlengths.size() * sizeof(uint16_t);

		return  uSizeInChars;
	}

	template <typename VoxelType>
	void Block<VoxelType>::compress(void)
	{
		assert(m_bIsCompressed == false);

		//If the uncompressed data hasn't actually been
		//modified then we don't need to redo the compression.
		if(m_bIsUncompressedDataModified)
		{
			uint32_t uNoOfVoxels = m_uSideLength * m_uSideLength * m_uSideLength;
			runlengths.clear();
			values.clear();

			VoxelType current = m_tUncompressedData[0];
			uint16_t runLength = 1;

			for(uint32_t ct = 1; ct < uNoOfVoxels; ++ct)
			{		
				VoxelType value = m_tUncompressedData[ct];
				if((value == current) && (runLength < (std::numeric_limits<uint16_t>::max)()))
				{
					runLength++;
				}
				else
				{
					runlengths.push_back(runLength);
					values.push_back(current);
					current = value;
					runLength = 1;
				}
			}

			runlengths.push_back(runLength);
			values.push_back(current);

			//Shrink the vectors to their contents (seems slow?):
			//http://stackoverflow.com/questions/1111078/reduce-the-capacity-of-an-stl-vector
			//C++0x may have a shrink_to_fit() function?
			//std::vector<uint8_t>(runlengths).swap(runlengths);
			//std::vector<VoxelType>(values).swap(values);
		}

		assert(m_tUncompressedData != 0);
		delete[] m_tUncompressedData;
		m_tUncompressedData = 0;
		m_bIsCompressed = true;
	}

	template <typename VoxelType>
	void Block<VoxelType>::uncompress(void)
	{
		assert(m_bIsCompressed == true);
		assert(m_tUncompressedData == 0);
		m_tUncompressedData = new VoxelType[m_uSideLength * m_uSideLength * m_uSideLength];

		VoxelType* pUncompressedData = m_tUncompressedData;
		
		//memset should provide the fastest way of expanding the data, but it works
		//on unsigned chars so is only possible if our voxel type is the right size.
		//Nore that memset takes an int type, but sonverts it to unsiogned char:
		//http://www.cplusplus.com/reference/clibrary/cstring/memset/
		/*if(sizeof(VoxelType) == sizeof(unsigned char))
		{
			for(uint32_t ct = 0; ct < runlengths.size(); ++ct)
			{
				memset(pUncompressedData, *((int*)(&values[ct])), runlengths[ct]);
				pUncompressedData += runlengths[ct];
			}
		}
		//Otherwise we fall back on a loop.
		else
		{*/
			for(uint32_t ct = 0; ct < runlengths.size(); ++ct)
			{
				for(uint32_t i = 0; i < runlengths[ct]; ++i)
				{
					*pUncompressedData = values[ct];
					++pUncompressedData;
				}
			}
		//}

		m_bIsCompressed = false;
		m_bIsUncompressedDataModified = false;
	}
}

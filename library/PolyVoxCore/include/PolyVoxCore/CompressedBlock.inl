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
	CompressedBlock<VoxelType>::CompressedBlock()
		:m_pData(0)
		,m_uDataSizeInBytes(0)
	{
	}

	template <typename VoxelType>
	CompressedBlock<VoxelType>::~CompressedBlock()
	{
		delete[] m_pData;
		m_pData = 0;
	}

	template <typename VoxelType>
	const uint8_t* CompressedBlock<VoxelType>::getData(void) const
	{
		return m_pData;
	}

	template <typename VoxelType>
	uint32_t CompressedBlock<VoxelType>::getDataSizeInBytes(void) const
	{
		return m_uDataSizeInBytes;
	}

	template <typename VoxelType>
	void CompressedBlock<VoxelType>::setData(const uint8_t* const pData, uint32_t uDataSizeInBytes)
	{
		POLYVOX_THROW_IF(pData == 0, std::invalid_argument, "Pointer to data cannot be null");
		POLYVOX_THROW_IF(m_pData == pData, std::invalid_argument, "Attempting to copy data onto itself");

		// Delete any existing data
		delete[] m_pData;

		// Allocate new data
		m_uDataSizeInBytes = uDataSizeInBytes;
		m_pData = new uint8_t[uDataSizeInBytes];

		// Copy the data across
		memcpy(m_pData, pData, uDataSizeInBytes);

		// Flag as modified
		this->m_bDataModified = true;
	}

	template <typename VoxelType>
	uint32_t CompressedBlock<VoxelType>::calculateSizeInBytes(void)
	{
		// Returns the size of this class plus the size of the compressed data.
		uint32_t uSizeInBytes = sizeof(CompressedBlock<VoxelType>) + m_uDataSizeInBytes;
		return  uSizeInBytes;
	}
}

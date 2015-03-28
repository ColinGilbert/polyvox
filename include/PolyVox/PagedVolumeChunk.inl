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

#include "PolyVox/Impl/Utility.h"

namespace PolyVox
{
	template <typename VoxelType>
	PagedVolume<VoxelType>::Chunk::Chunk(Vector3DInt32 v3dPosition, uint16_t uSideLength, Pager* pPager)
		:m_uChunkLastAccessed(0)
		,m_bDataModified(true)
		,m_tData(0)
		,m_uSideLength(0)
		,m_uSideLengthPower(0)
		,m_pPager(pPager)
		,m_v3dChunkSpacePosition(v3dPosition)
	{
		POLYVOX_ASSERT(m_pPager, "No valid pager supplied to chunk constructor.");

		// Compute the side length               
		m_uSideLength = uSideLength;
		m_uSideLengthPower = logBase2(uSideLength);

		// Allocate the data
		const uint32_t uNoOfVoxels = m_uSideLength * m_uSideLength * m_uSideLength;
		m_tData = new VoxelType[uNoOfVoxels];  

		// Pass the chunk to the Pager to give it a chance to initialise it with any data
		// From the coordinates of the chunk we deduce the coordinates of the contained voxels.
		Vector3DInt32 v3dLower = m_v3dChunkSpacePosition * static_cast<int32_t>(m_uSideLength);
		Vector3DInt32 v3dUpper = v3dLower + Vector3DInt32(m_uSideLength - 1, m_uSideLength - 1, m_uSideLength - 1);
		Region reg(v3dLower, v3dUpper);

		// A valid pager is normally present - this check is mostly to ease unit testing.
		if (m_pPager)
		{
			// Page the data in
			m_pPager->pageIn(reg, this);
		}

		// We'll use this later to decide if data needs to be paged out again.
		m_bDataModified = false;
	}

	template <typename VoxelType>
	PagedVolume<VoxelType>::Chunk::~Chunk()
	{
		if (m_bDataModified && m_pPager)
		{
			// From the coordinates of the chunk we deduce the coordinates of the contained voxels.
			Vector3DInt32 v3dLower = m_v3dChunkSpacePosition * static_cast<int32_t>(m_uSideLength);
			Vector3DInt32 v3dUpper = v3dLower + Vector3DInt32(m_uSideLength - 1, m_uSideLength - 1, m_uSideLength - 1);

			// Page the data out
			m_pPager->pageOut(Region(v3dLower, v3dUpper), this);
		}

		delete[] m_tData;
		m_tData = 0;
	}

	template <typename VoxelType>
	VoxelType* PagedVolume<VoxelType>::Chunk::getData(void) const
	{
		return m_tData;
	}

	template <typename VoxelType>
	uint32_t PagedVolume<VoxelType>::Chunk::getDataSizeInBytes(void) const
	{
		return m_uSideLength * m_uSideLength * m_uSideLength * sizeof(VoxelType);
	}

	// Based on https://fgiesen.wordpress.com/2009/12/13/decoding-morton-codes/
	inline uint32_t Part1By2(uint32_t x)
	{
		x &= 0x000003ff;                  // x = ---- ---- ---- ---- ---- --98 7654 3210
		x = (x ^ (x << 16)) & 0xff0000ff; // x = ---- --98 ---- ---- ---- ---- 7654 3210
		x = (x ^ (x << 8)) & 0x0300f00f; // x = ---- --98 ---- ---- 7654 ---- ---- 3210
		x = (x ^ (x << 4)) & 0x030c30c3; // x = ---- --98 ---- 76-- --54 ---- 32-- --10
		x = (x ^ (x << 2)) & 0x09249249; // x = ---- 9--8 --7- -6-- 5--4 --3- -2-- 1--0
		return x;
	}

	template <typename VoxelType>
	VoxelType PagedVolume<VoxelType>::Chunk::getVoxel(uint32_t uXPos, uint32_t uYPos, uint32_t uZPos) const
	{
		// This code is not usually expected to be called by the user, with the exception of when implementing paging 
		// of uncompressed data. It's a performance critical code path so  we use asserts rather than exceptions.
		POLYVOX_ASSERT(uXPos < m_uSideLength, "Supplied position is outside of the chunk");
		POLYVOX_ASSERT(uYPos < m_uSideLength, "Supplied position is outside of the chunk");
		POLYVOX_ASSERT(uZPos < m_uSideLength, "Supplied position is outside of the chunk");
		POLYVOX_ASSERT(m_tData, "No uncompressed data - chunk must be decompressed before accessing voxels.");

		uint32_t index = 0;
		index |= Part1By2(uXPos) | Part1By2(uYPos) << 1 | Part1By2(uZPos) << 2;

		return m_tData[index];
	}

	template <typename VoxelType>
	VoxelType PagedVolume<VoxelType>::Chunk::getVoxel(const Vector3DUint16& v3dPos) const
	{
		return getVoxel(v3dPos.getX(), v3dPos.getY(), v3dPos.getZ());
	}

	template <typename VoxelType>
	void PagedVolume<VoxelType>::Chunk::setVoxel(uint32_t uXPos, uint32_t uYPos, uint32_t uZPos, VoxelType tValue)
	{
		// This code is not usually expected to be called by the user, with the exception of when implementing paging 
		// of uncompressed data. It's a performance critical code path so  we use asserts rather than exceptions.
		POLYVOX_ASSERT(uXPos < m_uSideLength, "Supplied position is outside of the chunk");
		POLYVOX_ASSERT(uYPos < m_uSideLength, "Supplied position is outside of the chunk");
		POLYVOX_ASSERT(uZPos < m_uSideLength, "Supplied position is outside of the chunk");
		POLYVOX_ASSERT(m_tData, "No uncompressed data - chunk must be decompressed before accessing voxels.");

		uint32_t index = 0;
		index |= Part1By2(uXPos) | Part1By2(uYPos) << 1 | Part1By2(uZPos) << 2;

		m_tData[index] = tValue;

		this->m_bDataModified = true;
	}

	template <typename VoxelType>
	void PagedVolume<VoxelType>::Chunk::setVoxel(const Vector3DUint16& v3dPos, VoxelType tValue)
    {
		setVoxel(v3dPos.getX(), v3dPos.getY(), v3dPos.getZ(), tValue);
    }

	template <typename VoxelType>
	uint32_t PagedVolume<VoxelType>::Chunk::calculateSizeInBytes(void)
	{
		// Call through to the static version
		return calculateSizeInBytes(m_uSideLength);
	}

	template <typename VoxelType>
	uint32_t PagedVolume<VoxelType>::Chunk::calculateSizeInBytes(uint32_t uSideLength)
	{
		// Note: We disregard the size of the other class members as they are likely to be very small compared to the size of the
		// allocated voxel data. This also keeps the reported size as a power of two, which makes other memory calculations easier.
		uint32_t uSizeInBytes = uSideLength * uSideLength * uSideLength * sizeof(VoxelType);
		return  uSizeInBytes;
	}
}

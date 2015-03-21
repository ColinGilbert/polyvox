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

#include "PolyVox/Impl/ErrorHandling.h"

#include <algorithm>
#include <limits>

namespace PolyVox
{
	////////////////////////////////////////////////////////////////////////////////
	/// This constructor creates a volume with a fixed size which is specified as a parameter. By default this constructor will not enable paging but you can override this if desired. If you do wish to enable paging then you are required to provide the call back function (see the other PagedVolume constructor).
	/// \param pPager Called by PolyVox to load and unload data on demand.
	/// \param uTargetMemoryUsageInBytes The upper limit to how much memory this PagedVolume should aim to use.
	/// \param uChunkSideLength The size of the chunks making up the volume. Small chunks will compress/decompress faster, but there will also be more of them meaning voxel access could be slower.
	////////////////////////////////////////////////////////////////////////////////
	template <typename VoxelType>
	PagedVolume<VoxelType>::PagedVolume(Pager* pPager,	uint32_t uTargetMemoryUsageInBytes,	uint16_t uChunkSideLength)
		:BaseVolume<VoxelType>()
		, m_pPager(pPager)
		, m_uChunkSideLength(uChunkSideLength)
	{
		// Validation of parameters
		POLYVOX_THROW_IF(!pPager, std::invalid_argument, "You must provide a valid pager when constructing a PagedVolume");
		POLYVOX_THROW_IF(uTargetMemoryUsageInBytes < 1 * 1024 * 1024, std::invalid_argument, "Target memory usage is too small to be practical");
		POLYVOX_THROW_IF(m_uChunkSideLength == 0, std::invalid_argument, "Chunk side length cannot be zero.");
		POLYVOX_THROW_IF(m_uChunkSideLength > 256, std::invalid_argument, "Chunk size is too large to be practical.");
		POLYVOX_THROW_IF(!isPowerOf2(m_uChunkSideLength), std::invalid_argument, "Chunk side length must be a power of two.");

		// Used to perform multiplications and divisions by bit shifting.
		m_uChunkSideLengthPower = logBase2(m_uChunkSideLength);
		// Use to perform modulo by bit operations
		m_iChunkMask = m_uChunkSideLength - 1;

		// Calculate the number of chunks based on the memory limit and the size of each chunk.
		uint32_t uChunkSizeInBytes = PagedVolume<VoxelType>::Chunk::calculateSizeInBytes(m_uChunkSideLength);
		m_uChunkCountLimit = uTargetMemoryUsageInBytes / uChunkSizeInBytes;

		// Enforce sensible limits on the number of chunks.
		const uint32_t uMinPracticalNoOfChunks = 32; // Enough to make sure a chunks and it's neighbours can be loaded, with a few to spare.
		const uint32_t uMaxPracticalNoOfChunks = 32768; // Should prevent multi-gigabyte volumes when chunk sizes are reasonable.
		POLYVOX_LOG_WARNING_IF(m_uChunkCountLimit < uMinPracticalNoOfChunks, "Requested memory usage limit of "
			<< uTargetMemoryUsageInBytes / (1024 * 1024) << "Mb is too low and cannot be adhered to.");
		m_uChunkCountLimit = (std::max)(m_uChunkCountLimit, uMinPracticalNoOfChunks);
		m_uChunkCountLimit = (std::min)(m_uChunkCountLimit, uMaxPracticalNoOfChunks);

		// Inform the user about the chosen memory configuration.
		POLYVOX_LOG_DEBUG("Memory usage limit for volume now set to " << (m_uChunkCountLimit * uChunkSizeInBytes) / (1024 * 1024)
			<< "Mb (" << m_uChunkCountLimit << " chunks of " << uChunkSizeInBytes / 1024 << "Kb each).");
	}

	////////////////////////////////////////////////////////////////////////////////
	/// This function should never be called. Copying volumes by value would be expensive, and we want to prevent users from doing
	/// it by accident (such as when passing them as paramenters to functions). That said, there are times when you really do want to
	/// make a copy of a volume and in this case you should look at the VolumeResampler.
	///
	/// \sa VolumeResampler
	////////////////////////////////////////////////////////////////////////////////
	template <typename VoxelType>
	PagedVolume<VoxelType>::PagedVolume(const PagedVolume<VoxelType>& /*rhs*/)
	{
		POLYVOX_THROW(not_implemented, "Volume copy constructor not implemented to prevent accidental copying.");
	}

	////////////////////////////////////////////////////////////////////////////////
	/// Destroys the volume The destructor will call flushAll() to ensure that a paging volume has the chance to save it's data via the dataOverflowHandler() if desired.
	////////////////////////////////////////////////////////////////////////////////
	template <typename VoxelType>
	PagedVolume<VoxelType>::~PagedVolume()
	{
		flushAll();
	}

	////////////////////////////////////////////////////////////////////////////////
	/// This function should never be called. Copying volumes by value would be expensive, and we want to prevent users from doing
	/// it by accident (such as when passing them as paramenters to functions). That said, there are times when you really do want to
	/// make a copy of a volume and in this case you should look at the Volumeresampler.
	///
	/// \sa VolumeResampler
	////////////////////////////////////////////////////////////////////////////////
	template <typename VoxelType>
	PagedVolume<VoxelType>& PagedVolume<VoxelType>::operator=(const PagedVolume<VoxelType>& /*rhs*/)
	{
		POLYVOX_THROW(not_implemented, "Volume assignment operator not implemented to prevent accidental copying.");
	}

	////////////////////////////////////////////////////////////////////////////////
	/// This version of the function is provided so that the wrap mode does not need
	/// to be specified as a template parameter, as it may be confusing to some users.
	/// \param uXPos The \c x position of the voxel
	/// \param uYPos The \c y position of the voxel
	/// \param uZPos The \c z position of the voxel
	/// \return The voxel value
	////////////////////////////////////////////////////////////////////////////////
	template <typename VoxelType>
	VoxelType PagedVolume<VoxelType>::getVoxel(int32_t uXPos, int32_t uYPos, int32_t uZPos) const
	{
		const ChunkKey key = posToChunkKey(uXPos, uYPos, uZPos);

		// Only call get chunk if we can't reuse the chunk pointer from the last voxel access.
		auto pChunk = (key == m_v3dLastAccessedChunkKey) ? m_pLastAccessedChunk : getChunk(key);

		const uint16_t xOffset = static_cast<uint16_t>(uXPos & m_iChunkMask);
		const uint16_t yOffset = static_cast<uint16_t>(uYPos & m_iChunkMask);
		const uint16_t zOffset = static_cast<uint16_t>(uZPos & m_iChunkMask);

		return pChunk->getVoxel(xOffset, yOffset, zOffset);
	}

	////////////////////////////////////////////////////////////////////////////////
	/// This version of the function is provided so that the wrap mode does not need
	/// to be specified as a template parameter, as it may be confusing to some users.
	/// \param v3dPos The 3D position of the voxel
	/// \return The voxel value
	////////////////////////////////////////////////////////////////////////////////
	template <typename VoxelType>
	VoxelType PagedVolume<VoxelType>::getVoxel(const Vector3DInt32& v3dPos) const
	{
		return getVoxel(v3dPos.getX(), v3dPos.getY(), v3dPos.getZ());
	}

	////////////////////////////////////////////////////////////////////////////////
	/// \param uXPos the \c x position of the voxel
	/// \param uYPos the \c y position of the voxel
	/// \param uZPos the \c z position of the voxel
	////////////////////////////////////////////////////////////////////////////////
	template <typename VoxelType>
	void PagedVolume<VoxelType>::setVoxel(int32_t uXPos, int32_t uYPos, int32_t uZPos, VoxelType tValue)
	{
		const ChunkKey key = posToChunkKey(uXPos, uYPos, uZPos);

		// Only call get chunk if we can't reuse the chunk pointer from the last voxel access.
		auto pChunk = (key == m_v3dLastAccessedChunkKey) ? m_pLastAccessedChunk : getChunk(key);

		const uint16_t xOffset = static_cast<uint16_t>(uXPos & m_iChunkMask);
		const uint16_t yOffset = static_cast<uint16_t>(uYPos & m_iChunkMask);
		const uint16_t zOffset = static_cast<uint16_t>(uZPos & m_iChunkMask);

		pChunk->setVoxel(xOffset, yOffset, zOffset, tValue);
	}

	////////////////////////////////////////////////////////////////////////////////
	/// \param v3dPos the 3D position of the voxel
	/// \param tValue the value to which the voxel will be set
	////////////////////////////////////////////////////////////////////////////////
	template <typename VoxelType>
	void PagedVolume<VoxelType>::setVoxel(const Vector3DInt32& v3dPos, VoxelType tValue)
	{
		setVoxel(v3dPos.getX(), v3dPos.getY(), v3dPos.getZ(), tValue);
	}

	////////////////////////////////////////////////////////////////////////////////
	/// Note that if the memory usage limit is not large enough to support the region this function will only load part of the region. In this case it is undefined which parts will actually be loaded. If all the voxels in the given region are already loaded, this function will not do anything. Other voxels might be unloaded to make space for the new voxels.
	/// \param regPrefetch The Region of voxels to prefetch into memory.
	////////////////////////////////////////////////////////////////////////////////
	template <typename VoxelType>
	void PagedVolume<VoxelType>::prefetch(Region regPrefetch)
	{
		// Convert the start and end positions into chunk space coordinates
		Vector3DInt32 v3dStart;
		for(int i = 0; i < 3; i++)
		{
			v3dStart.setElement(i, regPrefetch.getLowerCorner().getElement(i) >> m_uChunkSideLengthPower);
		}

		Vector3DInt32 v3dEnd;
		for(int i = 0; i < 3; i++)
		{
			v3dEnd.setElement(i, regPrefetch.getUpperCorner().getElement(i) >> m_uChunkSideLengthPower);
		}

		// Ensure we don't page in more chunks than the volume can hold.
		Region region(v3dStart, v3dEnd);
		uint32_t uNoOfChunks = static_cast<uint32_t>(region.getWidthInVoxels() * region.getHeightInVoxels() * region.getDepthInVoxels());
		POLYVOX_LOG_WARNING_IF(uNoOfChunks > m_uChunkCountLimit, "Attempting to prefetch more than the maximum number of chunks (this will cause thrashing).");
		uNoOfChunks = (std::min)(uNoOfChunks, m_uChunkCountLimit);

		// Loops over the specified positions and touch the corresponding chunks.
		for(int32_t x = v3dStart.getX(); x <= v3dEnd.getX(); x++)
		{
			for(int32_t y = v3dStart.getY(); y <= v3dEnd.getY(); y++)
			{
				for(int32_t z = v3dStart.getZ(); z <= v3dEnd.getZ(); z++)
				{	
					const ChunkKey key = posToChunkKey(x, y, z);

					// Note that we don't check against the last chunk here. We're
					// not accessing the voxels, we just want to pull them into memory.
					getChunk(key);
				}
			}
		}
	}

	////////////////////////////////////////////////////////////////////////////////
	/// Removes all voxels from memory, and calls dataOverflowHandler() to ensure the application has a chance to store the data.
	////////////////////////////////////////////////////////////////////////////////
	template <typename VoxelType>
	void PagedVolume<VoxelType>::flushAll()
	{
		// Clear this pointer as all chunks are about to be removed.
		m_pLastAccessedChunk = nullptr;
		m_v3dLastAccessedChunkKey = 0;

		// Erase all the most recently used chunks.
		m_mapChunks.clear();
	}

	////////////////////////////////////////////////////////////////////////////////
	/// Removes all voxels in the specified Region from memory, and calls dataOverflowHandler() to ensure the application has a chance to store the data. It is possible that there are no voxels loaded in the Region, in which case the function will have no effect.
	////////////////////////////////////////////////////////////////////////////////
	template <typename VoxelType>
	void PagedVolume<VoxelType>::flush(Region regFlush)
	{
		// Clear this pointer in case the chunk it points at is flushed.
		m_pLastAccessedChunk = nullptr;
		m_v3dLastAccessedChunkKey = 0;

		// Convert the start and end positions into chunk space coordinates
		Vector3DInt32 v3dStart;
		for(int i = 0; i < 3; i++)
		{
			v3dStart.setElement(i, regFlush.getLowerCorner().getElement(i) >> m_uChunkSideLengthPower);
		}

		Vector3DInt32 v3dEnd;
		for(int i = 0; i < 3; i++)
		{
			v3dEnd.setElement(i, regFlush.getUpperCorner().getElement(i) >> m_uChunkSideLengthPower);
		}

		// Loops over the specified positions and delete the corresponding chunks.
		for(int32_t x = v3dStart.getX(); x <= v3dEnd.getX(); x++)
		{
			for(int32_t y = v3dStart.getY(); y <= v3dEnd.getY(); y++)
			{
				for(int32_t z = v3dStart.getZ(); z <= v3dEnd.getZ(); z++)
				{
					const ChunkKey key = posToChunkKey(x, y, z);
					m_mapChunks.erase(key);
				}
			}
		}
	}

	template <typename VoxelType>
	typename PagedVolume<VoxelType>::Chunk* PagedVolume<VoxelType>::getChunk(ChunkKey iKeyAsInt32) const
	{
		// This function is relatively large and slow because it involves searching for a chunk and creating it if it is not found. A natural
		// optimization is to only do this work if the chunk we are accessing is not the same as the last chunk we accessed (which it usually
		// is). We could (and previously did) include a simple check for this in this function. However, this function would then usually return
		// immediatly (which was good) but we still paid the overhead of a function call, probably because this function is not inlined due to
		// being quite large. Therefore we decided the check against the previous accessed chunk should always be done *before* calling this
		// function, and we add an assert here to try and catch if the user forgets to do it. Note that this is an internal function so the
		// 'user' here is actually PolyVox developers and not the developers of applications using PolyVox.
		//
		// A second benefit of only calling this function when really necessary is that we can minimize the number of times we update the
		// timestamp. This reduces the chance of timestamp overflow (particularly if it is only 32-bit).
		POLYVOX_ASSERT(iKeyAsInt32 != m_v3dLastAccessedChunkKey, "This should have been checked as an optimization before calling getChunk().");

		// The chunk was not the same as last time, but we can now hope it is in the set of most recently used chunks.
		Chunk* pChunk = nullptr;
		auto itChunk = m_mapChunks.find(iKeyAsInt32);

		// Check whether the chunk was found.
		if ((itChunk) != m_mapChunks.end())
		{
			// The chunk was found so we can use it.
			pChunk = itChunk->second.get();		
			POLYVOX_ASSERT(pChunk, "Recent chunk list shold never contain a null pointer.");
		}

		// If we still haven't found the chunk then it's time to create a new one and page it in from disk.
		if (!pChunk)
		{
			const int32_t uChunkX = (iKeyAsInt32 >> 20) & 0x3FF;
			const int32_t uChunkY = (iKeyAsInt32 >> 10) & 0x3FF;
			const int32_t uChunkZ = (iKeyAsInt32)       & 0x3FF;
			// The chunk was not found so we will create a new one.
			/*ChunkKeyConverter converter;
			converter.i = key;*/
			//ChunkKey realKey = force_cast<ChunkKey>(iKeyAsInt32);
			Vector3DInt32 v3dChunkPos(uChunkX, uChunkY, uChunkZ);
			pChunk = new PagedVolume<VoxelType>::Chunk(v3dChunkPos, m_uChunkSideLength, m_pPager);
			pChunk->m_uChunkLastAccessed = ++m_uTimestamper; // Important, as we may soon delete the oldest chunk
			m_mapChunks.insert(std::make_pair(iKeyAsInt32, std::unique_ptr<Chunk>(pChunk)));

			// As we are loading a new chunk we should try to ensure we don't go over our target memory usage.
			while (m_mapChunks.size() > m_uChunkCountLimit)
			{
				// Find the least recently used chunk. Hopefully this isn't too slow.
				auto itUnloadChunk = m_mapChunks.begin();
				for (auto i = m_mapChunks.begin(); i != m_mapChunks.end(); i++)
				{
					if (i->second->m_uChunkLastAccessed < itUnloadChunk->second->m_uChunkLastAccessed)
					{
						itUnloadChunk = i;
					}
				}

				// Erase the least recently used chunk
				m_mapChunks.erase(itUnloadChunk);
			}
		}
				
		m_pLastAccessedChunk = pChunk;
		//m_v3dLastAccessedChunkPos = v3dChunkPos;
		/*m_v3dLastAccessedChunkX = uChunkX;
		m_v3dLastAccessedChunkY = uChunkY;
		m_v3dLastAccessedChunkZ = uChunkZ;*/
		m_v3dLastAccessedChunkKey = iKeyAsInt32;

		return pChunk;	
	}

	////////////////////////////////////////////////////////////////////////////////
	/// Calculate the memory usage of the volume.
	////////////////////////////////////////////////////////////////////////////////
	template <typename VoxelType>
	uint32_t PagedVolume<VoxelType>::calculateSizeInBytes(void)
	{
		// Note: We disregard the size of the other class members as they are likely to be very small compared to the size of the
		// allocated voxel data. This also keeps the reported size as a power of two, which makes other memory calculations easier.
		return PagedVolume<VoxelType>::Chunk::calculateSizeInBytes(m_uChunkSideLength) * m_mapChunks.size();
	}
}


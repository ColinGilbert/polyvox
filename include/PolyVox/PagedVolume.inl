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
		const int32_t chunkX = uXPos >> m_uChunkSideLengthPower;
		const int32_t chunkY = uYPos >> m_uChunkSideLengthPower;
		const int32_t chunkZ = uZPos >> m_uChunkSideLengthPower;

		const uint16_t xOffset = static_cast<uint16_t>(uXPos & m_iChunkMask);
		const uint16_t yOffset = static_cast<uint16_t>(uYPos & m_iChunkMask);
		const uint16_t zOffset = static_cast<uint16_t>(uZPos & m_iChunkMask);

		auto pChunk = getChunk(chunkX, chunkY, chunkZ);
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
		const int32_t chunkX = uXPos >> m_uChunkSideLengthPower;
		const int32_t chunkY = uYPos >> m_uChunkSideLengthPower;
		const int32_t chunkZ = uZPos >> m_uChunkSideLengthPower;

		const uint16_t xOffset = static_cast<uint16_t>(uXPos - (chunkX << m_uChunkSideLengthPower));
		const uint16_t yOffset = static_cast<uint16_t>(uYPos - (chunkY << m_uChunkSideLengthPower));
		const uint16_t zOffset = static_cast<uint16_t>(uZPos - (chunkZ << m_uChunkSideLengthPower));

		auto pChunk = getChunk(chunkX, chunkY, chunkZ);
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
					getChunk(x,y,z);
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
		// Clear this pointer so it doesn't hang on to any chunks.
		m_pLastAccessedChunk = nullptr;

		/*for (auto chunk : m_mapChunks)
		{
			delete chunk.second;
		}*/

		// Erase all the most recently used chunks.
		m_mapChunks.clear();

		// Remove deleted chunks from the list of all loaded chunks.
		//purgeNullPtrsFromAllChunks();

		// If there are still some chunks left then this is a cause for concern. Perhaps samplers are holding on to them?
		//POLYVOX_LOG_WARNING_IF(m_pAllChunks.size() > 0, "Chunks still exist after performing flushAll()! Perhaps you have samplers attached?");
	}

	////////////////////////////////////////////////////////////////////////////////
	/// Removes all voxels in the specified Region from memory, and calls dataOverflowHandler() to ensure the application has a chance to store the data. It is possible that there are no voxels loaded in the Region, in which case the function will have no effect.
	////////////////////////////////////////////////////////////////////////////////
	template <typename VoxelType>
	void PagedVolume<VoxelType>::flush(Region regFlush)
	{
		// Clear this pointer so it doesn't hang on to any chunks.
		m_pLastAccessedChunk = nullptr;

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
					m_mapChunks.erase(Vector3DInt32(x, y, z));
				}
			}
		}

		m_pLastAccessedChunk = nullptr;

		// We might now have so null pointers in the 'all chunks' list so clean them up.
		purgeNullPtrsFromAllChunks();
	}

	template <typename VoxelType>
	typename PagedVolume<VoxelType>::Chunk* PagedVolume<VoxelType>::getChunk(int32_t uChunkX, int32_t uChunkY, int32_t uChunkZ) const
	{
		Vector3DInt32 v3dChunkPos(uChunkX, uChunkY, uChunkZ);

		//Check if we have the same chunk as last time, if so there's no need to even update
		//the time stamp. If we updated it everytime then that would be every time we touched
		//a voxel, which would overflow a uint32_t and require us to use a uint64_t instead.
		//This check should also provide a significant speed boost as usually it is true.
		if((v3dChunkPos == m_v3dLastAccessedChunkPos) && (m_pLastAccessedChunk != 0))
		{
			return m_pLastAccessedChunk;
		}

		// The chunk was not the same as last time, but we can now hope it is in the set of most recently used chunks.
		//std::shared_ptr<typename PagedVolume<VoxelType>::Chunk> pChunk = nullptr;
		//typename SharedPtrChunkMap::iterator itChunk = m_pRecentlyUsedChunks.find(v3dChunkPos);

		Chunk* pChunk = nullptr;
		auto itChunk = m_mapChunks.find(v3dChunkPos);

		// Check whether the chunk was found.
		if ((itChunk) != m_mapChunks.end())
		{
			// The chunk was found so we can use it.
			pChunk = itChunk->second.get();		
			POLYVOX_ASSERT(pChunk, "Recent chunk list shold never contain a null pointer.");
		}

		/*if (!pChunk)
		{
			// Although it's not in our recently use chunks, there's some (slim) chance that it
			// exists in the list of all loaded chunks, because a sampler may be holding on to it.
			typename WeakPtrChunkMap::iterator itWeakChunk = m_pAllChunks.find(v3dChunkPos);
			if (itWeakChunk != m_pAllChunks.end())
			{
				// We've found an entry in the 'all chunks' list, but it can be null. This happens if a sampler was the
				// last thing to be keeping it alive and then the sampler let it go. In this case we remove it from the
				// list, and it will get added again soon when we page it in and fill it with valid data.
				if (itWeakChunk->second.expired())
				{
					m_pAllChunks.erase(itWeakChunk);
				}
				else
				{
					// The chunk is valid. We know it's not in the recently used list (we checked earlier) so it should be added.
					pChunk = std::shared_ptr< PagedVolume<VoxelType>::Chunk >(itWeakChunk->second);
					m_pRecentlyUsedChunks.insert(std::make_pair(v3dChunkPos, pChunk));
				}
			}
		}*/

		// If we still haven't found the chunk then it's time to create a new one and page it in from disk.
		if (!pChunk)
		{
			// The chunk was not found so we will create a new one.
			//pChunk = std::make_shared< PagedVolume<VoxelType>::Chunk >(v3dChunkPos, m_uChunkSideLength, m_pPager);
			pChunk = new PagedVolume<VoxelType>::Chunk(v3dChunkPos, m_uChunkSideLength, m_pPager);

			// As we are loading a new chunk we should try to ensure we don't go over our target memory usage.
			bool erasedChunk = false;
			while (m_mapChunks.size() + 1 > m_uChunkCountLimit) // +1 ready for new chunk we will add next.
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
				//delete itUnloadChunk->second;
				m_mapChunks.erase(itUnloadChunk);
				erasedChunk = true;
			}

			// If we've deleted any chunks from the recently used list then this
			// seems like a good place to purge the 'all chunks' list as well.
			if (erasedChunk)
			{
				purgeNullPtrsFromAllChunks();
			}

			// Add our new chunk to the maps.
			//m_pAllChunks.insert(std::make_pair(v3dChunkPos, pChunk));
			//m_pRecentlyUsedChunks.insert(std::make_pair(v3dChunkPos, pChunk));
			m_mapChunks.insert(std::make_pair(v3dChunkPos, std::unique_ptr<Chunk>(pChunk)));
		}

		pChunk->m_uChunkLastAccessed = ++m_uTimestamper;
		m_pLastAccessedChunk = pChunk;
		m_v3dLastAccessedChunkPos = v3dChunkPos;

		return pChunk;	
	}

	////////////////////////////////////////////////////////////////////////////////
	/// Calculate the memory usage of the volume.
	////////////////////////////////////////////////////////////////////////////////
	template <typename VoxelType>
	uint32_t PagedVolume<VoxelType>::calculateSizeInBytes(void)
	{
		// Purge null chunks so we know that all chunks are used.
		purgeNullPtrsFromAllChunks();

		// Note: We disregard the size of the other class members as they are likely to be very small compared to the size of the
		// allocated voxel data. This also keeps the reported size as a power of two, which makes other memory calculations easier.
		return PagedVolume<VoxelType>::Chunk::calculateSizeInBytes(m_uChunkSideLength) * m_mapChunks.size();
	}

	template <typename VoxelType>
	void PagedVolume<VoxelType>::purgeNullPtrsFromAllChunks(void) const
	{
		/*for (auto chunkIter = m_pAllChunks.begin(); chunkIter != m_pAllChunks.end();)
		{
			if (chunkIter->second.expired())
			{
				chunkIter = m_pAllChunks.erase(chunkIter);
			}
			else
			{
				chunkIter++;
			}
		}*/
	}
}


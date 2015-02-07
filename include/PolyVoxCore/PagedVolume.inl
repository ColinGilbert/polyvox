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

#include "PolyVoxCore/Impl/ErrorHandling.h"

#include <algorithm>
#include <limits>

namespace PolyVox
{
	////////////////////////////////////////////////////////////////////////////////
	/// This constructor creates a volume with a fixed size which is specified as a parameter. By default this constructor will not enable paging but you can override this if desired. If you do wish to enable paging then you are required to provide the call back function (see the other PagedVolume constructor).
	/// \param regValid Specifies the minimum and maximum valid voxel positions.
	/// \param dataRequiredHandler The callback function which will be called when PolyVox tries to use data which is not currently in momory.
	/// \param dataOverflowHandler The callback function which will be called when PolyVox has too much data and needs to remove some from memory.
	/// \param bPagingEnabled Controls whether or not paging is enabled for this PagedVolume.
	/// \param uChunkSideLength The size of the chunks making up the volume. Small chunks will compress/decompress faster, but there will also be more of them meaning voxel access could be slower.
	////////////////////////////////////////////////////////////////////////////////
	template <typename VoxelType>
	PagedVolume<VoxelType>::PagedVolume
	(
		const Region& regValid,
		Pager* pPager,
		uint16_t uChunkSideLength
	)
	:BaseVolume<VoxelType>(regValid)
	{
		m_uChunkSideLength = uChunkSideLength;
		m_pPager = pPager;

		if (m_pPager)
		{
			// If the user is creating a vast (almost infinite) volume then we can bet they will be
			// expecting a high memory usage and will want a fair number of chunks to play around with.
			if (regValid == Region::MaxRegion())
			{
				m_uChunkCountLimit = 1024;
			}
			else
			{
				// Otherwise we try to choose a chunk count to avoid too much thrashing, particularly when iterating
				// over the whole volume. This means at least enough chunks to cover one edge of the volume, and ideally 
				// enough for a whole face. Which face? Longest edge by shortest edge seems like a reasonable guess.
				uint32_t longestSide = (std::max)(regValid.getWidthInVoxels(), (std::max)(regValid.getHeightInVoxels(), regValid.getDepthInVoxels()));
				uint32_t shortestSide = (std::min)(regValid.getWidthInVoxels(), (std::min)(regValid.getHeightInVoxels(), regValid.getDepthInVoxels()));

				longestSide /= m_uChunkSideLength;
				shortestSide /= m_uChunkSideLength;

				m_uChunkCountLimit = longestSide * shortestSide;
			}
		}
		else
		{
			// If there is no pager provided then we set the chunk limit to the maximum
			// value to ensure the system never attempts to page chunks out of memory.
			m_uChunkCountLimit = (std::numeric_limits<uint32_t>::max)();
		}

		// Make sure the calculated chunk limit is within practical bounds
		m_uChunkCountLimit = (std::max)(m_uChunkCountLimit, uMinPracticalNoOfChunks);
		m_uChunkCountLimit = (std::min)(m_uChunkCountLimit, uMaxPracticalNoOfChunks);

		uint32_t uChunkSizeInBytes = PagedVolume<VoxelType>::Chunk::calculateSizeInBytes(m_uChunkSideLength);
		POLYVOX_LOG_DEBUG("Memory usage limit for volume initially set to " << (m_uChunkCountLimit * uChunkSizeInBytes) / (1024 * 1024)
			<< "Mb (" << m_uChunkCountLimit << " chunks of " << uChunkSizeInBytes / 1024 << "Kb each).");

		initialise();
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
		POLYVOX_THROW(not_implemented, "Volume copy constructor not implemented for performance reasons.");
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
		POLYVOX_THROW(not_implemented, "Volume assignment operator not implemented for performance reasons.");
	}

	////////////////////////////////////////////////////////////////////////////////
	/// This version of the function requires the wrap mode to be specified as a
	/// template parameter, which can provide better performance.
	/// \param uXPos The \c x position of the voxel
	/// \param uYPos The \c y position of the voxel
	/// \param uZPos The \c z position of the voxel
	/// \tparam eWrapMode Specifies the behaviour when the requested position is outside of the volume.
	/// \param tBorder The border value to use if the wrap mode is set to 'Border'.
	/// \return The voxel value
	////////////////////////////////////////////////////////////////////////////////
	template <typename VoxelType>
	template <WrapMode eWrapMode>
	VoxelType PagedVolume<VoxelType>::getVoxel(int32_t uXPos, int32_t uYPos, int32_t uZPos, VoxelType tBorder) const
	{
		// Simply call through to the real implementation
		return getVoxelImpl(uXPos, uYPos, uZPos, WrapModeType<eWrapMode>(), tBorder);
	}

	////////////////////////////////////////////////////////////////////////////////
	/// This version of the function requires the wrap mode to be specified as a
	/// template parameter, which can provide better performance.
	/// \param uXPos The \c x position of the voxel
	/// \param uYPos The \c y position of the voxel
	/// \param uZPos The \c z position of the voxel
	/// \tparam eWrapMode Specifies the behaviour when the requested position is outside of the volume.
	/// \param tBorder The border value to use if the wrap mode is set to 'Border'.
	/// \return The voxel value
	////////////////////////////////////////////////////////////////////////////////
	template <typename VoxelType>
	template <WrapMode eWrapMode>
	VoxelType PagedVolume<VoxelType>::getVoxel(const Vector3DInt32& v3dPos, VoxelType tBorder) const
	{
		// Simply call through to the real implementation
		return getVoxel<eWrapMode>(v3dPos.getX(), v3dPos.getY(), v3dPos.getZ(), tBorder);
	}

	////////////////////////////////////////////////////////////////////////////////
	/// This version of the function is provided so that the wrap mode does not need
	/// to be specified as a template parameter, as it may be confusing to some users.
	/// \param uXPos The \c x position of the voxel
	/// \param uYPos The \c y position of the voxel
	/// \param uZPos The \c z position of the voxel
	/// \param eWrapMode Specifies the behaviour when the requested position is outside of the volume.
	/// \param tBorder The border value to use if the wrap mode is set to 'Border'.
	/// \return The voxel value
	////////////////////////////////////////////////////////////////////////////////
	template <typename VoxelType>
	VoxelType PagedVolume<VoxelType>::getVoxel(int32_t uXPos, int32_t uYPos, int32_t uZPos, WrapMode eWrapMode, VoxelType tBorder) const
	{
		switch(eWrapMode)
		{
		case WrapModes::Validate:
			return getVoxelImpl(uXPos, uYPos, uZPos, WrapModeType<WrapModes::Validate>(), tBorder);
		case WrapModes::Clamp:
			return getVoxelImpl(uXPos, uYPos, uZPos, WrapModeType<WrapModes::Clamp>(), tBorder);
		case WrapModes::Border:
			return getVoxelImpl(uXPos, uYPos, uZPos, WrapModeType<WrapModes::Border>(), tBorder);
		case WrapModes::AssumeValid:
			return getVoxelImpl(uXPos, uYPos, uZPos, WrapModeType<WrapModes::AssumeValid>(), tBorder);
		default:
			// Should never happen
			POLYVOX_ASSERT(false, "Invalid wrap mode");
			return VoxelType();
		}
	}

	////////////////////////////////////////////////////////////////////////////////
	/// This version of the function is provided so that the wrap mode does not need
	/// to be specified as a template parameter, as it may be confusing to some users.
	/// \param v3dPos The 3D position of the voxel
	/// \param eWrapMode Specifies the behaviour when the requested position is outside of the volume.
	/// \param tBorder The border value to use if the wrap mode is set to 'Border'.
	/// \return The voxel value
	////////////////////////////////////////////////////////////////////////////////
	template <typename VoxelType>
	VoxelType PagedVolume<VoxelType>::getVoxel(const Vector3DInt32& v3dPos, WrapMode eWrapMode, VoxelType tBorder) const
	{
		return getVoxel(v3dPos.getX(), v3dPos.getY(), v3dPos.getZ(), eWrapMode, tBorder);
	}

	////////////////////////////////////////////////////////////////////////////////
	/// \param uXPos The \c x position of the voxel
	/// \param uYPos The \c y position of the voxel
	/// \param uZPos The \c z position of the voxel
	/// \return The voxel value
	////////////////////////////////////////////////////////////////////////////////
	template <typename VoxelType>
	VoxelType PagedVolume<VoxelType>::getVoxelAt(int32_t uXPos, int32_t uYPos, int32_t uZPos) const
	{
		if(this->m_regValidRegion.containsPoint(Vector3DInt32(uXPos, uYPos, uZPos)))
		{
			const int32_t chunkX = uXPos >> m_uChunkSideLengthPower;
			const int32_t chunkY = uYPos >> m_uChunkSideLengthPower;
			const int32_t chunkZ = uZPos >> m_uChunkSideLengthPower;

			const uint16_t xOffset = static_cast<uint16_t>(uXPos - (chunkX << m_uChunkSideLengthPower));
			const uint16_t yOffset = static_cast<uint16_t>(uYPos - (chunkY << m_uChunkSideLengthPower));
			const uint16_t zOffset = static_cast<uint16_t>(uZPos - (chunkZ << m_uChunkSideLengthPower));

			auto pChunk = getChunk(chunkX, chunkY, chunkZ);

			return pChunk->getVoxel(xOffset, yOffset, zOffset);
		}
		else
		{
			return this->getBorderValue();
		}
	}

	////////////////////////////////////////////////////////////////////////////////
	/// \param v3dPos The 3D position of the voxel
	/// \return The voxel value
	////////////////////////////////////////////////////////////////////////////////
	template <typename VoxelType>
	VoxelType PagedVolume<VoxelType>::getVoxelAt(const Vector3DInt32& v3dPos) const
	{
		return getVoxelAt(v3dPos.getX(), v3dPos.getY(), v3dPos.getZ());
	}

	////////////////////////////////////////////////////////////////////////////////
	/// Increasing the size of the chunk cache will increase memory but may improve performance.
	/// You may want to set this to a large value (e.g. 1024) when you are first loading your
	/// volume data and then set it to a smaller value (e.g.64) for general processing.
	/// \param uMaxNumberOfChunks The number of chunks for which uncompressed data can be cached.
	////////////////////////////////////////////////////////////////////////////////
	template <typename VoxelType>
	void PagedVolume<VoxelType>::setMemoryUsageLimit(uint32_t uMemoryUsageInBytes)
	{
		POLYVOX_THROW_IF(!m_pPager, invalid_operation, "You cannot limit the memory usage of the volume because it was created without a pager attached.");

		// Calculate the number of chunks based on the memory limit and the size of each chunk.
		uint32_t uChunkSizeInBytes = PagedVolume<VoxelType>::Chunk::calculateSizeInBytes(m_uChunkSideLength);
		m_uChunkCountLimit = uMemoryUsageInBytes / uChunkSizeInBytes;

		// We need at least a few chunks available to avoid thrashing, and in pratice there will probably be hundreds.
		POLYVOX_LOG_WARNING_IF(m_uChunkCountLimit < uMinPracticalNoOfChunks, "Requested memory usage limit of " 
			<< uMemoryUsageInBytes / (1024 * 1024) << "Mb is too low and cannot be adhered to.");
		m_uChunkCountLimit = (std::max)(m_uChunkCountLimit, uMinPracticalNoOfChunks);
		m_uChunkCountLimit = (std::min)(m_uChunkCountLimit, uMaxPracticalNoOfChunks);

		// If the new limit is less than the number of chunks already loaded then the easiest solution is to flush and start loading again.
		if (m_pRecentlyUsedChunks.size() > m_uChunkCountLimit)
		{
			flushAll();
		}

		POLYVOX_LOG_DEBUG("Memory usage limit for volume now set to " << (m_uChunkCountLimit * uChunkSizeInBytes) / (1024 * 1024)
			<< "Mb (" << m_uChunkCountLimit << " chunks of " << uChunkSizeInBytes / 1024 << "Kb each).");
	}

	////////////////////////////////////////////////////////////////////////////////
	/// \param uXPos the \c x position of the voxel
	/// \param uYPos the \c y position of the voxel
	/// \param uZPos the \c z position of the voxel
	/// \param tValue the value to which the voxel will be set
	/// \param eWrapMode Specifies the behaviour when the requested position is outside of the volume.
	/// This must be set to 'None' or 'DontCheck'. Other wrap modes cannot be used when writing to volume data.
	////////////////////////////////////////////////////////////////////////////////
	template <typename VoxelType>
	void PagedVolume<VoxelType>::setVoxel(int32_t uXPos, int32_t uYPos, int32_t uZPos, VoxelType tValue, WrapMode eWrapMode)
	{
		if((eWrapMode != WrapModes::Validate) && (eWrapMode != WrapModes::AssumeValid))
		{
			POLYVOX_THROW(std::invalid_argument, "Invalid wrap mode in call to setVoxel(). It must be 'None' or 'DontCheck'.");
		}

		// This validation is skipped if the wrap mode is 'DontCheck'
		if(eWrapMode == WrapModes::Validate)
		{
			if(this->m_regValidRegion.containsPoint(Vector3DInt32(uXPos, uYPos, uZPos)) == false)
			{
				POLYVOX_THROW(std::out_of_range, "Position is outside valid region");
			}
		}

		const int32_t chunkX = uXPos >> m_uChunkSideLengthPower;
		const int32_t chunkY = uYPos >> m_uChunkSideLengthPower;
		const int32_t chunkZ = uZPos >> m_uChunkSideLengthPower;

		const uint16_t xOffset = static_cast<uint16_t>(uXPos - (chunkX << m_uChunkSideLengthPower));
		const uint16_t yOffset = static_cast<uint16_t>(uYPos - (chunkY << m_uChunkSideLengthPower));
		const uint16_t zOffset = static_cast<uint16_t>(uZPos - (chunkZ << m_uChunkSideLengthPower));

		auto pChunk = getChunk(chunkX, chunkY, chunkZ);
		pChunk->setVoxelAt(xOffset, yOffset, zOffset, tValue);
	}

	////////////////////////////////////////////////////////////////////////////////
	/// \param v3dPos the 3D position of the voxel
	/// \param tValue the value to which the voxel will be set
	/// \param eWrapMode Specifies the behaviour when the requested position is outside of the volume.
	/// This must be set to 'None' or 'DontCheck'. Other wrap modes cannot be used when writing to volume data.
	////////////////////////////////////////////////////////////////////////////////
	template <typename VoxelType>
	void PagedVolume<VoxelType>::setVoxel(const Vector3DInt32& v3dPos, VoxelType tValue, WrapMode eWrapMode)
	{
		setVoxel(v3dPos.getX(), v3dPos.getY(), v3dPos.getZ(), tValue, eWrapMode);
	}

	////////////////////////////////////////////////////////////////////////////////
	/// \param uXPos the \c x position of the voxel
	/// \param uYPos the \c y position of the voxel
	/// \param uZPos the \c z position of the voxel
	/// \param tValue the value to which the voxel will be set
	/// \return whether the requested position is inside the volume
	////////////////////////////////////////////////////////////////////////////////
	template <typename VoxelType>
	bool PagedVolume<VoxelType>::setVoxelAt(int32_t uXPos, int32_t uYPos, int32_t uZPos, VoxelType tValue)
	{
		// PolyVox does not throw an exception when a voxel is out of range. Please see 'Error Handling' in the User Manual.
		POLYVOX_ASSERT(this->m_regValidRegion.containsPoint(Vector3DInt32(uXPos, uYPos, uZPos)), "Position is outside valid region");

		const int32_t chunkX = uXPos >> m_uChunkSideLengthPower;
		const int32_t chunkY = uYPos >> m_uChunkSideLengthPower;
		const int32_t chunkZ = uZPos >> m_uChunkSideLengthPower;

		const uint16_t xOffset = static_cast<uint16_t>(uXPos - (chunkX << m_uChunkSideLengthPower));
		const uint16_t yOffset = static_cast<uint16_t>(uYPos - (chunkY << m_uChunkSideLengthPower));
		const uint16_t zOffset = static_cast<uint16_t>(uZPos - (chunkZ << m_uChunkSideLengthPower));

		auto pChunk = getChunk(chunkX, chunkY, chunkZ);

		pChunk->setVoxelAt(xOffset, yOffset, zOffset, tValue);

		//Return true to indicate that we modified a voxel.
		return true;
	}

	////////////////////////////////////////////////////////////////////////////////
	/// \param v3dPos the 3D position of the voxel
	/// \param tValue the value to which the voxel will be set
	/// \return whether the requested position is inside the volume
	////////////////////////////////////////////////////////////////////////////////
	template <typename VoxelType>
	bool PagedVolume<VoxelType>::setVoxelAt(const Vector3DInt32& v3dPos, VoxelType tValue)
	{
		return setVoxelAt(v3dPos.getX(), v3dPos.getY(), v3dPos.getZ(), tValue);
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
		POLYVOX_LOG_WARNING_IF(uNoOfChunks > m_uChunkCountLimit, "Attempting to prefetch more than the maximum number of chunks.");
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
		POLYVOX_LOG_WARNING_IF(!m_pPager, "Data discarded by flush operation as no pager is attached.");

		// Clear this pointer so it doesn't hang on to any chunks.
		m_pLastAccessedChunk = nullptr;

		// Erase all the most recently used chunks.
		m_pRecentlyUsedChunks.clear();

		// Remove deleted chunks from the list of all loaded chunks.
		purgeNullPtrsFromAllChunks();

		// If there are still some chunks left then this is a cause for concern. Perhaps samplers are holding on to them?
		POLYVOX_LOG_WARNING_IF(m_pAllChunks.size() > 0, "Chunks still exist after performing flushAll()! Perhaps you have samplers attached?");
	}

	////////////////////////////////////////////////////////////////////////////////
	/// Removes all voxels in the specified Region from memory, and calls dataOverflowHandler() to ensure the application has a chance to store the data. It is possible that there are no voxels loaded in the Region, in which case the function will have no effect.
	////////////////////////////////////////////////////////////////////////////////
	template <typename VoxelType>
	void PagedVolume<VoxelType>::flush(Region regFlush)
	{
		POLYVOX_LOG_WARNING_IF(!m_pPager, "Data discarded by flush operation as no pager is attached.");

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
					m_pRecentlyUsedChunks.erase(Vector3DInt32(x, y, z));
				}
			}
		}

		m_pLastAccessedChunk = nullptr;

		// We might now have so null pointers in the 'all chunks' list so clean them up.
		purgeNullPtrsFromAllChunks();
	}

	////////////////////////////////////////////////////////////////////////////////
	/// This function should probably be made internal...
	////////////////////////////////////////////////////////////////////////////////
	template <typename VoxelType>
	void PagedVolume<VoxelType>::initialise()
	{		
		//Validate parameters
		if(m_uChunkSideLength == 0)
		{
			POLYVOX_THROW(std::invalid_argument, "Chunk side length cannot be zero.");
		}

		if(!isPowerOf2(m_uChunkSideLength))
		{
			POLYVOX_THROW(std::invalid_argument, "Chunk side length must be a power of two.");
		}

		m_uTimestamper = 0;
		m_v3dLastAccessedChunkPos = Vector3DInt32(0,0,0); //There are no invalid positions, but initially the m_pLastAccessedChunk pointer will be null;
		m_pLastAccessedChunk = nullptr;

		//Compute the chunk side length
		m_uChunkSideLengthPower = logBase2(m_uChunkSideLength);

		m_regValidRegionInChunks.setLowerX(this->m_regValidRegion.getLowerX() >> m_uChunkSideLengthPower);
		m_regValidRegionInChunks.setLowerY(this->m_regValidRegion.getLowerY() >> m_uChunkSideLengthPower);
		m_regValidRegionInChunks.setLowerZ(this->m_regValidRegion.getLowerZ() >> m_uChunkSideLengthPower);
		m_regValidRegionInChunks.setUpperX(this->m_regValidRegion.getUpperX() >> m_uChunkSideLengthPower);
		m_regValidRegionInChunks.setUpperY(this->m_regValidRegion.getUpperY() >> m_uChunkSideLengthPower);
		m_regValidRegionInChunks.setUpperZ(this->m_regValidRegion.getUpperZ() >> m_uChunkSideLengthPower);

		//setMaxNumberOfChunks(m_uChunkCountLimit);

		//Clear the previous data
		m_pRecentlyUsedChunks.clear();

		//Other properties we might find useful later
		this->m_uLongestSideLength = (std::max)((std::max)(this->getWidth(),this->getHeight()),this->getDepth());
		this->m_uShortestSideLength = (std::min)((std::min)(this->getWidth(),this->getHeight()),this->getDepth());
		this->m_fDiagonalLength = sqrtf(static_cast<float>(this->getWidth() * this->getWidth() + this->getHeight() * this->getHeight() + this->getDepth() * this->getDepth()));
	}

	template <typename VoxelType>
	std::shared_ptr<typename PagedVolume<VoxelType>::Chunk> PagedVolume<VoxelType>::getChunk(int32_t uChunkX, int32_t uChunkY, int32_t uChunkZ) const
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
		std::shared_ptr<typename PagedVolume<VoxelType>::Chunk> pChunk = nullptr;
		typename SharedPtrChunkMap::iterator itChunk = m_pRecentlyUsedChunks.find(v3dChunkPos);

		// Check whether the chunk was found.
		if ((itChunk) != m_pRecentlyUsedChunks.end())
		{
			// The chunk was found so we can use it.
			pChunk = itChunk->second;		
			POLYVOX_ASSERT(pChunk, "Recent chunk list shold never contain a null pointer.");
		}

		if (!pChunk)
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
		}

		// If we still haven't found the chunk then it's time to create a new one and page it in from disk.
		if (!pChunk)
		{
			// The chunk was not found so we will create a new one.
			pChunk = std::make_shared< PagedVolume<VoxelType>::Chunk >(v3dChunkPos, m_uChunkSideLength, m_pPager);

			// As we are loading a new chunk we should try to ensure we don't go over our target memory usage.
			bool erasedChunk = false;
			while (m_pRecentlyUsedChunks.size() + 1 > m_uChunkCountLimit) // +1 ready for new chunk we will add next.
			{
				// This should never hit, because it should not have been possible for
				// the user to limit the number of chunks if they did not provide a pager.
				POLYVOX_ASSERT(m_pPager, "A valid pager is required to limit number of chunks");

				// Find the least recently used chunk. Hopefully this isn't too slow.
				typename SharedPtrChunkMap::iterator itUnloadChunk = m_pRecentlyUsedChunks.begin();
				for (typename SharedPtrChunkMap::iterator i = m_pRecentlyUsedChunks.begin(); i != m_pRecentlyUsedChunks.end(); i++)
				{
					if (i->second->m_uChunkLastAccessed < itUnloadChunk->second->m_uChunkLastAccessed)
					{
						itUnloadChunk = i;
					}
				}

				// Erase the least recently used chunk
				m_pRecentlyUsedChunks.erase(itUnloadChunk);
				erasedChunk = true;
			}

			// If we've deleted any chunks from the recently used list then this
			// seems like a good place to purge the 'all chunks' list as well.
			if (erasedChunk)
			{
				purgeNullPtrsFromAllChunks();
			}

			// Add our new chunk to the maps.
			m_pAllChunks.insert(std::make_pair(v3dChunkPos, pChunk));
			m_pRecentlyUsedChunks.insert(std::make_pair(v3dChunkPos, pChunk));
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
		return PagedVolume<VoxelType>::Chunk::calculateSizeInBytes(m_uChunkSideLength) * m_pAllChunks.size();
	}

	template <typename VoxelType>
	void PagedVolume<VoxelType>::purgeNullPtrsFromAllChunks(void) const
	{
		for (auto chunkIter = m_pAllChunks.begin(); chunkIter != m_pAllChunks.end();)
		{
			if (chunkIter->second.expired())
			{
				chunkIter = m_pAllChunks.erase(chunkIter);
			}
			else
			{
				chunkIter++;
			}
		}
	}

	template <typename VoxelType>
	template <WrapMode eWrapMode>
	VoxelType PagedVolume<VoxelType>::getVoxelImpl(int32_t uXPos, int32_t uYPos, int32_t uZPos, WrapModeType<eWrapMode>, VoxelType tBorder) const
	{
		// This function should never be called because one of the specialisations should always match.
		POLYVOX_ASSERT(false, "This function is not implemented and should never be called!");
	}

	template <typename VoxelType>
	VoxelType PagedVolume<VoxelType>::getVoxelImpl(int32_t uXPos, int32_t uYPos, int32_t uZPos, WrapModeType<WrapModes::Validate>, VoxelType tBorder) const
	{
		if(this->m_regValidRegion.containsPoint(Vector3DInt32(uXPos, uYPos, uZPos)) == false)
		{
			POLYVOX_THROW(std::out_of_range, "Position is outside valid region");
		}

		return getVoxelImpl(uXPos, uYPos, uZPos, WrapModeType<WrapModes::AssumeValid>(), tBorder); // No wrapping as we've just validated the position.
	}

	template <typename VoxelType>
	VoxelType PagedVolume<VoxelType>::getVoxelImpl(int32_t uXPos, int32_t uYPos, int32_t uZPos, WrapModeType<WrapModes::Clamp>, VoxelType tBorder) const
	{
		//Perform clamping
		uXPos = (std::max)(uXPos, this->m_regValidRegion.getLowerX());
		uYPos = (std::max)(uYPos, this->m_regValidRegion.getLowerY());
		uZPos = (std::max)(uZPos, this->m_regValidRegion.getLowerZ());
		uXPos = (std::min)(uXPos, this->m_regValidRegion.getUpperX());
		uYPos = (std::min)(uYPos, this->m_regValidRegion.getUpperY());
		uZPos = (std::min)(uZPos, this->m_regValidRegion.getUpperZ());

		return getVoxelImpl(uXPos, uYPos, uZPos, WrapModeType<WrapModes::AssumeValid>(), tBorder); // No wrapping as we've just validated the position.
	}

	template <typename VoxelType>
	VoxelType PagedVolume<VoxelType>::getVoxelImpl(int32_t uXPos, int32_t uYPos, int32_t uZPos, WrapModeType<WrapModes::Border>, VoxelType tBorder) const
	{
		if(this->m_regValidRegion.containsPoint(uXPos, uYPos, uZPos))
		{
			return getVoxelImpl(uXPos, uYPos, uZPos, WrapModeType<WrapModes::AssumeValid>(), tBorder); // No wrapping as we've just validated the position.
		}
		else
		{
			return tBorder;
		}
	}

	template <typename VoxelType>
	VoxelType PagedVolume<VoxelType>::getVoxelImpl(int32_t uXPos, int32_t uYPos, int32_t uZPos, WrapModeType<WrapModes::AssumeValid>, VoxelType /*tBorder*/) const
	{
		const int32_t chunkX = uXPos >> m_uChunkSideLengthPower;
		const int32_t chunkY = uYPos >> m_uChunkSideLengthPower;
		const int32_t chunkZ = uZPos >> m_uChunkSideLengthPower;

		const uint16_t xOffset = static_cast<uint16_t>(uXPos - (chunkX << m_uChunkSideLengthPower));
		const uint16_t yOffset = static_cast<uint16_t>(uYPos - (chunkY << m_uChunkSideLengthPower));
		const uint16_t zOffset = static_cast<uint16_t>(uZPos - (chunkZ << m_uChunkSideLengthPower));

		auto pChunk = getChunk(chunkX, chunkY, chunkZ);
		return pChunk->getVoxel(xOffset, yOffset, zOffset);
	}
}


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
	/// \param pBlockCompressor An implementation of the Compressor interface which is used to compress blocks in memory.
	/// \param dataRequiredHandler The callback function which will be called when PolyVox tries to use data which is not currently in momory.
	/// \param dataOverflowHandler The callback function which will be called when PolyVox has too much data and needs to remove some from memory.
	/// \param bPagingEnabled Controls whether or not paging is enabled for this PagedVolume.
	/// \param uBlockSideLength The size of the blocks making up the volume. Small blocks will compress/decompress faster, but there will also be more of them meaning voxel access could be slower.
	////////////////////////////////////////////////////////////////////////////////
	template <typename VoxelType>
	PagedVolume<VoxelType>::PagedVolume
	(
		const Region& regValid,
		Pager<VoxelType>* pPager,
		uint16_t uBlockSideLength
	)
	:BaseVolume<VoxelType>(regValid)
	{
		m_uBlockSideLength = uBlockSideLength;
		m_pPager = pPager;

		if (m_pPager)
		{
			// If the user is creating a vast (almost infinite) volume then we can bet they will be
			// expecting a high memory usage and will want a fair number of blocks to play around with.
			if (regValid == Region::MaxRegion)
			{
				m_uBlockCountLimit = 1024;
			}
			else
			{
				// Otherwise we try to choose a block count to avoid too much thrashing, particularly when iterating
				// over the whole volume. This means at least enough blocks to cover one edge of the volume, and ideally 
				// enough for a whole face. Which face? Longest edge by shortest edge seems like a reasonable guess.
				uint32_t longestSide = (std::max)(regValid.getWidthInVoxels(), (std::max)(regValid.getHeightInVoxels(), regValid.getDepthInVoxels()));
				uint32_t shortestSide = (std::min)(regValid.getWidthInVoxels(), (std::min)(regValid.getHeightInVoxels(), regValid.getDepthInVoxels()));

				longestSide /= m_uBlockSideLength;
				shortestSide /= m_uBlockSideLength;

				m_uBlockCountLimit = longestSide * shortestSide;
			}
		}
		else
		{
			// If there is no pager provided then we set the block limit to the maximum
			// value to ensure the system never attempts to page blocks out of memory.
			m_uBlockCountLimit = (std::numeric_limits<uint32_t>::max)();
		}

		// Make sure the calculated block limit is within practical bounds
		m_uBlockCountLimit = (std::max)(m_uBlockCountLimit, uMinPracticalNoOfBlocks);
		m_uBlockCountLimit = (std::min)(m_uBlockCountLimit, uMaxPracticalNoOfBlocks);

		uint32_t uUncompressedBlockSizeInBytes = UncompressedBlock<VoxelType>::calculateSizeInBytes(m_uBlockSideLength);
		POLYVOX_LOG_DEBUG("Memory usage limit for volume initially set to " << (m_uBlockCountLimit * uUncompressedBlockSizeInBytes) / (1024 * 1024)
			<< "Mb (" << m_uBlockCountLimit << " blocks of " << uUncompressedBlockSizeInBytes / 1024 << "Kb each).");

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
			const int32_t blockX = uXPos >> m_uBlockSideLengthPower;
			const int32_t blockY = uYPos >> m_uBlockSideLengthPower;
			const int32_t blockZ = uZPos >> m_uBlockSideLengthPower;

			const uint16_t xOffset = static_cast<uint16_t>(uXPos - (blockX << m_uBlockSideLengthPower));
			const uint16_t yOffset = static_cast<uint16_t>(uYPos - (blockY << m_uBlockSideLengthPower));
			const uint16_t zOffset = static_cast<uint16_t>(uZPos - (blockZ << m_uBlockSideLengthPower));

			auto pUncompressedBlock = getUncompressedBlock(blockX, blockY, blockZ);

			return pUncompressedBlock->getVoxel(xOffset, yOffset, zOffset);
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
	/// Increasing the size of the block cache will increase memory but may improve performance.
	/// You may want to set this to a large value (e.g. 1024) when you are first loading your
	/// volume data and then set it to a smaller value (e.g.64) for general processing.
	/// \param uMaxNumberOfUncompressedBlocks The number of blocks for which uncompressed data can be cached.
	////////////////////////////////////////////////////////////////////////////////
	template <typename VoxelType>
	void PagedVolume<VoxelType>::setMemoryUsageLimit(uint32_t uMemoryUsageInBytes)
	{
		POLYVOX_THROW_IF(!m_pPager, invalid_operation, "You cannot limit the memory usage of the volume because it was created without a pager attached.");

		// Calculate the number of blocks based on the memory limit and the size of each block.
		uint32_t uUncompressedBlockSizeInBytes = UncompressedBlock<VoxelType>::calculateSizeInBytes(m_uBlockSideLength);
		m_uBlockCountLimit = uMemoryUsageInBytes / uUncompressedBlockSizeInBytes;

		// We need at least a few blocks available to avoid thrashing, and in pratice there will probably be hundreds.
		POLYVOX_LOG_WARNING_IF(m_uBlockCountLimit < uMinPracticalNoOfBlocks, "Requested memory usage limit of " 
			<< uMemoryUsageInBytes / (1024 * 1024) << "Mb is too low and cannot be adhered to.");
		m_uBlockCountLimit = (std::max)(m_uBlockCountLimit, uMinPracticalNoOfBlocks);
		m_uBlockCountLimit = (std::min)(m_uBlockCountLimit, uMaxPracticalNoOfBlocks);

		// If the new limit is less than the number of blocks already loaded then the easiest solution is to flush and start loading again.
		if (m_pRecentlyUsedBlocks.size() > m_uBlockCountLimit)
		{
			flushAll();
		}

		POLYVOX_LOG_DEBUG("Memory usage limit for volume now set to " << (m_uBlockCountLimit * uUncompressedBlockSizeInBytes) / (1024 * 1024)
			<< "Mb (" << m_uBlockCountLimit << " blocks of " << uUncompressedBlockSizeInBytes / 1024 << "Kb each).");
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

		const int32_t blockX = uXPos >> m_uBlockSideLengthPower;
		const int32_t blockY = uYPos >> m_uBlockSideLengthPower;
		const int32_t blockZ = uZPos >> m_uBlockSideLengthPower;

		const uint16_t xOffset = static_cast<uint16_t>(uXPos - (blockX << m_uBlockSideLengthPower));
		const uint16_t yOffset = static_cast<uint16_t>(uYPos - (blockY << m_uBlockSideLengthPower));
		const uint16_t zOffset = static_cast<uint16_t>(uZPos - (blockZ << m_uBlockSideLengthPower));

		auto pUncompressedBlock = getUncompressedBlock(blockX, blockY, blockZ);
		pUncompressedBlock->setVoxelAt(xOffset, yOffset, zOffset, tValue);
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

		const int32_t blockX = uXPos >> m_uBlockSideLengthPower;
		const int32_t blockY = uYPos >> m_uBlockSideLengthPower;
		const int32_t blockZ = uZPos >> m_uBlockSideLengthPower;

		const uint16_t xOffset = static_cast<uint16_t>(uXPos - (blockX << m_uBlockSideLengthPower));
		const uint16_t yOffset = static_cast<uint16_t>(uYPos - (blockY << m_uBlockSideLengthPower));
		const uint16_t zOffset = static_cast<uint16_t>(uZPos - (blockZ << m_uBlockSideLengthPower));

		auto pUncompressedBlock = getUncompressedBlock(blockX, blockY, blockZ);

		pUncompressedBlock->setVoxelAt(xOffset, yOffset, zOffset, tValue);

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
	/// Note that if *NOTE - update docs - MaxNumberOfBlocksInMemory no longer exists* MaxNumberOfBlocksInMemory is not large enough to support the region this function will only load part of the region. In this case it is undefined which parts will actually be loaded. If all the voxels in the given region are already loaded, this function will not do anything. Other voxels might be unloaded to make space for the new voxels.
	/// \param regPrefetch The Region of voxels to prefetch into memory.
	////////////////////////////////////////////////////////////////////////////////
	template <typename VoxelType>
	void PagedVolume<VoxelType>::prefetch(Region regPrefetch)
	{
		// Convert the start and end positions into block space coordinates
		Vector3DInt32 v3dStart;
		for(int i = 0; i < 3; i++)
		{
			v3dStart.setElement(i, regPrefetch.getLowerCorner().getElement(i) >> m_uBlockSideLengthPower);
		}

		Vector3DInt32 v3dEnd;
		for(int i = 0; i < 3; i++)
		{
			v3dEnd.setElement(i, regPrefetch.getUpperCorner().getElement(i) >> m_uBlockSideLengthPower);
		}

		// Ensure we don't page in more blocks than the volume can hold.
		Region region(v3dStart, v3dEnd);
		uint32_t uNoOfBlocks = static_cast<uint32_t>(region.getWidthInVoxels() * region.getHeightInVoxels() * region.getDepthInVoxels());
		POLYVOX_LOG_WARNING_IF(uNoOfBlocks > m_uBlockCountLimit, "Attempting to prefetch more than the maximum number of blocks.");
		uNoOfBlocks = (std::min)(uNoOfBlocks, m_uBlockCountLimit);

		// Loops over the specified positions and touch the corresponding blocks.
		for(int32_t x = v3dStart.getX(); x <= v3dEnd.getX(); x++)
		{
			for(int32_t y = v3dStart.getY(); y <= v3dEnd.getY(); y++)
			{
				for(int32_t z = v3dStart.getZ(); z <= v3dEnd.getZ(); z++)
				{					
					getUncompressedBlock(x,y,z);
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

		// Clear this pointer so it doesn't hang on to any blocks.
		m_pLastAccessedBlock = nullptr;

		// Erase all the most recently used blocks.
		m_pRecentlyUsedBlocks.clear();

		// Remove deleted blocks from the list of all loaded blocks.
		purgeNullPtrsFromAllBlocks();

		// If there are still some blocks left then this is a cause for concern. Perhaps samplers are holding on to them?
		POLYVOX_LOG_WARNING_IF(m_pAllBlocks.size() > 0, "Blocks still exist after performing flushAll()! Perhaps you have samplers attached?");
	}

	////////////////////////////////////////////////////////////////////////////////
	/// Removes all voxels in the specified Region from memory, and calls dataOverflowHandler() to ensure the application has a chance to store the data. It is possible that there are no voxels loaded in the Region, in which case the function will have no effect.
	////////////////////////////////////////////////////////////////////////////////
	template <typename VoxelType>
	void PagedVolume<VoxelType>::flush(Region regFlush)
	{
		POLYVOX_LOG_WARNING_IF(!m_pPager, "Data discarded by flush operation as no pager is attached.");

		// Clear this pointer so it doesn't hang on to any blocks.
		m_pLastAccessedBlock = nullptr;

		// Convert the start and end positions into block space coordinates
		Vector3DInt32 v3dStart;
		for(int i = 0; i < 3; i++)
		{
			v3dStart.setElement(i, regFlush.getLowerCorner().getElement(i) >> m_uBlockSideLengthPower);
		}

		Vector3DInt32 v3dEnd;
		for(int i = 0; i < 3; i++)
		{
			v3dEnd.setElement(i, regFlush.getUpperCorner().getElement(i) >> m_uBlockSideLengthPower);
		}

		// Loops over the specified positions and delete the corresponding blocks.
		for(int32_t x = v3dStart.getX(); x <= v3dEnd.getX(); x++)
		{
			for(int32_t y = v3dStart.getY(); y <= v3dEnd.getY(); y++)
			{
				for(int32_t z = v3dStart.getZ(); z <= v3dEnd.getZ(); z++)
				{
					m_pRecentlyUsedBlocks.erase(Vector3DInt32(x, y, z)); m_pLastAccessedBlock = 0;
				}
			}
		}

		// We might now have so null pointers in the 'all blocks' list so clean them up.
		purgeNullPtrsFromAllBlocks();
	}

	////////////////////////////////////////////////////////////////////////////////
	/// This function should probably be made internal...
	////////////////////////////////////////////////////////////////////////////////
	template <typename VoxelType>
	void PagedVolume<VoxelType>::initialise()
	{		
		//Validate parameters
		if(m_uBlockSideLength == 0)
		{
			POLYVOX_THROW(std::invalid_argument, "Block side length cannot be zero.");
		}

		if(!isPowerOf2(m_uBlockSideLength))
		{
			POLYVOX_THROW(std::invalid_argument, "Block side length must be a power of two.");
		}

		m_uTimestamper = 0;
		//m_uBlockCountLimit = 16;
		//m_uMaxNumberOfBlocksInMemory = 1024;
		m_v3dLastAccessedBlockPos = Vector3DInt32(0,0,0); //There are no invalid positions, but initially the m_pLastAccessedBlock pointer will be null;
		m_pLastAccessedBlock = 0;

		//Compute the block side length
		m_uBlockSideLengthPower = logBase2(m_uBlockSideLength);

		m_regValidRegionInBlocks.setLowerX(this->m_regValidRegion.getLowerX() >> m_uBlockSideLengthPower);
		m_regValidRegionInBlocks.setLowerY(this->m_regValidRegion.getLowerY() >> m_uBlockSideLengthPower);
		m_regValidRegionInBlocks.setLowerZ(this->m_regValidRegion.getLowerZ() >> m_uBlockSideLengthPower);
		m_regValidRegionInBlocks.setUpperX(this->m_regValidRegion.getUpperX() >> m_uBlockSideLengthPower);
		m_regValidRegionInBlocks.setUpperY(this->m_regValidRegion.getUpperY() >> m_uBlockSideLengthPower);
		m_regValidRegionInBlocks.setUpperZ(this->m_regValidRegion.getUpperZ() >> m_uBlockSideLengthPower);

		//setMaxNumberOfUncompressedBlocks(m_uBlockCountLimit);

		//Clear the previous data
		m_pRecentlyUsedBlocks.clear();

		//Other properties we might find useful later
		this->m_uLongestSideLength = (std::max)((std::max)(this->getWidth(),this->getHeight()),this->getDepth());
		this->m_uShortestSideLength = (std::min)((std::min)(this->getWidth(),this->getHeight()),this->getDepth());
		this->m_fDiagonalLength = sqrtf(static_cast<float>(this->getWidth() * this->getWidth() + this->getHeight() * this->getHeight() + this->getDepth() * this->getDepth()));
	}

	template <typename VoxelType>
	std::shared_ptr< UncompressedBlock<VoxelType> > PagedVolume<VoxelType>::getUncompressedBlock(int32_t uBlockX, int32_t uBlockY, int32_t uBlockZ) const
	{
		Vector3DInt32 v3dBlockPos(uBlockX, uBlockY, uBlockZ);

		//Check if we have the same block as last time, if so there's no need to even update
		//the time stamp. If we updated it everytime then that would be every time we touched
		//a voxel, which would overflow a uint32_t and require us to use a uint64_t instead.
		//This check should also provide a significant speed boost as usually it is true.
		if((v3dBlockPos == m_v3dLastAccessedBlockPos) && (m_pLastAccessedBlock != 0))
		{
			return m_pLastAccessedBlock;
		}

		// The block was not the same as last time, but we can now hope it is in the set of most recently used blocks.
		std::shared_ptr< UncompressedBlock<VoxelType> > pUncompressedBlock = nullptr;
		typename SharedPtrBlockMap::iterator itUncompressedBlock = m_pRecentlyUsedBlocks.find(v3dBlockPos);

		// Check whether the block was found.
		if ((itUncompressedBlock) != m_pRecentlyUsedBlocks.end())
		{
			// The block was found so we can use it.
			pUncompressedBlock = itUncompressedBlock->second;		
			POLYVOX_ASSERT(pUncompressedBlock, "Recent block list shold never contain a null pointer.");
		}

		if (!pUncompressedBlock)
		{
			// Although it's not in our recently use blocks, there's some (slim) chance that it
			// exists in the list of all loaded blocks, because a sampler may be holding on to it.
			typename WeakPtrBlockMap::iterator itWeakUncompressedBlock = m_pAllBlocks.find(v3dBlockPos);
			if (itWeakUncompressedBlock != m_pAllBlocks.end())
			{
				// We've found an entry in the 'all blocks' list, but it can be null. This happens if a sampler was the
				// last thing to be keeping it alive and then the sampler let it go. In this case we remove it from the
				// list, and it will get added again soon when we page it in and fill it with valid data.
				if (itWeakUncompressedBlock->second.expired())
				{
					m_pAllBlocks.erase(itWeakUncompressedBlock);
				}
				else
				{
					// The block is valid. We know it's not in the recently used list (we checked earlier) so it should be added.
					pUncompressedBlock = std::shared_ptr< UncompressedBlock<VoxelType> >(itWeakUncompressedBlock->second);
					m_pRecentlyUsedBlocks.insert(std::make_pair(v3dBlockPos, pUncompressedBlock));
				}
			}
		}

		// If we still haven't found the block then it's time to create a new one and page it in from disk.
		if (!pUncompressedBlock)
		{
			// The block was not found so we will create a new one.
			pUncompressedBlock = std::make_shared< UncompressedBlock<VoxelType> >(v3dBlockPos, m_uBlockSideLength, m_pPager);

			// As we are loading a new block we should try to ensure we don't go over our target memory usage.
			bool erasedBlock = false;
			while (m_pRecentlyUsedBlocks.size() + 1 > m_uBlockCountLimit) // +1 ready for new block we will add next.
			{
				// This should never hit, because it should not have been possible for
				// the user to limit the number of blocks if they did not provide a pager.
				POLYVOX_ASSERT(m_pPager, "A valid pager is required to limit number of blocks");

				// Find the least recently used block. Hopefully this isn't too slow.
				typename SharedPtrBlockMap::iterator itUnloadBlock = m_pRecentlyUsedBlocks.begin();
				for (typename SharedPtrBlockMap::iterator i = m_pRecentlyUsedBlocks.begin(); i != m_pRecentlyUsedBlocks.end(); i++)
				{
					if (i->second->m_uBlockLastAccessed < itUnloadBlock->second->m_uBlockLastAccessed)
					{
						itUnloadBlock = i;
					}
				}

				// Erase the least recently used block
				m_pRecentlyUsedBlocks.erase(itUnloadBlock);
				erasedBlock = true;
			}

			// If we've deleted any blocks from the recently used list then this
			// seems like a good place to purge the 'all blocks' list as well.
			if (erasedBlock)
			{
				purgeNullPtrsFromAllBlocks();
			}

			// Add our new block to the maps.
			m_pAllBlocks.insert(std::make_pair(v3dBlockPos, pUncompressedBlock));
			m_pRecentlyUsedBlocks.insert(std::make_pair(v3dBlockPos, pUncompressedBlock));
		}

		pUncompressedBlock->m_uBlockLastAccessed = ++m_uTimestamper;
		m_pLastAccessedBlock = pUncompressedBlock;
		m_v3dLastAccessedBlockPos = v3dBlockPos;

		return pUncompressedBlock;	
	}

	////////////////////////////////////////////////////////////////////////////////
	/// Calculate the memory usage of the volume.
	////////////////////////////////////////////////////////////////////////////////
	template <typename VoxelType>
	uint32_t PagedVolume<VoxelType>::calculateSizeInBytes(void)
	{
		// Purge null blocks so we know that all blocks are used.
		purgeNullPtrsFromAllBlocks();

		// Note: We disregard the size of the other class members as they are likely to be very small compared to the size of the
		// allocated voxel data. This also keeps the reported size as a power of two, which makes other memory calculations easier.
		return UncompressedBlock<VoxelType>::calculateSizeInBytes(m_uBlockSideLength) * m_pAllBlocks.size();
	}

	template <typename VoxelType>
	void PagedVolume<VoxelType>::purgeNullPtrsFromAllBlocks(void) const
	{
		for (auto blockIter = m_pAllBlocks.begin(); blockIter != m_pAllBlocks.end();)
		{
			if (blockIter->second.expired())
			{
				blockIter = m_pAllBlocks.erase(blockIter);
			}
			else
			{
				blockIter++;
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
		const int32_t blockX = uXPos >> m_uBlockSideLengthPower;
		const int32_t blockY = uYPos >> m_uBlockSideLengthPower;
		const int32_t blockZ = uZPos >> m_uBlockSideLengthPower;

		const uint16_t xOffset = static_cast<uint16_t>(uXPos - (blockX << m_uBlockSideLengthPower));
		const uint16_t yOffset = static_cast<uint16_t>(uYPos - (blockY << m_uBlockSideLengthPower));
		const uint16_t zOffset = static_cast<uint16_t>(uZPos - (blockZ << m_uBlockSideLengthPower));

		auto pUncompressedBlock = getUncompressedBlock(blockX, blockY, blockZ);
		return pUncompressedBlock->getVoxel(xOffset, yOffset, zOffset);
	}
}


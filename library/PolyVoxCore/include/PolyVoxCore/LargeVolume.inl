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

#include "PolyVoxCore/MinizBlockCompressor.h" // For creating a default compressor when none is provided.

#include <algorithm>

namespace PolyVox
{
	////////////////////////////////////////////////////////////////////////////////
	/// When construncting a very large volume you need to be prepared to handle the scenario where there is so much data that PolyVox cannot fit it all in memory. When PolyVox runs out of memory, it identifies the least recently used data and hands it back to the application via a callback function. It is then the responsibility of the application to store this data until PolyVox needs it again (as signalled by another callback function). Please see the LargeVolume  class documentation for a full description of this process and the required function signatures. If you really don't want to handle these events then you can provide null pointers here, in which case the data will be discarded and/or filled with default values.
	/// \param dataRequiredHandler The callback function which will be called when PolyVox tries to use data which is not currently in memory.
	/// \param dataOverflowHandler The callback function which will be called when PolyVox has too much data and needs to remove some from memory.
	/// \param uBlockSideLength The size of the blocks making up the volume. Small blocks will compress/decompress faster, but there will also be more of them meaning voxel access could be slower.
	////////////////////////////////////////////////////////////////////////////////
	template <typename VoxelType>
	LargeVolume<VoxelType>::LargeVolume
	(
		const Region& regValid,
		uint16_t uBlockSideLength	
	)
	:BaseVolume<VoxelType>(regValid)
	{
		m_uBlockSideLength = uBlockSideLength;

		m_pBlockCompressor = new MinizBlockCompressor<VoxelType>();
		m_bIsOurCompressor = true;

		m_pPager = 0;

		initialise();
	}

	////////////////////////////////////////////////////////////////////////////////
	/// This constructor creates a volume with a fixed size which is specified as a parameter. By default this constructor will not enable paging but you can override this if desired. If you do wish to enable paging then you are required to provide the call back function (see the other LargeVolume constructor).
	/// \param regValid Specifies the minimum and maximum valid voxel positions.
	/// \param pBlockCompressor An implementation of the Compressor interface which is used to compress blocks in memory.
	/// \param dataRequiredHandler The callback function which will be called when PolyVox tries to use data which is not currently in momory.
	/// \param dataOverflowHandler The callback function which will be called when PolyVox has too much data and needs to remove some from memory.
	/// \param bPagingEnabled Controls whether or not paging is enabled for this LargeVolume.
	/// \param uBlockSideLength The size of the blocks making up the volume. Small blocks will compress/decompress faster, but there will also be more of them meaning voxel access could be slower.
	////////////////////////////////////////////////////////////////////////////////
	template <typename VoxelType>
	LargeVolume<VoxelType>::LargeVolume
	(
		const Region& regValid,
		BlockCompressor<VoxelType>* pBlockCompressor,
		Pager<VoxelType>* pPager,
		uint16_t uBlockSideLength
	)
	:BaseVolume<VoxelType>(regValid)
	{
		m_uBlockSideLength = uBlockSideLength;

		m_pBlockCompressor = pBlockCompressor;
		m_bIsOurCompressor = false;

		m_pPager = pPager;

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
	LargeVolume<VoxelType>::LargeVolume(const LargeVolume<VoxelType>& /*rhs*/)
	{
		POLYVOX_THROW(not_implemented, "Volume copy constructor not implemented for performance reasons.");
	}

	////////////////////////////////////////////////////////////////////////////////
	/// Destroys the volume The destructor will call flushAll() to ensure that a paging volume has the chance to save it's data via the dataOverflowHandler() if desired.
	////////////////////////////////////////////////////////////////////////////////
	template <typename VoxelType>
	LargeVolume<VoxelType>::~LargeVolume()
	{
		flushAll();

		// Only delete the compressor if it was created by us (in the constructor), not by the user.
		if(m_bIsOurCompressor)
		{
			delete m_pBlockCompressor;
		}

		//delete m_pCompressedEmptyBlock;
	}

	////////////////////////////////////////////////////////////////////////////////
	/// This function should never be called. Copying volumes by value would be expensive, and we want to prevent users from doing
	/// it by accident (such as when passing them as paramenters to functions). That said, there are times when you really do want to
	/// make a copy of a volume and in this case you should look at the Volumeresampler.
	///
	/// \sa VolumeResampler
	////////////////////////////////////////////////////////////////////////////////
	template <typename VoxelType>
	LargeVolume<VoxelType>& LargeVolume<VoxelType>::operator=(const LargeVolume<VoxelType>& /*rhs*/)
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
	VoxelType LargeVolume<VoxelType>::getVoxel(int32_t uXPos, int32_t uYPos, int32_t uZPos, VoxelType tBorder) const
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
	VoxelType LargeVolume<VoxelType>::getVoxel(const Vector3DInt32& v3dPos, VoxelType tBorder) const
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
	VoxelType LargeVolume<VoxelType>::getVoxel(int32_t uXPos, int32_t uYPos, int32_t uZPos, WrapMode eWrapMode, VoxelType tBorder) const
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
	VoxelType LargeVolume<VoxelType>::getVoxel(const Vector3DInt32& v3dPos, WrapMode eWrapMode, VoxelType tBorder) const
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
	VoxelType LargeVolume<VoxelType>::getVoxelAt(int32_t uXPos, int32_t uYPos, int32_t uZPos) const
	{
		if(this->m_regValidRegion.containsPoint(Vector3DInt32(uXPos, uYPos, uZPos)))
		{
			const int32_t blockX = uXPos >> m_uBlockSideLengthPower;
			const int32_t blockY = uYPos >> m_uBlockSideLengthPower;
			const int32_t blockZ = uZPos >> m_uBlockSideLengthPower;

			const uint16_t xOffset = static_cast<uint16_t>(uXPos - (blockX << m_uBlockSideLengthPower));
			const uint16_t yOffset = static_cast<uint16_t>(uYPos - (blockY << m_uBlockSideLengthPower));
			const uint16_t zOffset = static_cast<uint16_t>(uZPos - (blockZ << m_uBlockSideLengthPower));

			const UncompressedBlock<VoxelType>* pUncompressedBlock = getUncompressedBlock(blockX, blockY, blockZ);

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
	VoxelType LargeVolume<VoxelType>::getVoxelAt(const Vector3DInt32& v3dPos) const
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
	void LargeVolume<VoxelType>::setMaxNumberOfUncompressedBlocks(uint32_t uMaxNumberOfUncompressedBlocks)
	{
		clearBlockCache();

		m_uMaxNumberOfUncompressedBlocks = uMaxNumberOfUncompressedBlocks;

		uint32_t uUncompressedBlockSizeInBytes = m_uBlockSideLength * m_uBlockSideLength * m_uBlockSideLength * sizeof(VoxelType);
		logDebug() << "The maximum number of uncompresed blocks has been set to " << m_uMaxNumberOfUncompressedBlocks
			<< ", which is " << m_uMaxNumberOfUncompressedBlocks * uUncompressedBlockSizeInBytes << " bytes"; 
	}

	////////////////////////////////////////////////////////////////////////////////
	/// Increasing the number of blocks in memory causes less paging to occur
	/// \param uMaxNumberOfBlocksInMemory The number of blocks
	////////////////////////////////////////////////////////////////////////////////
	template <typename VoxelType>
	void LargeVolume<VoxelType>::setMaxNumberOfBlocksInMemory(uint32_t uMaxNumberOfBlocksInMemory)
	{
		if(m_pBlocks.size() > uMaxNumberOfBlocksInMemory)
		{
			flushAll();
		}

		m_uMaxNumberOfBlocksInMemory  = uMaxNumberOfBlocksInMemory;
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
	void LargeVolume<VoxelType>::setVoxel(int32_t uXPos, int32_t uYPos, int32_t uZPos, VoxelType tValue, WrapMode eWrapMode)
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

		UncompressedBlock<VoxelType>* pUncompressedBlock = getUncompressedBlock(blockX, blockY, blockZ);
		pUncompressedBlock->setVoxelAt(xOffset, yOffset, zOffset, tValue);
	}

	////////////////////////////////////////////////////////////////////////////////
	/// \param v3dPos the 3D position of the voxel
	/// \param tValue the value to which the voxel will be set
	/// \param eWrapMode Specifies the behaviour when the requested position is outside of the volume.
	/// This must be set to 'None' or 'DontCheck'. Other wrap modes cannot be used when writing to volume data.
	////////////////////////////////////////////////////////////////////////////////
	template <typename VoxelType>
	void LargeVolume<VoxelType>::setVoxel(const Vector3DInt32& v3dPos, VoxelType tValue, WrapMode eWrapMode)
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
	bool LargeVolume<VoxelType>::setVoxelAt(int32_t uXPos, int32_t uYPos, int32_t uZPos, VoxelType tValue)
	{
		// PolyVox does not throw an exception when a voxel is out of range. Please see 'Error Handling' in the User Manual.
		POLYVOX_ASSERT(this->m_regValidRegion.containsPoint(Vector3DInt32(uXPos, uYPos, uZPos)), "Position is outside valid region");

		const int32_t blockX = uXPos >> m_uBlockSideLengthPower;
		const int32_t blockY = uYPos >> m_uBlockSideLengthPower;
		const int32_t blockZ = uZPos >> m_uBlockSideLengthPower;

		const uint16_t xOffset = static_cast<uint16_t>(uXPos - (blockX << m_uBlockSideLengthPower));
		const uint16_t yOffset = static_cast<uint16_t>(uYPos - (blockY << m_uBlockSideLengthPower));
		const uint16_t zOffset = static_cast<uint16_t>(uZPos - (blockZ << m_uBlockSideLengthPower));

		UncompressedBlock<VoxelType>* pUncompressedBlock = getUncompressedBlock(blockX, blockY, blockZ);

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
	bool LargeVolume<VoxelType>::setVoxelAt(const Vector3DInt32& v3dPos, VoxelType tValue)
	{
		return setVoxelAt(v3dPos.getX(), v3dPos.getY(), v3dPos.getZ(), tValue);
	}


	////////////////////////////////////////////////////////////////////////////////
	/// Note that if *NOTE - update docs - MaxNumberOfBlocksInMemory no longer exists* MaxNumberOfBlocksInMemory is not large enough to support the region this function will only load part of the region. In this case it is undefined which parts will actually be loaded. If all the voxels in the given region are already loaded, this function will not do anything. Other voxels might be unloaded to make space for the new voxels.
	/// \param regPrefetch The Region of voxels to prefetch into memory.
	////////////////////////////////////////////////////////////////////////////////
	template <typename VoxelType>
	void LargeVolume<VoxelType>::prefetch(Region regPrefetch)
	{
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

		Vector3DInt32 v3dSize = v3dEnd - v3dStart + Vector3DInt32(1,1,1);
		uint32_t numblocks = static_cast<uint32_t>(v3dSize.getX() * v3dSize.getY() * v3dSize.getZ());

		// FIXME - reinstate some logic to handle when the prefetched region is too large.

		/*if(numblocks > m_uMaxNumberOfBlocksInMemory)
		{
			// cannot support the amount of blocks... so only load the maximum possible
			numblocks = m_uMaxNumberOfBlocksInMemory;
		}*/
		for(int32_t x = v3dStart.getX(); x <= v3dEnd.getX(); x++)
		{
			for(int32_t y = v3dStart.getY(); y <= v3dEnd.getY(); y++)
			{
				for(int32_t z = v3dStart.getZ(); z <= v3dEnd.getZ(); z++)
				{
					Vector3DInt32 pos(x,y,z);
					typename CompressedBlockMap::iterator itBlock = m_pBlocks.find(pos);
					
					if(itBlock != m_pBlocks.end())
					{
						// If the block is already loaded then we don't load it again. This means it does not get uncompressed,
						// whereas if we were to call getUncompressedBlock() regardless then it would also get uncompressed.
						// This might be nice, but on the prefetch region could be bigger than the uncompressed cache size.
						// This would limit the amount of prefetching we could do.
						continue;
					}

					if(numblocks == 0)
					{
						// Loading any more blocks would attempt to overflow the memory and therefore erase blocks
						// we loaded in the beginning. This wouldn't cause logic problems but would be wasteful.
						return;
					}
					// load a block
					numblocks--;
					getUncompressedBlock(x,y,z);
				} // for z
			} // for y
		} // for x
	}

	////////////////////////////////////////////////////////////////////////////////
	/// Removes all voxels from memory, and calls dataOverflowHandler() to ensure the application has a chance to store the data.
	////////////////////////////////////////////////////////////////////////////////
	template <typename VoxelType>
	void LargeVolume<VoxelType>::flushAll()
	{
		typename CompressedBlockMap::iterator i;
		//Replaced the for loop here as the call to
		//eraseBlock was invalidating the iterator.
		while(m_pBlocks.size() > 0)
		{
			eraseBlock(m_pBlocks.begin());
		}
	}

	////////////////////////////////////////////////////////////////////////////////
	/// Removes all voxels in the specified Region from memory, and calls dataOverflowHandler() to ensure the application has a chance to store the data. It is possible that there are no voxels loaded in the Region, in which case the function will have no effect.
	////////////////////////////////////////////////////////////////////////////////
	template <typename VoxelType>
	void LargeVolume<VoxelType>::flush(Region regFlush)
	{
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

		for(int32_t x = v3dStart.getX(); x <= v3dEnd.getX(); x++)
		{
			for(int32_t y = v3dStart.getY(); y <= v3dEnd.getY(); y++)
			{
				for(int32_t z = v3dStart.getZ(); z <= v3dEnd.getZ(); z++)
				{
					Vector3DInt32 pos(x,y,z);
					typename CompressedBlockMap::iterator itBlock = m_pBlocks.find(pos);
					if(itBlock == m_pBlocks.end())
					{
						// not loaded, not unloading
						continue;
					}
					eraseBlock(itBlock);
					// eraseBlock might cause a call to getUncompressedBlock, which again sets m_pLastAccessedBlock
					if(m_v3dLastAccessedBlockPos == pos)
					{
						m_pLastAccessedBlock = 0;
					}
				} // for z
			} // for y
		} // for x
	}

	////////////////////////////////////////////////////////////////////////////////
	///
	////////////////////////////////////////////////////////////////////////////////
	template <typename VoxelType>
	void LargeVolume<VoxelType>::clearBlockCache(void)
	{
	}

	////////////////////////////////////////////////////////////////////////////////
	/// This function should probably be made internal...
	////////////////////////////////////////////////////////////////////////////////
	template <typename VoxelType>
	void LargeVolume<VoxelType>::initialise()
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

		if(!m_pBlockCompressor)
		{
			POLYVOX_THROW(std::invalid_argument, "You must provide a valid compressor for the LargeVolume to use.");
		}

		m_uTimestamper = 0;
		//m_uMaxNumberOfUncompressedBlocks = 16;
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

		//setMaxNumberOfUncompressedBlocks(m_uMaxNumberOfUncompressedBlocks);

		//Clear the previous data
		m_pBlocks.clear();

		//Clear the previous data
		m_pBlocks.clear();

		//Other properties we might find useful later
		this->m_uLongestSideLength = (std::max)((std::max)(this->getWidth(),this->getHeight()),this->getDepth());
		this->m_uShortestSideLength = (std::min)((std::min)(this->getWidth(),this->getHeight()),this->getDepth());
		this->m_fDiagonalLength = sqrtf(static_cast<float>(this->getWidth() * this->getWidth() + this->getHeight() * this->getHeight() + this->getDepth() * this->getDepth()));
	}

	template <typename VoxelType>
	void LargeVolume<VoxelType>::eraseBlock(typename CompressedBlockMap::iterator itCompressedBlock) const
	{
		CompressedBlock<VoxelType>* pCompressedBlock = itCompressedBlock->second;

		// Before deleting the block we may need to page out its data. We
		// only do this if the data has been modified since it was paged in.
		if(pCompressedBlock->m_bDataModified)
		{
			// The position of the block within the volume.
			Vector3DInt32 v3dBlockPos = itCompressedBlock->first;

			// From the coordinates of the block we deduce the coordinates of the contained voxels.
			Vector3DInt32 v3dLower(v3dBlockPos.getX() << m_uBlockSideLengthPower, v3dBlockPos.getY() << m_uBlockSideLengthPower, v3dBlockPos.getZ() << m_uBlockSideLengthPower);
			Vector3DInt32 v3dUpper = v3dLower + Vector3DInt32(m_uBlockSideLength-1, m_uBlockSideLength-1, m_uBlockSideLength-1);

			// Page the data out
			m_pPager->pageOut(Region(v3dLower, v3dUpper), pCompressedBlock);

			// The compressed data is no longer modified with respect to the data on disk
			pCompressedBlock->m_bDataModified = false;
		}

		delete itCompressedBlock->second;

		// We can now remove the block data from memory.
		m_pBlocks.erase(itCompressedBlock);
	}

	template <typename VoxelType>
	void LargeVolume<VoxelType>::eraseBlock(typename UncompressedBlockMap::iterator itUncompressedBlock) const
	{
		UncompressedBlock<VoxelType>* pUncompressedBlock = itUncompressedBlock->second;

		// This should not often happen as blocks are normally deleted based on being least recently used.
		// However, I can imagine that flushing a large number of blocks could cause this to occur. Just
		// to be safe we handle it by invalidating the last accessed block pointer.
		if(pUncompressedBlock == m_pLastAccessedBlock)
		{
			logWarning() << "The last accessed block is being erased from the uncompressed cache.";
			m_pLastAccessedBlock = 0;
		}

		// Before deleting the block we may need to recompress its data. We
		// only do this if the data has been modified since it was decompressed.
		if(pUncompressedBlock->m_bDataModified)
		{
			// Get the compressed block which we will copy the data back in to.
			Vector3DInt32 v3dBlockPos = itUncompressedBlock->first;
			CompressedBlock<VoxelType>* pCompressedBlock = getCompressedBlock(v3dBlockPos.getX(), v3dBlockPos.getY(), v3dBlockPos.getZ());

			m_pBlockCompressor->compress(pUncompressedBlock, pCompressedBlock);

			// The compressed data has been updated, so the uncompressed data is no longer modified with respect to it.
			pUncompressedBlock->m_bDataModified = false;
		}

		delete itUncompressedBlock->second;

		// We can now remove the block data from memory.
		m_pUncompressedBlockCache.erase(itUncompressedBlock);
	}

	template <typename VoxelType>
	CompressedBlock<VoxelType>* LargeVolume<VoxelType>::getCompressedBlock(int32_t uBlockX, int32_t uBlockY, int32_t uBlockZ) const
	{
		Vector3DInt32 v3dBlockPos(uBlockX, uBlockY, uBlockZ);

		CompressedBlock<VoxelType>* pBlock = 0;

		typename CompressedBlockMap::iterator itBlock = m_pBlocks.find(v3dBlockPos);
		if(itBlock != m_pBlocks.end())
		{
			pBlock = itBlock->second;
			pBlock->m_uBlockLastAccessed = ++m_uTimestamper;
			return pBlock;
		}
		else
		{
			// The block wasn't found so we create a new one
			pBlock = new CompressedBlock<VoxelType>;

			// It's important to set the timestamp before we flush later.
			pBlock->m_uBlockLastAccessed = ++m_uTimestamper;

			// Pass the block to the Pager to give it a chance to initialise it with any data
			Vector3DInt32 v3dLower(v3dBlockPos.getX() << m_uBlockSideLengthPower, v3dBlockPos.getY() << m_uBlockSideLengthPower, v3dBlockPos.getZ() << m_uBlockSideLengthPower);
			Vector3DInt32 v3dUpper = v3dLower + Vector3DInt32(m_uBlockSideLength-1, m_uBlockSideLength-1, m_uBlockSideLength-1);
			Region reg(v3dLower, v3dUpper);
			m_pPager->pageIn(reg, pBlock);

			// Add the block to the map
			itBlock = m_pBlocks.insert(std::make_pair(v3dBlockPos, pBlock)).first;

			// Paging in this new block may mean we are now using too much memory. If necessary, flush some old blocks.
			flushOldestExcessiveBlocks();

			return pBlock;
		}
	}

	template <typename VoxelType>
	UncompressedBlock<VoxelType>* LargeVolume<VoxelType>::getUncompressedBlock(int32_t uBlockX, int32_t uBlockY, int32_t uBlockZ) const
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

		typename UncompressedBlockMap::iterator itUncompressedBlock = m_pUncompressedBlockCache.find(v3dBlockPos);
		// check whether the block is already loaded
		if(itUncompressedBlock != m_pUncompressedBlockCache.end())
		{
			UncompressedBlock<VoxelType>* pUncompressedBlock = itUncompressedBlock->second;

			pUncompressedBlock->m_uBlockLastAccessed = ++m_uTimestamper;
			m_pLastAccessedBlock = pUncompressedBlock;
			m_v3dLastAccessedBlockPos = v3dBlockPos;

			return pUncompressedBlock;			
		}
		else
		{
			// At this point we just create a new block.
			UncompressedBlock<VoxelType>* pUncompressedBlock = new UncompressedBlock<VoxelType>(m_uBlockSideLength);
			
			// It's important to set the timestamp before we flush later.
			pUncompressedBlock->m_uBlockLastAccessed = ++m_uTimestamper;

			// We set these before flushing because the flush can cause block to be erased, and there
			// is a test to make sure the block which is being erase is not the last accessed block.
			m_pLastAccessedBlock = pUncompressedBlock;
			m_v3dLastAccessedBlockPos = v3dBlockPos;

			// An uncompressed bock is always backed by a compressed one, and this is created by getCompressedBlock() if it doesn't 
			// already exist. If it does already exist and has data then we bring this across into the ucompressed version.
			if(getCompressedBlock(uBlockX, uBlockY, uBlockZ)->getData() != 0)
			{
				// FIXME - multiple getCompressedBlock() calls (including the one above)
				CompressedBlock<VoxelType>* pBlock = getCompressedBlock(uBlockX, uBlockY, uBlockZ);

				m_pBlockCompressor->decompress(pBlock, pUncompressedBlock);
			}
			
			// Add our new block to the map.
			m_pUncompressedBlockCache.insert(std::make_pair(v3dBlockPos, pUncompressedBlock));			

			// Our block cache may now have grown too large. Flush some entries if necessary.
			// FIXME - Watch out for flushing the block we just created!
			flushExcessiveCacheEntries();

			return pUncompressedBlock;
		}
	}

	////////////////////////////////////////////////////////////////////////////////
	/// Note: This function needs reviewing for accuracy...
	////////////////////////////////////////////////////////////////////////////////
	template <typename VoxelType>
	float LargeVolume<VoxelType>::calculateCompressionRatio(void)
	{
		float fRawSize = static_cast<float>(m_pBlocks.size() * m_uBlockSideLength * m_uBlockSideLength* m_uBlockSideLength * sizeof(VoxelType));
		float fCompressedSize = static_cast<float>(calculateSizeInBytes());
		return fCompressedSize/fRawSize;
	}

	////////////////////////////////////////////////////////////////////////////////
	/// Note: This function needs reviewing for accuracy...
	////////////////////////////////////////////////////////////////////////////////
	template <typename VoxelType>
	uint32_t LargeVolume<VoxelType>::calculateSizeInBytes(void)
	{
		uint32_t uSizeInBytes = sizeof(LargeVolume);

		//Memory used by the blocks
		typename CompressedBlockMap::iterator i;
		for(i = m_pBlocks.begin(); i != m_pBlocks.end(); i++)
		{
			//Inaccurate - account for rest of loaded block.
			uSizeInBytes += i->second->calculateSizeInBytes();
		}

		//Memory used by the block cache.
		//uSizeInBytes += m_vecBlocksWithUncompressedData.capacity() * sizeof(Block<VoxelType>);
		//uSizeInBytes += m_vecBlocksWithUncompressedData.size() * m_uBlockSideLength * m_uBlockSideLength * m_uBlockSideLength * sizeof(VoxelType);

		return uSizeInBytes;
	}

	template <typename VoxelType>
	uint32_t LargeVolume<VoxelType>::calculateBlockMemoryUsage(void) const
	{
		uint32_t uMemoryUsage = 0;

		typename CompressedBlockMap::iterator i;
		for(i = m_pBlocks.begin(); i != m_pBlocks.end(); i++)
		{
			//Inaccurate - account for rest of loaded block.
			uMemoryUsage += i->second->calculateSizeInBytes();
		}

		return uMemoryUsage;
	}

	template <typename VoxelType>
	void LargeVolume<VoxelType>::flushOldestExcessiveBlocks(void) const
	{
		while(m_pBlocks.size() > m_uMaxNumberOfBlocksInMemory) 
		{
			// Find the least recently used block. This is somewhat inefficient as it searches through
			// the map, so if it proves to be a bottleneck we may want some kind of sorted structure.
			typename CompressedBlockMap::iterator i;
			typename CompressedBlockMap::iterator itUnloadBlock = m_pBlocks.begin();
			for(i = m_pBlocks.begin(); i != m_pBlocks.end(); i++)
			{
				if(i->second->m_uBlockLastAccessed < itUnloadBlock->second->m_uBlockLastAccessed)
				{
					itUnloadBlock = i;
				}
			}

			// Erase the least recently used block
			eraseBlock(itUnloadBlock);
		}
	}

	template <typename VoxelType>
	void LargeVolume<VoxelType>::flushExcessiveCacheEntries(void) const
	{
		while(m_pUncompressedBlockCache.size() > m_uMaxNumberOfUncompressedBlocks) 
		{
			// Find the least recently used block. The uncompressed block cache should be
			// much smaller than the total number of blocks, so hopefully this isn't too slow.
			typename UncompressedBlockMap::iterator i;
			typename UncompressedBlockMap::iterator itUnloadBlock = m_pUncompressedBlockCache.begin();
			for(i = m_pUncompressedBlockCache.begin(); i != m_pUncompressedBlockCache.end(); i++)
			{
				if(i->second->m_uBlockLastAccessed < itUnloadBlock->second->m_uBlockLastAccessed)
				{
					itUnloadBlock = i;
				}
			}

			// Erase the least recently used block
			eraseBlock(itUnloadBlock);
		}
	}

	template <typename VoxelType>
	template <WrapMode eWrapMode>
	VoxelType LargeVolume<VoxelType>::getVoxelImpl(int32_t uXPos, int32_t uYPos, int32_t uZPos, WrapModeType<eWrapMode>, VoxelType tBorder) const
	{
		// This function should never be called because one of the specialisations should always match.
		POLYVOX_ASSERT(false, "This function is not implemented and should never be called!");
	}

	template <typename VoxelType>
	VoxelType LargeVolume<VoxelType>::getVoxelImpl(int32_t uXPos, int32_t uYPos, int32_t uZPos, WrapModeType<WrapModes::Validate>, VoxelType tBorder) const
	{
		if(this->m_regValidRegion.containsPoint(Vector3DInt32(uXPos, uYPos, uZPos)) == false)
		{
			POLYVOX_THROW(std::out_of_range, "Position is outside valid region");
		}

		return getVoxelImpl(uXPos, uYPos, uZPos, WrapModeType<WrapModes::AssumeValid>(), tBorder); // No wrapping as we've just validated the position.
	}

	template <typename VoxelType>
	VoxelType LargeVolume<VoxelType>::getVoxelImpl(int32_t uXPos, int32_t uYPos, int32_t uZPos, WrapModeType<WrapModes::Clamp>, VoxelType tBorder) const
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
	VoxelType LargeVolume<VoxelType>::getVoxelImpl(int32_t uXPos, int32_t uYPos, int32_t uZPos, WrapModeType<WrapModes::Border>, VoxelType tBorder) const
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
	VoxelType LargeVolume<VoxelType>::getVoxelImpl(int32_t uXPos, int32_t uYPos, int32_t uZPos, WrapModeType<WrapModes::AssumeValid>, VoxelType /*tBorder*/) const
	{
		const int32_t blockX = uXPos >> m_uBlockSideLengthPower;
		const int32_t blockY = uYPos >> m_uBlockSideLengthPower;
		const int32_t blockZ = uZPos >> m_uBlockSideLengthPower;

		const uint16_t xOffset = static_cast<uint16_t>(uXPos - (blockX << m_uBlockSideLengthPower));
		const uint16_t yOffset = static_cast<uint16_t>(uYPos - (blockY << m_uBlockSideLengthPower));
		const uint16_t zOffset = static_cast<uint16_t>(uZPos - (blockZ << m_uBlockSideLengthPower));

		UncompressedBlock<VoxelType>* pUncompressedBlock = getUncompressedBlock(blockX, blockY, blockZ);
		return pUncompressedBlock->getVoxel(xOffset, yOffset, zOffset);
	}
}


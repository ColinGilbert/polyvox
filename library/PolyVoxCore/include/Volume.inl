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

#include "ConstVolumeProxy.h"
#include "PolyVoxImpl/Block.h"
#include "Log.h"
#include "VolumeSampler.h"
#include "Region.h"
#include "Vector.h"

#include <limits>
#include <cassert>
#include <cstring> //For memcpy
#include <list>
#include <stdexcept> //For invalid_argument

namespace PolyVox
{
	////////////////////////////////////////////////////////////////////////////////
	/// Builds a volume of the desired dimensions
	/// \param uWidth The desired width in voxels. This must be a power of two.
	/// \param uHeight The desired height in voxels. This must be a power of two.
	/// \param uDepth The desired depth in voxels. This must be a power of two.
	/// \param uBlockSideLength The size of the blocks which make up the volume. Small
	/// blocks are more likely to be homogeneous (so more easily shared) and have better
	/// cache behaviour. However, there is a memory overhead per block so if they are
	/// not shared it could actually be less efficient (this will depend on the data).
	/// The size of the volume may also be a factor when choosing block size. Accept 
	/// the default if you are not sure what to choose here.
	////////////////////////////////////////////////////////////////////////////////
	template <typename VoxelType>
	Volume<VoxelType>::Volume
	(
		polyvox_function<void(const ConstVolumeProxy<VoxelType>&, const Region&)> dataRequiredHandler,
		polyvox_function<void(const ConstVolumeProxy<VoxelType>&, const Region&)> dataOverflowHandler,
		uint16_t uBlockSideLength
	)
	{
		m_funcDataRequiredHandler = dataRequiredHandler;
		m_funcDataOverflowHandler = dataOverflowHandler;
		m_bStreamingEnabled = true;
		//Create a volume of the right size.
		resize(Region::MaxRegion,uBlockSideLength);
	}

	////////////////////////////////////////////////////////////////////////////////
	/// Builds a volume of the desired dimensions
	/// \param uWidth The desired width in voxels. This must be a power of two.
	/// \param uHeight The desired height in voxels. This must be a power of two.
	/// \param uDepth The desired depth in voxels. This must be a power of two.
	/// \param uBlockSideLength The size of the blocks which make up the volume. Small
	/// blocks are more likely to be homogeneous (so more easily shared) and have better
	/// cache behaviour. However, there is a memory overhead per block so if they are
	/// not shared it could actually be less efficient (this will depend on the data).
	/// The size of the volume may also be a factor when choosing block size. Accept 
	/// the default if you are not sure what to choose here.
	////////////////////////////////////////////////////////////////////////////////
	template <typename VoxelType>
	Volume<VoxelType>::Volume
	(
		int32_t uWidth, int32_t uHeight, int32_t uDepth,
		polyvox_function<void(const ConstVolumeProxy<VoxelType>&, const Region&)> dataRequiredHandler,
		polyvox_function<void(const ConstVolumeProxy<VoxelType>&, const Region&)> dataOverflowHandler,
		bool bStreamingEnabled,
		uint16_t uBlockSideLength
	)
	{
		m_funcDataRequiredHandler = dataRequiredHandler;
		m_funcDataOverflowHandler = dataOverflowHandler;
		m_bStreamingEnabled = bStreamingEnabled;

		Region regValid(Vector3DInt32(0,0,0), Vector3DInt32(uWidth - 1,uHeight - 1,uDepth - 1));
		resize(Region(Vector3DInt32(0,0,0), Vector3DInt32(uWidth - 1,uHeight - 1,uDepth - 1)), uBlockSideLength);
	}

	////////////////////////////////////////////////////////////////////////////////
	/// Builds a volume of the desired dimensions
	/// \param uWidth The desired width in voxels. This must be a power of two.
	/// \param uHeight The desired height in voxels. This must be a power of two.
	/// \param uDepth The desired depth in voxels. This must be a power of two.
	/// \param uBlockSideLength The size of the blocks which make up the volume. Small
	/// blocks are more likely to be homogeneous (so more easily shared) and have better
	/// cache behaviour. However, there is a memory overhead per block so if they are
	/// not shared it could actually be less efficient (this will depend on the data).
	/// The size of the volume may also be a factor when choosing block size. Accept 
	/// the default if you are not sure what to choose here.
	////////////////////////////////////////////////////////////////////////////////
	template <typename VoxelType>
	Volume<VoxelType>::Volume
	(
		const Region& regValid,
		polyvox_function<void(const ConstVolumeProxy<VoxelType>&, const Region&)> dataRequiredHandler,
		polyvox_function<void(const ConstVolumeProxy<VoxelType>&, const Region&)> dataOverflowHandler,
		bool bStreamingEnabled,
		uint16_t uBlockSideLength
	)
	{
		m_funcDataRequiredHandler = dataRequiredHandler;
		m_funcDataOverflowHandler = dataOverflowHandler;
		m_bStreamingEnabled = bStreamingEnabled;

		//Create a volume of the right size.
		resize(regValid,uBlockSideLength);
	}

	////////////////////////////////////////////////////////////////////////////////
	/// Destroys the volume and frees any blocks which are not in use by other volumes.
	////////////////////////////////////////////////////////////////////////////////
	template <typename VoxelType>
	Volume<VoxelType>::~Volume()
	{
		std::map<Vector3DInt32, LoadedBlock >::iterator i;
		for(i = m_pBlocks.begin(); i != m_pBlocks.end(); i = m_pBlocks.begin()) {
			eraseBlock(i);
		}
	}

	////////////////////////////////////////////////////////////////////////////////
	/// The border value is returned whenever an atempt is made to read a voxel which
	/// is outside the extents of the volume.
	/// \return The value used for voxels outside of the volume
	////////////////////////////////////////////////////////////////////////////////
	template <typename VoxelType>
	VoxelType Volume<VoxelType>::getBorderValue(void) const
	{
		return *m_pUncompressedBorderData;
	}

	////////////////////////////////////////////////////////////////////////////////
	/// The result will always have a lower corner at (0,0,0) and an upper corner at one
	/// less than the side length. For example, if a volume has dimensions 256x512x1024
	/// then the upper corner of the enclosing region will be at (255,511,1023).
	/// \return A Region representing the extent of the volume.
	////////////////////////////////////////////////////////////////////////////////
	template <typename VoxelType>
	Region Volume<VoxelType>::getEnclosingRegion(void) const
	{
		return m_regValidRegion;
	}

	////////////////////////////////////////////////////////////////////////////////
	/// \return The width of the volume in voxels
	/// \sa getHeight(), getDepth()
	////////////////////////////////////////////////////////////////////////////////
	template <typename VoxelType>
	int32_t Volume<VoxelType>::getWidth(void) const
	{
		return m_regValidRegion.getUpperCorner().getX() - m_regValidRegion.getLowerCorner().getX() + 1;
	}

	////////////////////////////////////////////////////////////////////////////////
	/// \return The height of the volume in voxels
	/// \sa getWidth(), getDepth()
	////////////////////////////////////////////////////////////////////////////////
	template <typename VoxelType>
	int32_t Volume<VoxelType>::getHeight(void) const
	{
		return m_regValidRegion.getUpperCorner().getY() - m_regValidRegion.getLowerCorner().getY() + 1;
	}

	////////////////////////////////////////////////////////////////////////////////
	/// \return The depth of the volume in voxels
	/// \sa getWidth(), getHeight()
	////////////////////////////////////////////////////////////////////////////////
	template <typename VoxelType>
	int32_t Volume<VoxelType>::getDepth(void) const
	{
		return m_regValidRegion.getUpperCorner().getZ() - m_regValidRegion.getLowerCorner().getZ() + 1;
	}

	////////////////////////////////////////////////////////////////////////////////
	/// \return The length of the shortest side in voxels. For example, if a volume has
	/// dimensions 256x512x1024 this function will return 256.
	/// \sa getLongestSideLength(), getDiagonalLength()
	////////////////////////////////////////////////////////////////////////////////
	template <typename VoxelType>
	int32_t Volume<VoxelType>::getShortestSideLength(void) const
	{
		return m_uShortestSideLength;
	}

	////////////////////////////////////////////////////////////////////////////////
	/// \return The length of the longest side in voxels. For example, if a volume has
	/// dimensions 256x512x1024 this function will return 1024.
	/// \sa getShortestSideLength(), getDiagonalLength()
	////////////////////////////////////////////////////////////////////////////////
	template <typename VoxelType>
	int32_t Volume<VoxelType>::getLongestSideLength(void) const
	{
		return m_uLongestSideLength;
	}	

	////////////////////////////////////////////////////////////////////////////////
	/// \return The length of the diagonal in voxels. For example, if a volume has
	/// dimensions 256x512x1024 this function will return sqrt(256*256+512*512+1024*1024)
	/// = 1173.139. This value is computed on volume creation so retrieving it is fast.
	/// \sa getShortestSideLength(), getLongestSideLength()
	////////////////////////////////////////////////////////////////////////////////
	template <typename VoxelType>
	float Volume<VoxelType>::getDiagonalLength(void) const
	{
		return m_fDiagonalLength;
	}

	////////////////////////////////////////////////////////////////////////////////
	/// \param uXPos the \c x position of the voxel
	/// \param uYPos the \c y position of the voxel
	/// \param uZPos the \c z position of the voxel
	/// \return the voxel value
	////////////////////////////////////////////////////////////////////////////////
	template <typename VoxelType>
	VoxelType Volume<VoxelType>::getVoxelAt(int32_t uXPos, int32_t uYPos, int32_t uZPos) const
	{
		if(m_regValidRegion.containsPoint(Vector3DInt32(uXPos, uYPos, uZPos)))
		{
			const int32_t blockX = uXPos >> m_uBlockSideLengthPower;
			const int32_t blockY = uYPos >> m_uBlockSideLengthPower;
			const int32_t blockZ = uZPos >> m_uBlockSideLengthPower;

			const uint16_t xOffset = uXPos - (blockX << m_uBlockSideLengthPower);
			const uint16_t yOffset = uYPos - (blockY << m_uBlockSideLengthPower);
			const uint16_t zOffset = uZPos - (blockZ << m_uBlockSideLengthPower);

			Block<VoxelType>* pUncompressedBlock = getUncompressedBlock(blockX, blockY, blockZ);

			return pUncompressedBlock->getVoxelAt(xOffset,yOffset,zOffset);
		}
		else
		{
			return getBorderValue();
		}
	}

	////////////////////////////////////////////////////////////////////////////////
	/// \param v3dPos the 3D position of the voxel
	/// \return the voxel value
	////////////////////////////////////////////////////////////////////////////////
	template <typename VoxelType>
	VoxelType Volume<VoxelType>::getVoxelAt(const Vector3DInt32& v3dPos) const
	{
		return getVoxelAt(v3dPos.getX(), v3dPos.getY(), v3dPos.getZ());
	}

	////////////////////////////////////////////////////////////////////////////////
	/// Increasing the size of the block cache will increase memory but may improve performance.
	/// You may want to set this to a large value (e.g. 1024) when you are first loading your
	/// volume data and then set it to a smaller value (e.g.64) for general processing.
	/// \param uBlockCacheSize The number of blocks for which uncompressed data can be cached.
	////////////////////////////////////////////////////////////////////////////////
	template <typename VoxelType>
	void Volume<VoxelType>::setMaxNumberOfUncompressedBlocks(uint16_t uMaxNumberOfUncompressedBlocks)
	{
		clearBlockCache();

		m_uMaxNumberOfUncompressedBlocks = uMaxNumberOfUncompressedBlocks;
	}

	////////////////////////////////////////////////////////////////////////////////
	/// Increasing the number of blocks in memory causes fewer calls to load/unload
	/// \param uMaxBlocks The number of blocks
	////////////////////////////////////////////////////////////////////////////////
	template <typename VoxelType>
	void Volume<VoxelType>::setMaxNumberOfBlocksInMemory(uint16_t uMaxNumberOfBlocksInMemory)
	{
		//FIXME? - I'm concerned about the logic in here... particularly the
		//way timestamps are handled. Perhaps extract all timestamps, sort them,
		//identify the cut-off point, and then discard those above it?
		if(m_pBlocks.size() > uMaxNumberOfBlocksInMemory)
		{
			// we need to unload some blocks
			for(int j = 0; j < m_pBlocks.size() - uMaxNumberOfBlocksInMemory; j++)
			{
				std::map<Vector3DInt32, LoadedBlock >::iterator i;
				std::map<Vector3DInt32, LoadedBlock >::iterator itUnloadBlock = m_pBlocks.begin();
				for(i = m_pBlocks.begin(); i != m_pBlocks.end(); i++)
				{
					if(i->second.timestamp < itUnloadBlock->second.timestamp)
					{
						itUnloadBlock = i;
					}
				}
				eraseBlock(itUnloadBlock);
			}
		}
		m_uMaxNumberOfBlocksInMemory  = uMaxNumberOfBlocksInMemory;
	}

	////////////////////////////////////////////////////////////////////////////////
	/// \param tBorder The value to use for voxels outside the volume.
	////////////////////////////////////////////////////////////////////////////////
	template <typename VoxelType>
	void Volume<VoxelType>::setBorderValue(const VoxelType& tBorder) 
	{
		/*Block<VoxelType>* pUncompressedBorderBlock = getUncompressedBlock(&m_pBorderBlock);
		return pUncompressedBorderBlock->fill(tBorder);*/
		std::fill(m_pUncompressedBorderData, m_pUncompressedBorderData + m_uBlockSideLength * m_uBlockSideLength * m_uBlockSideLength, tBorder);
	}

	////////////////////////////////////////////////////////////////////////////////
	/// \param uXPos the \c x position of the voxel
	/// \param uYPos the \c y position of the voxel
	/// \param uZPos the \c z position of the voxel
	/// \param tValue the value to which the voxel will be set
	/// \return whether the requested position is inside the volume
	////////////////////////////////////////////////////////////////////////////////
	template <typename VoxelType>
	bool Volume<VoxelType>::setVoxelAt(int32_t uXPos, int32_t uYPos, int32_t uZPos, VoxelType tValue)
	{
		assert(m_regValidRegion.containsPoint(Vector3DInt32(uXPos, uYPos, uZPos)));

		const int32_t blockX = uXPos >> m_uBlockSideLengthPower;
		const int32_t blockY = uYPos >> m_uBlockSideLengthPower;
		const int32_t blockZ = uZPos >> m_uBlockSideLengthPower;

		const uint16_t xOffset = uXPos - (blockX << m_uBlockSideLengthPower);
		const uint16_t yOffset = uYPos - (blockY << m_uBlockSideLengthPower);
		const uint16_t zOffset = uZPos - (blockZ << m_uBlockSideLengthPower);

		Block<VoxelType>* pUncompressedBlock = getUncompressedBlock(blockX, blockY, blockZ);

		pUncompressedBlock->setVoxelAt(xOffset,yOffset,zOffset, tValue);

		//Return true to indicate that we modified a voxel.
		return true;
	}

	////////////////////////////////////////////////////////////////////////////////
	/// \param v3dPos the 3D position of the voxel
	/// \param tValue the value to which the voxel will be set
	/// \return whether the requested position is inside the volume
	////////////////////////////////////////////////////////////////////////////////
	template <typename VoxelType>
	bool Volume<VoxelType>::setVoxelAt(const Vector3DInt32& v3dPos, VoxelType tValue)
	{
		return setVoxelAt(v3dPos.getX(), v3dPos.getY(), v3dPos.getZ(), tValue);
	}

	template <typename VoxelType>
	void Volume<VoxelType>::clearBlockCache(void)
	{
		for(uint32_t ct = 0; ct < m_vecUncompressedBlockCache.size(); ct++)
		{
			m_vecUncompressedBlockCache[ct]->block.compress();
		}
		m_vecUncompressedBlockCache.clear();
	}

	////////////////////////////////////////////////////////////////////////////////
	/// Note: Calling this function will destroy all existing data in the volume.
	/// \param uWidth The desired width in voxels. This must be a power of two.
	/// \param uHeight The desired height in voxels. This must be a power of two.
	/// \param uDepth The desired depth in voxels. This must be a power of two.
	/// \param uBlockSideLength The size of the blocks which make up the volume. Small
	/// blocks are more likely to be homogeneous (so more easily shared) and have better
	/// cache behaviour. However, there is a memory overhead per block so if they are
	/// not shared it could actually be less efficient (this will depend on the data).
	/// The size of the volume may also be a factor when choosing block size. Accept 
	/// the default if you are not sure what to choose here.
	////////////////////////////////////////////////////////////////////////////////
	template <typename VoxelType>
	void Volume<VoxelType>::resize(const Region& regValidRegion, uint16_t uBlockSideLength)
	{
		//Debug mode validation
		assert(uBlockSideLength > 0);
		
		//Release mode validation
		if(uBlockSideLength == 0)
		{
			throw std::invalid_argument("Block side length cannot be zero.");
		}
		if(!isPowerOf2(uBlockSideLength))
		{
			throw std::invalid_argument("Block side length must be a power of two.");
		}

		m_uTimestamper = 0;
		m_uMaxNumberOfUncompressedBlocks = 16;
		m_uBlockSideLength = uBlockSideLength;
		m_pUncompressedBorderData = 0;
		m_uMaxNumberOfBlocksInMemory = 1024;
		m_v3dLastAccessedBlockPos = Vector3DInt32((std::numeric_limits<int32_t>::max)(), (std::numeric_limits<int32_t>::max)(), (std::numeric_limits<int32_t>::max)()); //An invalid index
		m_pLastAccessedBlock = 0;

		m_regValidRegion = regValidRegion;

		m_regValidRegionInBlocks.setLowerCorner(m_regValidRegion.getLowerCorner()  / static_cast<int32_t>(uBlockSideLength));
		m_regValidRegionInBlocks.setUpperCorner(m_regValidRegion.getUpperCorner()  / static_cast<int32_t>(uBlockSideLength));

		setMaxNumberOfUncompressedBlocks(m_uMaxNumberOfUncompressedBlocks);

		//Clear the previous data
		m_pBlocks.clear();

		//Compute the block side length
		m_uBlockSideLength = uBlockSideLength;
		m_uBlockSideLengthPower = logBase2(m_uBlockSideLength);

		//Clear the previous data
		m_pBlocks.clear();

		//Create the border block
		m_pUncompressedBorderData = new VoxelType[m_uBlockSideLength * m_uBlockSideLength * m_uBlockSideLength];
		std::fill(m_pUncompressedBorderData, m_pUncompressedBorderData + m_uBlockSideLength * m_uBlockSideLength * m_uBlockSideLength, VoxelType());

		//Other properties we might find useful later
		m_uLongestSideLength = (std::max)((std::max)(getWidth(),getHeight()),getDepth());
		m_uShortestSideLength = (std::min)((std::min)(getWidth(),getHeight()),getDepth());
		m_fDiagonalLength = sqrtf(static_cast<float>(getWidth() * getWidth() + getHeight() * getHeight() + getDepth() * getDepth()));
	}

	template <typename VoxelType>
	void Volume<VoxelType>::eraseBlock(typename std::map<Vector3DInt32, LoadedBlock >::iterator itBlock) const
	{
		if(m_funcDataOverflowHandler)
		{
			Vector3DInt32 v3dPos = itBlock->first;
			Vector3DInt32 v3dLower(v3dPos.getX() << m_uBlockSideLengthPower, v3dPos.getY() << m_uBlockSideLengthPower, v3dPos.getZ() << m_uBlockSideLengthPower);
			Vector3DInt32 v3dUpper = v3dLower + Vector3DInt32(m_uBlockSideLength-1, m_uBlockSideLength-1, m_uBlockSideLength-1);

			Region reg(v3dLower, v3dUpper);
			ConstVolumeProxy<VoxelType> ConstVolumeProxy(*this, reg);

			m_funcDataOverflowHandler(ConstVolumeProxy, reg);
		}
		m_pBlocks.erase(itBlock);
	}

	template <typename VoxelType>
	bool Volume<VoxelType>::setVoxelAtConst(int32_t uXPos, int32_t uYPos, int32_t uZPos, VoxelType tValue) const
	{
		//We don't have any range checks in this function because it
		//is a private function only called by the ConstVolumeProxy. The
		//ConstVolumeProxy takes care of ensuring the range is appropriate.

		const int32_t blockX = uXPos >> m_uBlockSideLengthPower;
		const int32_t blockY = uYPos >> m_uBlockSideLengthPower;
		const int32_t blockZ = uZPos >> m_uBlockSideLengthPower;

		const uint16_t xOffset = uXPos - (blockX << m_uBlockSideLengthPower);
		const uint16_t yOffset = uYPos - (blockY << m_uBlockSideLengthPower);
		const uint16_t zOffset = uZPos - (blockZ << m_uBlockSideLengthPower);

		Block<VoxelType>* pUncompressedBlock = getUncompressedBlock(blockX, blockY, blockZ);

		pUncompressedBlock->setVoxelAt(xOffset,yOffset,zOffset, tValue);

		//Return true to indicate that we modified a voxel.
		return true;
	}


	template <typename VoxelType>
	Block<VoxelType>* Volume<VoxelType>::getUncompressedBlock(int32_t uBlockX, int32_t uBlockY, int32_t uBlockZ) const
	{
		Vector3DInt32 v3dBlockPos(uBlockX, uBlockY, uBlockZ);

		//Check if we have the same block as last time, if so there's no need to even update
		//the time stamp. If we updated it everytime then that would be every time we touched
		//a voxel, which would overflow a uint32_t and require us to use a uint64_t instead.
		//This check should also provide a significant speed boost as usually it is true.
		if((v3dBlockPos == m_v3dLastAccessedBlockPos) && (m_pLastAccessedBlock != 0))
		{
			assert(m_pLastAccessedBlock->m_tUncompressedData);
			return m_pLastAccessedBlock;
		}		

		std::map<Vector3DInt32, LoadedBlock >::iterator itBlock = m_pBlocks.find(v3dBlockPos);
		// check whether the block is already loaded
		if(itBlock == m_pBlocks.end())
		{
			//The block is not in the map, so we will have to create a new block and add it.
			//Before we do so, we might want to dump some existing data to make space. We 
			//Only do this if paging is enabled.
			if(m_bStreamingEnabled)
			{
				// check wether another block needs to be unloaded before this one can be loaded
				if(m_pBlocks.size() == m_uMaxNumberOfBlocksInMemory)
				{
					// find the least recently used block
					std::map<Vector3DInt32, LoadedBlock >::iterator i;
					std::map<Vector3DInt32, LoadedBlock >::iterator itUnloadBlock = m_pBlocks.begin();
					for(i = m_pBlocks.begin(); i != m_pBlocks.end(); i++)
					{
						if(i->second.timestamp < itUnloadBlock->second.timestamp)
						{
							itUnloadBlock = i;
						}
					}
					eraseBlock(itUnloadBlock);
				}
			}
			
			// create the new block
			LoadedBlock newBlock(m_uBlockSideLength);
			itBlock = m_pBlocks.insert(std::make_pair(v3dBlockPos, newBlock)).first;

			//We have created the new block. If paging is enabled it should be used to
			//fill in the required data. Otherwise it is just left in the default state.
			if(m_bStreamingEnabled)
			{
				if(m_funcDataRequiredHandler)
				{
					// "load" will actually call setVoxel, which will in turn call this function again but the block will be found
					// so this if(itBlock == m_pBlocks.end()) never is entered		
					//FIXME - can we pass the block around so that we don't have to find  it again when we recursively call this function?
					Vector3DInt32 v3dLower(v3dBlockPos.getX() << m_uBlockSideLengthPower, v3dBlockPos.getY() << m_uBlockSideLengthPower, v3dBlockPos.getZ() << m_uBlockSideLengthPower);
					Vector3DInt32 v3dUpper = v3dLower + Vector3DInt32(m_uBlockSideLength-1, m_uBlockSideLength-1, m_uBlockSideLength-1);
					Region reg(v3dLower, v3dUpper);
					ConstVolumeProxy<VoxelType> ConstVolumeProxy(*this, reg);
					m_funcDataRequiredHandler(ConstVolumeProxy, reg);
				}
			}
		}		

		//Get the block and mark that we accessed it
		LoadedBlock& loadedBlock = itBlock->second;
		loadedBlock.timestamp = ++m_uTimestamper;
		m_v3dLastAccessedBlockPos = v3dBlockPos;
		m_pLastAccessedBlock = &(loadedBlock.block);

		if(loadedBlock.block.m_bIsCompressed == false)
		{ 			
			assert(m_pLastAccessedBlock->m_tUncompressedData);
			return m_pLastAccessedBlock;
		}

		//Currently we find the oldest block by iterating over the whole array. Of course we could store the blocks sorted by
		//timestamp (set, priority_queue, etc) but then we'll need to move them around as the timestamp changes. Can come back 
		//to this if it proves to be a bottleneck (compraed to the cost of actually doing the compression/decompression).
		uint32_t uUncompressedBlockIndex = (std::numeric_limits<uint32_t>::max)();
		assert(m_vecUncompressedBlockCache.size() <= m_uMaxNumberOfUncompressedBlocks);
		if(m_vecUncompressedBlockCache.size() == m_uMaxNumberOfUncompressedBlocks)
		{
			int32_t leastRecentlyUsedBlockIndex = -1;
			uint32_t uLeastRecentTimestamp = (std::numeric_limits<uint32_t>::max)(); // you said not int64 ;)
			for(uint32_t ct = 0; ct < m_vecUncompressedBlockCache.size(); ct++)
			{
				if(m_vecUncompressedBlockCache[ct]->timestamp < uLeastRecentTimestamp)
				{
					uLeastRecentTimestamp = m_vecUncompressedBlockCache[ct]->timestamp;
					leastRecentlyUsedBlockIndex = ct;
				}
			}

			uUncompressedBlockIndex = leastRecentlyUsedBlockIndex;
			m_vecUncompressedBlockCache[leastRecentlyUsedBlockIndex]->block.compress();
			m_vecUncompressedBlockCache[leastRecentlyUsedBlockIndex] = &loadedBlock;
			//m_vecUncompressedBlockCache[leastRecentlyUsedBlockIndex]->block.m_tUncompressedData = 0;
		}
		else
		{
			//loadedBlock.block.m_tUncompressedData = new VoxelType[m_uBlockSideLength * m_uBlockSideLength * m_uBlockSideLength];
			m_vecUncompressedBlockCache.push_back(&loadedBlock);
			uUncompressedBlockIndex = m_vecUncompressedBlockCache.size() - 1;
		}
		
		loadedBlock.block.uncompress();
		//m_vecUncompressedBlockCache.push_back(&loadedBlock);

		m_pLastAccessedBlock = &(loadedBlock.block);
		assert(m_pLastAccessedBlock->m_tUncompressedData);
		return m_pLastAccessedBlock;
	}

	template <typename VoxelType>
	float Volume<VoxelType>::calculateCompressionRatio(void)
	{
		float fRawSize = m_pBlocks.size() * m_uBlockSideLength * m_uBlockSideLength* m_uBlockSideLength * sizeof(VoxelType);
		float fCompressedSize = calculateSizeInBytes();
		return fCompressedSize/fRawSize;
	}

	template <typename VoxelType>
	uint32_t Volume<VoxelType>::calculateSizeInBytes(void)
	{
		uint32_t uSizeInBytes = sizeof(Volume);

		//Memory used by the blocks
		std::map<Vector3DInt32, LoadedBlock >::iterator i;
		for(i = m_pBlocks.begin(); i != m_pBlocks.end(); i++)
		{
			//Inaccurate - account for rest of loaded block.
			uSizeInBytes += i->second.block.calculateSizeInBytes();
		}

		//Memory used by the block cache.
		uSizeInBytes += m_vecUncompressedBlockCache.capacity() * sizeof(LoadedBlock);
		uSizeInBytes += m_vecUncompressedBlockCache.size() * m_uBlockSideLength * m_uBlockSideLength * m_uBlockSideLength * sizeof(VoxelType);

		//Memory used by border data.
		if(m_pUncompressedBorderData)
		{
			uSizeInBytes += m_uBlockSideLength * m_uBlockSideLength * m_uBlockSideLength * sizeof(VoxelType);
		}

		return uSizeInBytes;
	}

}

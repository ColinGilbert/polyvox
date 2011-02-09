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

#include "PolyVoxImpl/Block.h"
#include "Log.h"
#include "VolumeSampler.h"
#include "Region.h"
#include "Vector.h"

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
	Volume<VoxelType>::Volume(uint16_t uWidth, uint16_t uHeight, uint16_t uDepth, uint16_t uBlockSideLength)
		:m_uTimestamper(0)
		,m_uBlockCacheSize(1024)
		,m_uCompressions(0)
		,m_uUncompressions(0)
		,m_uBlockSideLength(uBlockSideLength)
	{
		setBlockCacheSize(m_uBlockCacheSize);

		//Create a volume of the right size.
		resize(uWidth, uHeight, uDepth, uBlockSideLength);
	}

	////////////////////////////////////////////////////////////////////////////////
	/// Destroys the volume and frees any blocks which are not in use by other volumes.
	////////////////////////////////////////////////////////////////////////////////
	template <typename VoxelType>
	Volume<VoxelType>::~Volume()
	{
	}

	////////////////////////////////////////////////////////////////////////////////
	/// The border value is returned whenever an atempt is made to read a voxel which
	/// is outside the extents of the volume.
	/// \return The value used for voxels outside of the volume
	////////////////////////////////////////////////////////////////////////////////
	template <typename VoxelType>
	VoxelType Volume<VoxelType>::getBorderValue(void) const
	{
		Block<VoxelType>* pUncompressedBorderBlock = getUncompressedBlock(const_cast<Block<VoxelType>*>(&m_pBorderBlock));
		return pUncompressedBorderBlock->getVoxelAt(0,0,0);
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
		return Region(Vector3DInt16(0,0,0), Vector3DInt16(m_uWidth-1,m_uHeight-1,m_uDepth-1));
	}

	////////////////////////////////////////////////////////////////////////////////
	/// \return The width of the volume in voxels
	/// \sa getHeight(), getDepth()
	////////////////////////////////////////////////////////////////////////////////
	template <typename VoxelType>
	uint16_t Volume<VoxelType>::getWidth(void) const
	{
		return m_uWidth;
	}

	////////////////////////////////////////////////////////////////////////////////
	/// \return The height of the volume in voxels
	/// \sa getWidth(), getDepth()
	////////////////////////////////////////////////////////////////////////////////
	template <typename VoxelType>
	uint16_t Volume<VoxelType>::getHeight(void) const
	{
		return m_uHeight;
	}

	////////////////////////////////////////////////////////////////////////////////
	/// \return The depth of the volume in voxels
	/// \sa getWidth(), getHeight()
	////////////////////////////////////////////////////////////////////////////////
	template <typename VoxelType>
	uint16_t Volume<VoxelType>::getDepth(void) const
	{
		return m_uDepth;
	}

	////////////////////////////////////////////////////////////////////////////////
	/// \return The length of the shortest side in voxels. For example, if a volume has
	/// dimensions 256x512x1024 this function will return 256.
	/// \sa getLongestSideLength(), getDiagonalLength()
	////////////////////////////////////////////////////////////////////////////////
	template <typename VoxelType>
	uint16_t Volume<VoxelType>::getShortestSideLength(void) const
	{
		return m_uShortestSideLength;
	}

	////////////////////////////////////////////////////////////////////////////////
	/// \return The length of the longest side in voxels. For example, if a volume has
	/// dimensions 256x512x1024 this function will return 1024.
	/// \sa getShortestSideLength(), getDiagonalLength()
	////////////////////////////////////////////////////////////////////////////////
	template <typename VoxelType>
	uint16_t Volume<VoxelType>::getLongestSideLength(void) const
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
	VoxelType Volume<VoxelType>::getVoxelAt(uint16_t uXPos, uint16_t uYPos, uint16_t uZPos) const
	{
		//We don't use getEnclosingRegion here because we care
		//about speed and don't need to check the lower bound.
		if((uXPos < getWidth()) && (uYPos < getHeight()) && (uZPos < getDepth()))
		{
			const uint16_t blockX = uXPos >> m_uBlockSideLengthPower;
			const uint16_t blockY = uYPos >> m_uBlockSideLengthPower;
			const uint16_t blockZ = uZPos >> m_uBlockSideLengthPower;

			const uint16_t xOffset = uXPos - (blockX << m_uBlockSideLengthPower);
			const uint16_t yOffset = uYPos - (blockY << m_uBlockSideLengthPower);
			const uint16_t zOffset = uZPos - (blockZ << m_uBlockSideLengthPower);

			const Block<VoxelType>& block = m_pBlocks
				[
					blockX + 
					blockY * m_uWidthInBlocks + 
					blockZ * m_uWidthInBlocks * m_uHeightInBlocks
				];

			Block<VoxelType>* pUncompressedBlock = getUncompressedBlock(const_cast<Block<VoxelType>*>(&block));

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
	VoxelType Volume<VoxelType>::getVoxelAt(const Vector3DUint16& v3dPos) const
	{
		return getVoxelAt(v3dPos.getX(), v3dPos.getY(), v3dPos.getZ());
	}

	////////////////////////////////////////////////////////////////////////////////
	/// \param tBorder The value to use for voxels outside the volume.
	////////////////////////////////////////////////////////////////////////////////
	template <typename VoxelType>
	void Volume<VoxelType>::setBorderValue(const VoxelType& tBorder) 
	{
		Block<VoxelType>* pUncompressedBorderBlock = getUncompressedBlock(&m_pBorderBlock);
		return pUncompressedBorderBlock->fill(tBorder);
	}

	////////////////////////////////////////////////////////////////////////////////
	/// \param uXPos the \c x position of the voxel
	/// \param uYPos the \c y position of the voxel
	/// \param uZPos the \c z position of the voxel
	/// \param tValue the value to which the voxel will be set
	/// \return whether the requested position is inside the volume
	////////////////////////////////////////////////////////////////////////////////
	template <typename VoxelType>
	bool Volume<VoxelType>::setVoxelAt(uint16_t uXPos, uint16_t uYPos, uint16_t uZPos, VoxelType tValue)
	{
		//We don't use getEnclosingRegion here because we care
		//about speed and don't need to check the lower bound.
		if((uXPos < getWidth()) && (uYPos < getHeight()) && (uZPos < getDepth()))
		{
			const uint16_t blockX = uXPos >> m_uBlockSideLengthPower;
			const uint16_t blockY = uYPos >> m_uBlockSideLengthPower;
			const uint16_t blockZ = uZPos >> m_uBlockSideLengthPower;

			const uint16_t xOffset = uXPos - (blockX << m_uBlockSideLengthPower);
			const uint16_t yOffset = uYPos - (blockY << m_uBlockSideLengthPower);
			const uint16_t zOffset = uZPos - (blockZ << m_uBlockSideLengthPower);

			uint32_t uBlockIndex =
				blockX + 
				blockY * m_uWidthInBlocks + 
				blockZ * m_uWidthInBlocks * m_uHeightInBlocks;

			Block<VoxelType>& block = m_pBlocks[uBlockIndex];

			Block<VoxelType>* pUncompressedBlock = getUncompressedBlock(&block);

			pUncompressedBlock->setVoxelAt(xOffset,yOffset,zOffset, tValue);

			//Return true to indicate that we modified a voxel.
			return true;
		}
		else
		{
			//Return false to indicate that no voxel was modified.
			return false;
		}
	}

	////////////////////////////////////////////////////////////////////////////////
	/// \param v3dPos the 3D position of the voxel
	/// \param tValue the value to which the voxel will be set
	/// \return whether the requested position is inside the volume
	////////////////////////////////////////////////////////////////////////////////
	template <typename VoxelType>
	bool Volume<VoxelType>::setVoxelAt(const Vector3DUint16& v3dPos, VoxelType tValue)
	{
		return setVoxelAt(v3dPos.getX(), v3dPos.getY(), v3dPos.getZ(), tValue);
	}

	template <typename VoxelType>
	void Volume<VoxelType>::clearBlockCache(void)
	{
		for(uint32_t ct = 0; ct < m_pUncompressedBlocks.size(); ct++)
		{
			m_pUncompressedBlocks[ct]->compress();
			m_uCompressions++;
		}

		m_pUncompressedBlocks.clear();
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
	void Volume<VoxelType>::resize(uint16_t uWidth, uint16_t uHeight, uint16_t uDepth, uint16_t uBlockSideLength)
	{
		//Debug mode validation
		assert(uBlockSideLength > 0);
		assert(isPowerOf2(uBlockSideLength));
		assert(uBlockSideLength <= uWidth);
		assert(uBlockSideLength <= uHeight);
		assert(uBlockSideLength <= uDepth);

		//Release mode validation
		if(uBlockSideLength == 0)
		{
			throw std::invalid_argument("Block side length cannot be zero.");
		}
		if(!isPowerOf2(uBlockSideLength))
		{
			throw std::invalid_argument("Block side length must be a power of two.");
		}
		if(uBlockSideLength > uWidth)
		{
			throw std::invalid_argument("Block side length cannot be greater than volume width.");
		}
		if(uBlockSideLength > uHeight)
		{
			throw std::invalid_argument("Block side length cannot be greater than volume height.");
		}
		if(uBlockSideLength > uDepth)
		{
			throw std::invalid_argument("Block side length cannot be greater than volume depth.");
		}

		//Clear the previous data
		m_pBlocks.clear();

		//Compute the volume side lengths
		m_uWidth = uWidth;
		m_uHeight = uHeight;
		m_uDepth = uDepth;

		//Compute the block side length
		m_uBlockSideLength = uBlockSideLength;
		m_uBlockSideLengthPower = logBase2(m_uBlockSideLength);

		//Compute the side lengths in blocks
		m_uWidthInBlocks = m_uWidth / m_uBlockSideLength;
		m_uHeightInBlocks = m_uHeight / m_uBlockSideLength;
		m_uDepthInBlocks = m_uDepth / m_uBlockSideLength;

		//Compute number of blocks in the volume
		m_uNoOfBlocksInVolume = m_uWidthInBlocks * m_uHeightInBlocks * m_uDepthInBlocks;

		//Create the blocks
		m_pBlocks.resize(m_uNoOfBlocksInVolume);
		for(uint32_t i = 0; i < m_uNoOfBlocksInVolume; ++i)
		{
			m_pBlocks[i].resize(m_uBlockSideLength);
		}

		//Create the border block
		m_pBorderBlock.resize(uBlockSideLength);
		Block<VoxelType>* pUncompressedBorderBlock = getUncompressedBlock(&m_pBorderBlock);
		pUncompressedBorderBlock->fill(VoxelType());

		//Other properties we might find useful later
		m_uLongestSideLength = (std::max)((std::max)(m_uWidth,m_uHeight),m_uDepth);
		m_uShortestSideLength = (std::min)((std::min)(m_uWidth,m_uHeight),m_uDepth);
		m_fDiagonalLength = sqrtf(static_cast<float>(m_uWidth * m_uWidth + m_uHeight * m_uHeight + m_uDepth * m_uDepth));
	}

	template <typename VoxelType>
	void Volume<VoxelType>::setBlockCacheSize(uint16_t uBlockCacheSize)
	{
		clearBlockCache();

		m_uBlockCacheSize = uBlockCacheSize;
	}

	template <typename VoxelType>
	Block<VoxelType>* Volume<VoxelType>::getUncompressedBlock(Block<VoxelType>* block) const
	{
		block->m_uTimestamp = ++m_uTimestamper;

		if(block->m_bIsCompressed == false)
		{ 
			return block;
		}

		uint32_t uUncompressedBlockIndex = 100000000;

		assert(m_pUncompressedBlocks.size() <= m_uBlockCacheSize);
		if(m_pUncompressedBlocks.size() == m_uBlockCacheSize)
		{
			int32_t leastRecentlyUsedBlockIndex = -1;
			uint32_t uLeastRecentTimestamp = 1000000000000000;
			for(uint32_t ct = 0; ct < m_pUncompressedBlocks.size(); ct++)
			{
				if(m_pUncompressedBlocks[ct]->m_uTimestamp < uLeastRecentTimestamp)
				{
					uLeastRecentTimestamp = m_pUncompressedBlocks[ct]->m_uTimestamp;
					leastRecentlyUsedBlockIndex = ct;
				}
			}

			m_pUncompressedBlocks[leastRecentlyUsedBlockIndex]->compress();
			m_uCompressions++;
			m_pUncompressedBlocks[leastRecentlyUsedBlockIndex] = block;
		}
		else
		{
			m_pUncompressedBlocks.push_back(block);
		}

		block->uncompress();
		m_uUncompressions++;

		return block;
	}
}

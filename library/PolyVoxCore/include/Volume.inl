#pragma region License
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
#pragma endregion

#pragma region Headers
#include "PolyVoxImpl/Block.h"
#include "Log.h"
#include "VolumeSampler.h"
#include "Region.h"
#include "Vector.h"

#include <cassert>
#include <cstring> //For memcpy
#include <list>
#include <stdexcept> //For invalid_argument
#pragma endregion

namespace PolyVox
{
	#pragma region Constructors/Destructors
	////////////////////////////////////////////////////////////////////////////////
	/// Builds a volume of the desired dimensions
	/// \param uWidth The desired width in voxels. This must be a power of two.
	/// \param uHeight The desired height in voxels. This must be a power of two.
	/// \param uDepth The desired depth in voxels. This must be a power of two.
	/// \param uBlockSideLength The size of the blocks which make up the volume. Small
	/// blocks are more likely to be homogeneous (so more easily shared) and have better
	/// cache behaviour. However, there is a memory overhead per block so if they are
	/// not shared it could actually be less efficient (this will depend on the data).
	/// The size of the volume may also be a factor when choosing block size. Specifying
	/// '0' for	the block side length will cause the blocks to be as large as possible,
	/// which will basically be the length of the shortest side. Accept the default if
	/// you are not sure what to choose here.
	////////////////////////////////////////////////////////////////////////////////
	template <typename VoxelType>
	Volume<VoxelType>::Volume(uint16_t uWidth, uint16_t uHeight, uint16_t uDepth, uint16_t uBlockSideLength)
		:m_pBlocks(0)
		,m_uCurrentBlockForTidying(0)
	{
		//A values of zero for a block side length is a special value to indicate 
		//that the block side length should simply be made as large as possible.
		if(uBlockSideLength == 0)
		{
			uBlockSideLength = (std::min)((std::min)(uWidth,uHeight),uDepth);
		}

		//Debug mode validation
		assert(isPowerOf2(uWidth));
		assert(isPowerOf2(uHeight));
		assert(isPowerOf2(uDepth));
		assert(isPowerOf2(uBlockSideLength));
		assert(uBlockSideLength <= uWidth);
		assert(uBlockSideLength <= uHeight);
		assert(uBlockSideLength <= uDepth);

		//Release mode validation
		if(!(isPowerOf2(uWidth) && isPowerOf2(uHeight) && isPowerOf2(uDepth)))
		{
			throw std::invalid_argument("Volume width, height, and depth must all be a power of two.");
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

		//Compute the volume side lengths
		m_uWidth = uWidth;
		m_uWidthPower = logBase2(m_uWidth);

		m_uHeight = uHeight;
		m_uHeightPower = logBase2(m_uHeight);

		m_uDepth = uDepth;
		m_uDepthPower = logBase2(m_uDepth);

		//Compute the block side length
		m_uBlockSideLength = uBlockSideLength;
		m_uBlockSideLengthPower = logBase2(m_uBlockSideLength);

		//Compute the side length in blocks
		//m_uSideLengthInBlocks = m_uSideLength / m_uBlockSideLength;
		m_uWidthInBlocks = m_uWidth / m_uBlockSideLength;
		m_uHeightInBlocks = m_uHeight / m_uBlockSideLength;
		m_uDepthInBlocks = m_uDepth / m_uBlockSideLength;

		//Compute number of blocks in the volume
		m_uNoOfBlocksInVolume = m_uWidthInBlocks * m_uHeightInBlocks * m_uDepthInBlocks;

		//Create the blocks
		m_pBlocks.resize(m_uNoOfBlocksInVolume);
		m_vecBlockIsPotentiallyHomogenous.resize(m_uNoOfBlocksInVolume);
		for(uint32_t i = 0; i < m_uNoOfBlocksInVolume; ++i)
		{
			m_pBlocks[i] = getHomogenousBlock(0);
			m_vecBlockIsPotentiallyHomogenous[i] = false;
		}

		//Other properties we might find useful later
		m_uLongestSideLength = (std::max)((std::max)(m_uWidth,m_uHeight),m_uDepth);
		m_uShortestSideLength = (std::min)((std::min)(m_uWidth,m_uHeight),m_uDepth);
		m_fDiagonalLength = sqrtf(static_cast<float>(m_uWidth * m_uWidth + m_uHeight * m_uHeight + m_uDepth * m_uDepth));
	}

	////////////////////////////////////////////////////////////////////////////////
	/// Destroys the volume and frees any blocks which are not in use by other volumes.
	////////////////////////////////////////////////////////////////////////////////
	template <typename VoxelType>
	Volume<VoxelType>::~Volume()
	{
	}
	#pragma endregion

	#pragma region Operators
	#pragma endregion		

	#pragma region Getters
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

	template <typename VoxelType>
	VoxelType Volume<VoxelType>::getVoxelAt(uint16_t uXPos, uint16_t uYPos, uint16_t uZPos) const
	{
		assert(uXPos < getWidth());
		assert(uYPos < getHeight());
		assert(uZPos < getDepth());

		const uint16_t blockX = uXPos >> m_uBlockSideLengthPower;
		const uint16_t blockY = uYPos >> m_uBlockSideLengthPower;
		const uint16_t blockZ = uZPos >> m_uBlockSideLengthPower;

		const uint16_t xOffset = uXPos - (blockX << m_uBlockSideLengthPower);
		const uint16_t yOffset = uYPos - (blockY << m_uBlockSideLengthPower);
		const uint16_t zOffset = uZPos - (blockZ << m_uBlockSideLengthPower);

		const std::shared_ptr< Block< VoxelType > >& block = m_pBlocks
			[
				blockX + 
				blockY * m_uWidthInBlocks + 
				blockZ * m_uWidthInBlocks * m_uHeightInBlocks
			];

		return block->getVoxelAt(xOffset,yOffset,zOffset);
	}

	template <typename VoxelType>
	VoxelType Volume<VoxelType>::getVoxelAt(const Vector3DUint16& v3dPos) const
	{
		return getVoxelAt(v3dPos.getX(), v3dPos.getY(), v3dPos.getZ());
	}
	#pragma endregion	

	#pragma region Setters
	template <typename VoxelType>
	void Volume<VoxelType>::setVoxelAt(uint16_t uXPos, uint16_t uYPos, uint16_t uZPos, VoxelType tValue)
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

		std::shared_ptr< Block<VoxelType> >& block = m_pBlocks[uBlockIndex];

		//It's quite possible that the user might attempt to set a voxel to it's current value.
		//We test for this case firstly because it could help performance, but more importantly
		//because it lets us avoid unsharing blocks unnecessarily.
		if(block->getVoxelAt(xOffset, yOffset, zOffset) != tValue)
		{
			if(block.unique())
			{
				block->setVoxelAt(xOffset,yOffset,zOffset, tValue);
				//There is a chance that setting this voxel makes the block homogenous and therefore shareable.
				//But checking this will take some time, so for now just set a flag.
				m_vecBlockIsPotentiallyHomogenous[uBlockIndex] = true;
			}
			else
			{			
				std::shared_ptr< Block<VoxelType> > pNewBlock(new Block<VoxelType>(*(block)));
				block = pNewBlock;
				m_vecBlockIsPotentiallyHomogenous[uBlockIndex] = false;
				block->setVoxelAt(xOffset,yOffset,zOffset, tValue);
			}
		}
	}

	template <typename VoxelType>
	void Volume<VoxelType>::setVoxelAt(const Vector3DUint16& v3dPos, VoxelType tValue)
	{
		setVoxelAt(v3dPos.getX(), v3dPos.getY(), v3dPos.getZ(), tValue);
	}
	#pragma endregion

	#pragma region Other
	template <typename VoxelType>
	void Volume<VoxelType>::tidyUpMemory(uint32_t uNoOfBlocksToProcess)
	{
		//Track the number of blocks we have processed.
		uint32_t m_uNoOfProcessedBlocks = 0;

		//We will loop around, and finish if we get back to our start position
		uint32_t uFinishBlock = m_uCurrentBlockForTidying;

		//Increment the current block, looping around if necessary
		++m_uCurrentBlockForTidying;
		m_uCurrentBlockForTidying %= m_uNoOfBlocksInVolume;

		//While we have not reached the user specified limit and there are more blocks to process...
		while((m_uNoOfProcessedBlocks < uNoOfBlocksToProcess) && (m_uCurrentBlockForTidying != uFinishBlock))
		{
			//We only do any work if the block is flagged as potentially homogeneous.
			if(m_vecBlockIsPotentiallyHomogenous[m_uCurrentBlockForTidying])
			{
				//Check if it's really homogeneous (this can be slow).
				if(m_pBlocks[m_uCurrentBlockForTidying]->isHomogeneous())
				{
					//If so, replace is with a block from out homogeneous collection.
					VoxelType homogeneousValue = m_pBlocks[m_uCurrentBlockForTidying]->getVoxelAt(0,0,0);
					m_pBlocks[m_uCurrentBlockForTidying] = getHomogenousBlock(homogeneousValue);
				}

				//Either way, we have now determined whether the block was sharable. So it's not *potentially* sharable.
				m_vecBlockIsPotentiallyHomogenous[m_uCurrentBlockForTidying] = false;

				//We've processed a block. This is inside the 'if' because the path outside the 'if' is trivially fast.
				++m_uNoOfProcessedBlocks;
			}

			//Increment the current block, looping around if necessary
			++m_uCurrentBlockForTidying;
			m_uCurrentBlockForTidying %= m_uNoOfBlocksInVolume;
		}

		//Identify and remove any homogeneous blocks which are not actually in use.
		typename std::map<VoxelType, std::shared_ptr< Block<VoxelType> > >::iterator iter = m_pHomogenousBlock.begin();
		while(iter != m_pHomogenousBlock.end())
		{
			if(iter->second.unique())
			{
				m_pHomogenousBlock.erase(iter++); //Increments the iterator and returns the previous position to be erased.
			}
			else
			{
				++iter; //Just increments the iterator.
			}
		}
	}	
	#pragma endregion

	#pragma region Private Implementation
	template <typename VoxelType>
	std::shared_ptr< Block<VoxelType> > Volume<VoxelType>::getHomogenousBlock(VoxelType tHomogenousValue) 
	{
		typename std::map<VoxelType, std::shared_ptr< Block<VoxelType> > >::iterator iterResult = m_pHomogenousBlock.find(tHomogenousValue);
		if(iterResult == m_pHomogenousBlock.end())
		{
			//Block<VoxelType> block;
			std::shared_ptr< Block<VoxelType> > pHomogeneousBlock(new Block<VoxelType>(m_uBlockSideLength));
			//block.m_pBlock = temp;
			//block.m_uReferenceCount++;
			pHomogeneousBlock->fill(tHomogenousValue);
			m_pHomogenousBlock.insert(std::make_pair(tHomogenousValue, pHomogeneousBlock));
			return pHomogeneousBlock;
		}
		else
		{
			//iterResult->second.m_uReferenceCount++;
			//std::shared_ptr< Block<VoxelType> > result(iterResult->second);
			return iterResult->second;
		}
	}
	#pragma endregion
}

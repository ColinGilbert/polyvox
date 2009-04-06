#pragma region License
/******************************************************************************
This file is part of the PolyVox library
Copyright (C) 2006  David Williams

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
******************************************************************************/
#pragma endregion

#pragma region Headers
#include "PolyVoxImpl/BlockData.h"
#include "Log.h"
#include "VolumeIterator.h"
#include "Region.h"
#include "Vector.h"

#include <cassert>
#include <cstring> //For memcpy
#include <stdexcept> //For invalid_argument
#pragma endregion

namespace PolyVox
{
	#pragma region Constructors/Destructors
	template <typename VoxelType>
	Volume<VoxelType>::Volume(uint16_t uSideLength, uint16_t uBlockSideLength)
		:m_pBlocks(0)
	{
		//Debug mode validation
		assert(isPowerOf2(uSideLength));
		assert(isPowerOf2(uBlockSideLength));
		assert(uBlockSideLength <= uSideLength);

		//Release mode validation
		if(!isPowerOf2(uSideLength))
		{
			throw std::invalid_argument("Volume side length must be a power of two.");
		}
		if(!isPowerOf2(uBlockSideLength))
		{
			throw std::invalid_argument("Block side length must be a power of two.");
		}
		if(uBlockSideLength > uSideLength)
		{
			throw std::invalid_argument("Block side length cannot be greater than volume side length.");
		}

		//Compute the volume side length
		m_uSideLength = uSideLength;
		m_uSideLengthPower = logBase2(m_uSideLength);

		//Compute the block side length
		m_uBlockSideLength = uBlockSideLength;
		m_uBlockSideLengthPower = logBase2(m_uBlockSideLength);

		//Compute the side length in blocks
		m_uSideLengthInBlocks = m_uSideLength / m_uBlockSideLength;

		//Compute number of blocks in the volume
		m_uNoOfBlocksInVolume = m_uSideLengthInBlocks * m_uSideLengthInBlocks * m_uSideLengthInBlocks;

		//Create the blocks
		m_pBlocks = new Block<VoxelType>[m_uNoOfBlocksInVolume];
		for(uint32_t i = 0; i < m_uNoOfBlocksInVolume; ++i)
		{
			m_pBlocks[i].m_pBlockData = getHomogenousBlockData(0);
			m_pBlocks[i].m_bIsShared = true;
			m_pBlocks[i].m_bIsPotentiallySharable = false;
			m_pBlocks[i].m_pHomogenousValue = 0;
		}
	}

	/*template <typename VoxelType>
	Volume<VoxelType>::Volume(const Volume<VoxelType>& rhs)
	{
		*this = rhs;
	}*/

	template <typename VoxelType>
	Volume<VoxelType>::~Volume()
	{
		delete[] m_pBlocks;
	}
	#pragma endregion

	#pragma region Operators
	/*template <typename VoxelType>
	Volume<VoxelType>& Volume<VoxelType>::operator=(const Volume& rhs)
	{
		
	}*/
	#pragma endregion		

	#pragma region Getters
	template <typename VoxelType>
	Region Volume<VoxelType>::getEnclosingRegion(void) const
	{
		return Region(Vector3DInt32(0,0,0), Vector3DInt32(m_uSideLength-1,m_uSideLength-1,m_uSideLength-1));
	}

	template <typename VoxelType>
	uint16_t Volume<VoxelType>::getSideLength(void) const
	{
		return m_uSideLength;
	}

	template <typename VoxelType>
	VoxelType Volume<VoxelType>::getVoxelAt(uint16_t uXPos, uint16_t uYPos, uint16_t uZPos) const
	{
		assert(uXPos < getSideLength());
		assert(uYPos < getSideLength());
		assert(uZPos < getSideLength());

		const uint16_t blockX = uXPos >> m_uBlockSideLengthPower;
		const uint16_t blockY = uYPos >> m_uBlockSideLengthPower;
		const uint16_t blockZ = uZPos >> m_uBlockSideLengthPower;

		const uint16_t xOffset = uXPos - (blockX << m_uBlockSideLengthPower);
		const uint16_t yOffset = uYPos - (blockY << m_uBlockSideLengthPower);
		const uint16_t zOffset = uZPos - (blockZ << m_uBlockSideLengthPower);

		const Block<VoxelType>& block = m_pBlocks
			[
				blockX + 
				blockY * m_uSideLengthInBlocks + 
				blockZ * m_uSideLengthInBlocks * m_uSideLengthInBlocks
			];

		return block.m_pBlockData->getVoxelAt(xOffset,yOffset,zOffset);
	}

	template <typename VoxelType>
	VoxelType Volume<VoxelType>::getVoxelAt(const Vector3DUint16& v3dPos) const
	{
		assert(v3dPos.getX() < m_uSideLength);
		assert(v3dPos.getY() < m_uSideLength);
		assert(v3dPos.getZ() < m_uSideLength);

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

		Block<VoxelType>& block = m_pBlocks
			[
				blockX + 
				blockY * m_uSideLengthInBlocks + 
				blockZ * m_uSideLengthInBlocks * m_uSideLengthInBlocks
			];

		if(block.m_bIsShared)
		{
			const VoxelType tHomogenousValue = block.m_pHomogenousValue;
			if(tHomogenousValue != tValue)
			{
				POLYVOX_SHARED_PTR< BlockData<VoxelType> > pNewBlockData(new BlockData<VoxelType>(m_uBlockSideLength));
				block.m_pBlockData = pNewBlockData;
				block.m_bIsShared = false;
				block.m_bIsPotentiallySharable = false;
				block.m_pBlockData->fill(tHomogenousValue);
				block.m_pBlockData->setVoxelAt(xOffset,yOffset,zOffset, tValue);
			}
		}
		else
		{			
			block.m_pBlockData->setVoxelAt(xOffset,yOffset,zOffset, tValue);
			//There is a chance that setting this voxel makes the block homogenous and therefore shareable. But checking
			//this will take some time, so for now just set a flag.
			block.m_bIsPotentiallySharable = true;
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
	void Volume<VoxelType>::idle(uint32_t uAmount)
	{
		//This function performs two roles. Firstly, it examines all of the blocks which are marked as
		//'potentially sharable' to determine whether they really are sharable or not. For those which
		//are sharable, it adjusts the pointer and deletes tho old data. Secondly, it determines which
		//homogeneous regions are not actually being used (by thier reference count) and frees them.

		for(uint32_t i = 0; i < m_uNoOfBlocksInVolume; ++i)
		{
			Block<VoxelType> block = m_pBlocks[i];
			if(block.m_bIsPotentiallySharable)
			{
				bool isSharable = block.m_pBlockData->isHomogeneous();
				if(isSharable)
				{
					VoxelType homogeneousValue = block.m_pBlockData->getVoxelAt(0,0,0);
					delete block.m_pBlockData;

					block.m_pBlockData = getHomogenousBlockData(homogeneousValue);
					block.m_pHomogenousValue = homogeneousValue;
					block.m_bIsShared = true;
				}

				//Either way, we have now determined whether the block was sharable. So it's not *potentially* sharable.
				block.m_bIsPotentiallySharable = false;
			}
		}

		//Note - this second step should probably happen immediatly, rather than in this function. 
		//Use of a shared pointer system would allow this.
	}	

	template <typename VoxelType>
	bool Volume<VoxelType>::isRegionHomogenous(const Region& region)
	{
		VolumeIterator<VoxelType> iter(*this);
		iter.setValidRegion(region);
		iter.setPosition(static_cast<Vector3DInt16>(region.getLowerCorner()));

		VoxelType tFirst = iter.getVoxel();
		iter.moveForwardInRegionXYZ();

		do
		{
			VoxelType tCurrent = iter.getVoxel();
			if(tCurrent != tFirst)
			{
				return false;
			}
		}while(iter.moveForwardInRegionXYZ());

		return true;
	}
	#pragma endregion

	#pragma region Private Implementation
	template <typename VoxelType>
	POLYVOX_SHARED_PTR< BlockData<VoxelType> > Volume<VoxelType>::getHomogenousBlockData(VoxelType tHomogenousValue) const
	{
		typename std::map<VoxelType, POLYVOX_WEAK_PTR< BlockData<VoxelType> > >::iterator iterResult = m_pHomogenousBlockData.find(tHomogenousValue);
		if(iterResult == m_pHomogenousBlockData.end())
		{
			Block<VoxelType> block;
			POLYVOX_SHARED_PTR< BlockData<VoxelType> > temp(new BlockData<VoxelType>(m_uBlockSideLength));
			block.m_pBlockData = temp;
			//block.m_uReferenceCount++;
			block.m_pBlockData->fill(tHomogenousValue);
			m_pHomogenousBlockData.insert(std::make_pair(tHomogenousValue, temp));
			return block.m_pBlockData;
		}
		else
		{
			//iterResult->second.m_uReferenceCount++;
			POLYVOX_SHARED_PTR< BlockData<VoxelType> > result(iterResult->second);
			return result;
		}
	}
	#pragma endregion
}

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

#include "PolyVoxCore/ConstVolumeProxy.h"
#include "PolyVoxImpl/Block.h"
#include "PolyVoxCore/Log.h"
#include "PolyVoxCore/Region.h"
#include "PolyVoxCore/Vector.h"

#include <limits>
#include <cassert>
#include <cstdlib> //For abort()
#include <stdexcept> //For invalid_argument

namespace PolyVox
{
	////////////////////////////////////////////////////////////////////////////////
	/// Deprecated - do not use this constructor.
	////////////////////////////////////////////////////////////////////////////////
	template <typename VoxelType>
	SimpleVolume<VoxelType>::SimpleVolume
	(
		int32_t dont_use_this_constructor_1, int32_t dont_use_this_constructor_2, int32_t dont_use_this_constructor_3
	)
	{
		//In earlier verions of PolyVox the constructor took three values indicating width, height, and depth. However, this
		//causes confusion because these three parameters can be interpreted as two function pointers and a block size instead,
		//hence calling a different constructor. And simply removing this constructor will cause confusion because existing
		//code with three parameters will then always resolve to the constructor with two function pointers and a block size.
		//
		//Eventually this constructor will be removed, it's just here to make people change their code to the new version.
		//
		//IF YOU HIT THIS ASSERT/ABORT, CHANGE YOUR CODE TO USE THE CONSTRUCTOR TAKING A 'Region' INSTEAD.
		assert(false);
		abort();
	}

	////////////////////////////////////////////////////////////////////////////////
	/// This constructor creates a volume with a fixed size which is specified as a parameter. By default this constructor will not enable paging but you can override this if desired. If you do wish to enable paging then you are required to provide the call back function (see the other SimpleVolume constructor).
	/// \param regValid Specifies the minimum and maximum valid voxel positions.
	/// \param dataRequiredHandler The callback function which will be called when PolyVox tries to use data which is not currently in momory.
	/// \param dataOverflowHandler The callback function which will be called when PolyVox has too much data and needs to remove some from memory.
	/// \param bPagingEnabled Controls whether or not paging is enabled for this SimpleVolume.
	/// \param uBlockSideLength The size of the blocks making up the volume. Small blocks will compress/decompress faster, but there will also be more of them meaning voxel access could be slower.
	////////////////////////////////////////////////////////////////////////////////
	template <typename VoxelType>
	SimpleVolume<VoxelType>::SimpleVolume
	(
		const Region& regValid,
		uint16_t uBlockSideLength
	)
	{
		//Create a volume of the right size.
		resize(regValid,uBlockSideLength);
	}

	////////////////////////////////////////////////////////////////////////////////
	/// Destroys the volume
	////////////////////////////////////////////////////////////////////////////////
	template <typename VoxelType>
	SimpleVolume<VoxelType>::~SimpleVolume()
	{
		delete[] m_pBlocks;
		m_pBlocks = 0;
	}

	////////////////////////////////////////////////////////////////////////////////
	/// The border value is returned whenever an atempt is made to read a voxel which
	/// is outside the extents of the volume.
	/// \return The value used for voxels outside of the volume
	////////////////////////////////////////////////////////////////////////////////
	template <typename VoxelType>
	VoxelType SimpleVolume<VoxelType>::getBorderValue(void) const
	{
		return *m_pUncompressedBorderData;
	}

	////////////////////////////////////////////////////////////////////////////////
	/// \return A Region representing the extent of the volume.
	////////////////////////////////////////////////////////////////////////////////
	template <typename VoxelType>
	Region SimpleVolume<VoxelType>::getEnclosingRegion(void) const
	{
		return m_regValidRegion;
	}

	////////////////////////////////////////////////////////////////////////////////
	/// \return The width of the volume in voxels. Note that this value is inclusive, so that if the valid range is e.g. 0 to 63 then the width is 64.
	/// \sa getHeight(), getDepth()
	////////////////////////////////////////////////////////////////////////////////
	template <typename VoxelType>
	int32_t SimpleVolume<VoxelType>::getWidth(void) const
	{
		return m_regValidRegion.getUpperCorner().getX() - m_regValidRegion.getLowerCorner().getX() + 1;
	}

	////////////////////////////////////////////////////////////////////////////////
	/// \return The height of the volume in voxels. Note that this value is inclusive, so that if the valid range is e.g. 0 to 63 then the height is 64.
	/// \sa getWidth(), getDepth()
	////////////////////////////////////////////////////////////////////////////////
	template <typename VoxelType>
	int32_t SimpleVolume<VoxelType>::getHeight(void) const
	{
		return m_regValidRegion.getUpperCorner().getY() - m_regValidRegion.getLowerCorner().getY() + 1;
	}

	////////////////////////////////////////////////////////////////////////////////
	/// \return The depth of the volume in voxels. Note that this value is inclusive, so that if the valid range is e.g. 0 to 63 then the depth is 64.
	/// \sa getWidth(), getHeight()
	////////////////////////////////////////////////////////////////////////////////
	template <typename VoxelType>
	int32_t SimpleVolume<VoxelType>::getDepth(void) const
	{
		return m_regValidRegion.getUpperCorner().getZ() - m_regValidRegion.getLowerCorner().getZ() + 1;
	}

	////////////////////////////////////////////////////////////////////////////////
	/// \return The length of the shortest side in voxels. For example, if a volume has
	/// dimensions 256x512x1024 this function will return 256.
	/// \sa getLongestSideLength(), getDiagonalLength()
	////////////////////////////////////////////////////////////////////////////////
	template <typename VoxelType>
	int32_t SimpleVolume<VoxelType>::getShortestSideLength(void) const
	{
		return m_uShortestSideLength;
	}

	////////////////////////////////////////////////////////////////////////////////
	/// \return The length of the longest side in voxels. For example, if a volume has
	/// dimensions 256x512x1024 this function will return 1024.
	/// \sa getShortestSideLength(), getDiagonalLength()
	////////////////////////////////////////////////////////////////////////////////
	template <typename VoxelType>
	int32_t SimpleVolume<VoxelType>::getLongestSideLength(void) const
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
	float SimpleVolume<VoxelType>::getDiagonalLength(void) const
	{
		return m_fDiagonalLength;
	}

	////////////////////////////////////////////////////////////////////////////////
	/// \param uXPos The \c x position of the voxel
	/// \param uYPos The \c y position of the voxel
	/// \param uZPos The \c z position of the voxel
	/// \return The voxel value
	////////////////////////////////////////////////////////////////////////////////
	template <typename VoxelType>
	VoxelType SimpleVolume<VoxelType>::getVoxelAt(int32_t uXPos, int32_t uYPos, int32_t uZPos) const
	{
		if(m_regValidRegion.containsPoint(Vector3DInt32(uXPos, uYPos, uZPos)))
		{
			const int32_t blockX = uXPos >> m_uBlockSideLengthPower;
			const int32_t blockY = uYPos >> m_uBlockSideLengthPower;
			const int32_t blockZ = uZPos >> m_uBlockSideLengthPower;

			const uint16_t xOffset = uXPos - (blockX << m_uBlockSideLengthPower);
			const uint16_t yOffset = uYPos - (blockY << m_uBlockSideLengthPower);
			const uint16_t zOffset = uZPos - (blockZ << m_uBlockSideLengthPower);

			SimpleVolume<VoxelType>::Block* pUncompressedBlock = getUncompressedBlock(blockX, blockY, blockZ);

			return pUncompressedBlock->getVoxelAt(xOffset,yOffset,zOffset);
		}
		else
		{
			return getBorderValue();
		}
	}

	////////////////////////////////////////////////////////////////////////////////
	/// \param v3dPos The 3D position of the voxel
	/// \return The voxel value
	////////////////////////////////////////////////////////////////////////////////
	template <typename VoxelType>
	VoxelType SimpleVolume<VoxelType>::getVoxelAt(const Vector3DInt32& v3dPos) const
	{
		return getVoxelAt(v3dPos.getX(), v3dPos.getY(), v3dPos.getZ());
	}

	////////////////////////////////////////////////////////////////////////////////
	/// \param tBorder The value to use for voxels outside the volume.
	////////////////////////////////////////////////////////////////////////////////
	template <typename VoxelType>
	void SimpleVolume<VoxelType>::setBorderValue(const VoxelType& tBorder) 
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
	bool SimpleVolume<VoxelType>::setVoxelAt(int32_t uXPos, int32_t uYPos, int32_t uZPos, VoxelType tValue)
	{
		assert(m_regValidRegion.containsPoint(Vector3DInt32(uXPos, uYPos, uZPos)));

		const int32_t blockX = uXPos >> m_uBlockSideLengthPower;
		const int32_t blockY = uYPos >> m_uBlockSideLengthPower;
		const int32_t blockZ = uZPos >> m_uBlockSideLengthPower;

		const uint16_t xOffset = uXPos - (blockX << m_uBlockSideLengthPower);
		const uint16_t yOffset = uYPos - (blockY << m_uBlockSideLengthPower);
		const uint16_t zOffset = uZPos - (blockZ << m_uBlockSideLengthPower);

		SimpleVolume<VoxelType>::Block* pUncompressedBlock = getUncompressedBlock(blockX, blockY, blockZ);

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
	bool SimpleVolume<VoxelType>::setVoxelAt(const Vector3DInt32& v3dPos, VoxelType tValue)
	{
		return setVoxelAt(v3dPos.getX(), v3dPos.getY(), v3dPos.getZ(), tValue);
	}

	////////////////////////////////////////////////////////////////////////////////
	/// This function should probably be made internal...
	////////////////////////////////////////////////////////////////////////////////
	template <typename VoxelType>
	void SimpleVolume<VoxelType>::resize(const Region& regValidRegion, uint16_t uBlockSideLength)
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

		m_uBlockSideLength = uBlockSideLength;
		m_pUncompressedBorderData = 0;

		m_regValidRegion = regValidRegion;

		m_regValidRegionInBlocks.setLowerCorner(m_regValidRegion.getLowerCorner()  / static_cast<int32_t>(uBlockSideLength));
		m_regValidRegionInBlocks.setUpperCorner(m_regValidRegion.getUpperCorner()  / static_cast<int32_t>(uBlockSideLength));

		//Compute the block side length
		m_uBlockSideLength = uBlockSideLength;
		m_uBlockSideLengthPower = logBase2(m_uBlockSideLength);

		//Compute the size of the volume in blocks (and note +1 at the end)
		m_uWidthInBlocks = m_regValidRegionInBlocks.getUpperCorner().getX() - m_regValidRegionInBlocks.getLowerCorner().getX() + 1;
		m_uHeightInBlocks = m_regValidRegionInBlocks.getUpperCorner().getY() - m_regValidRegionInBlocks.getLowerCorner().getY() + 1;
		m_uDepthInBlocks = m_regValidRegionInBlocks.getUpperCorner().getZ() - m_regValidRegionInBlocks.getLowerCorner().getZ() + 1;
		m_uNoOfBlocksInVolume = m_uWidthInBlocks * m_uHeightInBlocks * m_uDepthInBlocks;

		//Allocate the data
		m_pBlocks = new Block[m_uNoOfBlocksInVolume];
		for(uint32_t i = 0; i < m_uNoOfBlocksInVolume; ++i)
		{
			m_pBlocks[i].initialise(m_uBlockSideLength);
		}

		//Create the border block
		m_pUncompressedBorderData = new VoxelType[m_uBlockSideLength * m_uBlockSideLength * m_uBlockSideLength];
		std::fill(m_pUncompressedBorderData, m_pUncompressedBorderData + m_uBlockSideLength * m_uBlockSideLength * m_uBlockSideLength, VoxelType());

		//Other properties we might find useful later
		m_uLongestSideLength = (std::max)((std::max)(getWidth(),getHeight()),getDepth());
		m_uShortestSideLength = (std::min)((std::min)(getWidth(),getHeight()),getDepth());
		m_fDiagonalLength = sqrtf(static_cast<float>(getWidth() * getWidth() + getHeight() * getHeight() + getDepth() * getDepth()));
	}

	template <typename VoxelType>
	typename SimpleVolume<VoxelType>::Block* SimpleVolume<VoxelType>::getUncompressedBlock(int32_t uBlockX, int32_t uBlockY, int32_t uBlockZ) const
	{
		//The lower left corner of the volume could be
		//anywhere, but array indices need to start at zero.
		uBlockX -= m_regValidRegionInBlocks.getLowerCorner().getX();
		uBlockY -= m_regValidRegionInBlocks.getLowerCorner().getY();
		uBlockZ -= m_regValidRegionInBlocks.getLowerCorner().getZ();

		//Compute the block index
		uint32_t uBlockIndex =
				uBlockX + 
				uBlockY * m_uWidthInBlocks + 
				uBlockZ * m_uWidthInBlocks * m_uHeightInBlocks;

		//Return the block
		return &(m_pBlocks[uBlockIndex]);
	}

	////////////////////////////////////////////////////////////////////////////////
	/// Note: This function needs reviewing for accuracy...
	////////////////////////////////////////////////////////////////////////////////
	template <typename VoxelType>
	uint32_t SimpleVolume<VoxelType>::calculateSizeInBytes(void)
	{
		uint32_t uSizeInBytes = sizeof(SimpleVolume);
		
		uint32_t uSizeOfBlockInBytes = m_uBlockSideLength * m_uBlockSideLength * m_uBlockSideLength * sizeof(VoxelType);

		//Memory used by the blocks ( + 1 is for border)
		uSizeInBytes += uSizeOfBlockInBytes * (m_uNoOfBlocksInVolume + 1);

		return uSizeInBytes;
	}

}


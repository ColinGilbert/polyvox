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

namespace PolyVox
{
	////////////////////////////////////////////////////////////////////////////////
	/// This constructor creates a volume with a fixed size which is specified as a parameter.
	/// \param regValid Specifies the minimum and maximum valid voxel positions.
	/// \param uBlockSideLength The size of the block to use within the volume
	////////////////////////////////////////////////////////////////////////////////
	template <typename VoxelType>
	SimpleVolume<VoxelType>::SimpleVolume(const Region& regValid, uint16_t uBlockSideLength)
		:BaseVolume<VoxelType>(regValid)
	{
		//Create a volume of the right size.
		initialise(regValid,uBlockSideLength);
	}

	////////////////////////////////////////////////////////////////////////////////
	/// This function should never be called. Copying volumes by value would be expensive, and we want to prevent users from doing
	/// it by accident (such as when passing them as paramenters to functions). That said, there are times when you really do want to
	/// make a copy of a volume and in this case you should look at the Volumeresampler.
	///
	/// \sa VolumeResampler
	////////////////////////////////////////////////////////////////////////////////
	template <typename VoxelType>
	SimpleVolume<VoxelType>::SimpleVolume(const SimpleVolume<VoxelType>& /*rhs*/)
	{
		POLYVOX_THROW(not_implemented, "Volume copy constructor not implemented for performance reasons.");
	}

	////////////////////////////////////////////////////////////////////////////////
	/// Destroys the volume
	////////////////////////////////////////////////////////////////////////////////
	template <typename VoxelType>
	SimpleVolume<VoxelType>::~SimpleVolume()
	{
		delete[] m_pBlocks;
	}

	////////////////////////////////////////////////////////////////////////////////
	/// This function should never be called. Copying volumes by value would be expensive, and we want to prevent users from doing
	/// it by accident (such as when passing them as paramenters to functions). That said, there are times when you really do want to
	/// make a copy of a volume and in this case you should look at the Volumeresampler.
	///
	/// \sa VolumeResampler
	////////////////////////////////////////////////////////////////////////////////
	template <typename VoxelType>
	SimpleVolume<VoxelType>& SimpleVolume<VoxelType>::operator=(const SimpleVolume<VoxelType>& /*rhs*/)
	{
		POLYVOX_THROW(not_implemented, "Volume assignment operator not implemented for performance reasons.");
	}

	////////////////////////////////////////////////////////////////////////////////
	/// \param uXPos The \c x position of the voxel
	/// \param uYPos The \c y position of the voxel
	/// \param uZPos The \c z position of the voxel
	/// \return The voxel value
	////////////////////////////////////////////////////////////////////////////////
	template <typename VoxelType>
	VoxelType SimpleVolume<VoxelType>::getVoxel(int32_t uXPos, int32_t uYPos, int32_t uZPos) const
	{
		// PolyVox does not throw an exception when a voxel is out of range. Please see 'Error Handling' in the User Manual.
		POLYVOX_ASSERT(this->m_regValidRegion.containsPoint(Vector3DInt32(uXPos, uYPos, uZPos)), "Position is outside valid region");

		const int32_t blockX = uXPos >> m_uBlockSideLengthPower;
		const int32_t blockY = uYPos >> m_uBlockSideLengthPower;
		const int32_t blockZ = uZPos >> m_uBlockSideLengthPower;

		const uint16_t xOffset = static_cast<uint16_t>(uXPos - (blockX << m_uBlockSideLengthPower));
		const uint16_t yOffset = static_cast<uint16_t>(uYPos - (blockY << m_uBlockSideLengthPower));
		const uint16_t zOffset = static_cast<uint16_t>(uZPos - (blockZ << m_uBlockSideLengthPower));

		typename SimpleVolume<VoxelType>::Block* pUncompressedBlock = getUncompressedBlock(blockX, blockY, blockZ);

		return pUncompressedBlock->getVoxelAt(xOffset,yOffset,zOffset);
	}

	////////////////////////////////////////////////////////////////////////////////
	/// \param v3dPos The 3D position of the voxel
	/// \return The voxel value
	////////////////////////////////////////////////////////////////////////////////
	template <typename VoxelType>
	VoxelType SimpleVolume<VoxelType>::getVoxel(const Vector3DInt32& v3dPos) const
	{
		return getVoxel(v3dPos.getX(), v3dPos.getY(), v3dPos.getZ());
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
		if(this->m_regValidRegion.containsPoint(Vector3DInt32(uXPos, uYPos, uZPos)))
		{
			const int32_t blockX = uXPos >> m_uBlockSideLengthPower;
			const int32_t blockY = uYPos >> m_uBlockSideLengthPower;
			const int32_t blockZ = uZPos >> m_uBlockSideLengthPower;

			const uint16_t xOffset = static_cast<uint16_t>(uXPos - (blockX << m_uBlockSideLengthPower));
			const uint16_t yOffset = static_cast<uint16_t>(uYPos - (blockY << m_uBlockSideLengthPower));
			const uint16_t zOffset = static_cast<uint16_t>(uZPos - (blockZ << m_uBlockSideLengthPower));

			typename SimpleVolume<VoxelType>::Block* pUncompressedBlock = getUncompressedBlock(blockX, blockY, blockZ);

			return pUncompressedBlock->getVoxelAt(xOffset,yOffset,zOffset);
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
	VoxelType SimpleVolume<VoxelType>::getVoxelAt(const Vector3DInt32& v3dPos) const
	{
		return getVoxelAt(v3dPos.getX(), v3dPos.getY(), v3dPos.getZ());
	}

	////////////////////////////////////////////////////////////////////////////////
	/// \param uXPos The \c x position of the voxel
	/// \param uYPos The \c y position of the voxel
	/// \param uZPos The \c z position of the voxel
	/// \return The voxel value
	////////////////////////////////////////////////////////////////////////////////
	template <typename VoxelType>
	VoxelType SimpleVolume<VoxelType>::getVoxelWithWrapping(int32_t uXPos, int32_t uYPos, int32_t uZPos, WrapMode eWrapMode, VoxelType tBorder) const
	{
		switch(eWrapMode)
		{
			case WrapModes::Clamp:
			{
				//Perform clamping
				uXPos = (std::max)(uXPos, this->m_regValidRegion.getLowerX());
				uYPos = (std::max)(uYPos, this->m_regValidRegion.getLowerY());
				uZPos = (std::max)(uZPos, this->m_regValidRegion.getLowerZ());
				uXPos = (std::min)(uXPos, this->m_regValidRegion.getUpperX());
				uYPos = (std::min)(uYPos, this->m_regValidRegion.getUpperY());
				uZPos = (std::min)(uZPos, this->m_regValidRegion.getUpperZ());

				//Get the voxel value
				return getVoxel(uXPos, uYPos, uZPos);
				//No need to break as we've returned
			}
			case WrapModes::Border:
			{
				if(this->m_regValidRegion.containsPoint(uXPos, uYPos, uZPos))
				{
					return getVoxel(uXPos, uYPos, uZPos);
				}
				else
				{
					return tBorder;
				}
				//No need to break as we've returned
			}
			default:
			{
				//Should never happen
				POLYVOX_THROW(std::invalid_argument, "Wrap mode parameter has an unrecognised value.");
			}
		}
	}

	////////////////////////////////////////////////////////////////////////////////
	/// \param v3dPos The 3D position of the voxel
	/// \return The voxel value
	////////////////////////////////////////////////////////////////////////////////
	template <typename VoxelType>
	VoxelType SimpleVolume<VoxelType>::getVoxelWithWrapping(const Vector3DInt32& v3dPos, WrapMode eWrapMode, VoxelType tBorder) const
	{
		return getVoxelWithWrapping(v3dPos.getX(), v3dPos.getY(), v3dPos.getZ(), eWrapMode, tBorder);
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
		// PolyVox does not throw an exception when a voxel is out of range. Please see 'Error Handling' in the User Manual.
		POLYVOX_ASSERT(this->m_regValidRegion.containsPoint(Vector3DInt32(uXPos, uYPos, uZPos)), "Position is outside valid region");

		const int32_t blockX = uXPos >> m_uBlockSideLengthPower;
		const int32_t blockY = uYPos >> m_uBlockSideLengthPower;
		const int32_t blockZ = uZPos >> m_uBlockSideLengthPower;

		const uint16_t xOffset = uXPos - (blockX << m_uBlockSideLengthPower);
		const uint16_t yOffset = uYPos - (blockY << m_uBlockSideLengthPower);
		const uint16_t zOffset = uZPos - (blockZ << m_uBlockSideLengthPower);

		typename SimpleVolume<VoxelType>::Block* pUncompressedBlock = getUncompressedBlock(blockX, blockY, blockZ);

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
	void SimpleVolume<VoxelType>::initialise(const Region& regValidRegion, uint16_t uBlockSideLength)
	{		
		//Release mode validation
		if(uBlockSideLength < 8)
		{
			POLYVOX_THROW(std::invalid_argument, "Block side length should be at least 8");
		}
		if(uBlockSideLength > 256)
		{
			POLYVOX_THROW(std::invalid_argument, "Block side length should not be more than 256");
		}
		if(!isPowerOf2(uBlockSideLength))
		{
			POLYVOX_THROW(std::invalid_argument, "Block side length must be a power of two.");
		}

		this->m_regValidRegion = regValidRegion;

		//Compute the block side length
		m_uBlockSideLength = uBlockSideLength;
		m_uBlockSideLengthPower = logBase2(m_uBlockSideLength);
		m_uNoOfVoxelsPerBlock = m_uBlockSideLength * m_uBlockSideLength * m_uBlockSideLength;

		m_regValidRegionInBlocks.setLowerX(this->m_regValidRegion.getLowerX() >> m_uBlockSideLengthPower);
		m_regValidRegionInBlocks.setLowerY(this->m_regValidRegion.getLowerY() >> m_uBlockSideLengthPower);
		m_regValidRegionInBlocks.setLowerZ(this->m_regValidRegion.getLowerZ() >> m_uBlockSideLengthPower);
		m_regValidRegionInBlocks.setUpperX(this->m_regValidRegion.getUpperX() >> m_uBlockSideLengthPower);
		m_regValidRegionInBlocks.setUpperY(this->m_regValidRegion.getUpperY() >> m_uBlockSideLengthPower);
		m_regValidRegionInBlocks.setUpperZ(this->m_regValidRegion.getUpperZ() >> m_uBlockSideLengthPower);

		//Compute the size of the volume in blocks (and note +1 at the end)
		m_uWidthInBlocks = m_regValidRegionInBlocks.getUpperX() - m_regValidRegionInBlocks.getLowerX() + 1;
		m_uHeightInBlocks = m_regValidRegionInBlocks.getUpperY() - m_regValidRegionInBlocks.getLowerY() + 1;
		m_uDepthInBlocks = m_regValidRegionInBlocks.getUpperZ() - m_regValidRegionInBlocks.getLowerZ() + 1;
		m_uNoOfBlocksInVolume = m_uWidthInBlocks * m_uHeightInBlocks * m_uDepthInBlocks;

		//Allocate the data
		m_pBlocks = new Block[m_uNoOfBlocksInVolume];
		for(uint32_t i = 0; i < m_uNoOfBlocksInVolume; ++i)
		{
			m_pBlocks[i].initialise(m_uBlockSideLength);
		}

		//Other properties we might find useful later
		this->m_uLongestSideLength = (std::max)((std::max)(this->getWidth(),this->getHeight()),this->getDepth());
		this->m_uShortestSideLength = (std::min)((std::min)(this->getWidth(),this->getHeight()),this->getDepth());
		this->m_fDiagonalLength = sqrtf(static_cast<float>(this->getWidth() * this->getWidth() + this->getHeight() * this->getHeight() + this->getDepth() * this->getDepth()));
	}

	template <typename VoxelType>
	typename SimpleVolume<VoxelType>::Block* SimpleVolume<VoxelType>::getUncompressedBlock(int32_t uBlockX, int32_t uBlockY, int32_t uBlockZ) const
	{
		//The lower left corner of the volume could be
		//anywhere, but array indices need to start at zero.
		uBlockX -= m_regValidRegionInBlocks.getLowerX();
		uBlockY -= m_regValidRegionInBlocks.getLowerY();
		uBlockZ -= m_regValidRegionInBlocks.getLowerZ();

		POLYVOX_ASSERT(uBlockX >= 0, "Block coordinate must not be negative.");
		POLYVOX_ASSERT(uBlockY >= 0, "Block coordinate must not be negative.");
		POLYVOX_ASSERT(uBlockZ >= 0, "Block coordinate must not be negative.");

		//Compute the block index
		uint32_t uBlockIndex =
				uBlockX + 
				uBlockY * m_uWidthInBlocks + 
				uBlockZ * m_uWidthInBlocks * m_uHeightInBlocks;

		//Return the block
		return &(m_pBlocks[uBlockIndex]);
	}

	////////////////////////////////////////////////////////////////////////////////
	/// \todo This function needs reviewing for accuracy...
	///
	/// \return The number of bytes used
	////////////////////////////////////////////////////////////////////////////////
	template <typename VoxelType>
	uint32_t SimpleVolume<VoxelType>::calculateSizeInBytes(void)
	{
		uint32_t uSizeInBytes = sizeof(SimpleVolume);
		
		uint32_t uSizeOfBlockInBytes = m_uNoOfVoxelsPerBlock * sizeof(VoxelType);

		//Memory used by the blocks
		uSizeInBytes += uSizeOfBlockInBytes * (m_uNoOfBlocksInVolume);

		return uSizeInBytes;
	}

}


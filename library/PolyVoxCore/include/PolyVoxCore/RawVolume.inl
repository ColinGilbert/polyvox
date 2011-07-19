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
	/// This constructor creates a volume with a fixed size which is specified as a parameter. By default this constructor will not enable paging but you can override this if desired. If you do wish to enable paging then you are required to provide the call back function (see the other RawVolume constructor).
	/// \param regValid Specifies the minimum and maximum valid voxel positions.
	/// \param dataRequiredHandler The callback function which will be called when PolyVox tries to use data which is not currently in momory.
	/// \param dataOverflowHandler The callback function which will be called when PolyVox has too much data and needs to remove some from memory.
	/// \param bPagingEnabled Controls whether or not paging is enabled for this RawVolume.
	/// \param uBlockSideLength The size of the blocks making up the volume. Small blocks will compress/decompress faster, but there will also be more of them meaning voxel access could be slower.
	////////////////////////////////////////////////////////////////////////////////
	template <typename VoxelType>
	RawVolume<VoxelType>::RawVolume
	(
		const Region& regValid
	)
	:Volume(regValid)
	{
		//Create a volume of the right size.
		resize(regValid);
	}

	////////////////////////////////////////////////////////////////////////////////
	/// Destroys the volume
	////////////////////////////////////////////////////////////////////////////////
	template <typename VoxelType>
	RawVolume<VoxelType>::~RawVolume()
	{
		delete[] m_pData;
		m_pData = 0;
	}

	////////////////////////////////////////////////////////////////////////////////
	/// \param uXPos The \c x position of the voxel
	/// \param uYPos The \c y position of the voxel
	/// \param uZPos The \c z position of the voxel
	/// \return The voxel value
	////////////////////////////////////////////////////////////////////////////////
	template <typename VoxelType>
	VoxelType RawVolume<VoxelType>::getVoxelAt(int32_t uXPos, int32_t uYPos, int32_t uZPos) const
	{
		if(m_regValidRegion.containsPoint(Vector3DInt32(uXPos, uYPos, uZPos)))
		{
			return m_pData
			[
				uXPos + 
				uYPos * getWidth() + 
				uZPos * getWidth() * getHeight()
			];
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
	VoxelType RawVolume<VoxelType>::getVoxelAt(const Vector3DInt32& v3dPos) const
	{
		return getVoxelAt(v3dPos.getX(), v3dPos.getY(), v3dPos.getZ());
	}

	////////////////////////////////////////////////////////////////////////////////
	/// \param uXPos the \c x position of the voxel
	/// \param uYPos the \c y position of the voxel
	/// \param uZPos the \c z position of the voxel
	/// \param tValue the value to which the voxel will be set
	/// \return whether the requested position is inside the volume
	////////////////////////////////////////////////////////////////////////////////
	template <typename VoxelType>
	bool RawVolume<VoxelType>::setVoxelAt(int32_t uXPos, int32_t uYPos, int32_t uZPos, VoxelType tValue)
	{
		assert(m_regValidRegion.containsPoint(Vector3DInt32(uXPos, uYPos, uZPos)));

		m_pData
		[
			uXPos + 
			uYPos * getWidth() + 
			uZPos * getWidth() * getHeight()
		] = tValue;

		//Return true to indicate that we modified a voxel.
		return true;
	}

	////////////////////////////////////////////////////////////////////////////////
	/// \param v3dPos the 3D position of the voxel
	/// \param tValue the value to which the voxel will be set
	/// \return whether the requested position is inside the volume
	////////////////////////////////////////////////////////////////////////////////
	template <typename VoxelType>
	bool RawVolume<VoxelType>::setVoxelAt(const Vector3DInt32& v3dPos, VoxelType tValue)
	{
		return setVoxelAt(v3dPos.getX(), v3dPos.getY(), v3dPos.getZ(), tValue);
	}

	////////////////////////////////////////////////////////////////////////////////
	/// This function should probably be made internal...
	////////////////////////////////////////////////////////////////////////////////
	template <typename VoxelType>
	void RawVolume<VoxelType>::resize(const Region& regValidRegion)
	{
		m_regValidRegion = regValidRegion;

		//Ensure dimensions of the specified Region are valid
		assert(getWidth() > 0);
		assert(getHeight() > 0);
		assert(getDepth() > 0);

		//Create the data
		m_pData = new VoxelType[getWidth() * getHeight()* getDepth()];

		//Other properties we might find useful later
		m_uLongestSideLength = (std::max)((std::max)(getWidth(),getHeight()),getDepth());
		m_uShortestSideLength = (std::min)((std::min)(getWidth(),getHeight()),getDepth());
		m_fDiagonalLength = sqrtf(static_cast<float>(getWidth() * getWidth() + getHeight() * getHeight() + getDepth() * getDepth()));
	}

	////////////////////////////////////////////////////////////////////////////////
	/// Note: This function needs reviewing for accuracy...
	////////////////////////////////////////////////////////////////////////////////
	template <typename VoxelType>
	uint32_t RawVolume<VoxelType>::calculateSizeInBytes(void)
	{
		return getWidth() * getHeight() * getDepth() * sizeof(VoxelType);
	}

}


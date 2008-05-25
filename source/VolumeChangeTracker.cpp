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

#include "GradientEstimators.h"
#include "IndexedSurfacePatch.h"
#include "LinearVolume.h"
#include "MarchingCubesTables.h"
#include "VolumeChangeTracker.h"
#include "RegionGeometry.h"
#include "SurfaceExtractors.h"
#include "SurfaceVertex.h"
#include "Utility.h"
#include "Vector.h"
#include "BlockVolume.h"
#include "VolumeIterator.h"

using namespace boost;

namespace PolyVox
{

	//////////////////////////////////////////////////////////////////////////
	// VolumeChangeTracker
	//////////////////////////////////////////////////////////////////////////
	VolumeChangeTracker::VolumeChangeTracker()
		:volumeData(0)
		,useNormalSmoothing(false)
		,normalSmoothingFilterSize(1)
		,m_normalGenerationMethod(SOBEL)
		,m_bHaveGeneratedMeshes(false)
	{	
		//sceneNodes.clear();`
	}

	VolumeChangeTracker::~VolumeChangeTracker()
	{
	}

	void VolumeChangeTracker::setVolumeData(BlockVolume<boost::uint8_t>* volumeDataToSet)
	{
		volumeData = volumeDataToSet;
		volSurfaceUpToDate = new LinearVolume<bool>(PolyVox::logBase2(POLYVOX_VOLUME_SIDE_LENGTH_IN_REGIONS));
	}

	std::list<RegionGeometry> VolumeChangeTracker::getChangedRegionGeometry(void)
	{
		std::list<RegionGeometry> listChangedRegionGeometry;

		//Regenerate meshes.
		for(uint16_t regionZ = 0; regionZ < POLYVOX_VOLUME_SIDE_LENGTH_IN_REGIONS; ++regionZ)
		//for(uint16_t regionZ = 6; regionZ < 7; ++regionZ)
		{		
			for(uint16_t regionY = 0; regionY < POLYVOX_VOLUME_SIDE_LENGTH_IN_REGIONS; ++regionY)
			//for(uint16_t regionY = 6; regionY < 7; ++regionY)
			{
				for(uint16_t regionX = 0; regionX < POLYVOX_VOLUME_SIDE_LENGTH_IN_REGIONS; ++regionX)
				//for(uint16_t regionX = 3; regionX < 4; ++regionX)
				{
					//if(surfaceUpToDate[regionX][regionY][regionZ] == false)
					if(volSurfaceUpToDate->getVoxelAt(regionX, regionY, regionZ) == false)
					{
						//Generate the surface
						RegionGeometry regionGeometry;
						regionGeometry.m_patchSingleMaterial = new IndexedSurfacePatch(false);
						regionGeometry.m_patchMultiMaterial = new IndexedSurfacePatch(true);
						regionGeometry.m_v3dRegionPosition = Vector3DInt32(regionX, regionY, regionZ);

						const uint16_t firstX = regionX * POLYVOX_REGION_SIDE_LENGTH;
						const uint16_t firstY = regionY * POLYVOX_REGION_SIDE_LENGTH;
						const uint16_t firstZ = regionZ * POLYVOX_REGION_SIDE_LENGTH;
						const uint16_t lastX = firstX + POLYVOX_REGION_SIDE_LENGTH-1;
						const uint16_t lastY = firstY + POLYVOX_REGION_SIDE_LENGTH-1;
						const uint16_t lastZ = firstZ + POLYVOX_REGION_SIDE_LENGTH-1;

						generateRoughMeshDataForRegion(volumeData, Region(Vector3DInt32(firstX, firstY, firstZ), Vector3DInt32(lastX, lastY, lastZ)), regionGeometry.m_patchSingleMaterial, regionGeometry.m_patchMultiMaterial);

						regionGeometry.m_bContainsSingleMaterialPatch = regionGeometry.m_patchSingleMaterial->getVertices().size() > 0;
						regionGeometry.m_bContainsMultiMaterialPatch = regionGeometry.m_patchMultiMaterial->getVertices().size() > 0;
						regionGeometry.m_bIsEmpty = ((regionGeometry.m_patchSingleMaterial->getVertices().size() == 0) && (regionGeometry.m_patchMultiMaterial->getIndices().size() == 0));

						listChangedRegionGeometry.push_back(regionGeometry);
					}
				}
			}
		}

		return listChangedRegionGeometry;
	}

	void VolumeChangeTracker::setAllUpToDateFlagsTo(bool newUpToDateValue)
	{
		for(uint16_t blockZ = 0; blockZ < POLYVOX_VOLUME_SIDE_LENGTH_IN_REGIONS; ++blockZ)
		{
			for(uint16_t blockY = 0; blockY < POLYVOX_VOLUME_SIDE_LENGTH_IN_REGIONS; ++blockY)
			{
				for(uint16_t blockX = 0; blockX < POLYVOX_VOLUME_SIDE_LENGTH_IN_REGIONS; ++blockX)
				{
					volSurfaceUpToDate->setVoxelAt(blockX, blockY, blockZ, newUpToDateValue);
				}
			}
		}
	}

	void VolumeChangeTracker::markRegionChanged(uint16_t firstX, uint16_t firstY, uint16_t firstZ, uint16_t lastX, uint16_t lastY, uint16_t lastZ)
	{
		const uint16_t firstRegionX = firstX >> POLYVOX_REGION_SIDE_LENGTH_POWER;
		const uint16_t firstRegionY = firstY >> POLYVOX_REGION_SIDE_LENGTH_POWER;
		const uint16_t firstRegionZ = firstZ >> POLYVOX_REGION_SIDE_LENGTH_POWER;

		const uint16_t lastRegionX = lastX >> POLYVOX_REGION_SIDE_LENGTH_POWER;
		const uint16_t lastRegionY = lastY >> POLYVOX_REGION_SIDE_LENGTH_POWER;
		const uint16_t lastRegionZ = lastZ >> POLYVOX_REGION_SIDE_LENGTH_POWER;

		for(uint16_t zCt = firstRegionZ; zCt <= lastRegionZ; zCt++)
		{
			for(uint16_t yCt = firstRegionY; yCt <= lastRegionY; yCt++)
			{
				for(uint16_t xCt = firstRegionX; xCt <= lastRegionX; xCt++)
				{
					//surfaceUpToDate[xCt][yCt][zCt] = false;
					volSurfaceUpToDate->setVoxelAt(xCt,yCt,zCt,false);
				}
			}
		}
	}

	uint16_t VolumeChangeTracker::getSideLength(void)
	{
		return volumeData->getSideLength();
	}

	uint8_t VolumeChangeTracker::getMaterialIndexAt(uint16_t uX, uint16_t uY, uint16_t uZ)
	{
		if(volumeData->containsPoint(Vector3DInt32(uX,uY,uZ),0))
		{
			VolumeIterator<boost::uint8_t> volIter(*volumeData);
			volIter.setPosition(uX,uY,uZ);
			return volIter.getVoxel();
		}
		else
		{
			return 0;
		}
	}

	void VolumeChangeTracker::setNormalGenerationMethod(NormalGenerationMethod method)
	{
		m_normalGenerationMethod = method;
	}

	const BlockVolume<boost::uint8_t>* VolumeChangeTracker::getVolumeData(void) const
	{
		return volumeData;
	}

	void VolumeChangeTracker::setVoxelAt(boost::uint16_t x, boost::uint16_t y, boost::uint16_t z, boost::uint8_t value)
	{
		//FIXME - rather than creating a iterator each time we should have one stored
		VolumeIterator<boost::uint8_t> iterVol(*volumeData);
		iterVol.setPosition(x,y,z);
		iterVol.setVoxel(value);
		
		//If we are not on a boundary, just mark one region.
		if((x % POLYVOX_REGION_SIDE_LENGTH != 0) &&
			(x % POLYVOX_REGION_SIDE_LENGTH != POLYVOX_REGION_SIDE_LENGTH-1) &&
			(y % POLYVOX_REGION_SIDE_LENGTH != 0) &&
			(y % POLYVOX_REGION_SIDE_LENGTH != POLYVOX_REGION_SIDE_LENGTH-1) &&
			(z % POLYVOX_REGION_SIDE_LENGTH != 0) &&
			(z % POLYVOX_REGION_SIDE_LENGTH != POLYVOX_REGION_SIDE_LENGTH-1))
		{
			volSurfaceUpToDate->setVoxelAt(x >> POLYVOX_REGION_SIDE_LENGTH_POWER, y >> POLYVOX_REGION_SIDE_LENGTH_POWER, z >> POLYVOX_REGION_SIDE_LENGTH_POWER, false);
		}
		else //Mark surrounding regions as well
		{
			const uint16_t regionX = x >> POLYVOX_REGION_SIDE_LENGTH_POWER;
			const uint16_t regionY = y >> POLYVOX_REGION_SIDE_LENGTH_POWER;
			const uint16_t regionZ = z >> POLYVOX_REGION_SIDE_LENGTH_POWER;

			const uint16_t minRegionX = (std::max)(uint16_t(0),uint16_t(regionX-1));
			const uint16_t minRegionY = (std::max)(uint16_t(0),uint16_t(regionY-1));
			const uint16_t minRegionZ = (std::max)(uint16_t(0),uint16_t(regionZ-1));

			const uint16_t maxRegionX = (std::min)(uint16_t(POLYVOX_VOLUME_SIDE_LENGTH_IN_REGIONS-1),uint16_t(regionX+1));
			const uint16_t maxRegionY = (std::min)(uint16_t(POLYVOX_VOLUME_SIDE_LENGTH_IN_REGIONS-1),uint16_t(regionY+1));
			const uint16_t maxRegionZ = (std::min)(uint16_t(POLYVOX_VOLUME_SIDE_LENGTH_IN_REGIONS-1),uint16_t(regionZ+1));

			for(uint16_t zCt = minRegionZ; zCt <= maxRegionZ; zCt++)
			{
				for(uint16_t yCt = minRegionY; yCt <= maxRegionY; yCt++)
				{
					for(uint16_t xCt = minRegionX; xCt <= maxRegionX; xCt++)
					{
						volSurfaceUpToDate->setVoxelAt(xCt,yCt,zCt,false);
					}
				}
			}
		}
	}
}

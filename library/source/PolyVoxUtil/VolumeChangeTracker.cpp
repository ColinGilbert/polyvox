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

#include "PolyVoxUtil/VolumeChangeTracker.h"

#include "PolyVoxCore/GradientEstimators.h"
#include "PolyVoxCore/IndexedSurfacePatch.h"
#include "PolyVoxCore/LinearVolume.h"
#include "PolyVoxCore/MarchingCubesTables.h"
#include "PolyVoxCore/SurfaceExtractors.h"
#include "PolyVoxCore/SurfaceVertex.h"
#include "PolyVoxCore/Utility.h"
#include "PolyVoxCore/Vector.h"
#include "PolyVoxCore/BlockVolume.h"
#include "PolyVoxCore/BlockVolumeIterator.h"

using namespace std;

namespace PolyVox
{

	//////////////////////////////////////////////////////////////////////////
	// VolumeChangeTracker
	//////////////////////////////////////////////////////////////////////////
	VolumeChangeTracker::VolumeChangeTracker()
		:m_bIsLocked(false)
		,volumeData(0)
	{	
	}

	VolumeChangeTracker::~VolumeChangeTracker()
	{
	}

	void VolumeChangeTracker::setVolumeData(BlockVolume<uint8>* volumeDataToSet)
	{
		volumeData = volumeDataToSet;
		volRegionUpToDate = new LinearVolume<bool>(PolyVox::logBase2(POLYVOX_VOLUME_SIDE_LENGTH_IN_REGIONS));
	}

	

	void VolumeChangeTracker::getChangedRegions(std::list<Region>& listToFill) const
	{
		//Clear the list
		listToFill.clear();

		//Regenerate meshes.
		for(uint16 regionZ = 0; regionZ < POLYVOX_VOLUME_SIDE_LENGTH_IN_REGIONS; ++regionZ)
		//for(uint16 regionZ = 0; regionZ < 1; ++regionZ)
		{		
			for(uint16 regionY = 0; regionY < POLYVOX_VOLUME_SIDE_LENGTH_IN_REGIONS; ++regionY)
			//for(uint16 regionY = 0; regionY < 2; ++regionY)
			{
				for(uint16 regionX = 0; regionX < POLYVOX_VOLUME_SIDE_LENGTH_IN_REGIONS; ++regionX)
				//for(uint16 regionX = 0; regionX < 2; ++regionX)
				{
					if(volRegionUpToDate->getVoxelAt(regionX, regionY, regionZ) == false)
					{
						const uint16 firstX = regionX * POLYVOX_REGION_SIDE_LENGTH;
						const uint16 firstY = regionY * POLYVOX_REGION_SIDE_LENGTH;
						const uint16 firstZ = regionZ * POLYVOX_REGION_SIDE_LENGTH;
						const uint16 lastX = firstX + POLYVOX_REGION_SIDE_LENGTH;
						const uint16 lastY = firstY + POLYVOX_REGION_SIDE_LENGTH;
						const uint16 lastZ = firstZ + POLYVOX_REGION_SIDE_LENGTH;

						listToFill.push_back(Region(Vector3DInt32(firstX, firstY, firstZ), Vector3DInt32(lastX, lastY, lastZ)));
					}
				}
			}
		}
	}

	void VolumeChangeTracker::setAllRegionsUpToDate(bool newUpToDateValue)
	{
		for(uint16 blockZ = 0; blockZ < POLYVOX_VOLUME_SIDE_LENGTH_IN_REGIONS; ++blockZ)
		{
			for(uint16 blockY = 0; blockY < POLYVOX_VOLUME_SIDE_LENGTH_IN_REGIONS; ++blockY)
			{
				for(uint16 blockX = 0; blockX < POLYVOX_VOLUME_SIDE_LENGTH_IN_REGIONS; ++blockX)
				{
					volRegionUpToDate->setVoxelAt(blockX, blockY, blockZ, newUpToDateValue);
				}
			}
		}
	}

	uint16 VolumeChangeTracker::getSideLength(void)
	{
		return volumeData->getSideLength();
	}

	Region VolumeChangeTracker::getEnclosingRegion(void) const
	{
		return volumeData->getEnclosingRegion();
	}

	uint8 VolumeChangeTracker::getVoxelAt(const Vector3DUint16& pos)
	{
		return getVoxelAt(pos.getX(), pos.getY(), pos.getZ());
	}

	uint8 VolumeChangeTracker::getVoxelAt(uint16 uX, uint16 uY, uint16 uZ)
	{
		assert(uX < volumeData->getSideLength());
		assert(uY < volumeData->getSideLength());
		assert(uZ < volumeData->getSideLength());

		BlockVolumeIterator<uint8> volIter(*volumeData);
		volIter.setPosition(uX,uY,uZ);
		return volIter.getVoxel();
	}

	BlockVolume<uint8>* VolumeChangeTracker::getVolumeData(void) const
	{
		return volumeData;
	}

	void VolumeChangeTracker::setVoxelAt(uint16 x, uint16 y, uint16 z, uint8 value)
	{
		//FIXME - rather than creating a iterator each time we should have one stored
		BlockVolumeIterator<uint8> iterVol(*volumeData);
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
			volRegionUpToDate->setVoxelAt(x >> POLYVOX_REGION_SIDE_LENGTH_POWER, y >> POLYVOX_REGION_SIDE_LENGTH_POWER, z >> POLYVOX_REGION_SIDE_LENGTH_POWER, false);
		}
		else //Mark surrounding regions as well
		{
			const uint16 regionX = x >> POLYVOX_REGION_SIDE_LENGTH_POWER;
			const uint16 regionY = y >> POLYVOX_REGION_SIDE_LENGTH_POWER;
			const uint16 regionZ = z >> POLYVOX_REGION_SIDE_LENGTH_POWER;

			const uint16 minRegionX = (std::max)(uint16(0),uint16(regionX-1));
			const uint16 minRegionY = (std::max)(uint16(0),uint16(regionY-1));
			const uint16 minRegionZ = (std::max)(uint16(0),uint16(regionZ-1));

			const uint16 maxRegionX = (std::min)(uint16(POLYVOX_VOLUME_SIDE_LENGTH_IN_REGIONS-1),uint16(regionX+1));
			const uint16 maxRegionY = (std::min)(uint16(POLYVOX_VOLUME_SIDE_LENGTH_IN_REGIONS-1),uint16(regionY+1));
			const uint16 maxRegionZ = (std::min)(uint16(POLYVOX_VOLUME_SIDE_LENGTH_IN_REGIONS-1),uint16(regionZ+1));

			for(uint16 zCt = minRegionZ; zCt <= maxRegionZ; zCt++)
			{
				for(uint16 yCt = minRegionY; yCt <= maxRegionY; yCt++)
				{
					for(uint16 xCt = minRegionX; xCt <= maxRegionX; xCt++)
					{
						volRegionUpToDate->setVoxelAt(xCt,yCt,zCt,false);
					}
				}
			}
		}
	}

	void VolumeChangeTracker::setLockedVoxelAt(uint16 x, uint16 y, uint16 z, uint8 value)
	{
		assert(m_bIsLocked);

		//FIXME - rather than creating a iterator each time we should have one stored
		BlockVolumeIterator<uint8> iterVol(*volumeData);
		iterVol.setPosition(x,y,z);
		iterVol.setVoxel(value);
	}

	void VolumeChangeTracker::lockRegion(const Region& regToLock)
	{
		if(m_bIsLocked)
		{
			throw std::logic_error("A region is already locked. Please unlock it before locking another.");
		}

		m_regLastLocked = regToLock;
		m_bIsLocked = true;
	}

	void VolumeChangeTracker::unlockRegion(void)
	{
		if(!m_bIsLocked)
		{
			throw std::logic_error("No region is locked. You must lock a region before you can unlock it.");
		}

		const uint16 firstRegionX = m_regLastLocked.getLowerCorner().getX() >> POLYVOX_REGION_SIDE_LENGTH_POWER;
		const uint16 firstRegionY = m_regLastLocked.getLowerCorner().getY() >> POLYVOX_REGION_SIDE_LENGTH_POWER;
		const uint16 firstRegionZ = m_regLastLocked.getLowerCorner().getZ() >> POLYVOX_REGION_SIDE_LENGTH_POWER;

		const uint16 lastRegionX = m_regLastLocked.getUpperCorner().getX() >> POLYVOX_REGION_SIDE_LENGTH_POWER;
		const uint16 lastRegionY = m_regLastLocked.getUpperCorner().getY() >> POLYVOX_REGION_SIDE_LENGTH_POWER;
		const uint16 lastRegionZ = m_regLastLocked.getUpperCorner().getZ() >> POLYVOX_REGION_SIDE_LENGTH_POWER;

		for(uint16 zCt = firstRegionZ; zCt <= lastRegionZ; zCt++)
		{
			for(uint16 yCt = firstRegionY; yCt <= lastRegionY; yCt++)
			{
				for(uint16 xCt = firstRegionX; xCt <= lastRegionX; xCt++)
				{
					volRegionUpToDate->setVoxelAt(xCt,yCt,zCt,false);
				}
			}
		}

		m_bIsLocked = false;
	}
}

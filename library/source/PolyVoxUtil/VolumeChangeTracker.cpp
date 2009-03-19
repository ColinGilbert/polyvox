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
#include "PolyVoxCore/MarchingCubesTables.h"
#include "PolyVoxCore/SurfaceExtractors.h"
#include "PolyVoxCore/SurfaceVertex.h"
#include "PolyVoxCore/Utility.h"
#include "PolyVoxCore/Vector.h"
#include "PolyVoxCore/Volume.h"
#include "PolyVoxCore/VolumeIterator.h"

using namespace std;

namespace PolyVox
{
	int32 VolumeChangeTracker::m_iCurrentTime = 0;

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

	void VolumeChangeTracker::setVolumeData(Volume<uint8>* volumeDataToSet)
	{
		volumeData = volumeDataToSet;
		volRegionLastModified = new Block<int32>(PolyVox::logBase2(POLYVOX_VOLUME_SIDE_LENGTH_IN_REGIONS));
	}

	void VolumeChangeTracker::setAllRegionsModified(void)
	{
		for(uint16 blockZ = 0; blockZ < POLYVOX_VOLUME_SIDE_LENGTH_IN_REGIONS; ++blockZ)
		{
			for(uint16 blockY = 0; blockY < POLYVOX_VOLUME_SIDE_LENGTH_IN_REGIONS; ++blockY)
			{
				for(uint16 blockX = 0; blockX < POLYVOX_VOLUME_SIDE_LENGTH_IN_REGIONS; ++blockX)
				{
					volRegionLastModified->setVoxelAt(blockX, blockY, blockZ, m_iCurrentTime);
					++m_iCurrentTime;
				}
			}
		}
	}

	int32 VolumeChangeTracker::getCurrentTime(void) const
	{
		return m_iCurrentTime;
	}

	uint16 VolumeChangeTracker::getSideLength(void)
	{
		return volumeData->getSideLength();
	}

	Region VolumeChangeTracker::getEnclosingRegion(void) const
	{
		return volumeData->getEnclosingRegion();
	}

	int32 VolumeChangeTracker::getLastModifiedTimeForRegion(uint16 uX, uint16 uY, uint16 uZ)
	{
		return volRegionLastModified->getVoxelAt(uX, uY, uZ);
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

		VolumeIterator<uint8> volIter(*volumeData);
		volIter.setPosition(uX,uY,uZ);
		return volIter.getVoxel();
	}

	Volume<uint8>* VolumeChangeTracker::getVolumeData(void) const
	{
		return volumeData;
	}

	//NOTE - Document the fact that the time stamp is incremented at the start, not the end.
	void VolumeChangeTracker::setVoxelAt(uint16 x, uint16 y, uint16 z, uint8 value)
	{
		++m_iCurrentTime;
		//FIXME - rather than creating a iterator each time we should have one stored
		//VolumeIterator<uint8> iterVol(*volumeData);
		/*iterVol.setPosition(x,y,z);
		iterVol.setVoxel(value);*/

		volumeData->setVoxelAt(x,y,z,value);
		
		//If we are not on a boundary, just mark one region.
		if((x % POLYVOX_REGION_SIDE_LENGTH != 0) &&
			(x % POLYVOX_REGION_SIDE_LENGTH != POLYVOX_REGION_SIDE_LENGTH-1) &&
			(y % POLYVOX_REGION_SIDE_LENGTH != 0) &&
			(y % POLYVOX_REGION_SIDE_LENGTH != POLYVOX_REGION_SIDE_LENGTH-1) &&
			(z % POLYVOX_REGION_SIDE_LENGTH != 0) &&
			(z % POLYVOX_REGION_SIDE_LENGTH != POLYVOX_REGION_SIDE_LENGTH-1))
		{
			volRegionLastModified->setVoxelAt(x >> POLYVOX_REGION_SIDE_LENGTH_POWER, y >> POLYVOX_REGION_SIDE_LENGTH_POWER, z >> POLYVOX_REGION_SIDE_LENGTH_POWER, m_iCurrentTime);
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
						volRegionLastModified->setVoxelAt(xCt,yCt,zCt,m_iCurrentTime);
					}
				}
			}
		}
		//++m_iCurrentTime;
	}

	void VolumeChangeTracker::setLockedVoxelAt(uint16 x, uint16 y, uint16 z, uint8 value)
	{
		assert(m_bIsLocked);

		//FIXME - rather than creating a iterator each time we should have one stored
		/*VolumeIterator<uint8> iterVol(*volumeData);
		iterVol.setPosition(x,y,z);
		iterVol.setVoxel(value);*/
		volumeData->setVoxelAt(x,y,z,value);
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
		++m_iCurrentTime;
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
					volRegionLastModified->setVoxelAt(xCt,yCt,zCt,m_iCurrentTime);
				}
			}
		}

		//++m_iCurrentTime;
		m_bIsLocked = false;
	}
}

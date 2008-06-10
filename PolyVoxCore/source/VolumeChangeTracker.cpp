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
#include "BlockVolumeIterator.h"

using namespace boost;

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

	void VolumeChangeTracker::setVolumeData(BlockVolume<boost::uint8_t>* volumeDataToSet)
	{
		volumeData = volumeDataToSet;
		volRegionUpToDate = new LinearVolume<bool>(PolyVox::logBase2(POLYVOX_VOLUME_SIDE_LENGTH_IN_REGIONS));
	}

	

	void VolumeChangeTracker::getChangedRegions(std::list<Region>& listToFill) const
	{
		//Clear the list
		listToFill.clear();

		//Regenerate meshes.
		for(uint16_t regionZ = 0; regionZ < POLYVOX_VOLUME_SIDE_LENGTH_IN_REGIONS-1; ++regionZ)
		//for(uint16_t regionZ = 0; regionZ < 1; ++regionZ)
		{		
			for(uint16_t regionY = 0; regionY < POLYVOX_VOLUME_SIDE_LENGTH_IN_REGIONS-1; ++regionY)
			//for(uint16_t regionY = 0; regionY < 2; ++regionY)
			{
				for(uint16_t regionX = 0; regionX < POLYVOX_VOLUME_SIDE_LENGTH_IN_REGIONS-1; ++regionX)
				//for(uint16_t regionX = 0; regionX < 2; ++regionX)
				{
					if(volRegionUpToDate->getVoxelAt(regionX, regionY, regionZ) == false)
					{
						const uint16_t firstX = regionX * POLYVOX_REGION_SIDE_LENGTH;
						const uint16_t firstY = regionY * POLYVOX_REGION_SIDE_LENGTH;
						const uint16_t firstZ = regionZ * POLYVOX_REGION_SIDE_LENGTH;
						const uint16_t lastX = firstX + POLYVOX_REGION_SIDE_LENGTH;
						const uint16_t lastY = firstY + POLYVOX_REGION_SIDE_LENGTH;
						const uint16_t lastZ = firstZ + POLYVOX_REGION_SIDE_LENGTH;

						listToFill.push_back(Region(Vector3DInt32(firstX, firstY, firstZ), Vector3DInt32(lastX, lastY, lastZ)));
					}
				}
			}
		}
	}

	void VolumeChangeTracker::setAllRegionsUpToDate(bool newUpToDateValue)
	{
		for(uint16_t blockZ = 0; blockZ < POLYVOX_VOLUME_SIDE_LENGTH_IN_REGIONS; ++blockZ)
		{
			for(uint16_t blockY = 0; blockY < POLYVOX_VOLUME_SIDE_LENGTH_IN_REGIONS; ++blockY)
			{
				for(uint16_t blockX = 0; blockX < POLYVOX_VOLUME_SIDE_LENGTH_IN_REGIONS; ++blockX)
				{
					volRegionUpToDate->setVoxelAt(blockX, blockY, blockZ, newUpToDateValue);
				}
			}
		}
	}

	uint16_t VolumeChangeTracker::getSideLength(void)
	{
		return volumeData->getSideLength();
	}

	Region VolumeChangeTracker::getEnclosingRegion(void) const
	{
		return volumeData->getEnclosingRegion();
	}

	uint8_t VolumeChangeTracker::getVoxelAt(const Vector3DUint16& pos)
	{
		return getVoxelAt(pos.getX(), pos.getY(), pos.getZ());
	}

	uint8_t VolumeChangeTracker::getVoxelAt(uint16_t uX, uint16_t uY, uint16_t uZ)
	{
		assert(uX < volumeData->getSideLength());
		assert(uY < volumeData->getSideLength());
		assert(uZ < volumeData->getSideLength());

		BlockVolumeIterator<boost::uint8_t> volIter(*volumeData);
		volIter.setPosition(uX,uY,uZ);
		return volIter.getVoxel();
	}

	BlockVolume<boost::uint8_t>* VolumeChangeTracker::getVolumeData(void) const
	{
		return volumeData;
	}

	void VolumeChangeTracker::setVoxelAt(boost::uint16_t x, boost::uint16_t y, boost::uint16_t z, boost::uint8_t value)
	{
		//FIXME - rather than creating a iterator each time we should have one stored
		BlockVolumeIterator<boost::uint8_t> iterVol(*volumeData);
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
						volRegionUpToDate->setVoxelAt(xCt,yCt,zCt,false);
					}
				}
			}
		}
	}

	void VolumeChangeTracker::setLockedVoxelAt(boost::uint16_t x, boost::uint16_t y, boost::uint16_t z, boost::uint8_t value)
	{
		assert(m_bIsLocked);

		//FIXME - rather than creating a iterator each time we should have one stored
		BlockVolumeIterator<boost::uint8_t> iterVol(*volumeData);
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

		const uint16_t firstRegionX = m_regLastLocked.getLowerCorner().getX() >> POLYVOX_REGION_SIDE_LENGTH_POWER;
		const uint16_t firstRegionY = m_regLastLocked.getLowerCorner().getY() >> POLYVOX_REGION_SIDE_LENGTH_POWER;
		const uint16_t firstRegionZ = m_regLastLocked.getLowerCorner().getZ() >> POLYVOX_REGION_SIDE_LENGTH_POWER;

		const uint16_t lastRegionX = m_regLastLocked.getUpperCorner().getX() >> POLYVOX_REGION_SIDE_LENGTH_POWER;
		const uint16_t lastRegionY = m_regLastLocked.getUpperCorner().getY() >> POLYVOX_REGION_SIDE_LENGTH_POWER;
		const uint16_t lastRegionZ = m_regLastLocked.getUpperCorner().getZ() >> POLYVOX_REGION_SIDE_LENGTH_POWER;

		for(uint16_t zCt = firstRegionZ; zCt <= lastRegionZ; zCt++)
		{
			for(uint16_t yCt = firstRegionY; yCt <= lastRegionY; yCt++)
			{
				for(uint16_t xCt = firstRegionX; xCt <= lastRegionX; xCt++)
				{
					volRegionUpToDate->setVoxelAt(xCt,yCt,zCt,false);
				}
			}
		}

		m_bIsLocked = false;
	}
}

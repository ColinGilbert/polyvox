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

#include "VolumeChangeTracker.h"

#include "GradientEstimators.h"
#include "IndexedSurfacePatch.h"
#include "PolyVoxImpl/MarchingCubesTables.h"
#include "SurfaceExtractors.h"
#include "SurfaceVertex.h"
#include "PolyVoxImpl/Utility.h"
#include "Vector.h"
#include "Volume.h"
#include "VolumeIterator.h"

using namespace std;

namespace PolyVox
{
	int32_t VolumeChangeTracker::m_iCurrentTime = 0;

	//////////////////////////////////////////////////////////////////////////
	// VolumeChangeTracker
	//////////////////////////////////////////////////////////////////////////
	VolumeChangeTracker::VolumeChangeTracker(Volume<uint8_t>* volumeDataToSet, uint16_t regionSideLength)
		:m_bIsLocked(false)
		,volumeData(0)
		,m_uRegionSideLength(regionSideLength)
	{	
		volumeData = volumeDataToSet;
		m_uVolumeWidthInRegions = volumeData->getWidth() / m_uRegionSideLength;
		m_uVolumeHeightInRegions = volumeData->getHeight() / m_uRegionSideLength;
		m_uVolumeDepthInRegions = volumeData->getDepth() / m_uRegionSideLength;
		m_uRegionSideLengthPower = PolyVox::logBase2(m_uRegionSideLength);

		uint16_t uLongestSideLength = (std::max)((std::max)(m_uVolumeWidthInRegions,m_uVolumeHeightInRegions),m_uVolumeDepthInRegions);

		volRegionLastModified = new BlockData<int32_t>(uLongestSideLength); //FIXME - Maybe using a block here isn't optimal as it must always be cubic...
	}

	VolumeChangeTracker::~VolumeChangeTracker()
	{
	}

	void VolumeChangeTracker::setAllRegionsModified(void)
	{
		for(uint16_t blockZ = 0; blockZ < m_uVolumeDepthInRegions; ++blockZ)
		{
			for(uint16_t blockY = 0; blockY < m_uVolumeHeightInRegions; ++blockY)
			{
				for(uint16_t blockX = 0; blockX < m_uVolumeWidthInRegions; ++blockX)
				{
					volRegionLastModified->setVoxelAt(blockX, blockY, blockZ, m_iCurrentTime);
					++m_iCurrentTime;
				}
			}
		}
	}

	int32_t VolumeChangeTracker::getCurrentTime(void) const
	{
		return m_iCurrentTime;
	}

	uint16_t VolumeChangeTracker::getWidth(void)
	{
		return volumeData->getWidth();
	}

	uint16_t VolumeChangeTracker::getHeight(void)
	{
		return volumeData->getHeight();
	}

	uint16_t VolumeChangeTracker::getDepth(void)
	{
		return volumeData->getDepth();
	}

	Region VolumeChangeTracker::getEnclosingRegion(void) const
	{
		return volumeData->getEnclosingRegion();
	}

	int32_t VolumeChangeTracker::getLastModifiedTimeForRegion(uint16_t uX, uint16_t uY, uint16_t uZ)
	{
		return volRegionLastModified->getVoxelAt(uX, uY, uZ);
	}

	uint8_t VolumeChangeTracker::getVoxelAt(const Vector3DUint16& pos)
	{
		return getVoxelAt(pos.getX(), pos.getY(), pos.getZ());
	}

	uint8_t VolumeChangeTracker::getVoxelAt(uint16_t uX, uint16_t uY, uint16_t uZ)
	{
		assert(uX < volumeData->getWidth());
		assert(uY < volumeData->getHeight());
		assert(uZ < volumeData->getDepth());

		VolumeIterator<uint8_t> volIter(*volumeData);
		volIter.setPosition(uX,uY,uZ);
		return volIter.getVoxel();
	}

	Volume<uint8_t>* VolumeChangeTracker::getVolumeData(void) const
	{
		return volumeData;
	}

	//NOTE - Document the fact that the time stamp is incremented at the start, not the end.
	void VolumeChangeTracker::setVoxelAt(uint16_t x, uint16_t y, uint16_t z, uint8_t value)
	{
		++m_iCurrentTime;
		//FIXME - rather than creating a iterator each time we should have one stored
		//VolumeIterator<uint8_t> iterVol(*volumeData);
		/*iterVol.setPosition(x,y,z);
		iterVol.setVoxel(value);*/

		volumeData->setVoxelAt(x,y,z,value);
		
		//If we are not on a boundary, just mark one region.
		if((x % m_uRegionSideLength != 0) &&
			(x % m_uRegionSideLength != m_uRegionSideLength-1) &&
			(y % m_uRegionSideLength != 0) &&
			(y % m_uRegionSideLength != m_uRegionSideLength-1) &&
			(z % m_uRegionSideLength != 0) &&
			(z % m_uRegionSideLength != m_uRegionSideLength-1))
		{
			volRegionLastModified->setVoxelAt(x >> m_uRegionSideLengthPower, y >> m_uRegionSideLengthPower, z >> m_uRegionSideLengthPower, m_iCurrentTime);
		}
		else //Mark surrounding regions as well
		{
			const uint16_t regionX = x >> m_uRegionSideLengthPower;
			const uint16_t regionY = y >> m_uRegionSideLengthPower;
			const uint16_t regionZ = z >> m_uRegionSideLengthPower;

			const uint16_t minRegionX = (std::max)(uint16_t(0),uint16_t(regionX-1));
			const uint16_t minRegionY = (std::max)(uint16_t(0),uint16_t(regionY-1));
			const uint16_t minRegionZ = (std::max)(uint16_t(0),uint16_t(regionZ-1));

			const uint16_t maxRegionX = (std::min)(uint16_t(m_uVolumeWidthInRegions-1),uint16_t(regionX+1));
			const uint16_t maxRegionY = (std::min)(uint16_t(m_uVolumeHeightInRegions-1),uint16_t(regionY+1));
			const uint16_t maxRegionZ = (std::min)(uint16_t(m_uVolumeDepthInRegions-1),uint16_t(regionZ+1));

			for(uint16_t zCt = minRegionZ; zCt <= maxRegionZ; zCt++)
			{
				for(uint16_t yCt = minRegionY; yCt <= maxRegionY; yCt++)
				{
					for(uint16_t xCt = minRegionX; xCt <= maxRegionX; xCt++)
					{
						volRegionLastModified->setVoxelAt(xCt,yCt,zCt,m_iCurrentTime);
					}
				}
			}
		}
		//++m_iCurrentTime;
	}

	void VolumeChangeTracker::setLockedVoxelAt(uint16_t x, uint16_t y, uint16_t z, uint8_t value)
	{
		assert(m_bIsLocked);

		//FIXME - rather than creating a iterator each time we should have one stored
		/*VolumeIterator<uint8_t> iterVol(*volumeData);
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

		const uint16_t firstRegionX = m_regLastLocked.getLowerCorner().getX() >> m_uRegionSideLengthPower;
		const uint16_t firstRegionY = m_regLastLocked.getLowerCorner().getY() >> m_uRegionSideLengthPower;
		const uint16_t firstRegionZ = m_regLastLocked.getLowerCorner().getZ() >> m_uRegionSideLengthPower;

		const uint16_t lastRegionX = m_regLastLocked.getUpperCorner().getX() >> m_uRegionSideLengthPower;
		const uint16_t lastRegionY = m_regLastLocked.getUpperCorner().getY() >> m_uRegionSideLengthPower;
		const uint16_t lastRegionZ = m_regLastLocked.getUpperCorner().getZ() >> m_uRegionSideLengthPower;

		for(uint16_t zCt = firstRegionZ; zCt <= lastRegionZ; zCt++)
		{
			for(uint16_t yCt = firstRegionY; yCt <= lastRegionY; yCt++)
			{
				for(uint16_t xCt = firstRegionX; xCt <= lastRegionX; xCt++)
				{
					volRegionLastModified->setVoxelAt(xCt,yCt,zCt,m_iCurrentTime);
				}
			}
		}

		//++m_iCurrentTime;
		m_bIsLocked = false;
	}
}

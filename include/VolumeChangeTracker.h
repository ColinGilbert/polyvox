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

#ifndef __PolyVox_VolumeChangeTracker_H__
#define __PolyVox_VolumeChangeTracker_H__

#include <list>

#include "boost/cstdint.hpp"

#include "Constants.h"
#include "PolyVoxForwardDeclarations.h"
#include "Region.h"
#include "TypeDef.h"

namespace PolyVox
{	
	/// Voxel scene manager
	class POLYVOX_API VolumeChangeTracker
	{
	public:
		//Constructors, etc
		VolumeChangeTracker();
		~VolumeChangeTracker();

		//Getters
		void getChangedRegions(std::list<Region>& listToFill) const;
		Region getEnclosingRegion(void) const;		
		boost::uint16_t getSideLength(void);
		BlockVolume<boost::uint8_t>* getVolumeData(void) const;
		boost::uint8_t getVoxelAt(const Vector3DUint16& pos);
		boost::uint8_t getVoxelAt(boost::uint16_t uX, boost::uint16_t uY, boost::uint16_t uZ);

		//Setters
		void setAllRegionsUpToDate(bool newUpToDateValue);
		void setLockedVoxelAt(boost::uint16_t x, boost::uint16_t y, boost::uint16_t z, boost::uint8_t value);		
		void setVolumeData(BlockVolume<boost::uint8_t>* volumeDataToSet);
		void setVoxelAt(boost::uint16_t x, boost::uint16_t y, boost::uint16_t z, boost::uint8_t value);

		//Others	
		void lockRegion(const Region& regToLock);
		void unlockRegion(void);
		//void markRegionChanged(boost::uint16_t firstX, boost::uint16_t firstY, boost::uint16_t firstZ, boost::uint16_t lastX, boost::uint16_t lastY, boost::uint16_t lastZ);

	private:
		bool m_bIsLocked;
		Region m_regLastLocked;
		BlockVolume<boost::uint8_t>* volumeData;
		LinearVolume<bool>* volRegionUpToDate;
	};
}

#endif

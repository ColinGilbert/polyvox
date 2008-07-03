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

#include "PolyVoxCore/Constants.h"
#include "PolyVoxCore/PolyVoxForwardDeclarations.h"
#include "PolyVoxCore/Region.h"
#include "PolyVoxCore/TypeDef.h"

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
		uint16 getSideLength(void);
		BlockVolume<uint8>* getVolumeData(void) const;
		uint8 getVoxelAt(const Vector3DUint16& pos);
		uint8 getVoxelAt(uint16 uX, uint16 uY, uint16 uZ);

		//Setters
		void setAllRegionsUpToDate(bool newUpToDateValue);
		void setLockedVoxelAt(uint16 x, uint16 y, uint16 z, uint8 value);		
		void setVolumeData(BlockVolume<uint8>* volumeDataToSet);
		void setVoxelAt(uint16 x, uint16 y, uint16 z, uint8 value);

		//Others	
		void lockRegion(const Region& regToLock);
		void unlockRegion(void);
		//void markRegionChanged(uint16 firstX, uint16 firstY, uint16 firstZ, uint16 lastX, uint16 lastY, uint16 lastZ);

	private:
		bool m_bIsLocked;
		Region m_regLastLocked;
		BlockVolume<uint8>* volumeData;
		LinearVolume<bool>* volRegionUpToDate;
	};
}

#endif

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

#include "PolyVoxCore/PolyVoxForwardDeclarations.h"
#include "PolyVoxCore/Region.h"
#include "PolyVoxCore/PolyVoxImpl/TypeDef.h"

namespace PolyVox
{	
	/// Voxel scene manager
	class POLYVOX_API VolumeChangeTracker
	{
	public:
		//Constructors, etc
		VolumeChangeTracker(Volume<uint8_t>* volumeDataToSet, uint16_t regionSideLength);
		~VolumeChangeTracker();

		//Getters
		int32_t getCurrentTime(void) const;
		Region getEnclosingRegion(void) const;		
		int32_t getLastModifiedTimeForRegion(uint16_t uX, uint16_t uY, uint16_t uZ);
		uint16_t getSideLength(void);
		Volume<uint8_t>* getVolumeData(void) const;
		uint8_t getVoxelAt(const Vector3DUint16& pos);
		uint8_t getVoxelAt(uint16_t uX, uint16_t uY, uint16_t uZ);

		//Setters
		void setAllRegionsModified(void);
		void setLockedVoxelAt(uint16_t x, uint16_t y, uint16_t z, uint8_t value);
		void setVoxelAt(uint16_t x, uint16_t y, uint16_t z, uint8_t value);

		//Others	
		void lockRegion(const Region& regToLock);
		void unlockRegion(void);
		//void markRegionChanged(uint16_t firstX, uint16_t firstY, uint16_t firstZ, uint16_t lastX, uint16_t lastY, uint16_t lastZ);

	private:
		bool m_bIsLocked;
		Region m_regLastLocked;
		Volume<uint8_t>* volumeData;

		uint16_t m_uRegionSideLength;
		uint8_t m_uRegionSideLengthPower;
		uint16_t m_uVolumeSideLengthInRegions;


		//It's not what the block class was designed for, but it 
		//provides a handy way of storing a 3D grid of values.
		BlockData<int32_t>* volRegionLastModified;

		static int32_t m_iCurrentTime;
	};
}

#endif

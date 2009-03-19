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

#ifndef __VolumeIterator_H__
#define __VolumeIterator_H__

#pragma region Headers
#include "PolyVoxForwardDeclarations.h"

#include "PolyVoxCStdInt.h"
#pragma endregion

namespace PolyVox
{
	template <typename VoxelType>
	class VolumeIterator
	{
	public:
		VolumeIterator(Volume<VoxelType>& volume);
		~VolumeIterator();		

		bool operator==(const VolumeIterator& rhs);
		bool operator<(const VolumeIterator& rhs);
		bool operator>(const VolumeIterator& rhs);
		bool operator<=(const VolumeIterator& rhs);
		bool operator>=(const VolumeIterator& rhs);

		uint16 getPosX(void) const;
		uint16 getPosY(void) const;
		uint16 getPosZ(void) const;
		VoxelType getSubSampledVoxel(uint8 uLevel) const;
		const Volume<VoxelType>& getVolume(void) const;
		VoxelType getVoxel(void) const;			

		void setPosition(const Vector3DInt16& v3dNewPos);
		void setPosition(uint16 xPos, uint16 yPos, uint16 zPos);
		void setValidRegion(const Region& region);
		void setValidRegion(uint16 xFirst, uint16 yFirst, uint16 zFirst, uint16 xLast, uint16 yLast, uint16 zLast);	

		bool isValidForRegion(void) const;
		void moveForwardInRegionFast(void);		
		bool moveForwardInRegionXYZ(void);	

		VoxelType peekVoxel1nx1ny1nz(void) const;
		VoxelType peekVoxel1nx1ny0pz(void) const;
		VoxelType peekVoxel1nx1ny1pz(void) const;
		VoxelType peekVoxel1nx0py1nz(void) const;
		VoxelType peekVoxel1nx0py0pz(void) const;
		VoxelType peekVoxel1nx0py1pz(void) const;
		VoxelType peekVoxel1nx1py1nz(void) const;
		VoxelType peekVoxel1nx1py0pz(void) const;
		VoxelType peekVoxel1nx1py1pz(void) const;

		VoxelType peekVoxel0px1ny1nz(void) const;
		VoxelType peekVoxel0px1ny0pz(void) const;
		VoxelType peekVoxel0px1ny1pz(void) const;
		VoxelType peekVoxel0px0py1nz(void) const;
		VoxelType peekVoxel0px0py0pz(void) const;
		VoxelType peekVoxel0px0py1pz(void) const;
		VoxelType peekVoxel0px1py1nz(void) const;
		VoxelType peekVoxel0px1py0pz(void) const;
		VoxelType peekVoxel0px1py1pz(void) const;

		VoxelType peekVoxel1px1ny1nz(void) const;
		VoxelType peekVoxel1px1ny0pz(void) const;
		VoxelType peekVoxel1px1ny1pz(void) const;
		VoxelType peekVoxel1px0py1nz(void) const;
		VoxelType peekVoxel1px0py0pz(void) const;
		VoxelType peekVoxel1px0py1pz(void) const;
		VoxelType peekVoxel1px1py1nz(void) const;
		VoxelType peekVoxel1px1py0pz(void) const;
		VoxelType peekVoxel1px1py1pz(void) const;

	private:

		//The current volume
		Volume<VoxelType>& mVolume;

		//The current position in the volume
		uint16 mXPosInVolume;
		uint16 mYPosInVolume;
		uint16 mZPosInVolume;

		//The position of the current block
		uint16 mXBlock;
		uint16 mYBlock;
		uint16 mZBlock;

		//The offset into the current block
		uint16 mXPosInBlock;
		uint16 mYPosInBlock;
		uint16 mZPosInBlock;

		//Other current position information
		VoxelType* mCurrentVoxel;
		uint32 mBlockIndexInVolume;
		uint32 mVoxelIndexInBlock;

		uint16 mXRegionFirst;
		uint16 mYRegionFirst;
		uint16 mZRegionFirst;
		uint16 mXRegionLast;
		uint16 mYRegionLast;
		uint16 mZRegionLast;

		uint16 mXRegionFirstBlock;
		uint16 mYRegionFirstBlock;
		uint16 mZRegionFirstBlock;
		uint16 mXRegionLastBlock;
		uint16 mYRegionLastBlock;
		uint16 mZRegionLastBlock;

		bool mIsValidForRegion;
	};
}

#include "VolumeIterator.inl"

#endif

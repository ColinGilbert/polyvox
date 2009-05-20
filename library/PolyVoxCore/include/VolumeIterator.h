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

#include "PolyVoxImpl/CPlusPlusZeroXSupport.h"
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

		uint16_t getPosX(void) const;
		uint16_t getPosY(void) const;
		uint16_t getPosZ(void) const;
		VoxelType getSubSampledVoxel(uint8_t uLevel) const;
		VoxelType getSubSampledVoxelWithBoundsCheck(uint8_t uLevel) const;
		const Volume<VoxelType>& getVolume(void) const;
		VoxelType getVoxel(void) const;			

		void setPosition(const Vector3DInt16& v3dNewPos);
		void setPosition(uint16_t xPos, uint16_t yPos, uint16_t zPos);
		void setValidRegion(const Region& region);
		void setValidRegion(uint16_t xFirst, uint16_t yFirst, uint16_t zFirst, uint16_t xLast, uint16_t yLast, uint16_t zLast);	

		bool isValidForRegion(void) const;
		bool moveForwardInRegionXYZ(void);
		void moveForwardInRegionXYZFast(void);

		void movePositiveX(void);

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
		uint16_t mXPosInVolume;
		uint16_t mYPosInVolume;
		uint16_t mZPosInVolume;

		//The position of the current block
		uint16_t mXBlock;
		uint16_t mYBlock;
		uint16_t mZBlock;

		//The offset into the current block
		uint16_t mXPosInBlock;
		uint16_t mYPosInBlock;
		uint16_t mZPosInBlock;

		//Other current position information
		VoxelType* mCurrentVoxel;
		uint32_t mBlockIndexInVolume;
		uint32_t mVoxelIndexInBlock;

		uint16_t mXRegionFirst;
		uint16_t mYRegionFirst;
		uint16_t mZRegionFirst;
		uint16_t mXRegionLast;
		uint16_t mYRegionLast;
		uint16_t mZRegionLast;

		uint16_t mXRegionFirstBlock;
		uint16_t mYRegionFirstBlock;
		uint16_t mZRegionFirstBlock;
		uint16_t mXRegionLastBlock;
		uint16_t mYRegionLastBlock;
		uint16_t mZRegionLastBlock;

		bool mIsValidForRegion;
	};
}

#include "VolumeIterator.inl"

#endif

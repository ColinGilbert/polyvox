/******************************************************************************
This file is part of a voxel plugin for OGRE
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
#ifndef __VolumeIterator_H__
#define __VolumeIterator_H__

#include "OgrePrerequisites.h"
#include "OgreSharedPtr.h"

#include "TypeDef.h"

namespace Ogre
{
	class Volume;

	class VOXEL_SCENE_MANAGER_API VolumeIterator
	{
	public:
		VolumeIterator(Volume& volume);
		~VolumeIterator();

		void setVoxel(uchar value);
		uchar getVoxel(void);

		uchar getVoxelAt(const uint xPosition, const uint yPosition, const uint zPosition) const;
		void setVoxelAt(const uint xPosition, const uint yPosition, const uint zPosition, const uchar value);

		float getAveragedVoxelAt(const uint xPosition, const uint yPosition, const uint zPosition, uint size) const;

		//FIXME - this shouldn't return float vector
		Vector3 getCentralDifferenceGradient(void) const;
		Vector3 getAveragedCentralDifferenceGradient(void) const;
		Vector3 getSobelGradient(void) const;

		uint getPosX(void);
		uint getPosY(void);
		uint getPosZ(void);

		void setPosition(uint xPos, uint yPos, uint zPos);
		void setValidRegion(uint xFirst, uint yFirst, uint zFirst, uint xLast, uint yLast, uint zLast);
		void moveForwardInRegion(void);

		bool isValidForRegion(void);

		uchar peekVoxel1nx1ny1nz(void) const;
		uchar peekVoxel1nx1ny0pz(void) const;
		uchar peekVoxel1nx1ny1pz(void) const;
		uchar peekVoxel1nx0py1nz(void) const;
		uchar peekVoxel1nx0py0pz(void) const;
		uchar peekVoxel1nx0py1pz(void) const;
		uchar peekVoxel1nx1py1nz(void) const;
		uchar peekVoxel1nx1py0pz(void) const;
		uchar peekVoxel1nx1py1pz(void) const;

		uchar peekVoxel0px1ny1nz(void) const;
		uchar peekVoxel0px1ny0pz(void) const;
		uchar peekVoxel0px1ny1pz(void) const;
		uchar peekVoxel0px0py1nz(void) const;
		uchar peekVoxel0px0py0pz(void) const;
		uchar peekVoxel0px0py1pz(void) const;
		uchar peekVoxel0px1py1nz(void) const;
		uchar peekVoxel0px1py0pz(void) const;
		uchar peekVoxel0px1py1pz(void) const;

		uchar peekVoxel1px1ny1nz(void) const;
		uchar peekVoxel1px1ny0pz(void) const;
		uchar peekVoxel1px1ny1pz(void) const;
		uchar peekVoxel1px0py1nz(void) const;
		uchar peekVoxel1px0py0pz(void) const;
		uchar peekVoxel1px0py1pz(void) const;
		uchar peekVoxel1px1py1nz(void) const;
		uchar peekVoxel1px1py0pz(void) const;
		uchar peekVoxel1px1py1pz(void) const;

	private:
		//The current volume
		Volume& mVolume;

		//The current position in the volume
		uint mXPosInVolume;
		uint mYPosInVolume;
		uint mZPosInVolume;

		//The position of the current block
		uint mXBlock;
		uint mYBlock;
		uint mZBlock;

		//The offset into the current block
		uint mXPosInBlock;
		uint mYPosInBlock;
		uint mZPosInBlock;

		//Other current position information
		uchar* mCurrentVoxel;
		//SharedPtr<Block> mCurrentBlock;
		ulong mBlockIndexInVolume;
		ulong mVoxelIndexInBlock;

		uint mXRegionFirst;
		uint mYRegionFirst;
		uint mZRegionFirst;
		uint mXRegionLast;
		uint mYRegionLast;
		uint mZRegionLast;

		uint mXRegionFirstBlock;
		uint mYRegionFirstBlock;
		uint mZRegionFirstBlock;
		uint mXRegionLastBlock;
		uint mYRegionLastBlock;
		uint mZRegionLastBlock;

		bool mIsValidForRegion;
	};
}

#endif

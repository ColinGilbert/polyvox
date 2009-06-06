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

#ifndef __VolumeSampler_H__
#define __VolumeSampler_H__

#pragma region Headers
#include "PolyVoxForwardDeclarations.h"

#include "PolyVoxImpl/CPlusPlusZeroXSupport.h"
#pragma endregion

namespace PolyVox
{
	template <typename VoxelType>
	class VolumeSampler
	{
	public:
		VolumeSampler(Volume<VoxelType>& volume);
		~VolumeSampler();		

		bool operator==(const VolumeSampler& rhs);
		bool operator<(const VolumeSampler& rhs);
		bool operator>(const VolumeSampler& rhs);
		bool operator<=(const VolumeSampler& rhs);
		bool operator>=(const VolumeSampler& rhs);

		uint16_t getPosX(void) const;
		uint16_t getPosY(void) const;
		uint16_t getPosZ(void) const;
		VoxelType getSubSampledVoxel(uint8_t uLevel) const;
		const Volume<VoxelType>& getVolume(void) const;
		inline VoxelType getVoxel(void) const;			

		void setPosition(const Vector3DInt16& v3dNewPos);
		void setPosition(uint16_t xPos, uint16_t yPos, uint16_t zPos);

		void movePositiveX(void);
		void movePositiveY(void);
		void movePositiveZ(void);

		void moveNegativeX(void);
		void moveNegativeY(void);
		void moveNegativeZ(void);

		inline VoxelType peekVoxel1nx1ny1nz(void) const;
		inline VoxelType peekVoxel1nx1ny0pz(void) const;
		inline VoxelType peekVoxel1nx1ny1pz(void) const;
		inline VoxelType peekVoxel1nx0py1nz(void) const;
		inline VoxelType peekVoxel1nx0py0pz(void) const;
		inline VoxelType peekVoxel1nx0py1pz(void) const;
		inline VoxelType peekVoxel1nx1py1nz(void) const;
		inline VoxelType peekVoxel1nx1py0pz(void) const;
		inline VoxelType peekVoxel1nx1py1pz(void) const;

		inline VoxelType peekVoxel0px1ny1nz(void) const;
		inline VoxelType peekVoxel0px1ny0pz(void) const;
		inline VoxelType peekVoxel0px1ny1pz(void) const;
		inline VoxelType peekVoxel0px0py1nz(void) const;
		inline VoxelType peekVoxel0px0py0pz(void) const;
		inline VoxelType peekVoxel0px0py1pz(void) const;
		inline VoxelType peekVoxel0px1py1nz(void) const;
		inline VoxelType peekVoxel0px1py0pz(void) const;
		inline VoxelType peekVoxel0px1py1pz(void) const;

		inline VoxelType peekVoxel1px1ny1nz(void) const;
		inline VoxelType peekVoxel1px1ny0pz(void) const;
		inline VoxelType peekVoxel1px1ny1pz(void) const;
		inline VoxelType peekVoxel1px0py1nz(void) const;
		inline VoxelType peekVoxel1px0py0pz(void) const;
		inline VoxelType peekVoxel1px0py1pz(void) const;
		inline VoxelType peekVoxel1px1py1nz(void) const;
		inline VoxelType peekVoxel1px1py0pz(void) const;
		inline VoxelType peekVoxel1px1py1pz(void) const;

	private:

		//The current volume
		Volume<VoxelType>& mVolume;

		//The current position in the volume
		uint16_t mXPosInVolume;
		uint16_t mYPosInVolume;
		uint16_t mZPosInVolume;

		//Other current position information
		VoxelType* mCurrentVoxel;
	};
}

#include "VolumeSampler.inl"

#endif

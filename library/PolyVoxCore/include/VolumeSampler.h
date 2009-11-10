#pragma region License
/*******************************************************************************
Copyright (c) 2005-2009 David Williams

This software is provided 'as-is', without any express or implied
warranty. In no event will the authors be held liable for any damages
arising from the use of this software.

Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute it
freely, subject to the following restrictions:

    1. The origin of this software must not be misrepresented; you must not
    claim that you wrote the original software. If you use this software
    in a product, an acknowledgment in the product documentation would be
    appreciated but is not required.

    2. Altered source versions must be plainly marked as such, and must not be
    misrepresented as being the original software.

    3. This notice may not be removed or altered from any source
    distribution. 	
*******************************************************************************/
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
		VolumeSampler(Volume<VoxelType>* volume);
		~VolumeSampler();

		VolumeSampler<VoxelType>& operator=(const VolumeSampler<VoxelType>& rhs) throw();

		uint16_t getPosX(void) const;
		uint16_t getPosY(void) const;
		uint16_t getPosZ(void) const;
		VoxelType getSubSampledVoxel(uint8_t uLevel) const;
		const Volume<VoxelType>* getVolume(void) const;
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
		Volume<VoxelType>* mVolume;

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

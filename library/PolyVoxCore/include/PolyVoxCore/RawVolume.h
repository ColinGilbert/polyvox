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

#ifndef __PolyVox_RawVolume_H__
#define __PolyVox_RawVolume_H__

#include "PolyVoxCore/Region.h"
#include "PolyVoxCore/PolyVoxForwardDeclarations.h"

#include <limits>
#include <memory>

namespace PolyVox
{
	template <typename VoxelType>
	class RawVolume
	{
	public:
		#ifndef SWIG
		class Sampler
		{
		public:
			Sampler(RawVolume<VoxelType>* volume);
			~Sampler();

			Sampler& operator=(const Sampler& rhs) throw();

			int32_t getPosX(void) const;
			int32_t getPosY(void) const;
			int32_t getPosZ(void) const;
			const RawVolume<VoxelType>* getVolume(void) const;
			inline VoxelType getVoxel(void) const;			

			void setPosition(const Vector3DInt32& v3dNewPos);
			void setPosition(int32_t xPos, int32_t yPos, int32_t zPos);

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
			RawVolume<VoxelType>* mVolume;

			//The current position in the volume
			int32_t mXPosInVolume;
			int32_t mYPosInVolume;
			int32_t mZPosInVolume;

			//Other current position information
			VoxelType* mCurrentVoxel;

			//Whether the current position is inside the volume
			bool m_bIsCurrentPositionValid;
		};
		#endif

	public:
		/// Constructor for creating a fixed size volume.
		RawVolume
		(
			const Region& regValid
		);
		/// Destructor
		~RawVolume();

		/// Gets the value used for voxels which are outside the volume
		VoxelType getBorderValue(void) const;
		/// Gets a Region representing the extents of the RawVolume.
		Region getEnclosingRegion(void) const;
		/// Gets the width of the volume in voxels.
		int32_t getWidth(void) const;
		/// Gets the height of the volume in voxels.
		int32_t getHeight(void) const;
		/// Gets the depth of the volume in voxels.
		int32_t getDepth(void) const;
		/// Gets the length of the longest side in voxels
		int32_t getLongestSideLength(void) const;
		/// Gets the length of the shortest side in voxels
		int32_t getShortestSideLength(void) const;
		/// Gets the length of the diagonal in voxels
		float getDiagonalLength(void) const;
		/// Gets a voxel at the position given by <tt>x,y,z</tt> coordinates
		VoxelType getVoxelAt(int32_t uXPos, int32_t uYPos, int32_t uZPos) const;
		/// Gets a voxel at the position given by a 3D vector
		VoxelType getVoxelAt(const Vector3DInt32& v3dPos) const;

		/// Sets the value used for voxels which are outside the volume
		void setBorderValue(const VoxelType& tBorder);
		/// Sets the voxel at the position given by <tt>x,y,z</tt> coordinates
		bool setVoxelAt(int32_t uXPos, int32_t uYPos, int32_t uZPos, VoxelType tValue);
		/// Sets the voxel at the position given by a 3D vector
		bool setVoxelAt(const Vector3DInt32& v3dPos, VoxelType tValue);

		/// Calculates approximatly how many bytes of memory the volume is currently using.
		uint32_t calculateSizeInBytes(void);

		/// Deprecated - I don't think we should expose this function? Let us know if you disagree...
		void resize(const Region& regValidRegion);

private:	
		//The block data
		VoxelType* m_pData;

		//The border value
		VoxelType m_tBorderValue;

		//The size of the volume
		Region m_regValidRegion;

		//Some useful sizes
		int32_t m_uLongestSideLength;
		int32_t m_uShortestSideLength;
		float m_fDiagonalLength;
	};
}

#include "PolyVoxCore/RawVolume.inl"
#include "PolyVoxCore/RawVolumeSampler.inl"

#endif //__PolyVox_RawVolume_H__

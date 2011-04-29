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

#ifndef __PolyVox_SimpleVolume_H__
#define __PolyVox_SimpleVolume_H__

#include "PolyVoxImpl/Block.h"
#include "Region.h"
#include "PolyVoxForwardDeclarations.h"

#include <limits>
#include <map>
#include <set>
#include <memory>
#include <vector>

namespace PolyVox
{
	template <typename VoxelType>
	class SimpleVolume
	{
	public:
		class Sampler
		{
		public:
			Sampler(SimpleVolume<VoxelType>* volume);
			~Sampler();

			Sampler& operator=(const Sampler& rhs) throw();

			int32_t getPosX(void) const;
			int32_t getPosY(void) const;
			int32_t getPosZ(void) const;
			VoxelType getSubSampledVoxel(uint8_t uLevel) const;
			const SimpleVolume<VoxelType>* getVolume(void) const;
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
			SimpleVolume<VoxelType>* mVolume;

			//The current position in the volume
			int32_t mXPosInVolume;
			int32_t mYPosInVolume;
			int32_t mZPosInVolume;

			//Other current position information
			VoxelType* mCurrentVoxel;
		};

		// Make the ConstVolumeProxy a friend
		friend class ConstVolumeProxy<VoxelType>;

		struct LoadedBlock
		{
		public:
			LoadedBlock(uint16_t uSideLength = 0)
				:block(uSideLength)
				,timestamp(0)
			{
			}

			Block<VoxelType> block;
			uint32_t timestamp;
		};

	public:
		/// Constructor for creating a fixed size volume.
		SimpleVolume
		(
			const Region& regValid,
			uint16_t uBlockSideLength = 32
		);
		/// Deprecated constructor - do not use.
		SimpleVolume
		(
			int32_t dont_use_this_constructor_1, int32_t dont_use_this_constructor_2, int32_t dont_use_this_constructor_3
		);
		/// Destructor
		~SimpleVolume();

		/// Gets the value used for voxels which are outside the volume
		VoxelType getBorderValue(void) const;
		/// Gets a Region representing the extents of the SimpleVolume.
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

		//Sets whether or not blocks are compressed in memory
		void setCompressionEnabled(bool bCompressionEnabled);
		/// Sets the number of blocks for which uncompressed data is stored
		void setMaxNumberOfUncompressedBlocks(uint16_t uMaxNumberOfUncompressedBlocks);
		/// Sets the value used for voxels which are outside the volume
		void setBorderValue(const VoxelType& tBorder);
		/// Sets the voxel at the position given by <tt>x,y,z</tt> coordinates
		bool setVoxelAt(int32_t uXPos, int32_t uYPos, int32_t uZPos, VoxelType tValue);
		/// Sets the voxel at the position given by a 3D vector
		bool setVoxelAt(const Vector3DInt32& v3dPos, VoxelType tValue);

		/// Empties the cache of uncompressed blocks
		void clearBlockCache(void);
		/// Calculates the approximate compression ratio of the store volume data
		float calculateCompressionRatio(void);
		/// Calculates approximatly how many bytes of memory the volume is currently using.
		uint32_t calculateSizeInBytes(void);

		/// Deprecated - I don't think we should expose this function? Let us know if you disagree...
		void resize(const Region& regValidRegion, uint16_t uBlockSideLength);

private:
		/// gets called when a new region is allocated and needs to be filled
		/// NOTE: accessing ANY voxels outside this region during the process of this function
		/// is absolutely unsafe
		polyvox_function<void(const ConstVolumeProxy<VoxelType>&, const Region&)> m_funcDataRequiredHandler;
		/// gets called when a Region needs to be stored by the user, because SimpleVolume will erase it right after
		/// this function returns
		/// NOTE: accessing ANY voxels outside this region during the process of this function
		/// is absolutely unsafe
		polyvox_function<void(const ConstVolumeProxy<VoxelType>&, const Region&)> m_funcDataOverflowHandler;
	
		Block<VoxelType>* getUncompressedBlock(int32_t uBlockX, int32_t uBlockY, int32_t uBlockZ) const;
		void eraseBlock(typename std::map<Vector3DInt32, LoadedBlock >::iterator itBlock) const;
		/// this function can be called by m_funcDataRequiredHandler without causing any weird effects
		bool setVoxelAtConst(int32_t uXPos, int32_t uYPos, int32_t uZPos, VoxelType tValue) const;

		//The block data
		mutable std::map<Vector3DInt32, LoadedBlock > m_pBlocks;

		//The cache of uncompressed blocks. The uncompressed block data and the timestamps are stored here rather
		//than in the Block class. This is so that in the future each VolumeIterator might to maintain its own cache
		//of blocks. However, this could mean the same block data is uncompressed and modified in more than one
		//location in memory... could be messy with threading.
		mutable std::vector< LoadedBlock* > m_vecUncompressedBlockCache;
		mutable uint32_t m_uTimestamper;
		mutable Vector3DInt32 m_v3dLastAccessedBlockPos;
		mutable Block<VoxelType>* m_pLastAccessedBlock;
		uint32_t m_uMaxNumberOfUncompressedBlocks;

		//We don't store an actual Block for the border, just the uncompressed data. This is partly because the border
		//block does not have a position (so can't be passed to getUncompressedBlock()) and partly because there's a
		//good chance we'll often hit it anyway. It's a chunk of homogenous data (rather than a single value) so that
		//the VolumeIterator can do it's usual pointer arithmetic without needing to know it's gone outside the volume.
		VoxelType* m_pUncompressedBorderData;

		//The size of the volume
		Region m_regValidRegion;
		Region m_regValidRegionInBlocks;

		//The size of the blocks
		uint16_t m_uBlockSideLength;
		uint8_t m_uBlockSideLengthPower;

		//Some useful sizes
		int32_t m_uLongestSideLength;
		int32_t m_uShortestSideLength;
		float m_fDiagonalLength;

		bool m_bCompressionEnabled;
	};
}

#include "SimpleVolume.inl"
#include "SimpleVolumeSampler.inl"

#endif //__PolyVox_SimpleVolume_H__

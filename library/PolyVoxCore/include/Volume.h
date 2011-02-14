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

#ifndef __PolyVox_Volume_H__
#define __PolyVox_Volume_H__

#include "PolyVoxImpl/Block.h"
#include "PolyVoxForwardDeclarations.h"

#include <limits>
#include <map>
#include <set>
#include <memory>
#include <vector>

namespace PolyVox
{
	///The Volume class provides a memory efficient method of storing voxel data while also allowing fast access and modification.
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	/// A Volume is essentially a '3D image' in which each element (voxel) is identified by a three dimensional (x,y,z) coordinate,
	/// rather than the two dimensional (x,y) coordinate which is used to identify an element (pixel) in a normal image. Within PolyVox,
	/// the Volume class is used to store and manipulate our data before we extract our SurfaceMeshs from it.
	/// 
	/// <b>Data Representaion</b>
	/// If stored carelessly, volume data can take up a huge amount of memory. For example, a volume of dimensions 1024x1024x1024 with
	/// 1 byte per voxel will require 1GB of memory if stored in an uncompressed form. Natuarally our Volume class is much more efficient
	/// than this and it is worth understanding (at least at a high level) the approach which is used.
	///
	/// Essentially, the Volume class stores its data as a collection of blocks. Each of these block is much smaller than the whole volume,
	/// for example a typical size might be 32x32x32 voxels (though is is configurable by the user). In this case, a 256x512x1024 volume
	/// would contain 8x16x32 = 4096 blocks. The data for each block is stored in a compressed form, which uses only a small amout of
	/// memory but it is hard to modify the data. Therefore, before any given voxel can be modified, its corresponding block must be uncompressed.
	///
	/// The compression and decompression of block is a relatively slow process and so we aim to do this as rarely as possible. In order
	/// to achive this, the volume class store a cache of recently used blocks and their associated uncompressed data. Each time a voxel
	/// is touched a timestamp is updated on the corresponding block, when the cache becomes full the block with the oldest timestamp is
	/// recompressed and moved out of the cache.
	///
	/// <b>Achieving high compression rates</b>
	/// Note: This section is theorectical and not well tested. Please let us know if you find the tips below do or do not work.
	///
	/// The compression rates which can be achieved can vary significantly depending the nature of the data you are storing, but you can
	/// encourage high compression rates by making your data as homogenous as possible. If you are simply storing a material with each
	/// voxel then this will probably happen naturally. Games such as Minecraft which use this approach will typically involve large ares
	/// of th same material which will compress down well.
	///
	/// However, if you are storing density values then you may want to take some care. The advantage of storing smoothly changing values
	/// is that you can get smooth surfaces extracted, but storing smoothly changing values inside or outside objects (rather than just
	/// on the boundary) does not benefit the surface and is very hard to compress effectively. You should apply some thresholding to your
	/// density values to reduce this problem (this threasholding should only be applied to voxels who don't contribute to the surface).
	/// 
	/// For example, suppose you are using layers of 3D Perlin noise to create a 3D terrain (not a heightmap). If you store the raw Perlin
	/// noise value at each voxel then a slice through the volume might look like the following:
	///
	/// <insert image here>
	///
	/// However, by setting high values to be fixed to one and low values to be fixed to zero you can make a slice through your volume look more like this:
	///
	/// <insert image here>
	///
	/// The boundary is in the same place and is still smooth, but the large homogenous regions mean the data should compress much more effectively.
	/// Although it may look like you have lost some precision in this process this is only because the images above are constrained to 256
	/// greyscale values, where as true Perlin noise will give you floating point values.
	///
	/// <b>Cache-aware traversal</b>
	/// You might be suprised at just how many cache misses can occur when you traverse the volume in a naive manner. Consider a 1024x1024x1024 volume
	/// with blocks of size 32x32x32. And imagine you iterate over this volume with a simple three-level for loop which iterates over x, the y, then z.
	/// If you start at position (0,0,0) then ny the time you reach position (1023,0,0) you have touched 1024 voxels along one edge of the volume and
	/// have pulled 32 blocks into the cache. By the time you reach (1023,1023,0) you have hit 1024x1024 voxels and pulled 32x32 blocks into the cache.
	/// You are now ready to touch voxel (0,0,1) which is right nect to where you started, but unless your cache is at least 32x32 blocks large then this
	/// initial block has already been cleared from the cache.
	///
	/// Ensuring you have a large enough cache size can obviously help the above situation, but you might also consider iterating over the voxels in a
	/// different order. For example, if you replace your three-level loop with a six-level loop then you can first process all the voxels between (0,0,0)
	/// and (31,31,31), then process all the voxels between (32,0,0) and (63,0,0), and so forth. Using this approach you will have no cache misses even
	/// is your cache sise is only one. Of course the logic is more complex, but writing code in such a cache-aware manner may be beneficial in some situations.
	///
	/// <b>Threading</b>
	/// The volume class does not provide any thread safety constructs and can therefore not be assumed to be thread safe. To be safe you should only allow
	/// one thread to access the volume at a time. Even if you have several threads just reading data from the volume they can cause blocks to be pushed
	/// out of the cache, potentially invalidating any pointers other threads might be using.
	///
	/// That said, we believe that if care is taken then multiple threads can be used, and are currently experimenting with this.
	///
	/// <b>Use of templates</b>
	/// Although this class is templatised on the voxel type it is not expected that you can use any primative type to represent your voxels. It is only
	/// intended for PolyVox's voxel types such as Material, Density, and MarterialDensityPair. If you need to store 3D grids of ints, floats, or pointers
	/// you should look ar the Array class instead.
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	template <typename VoxelType>
	class Volume
	{
		// Make VolumeSampler a friend
		friend class VolumeSampler<VoxelType>;

		struct UncompressedBlock
		{
			uint32_t uBlockIndex;
			VoxelType* data;

		};

	public:		
		/// Constructor
		Volume(uint16_t uWidth, uint16_t uHeight, uint16_t uDepth, uint16_t uBlockSideLength = 32);
		/// Destructor
		~Volume();	

		/// Gets the value used for voxels which are outside the volume
		VoxelType getBorderValue(void) const;
		/// Gets a Region representing the extents of the Volume.
		Region getEnclosingRegion(void) const;
		/// Gets the width of the volume in voxels.
		uint16_t getWidth(void) const;
		/// Gets the height of the volume in voxels.
		uint16_t getHeight(void) const;
		/// Gets the depth of the volume in voxels.
		uint16_t getDepth(void) const;
		/// Gets the length of the longest side in voxels
		uint16_t getLongestSideLength(void) const;
		/// Gets the length of the shortest side in voxels
		uint16_t getShortestSideLength(void) const;
		/// Gets the length of the diagonal in voxels
		float getDiagonalLength(void) const;
		/// Gets a voxel by <tt>x,y,z</tt> position
		VoxelType getVoxelAt(uint16_t uXPos, uint16_t uYPos, uint16_t uZPos) const;
		/// Gets a voxel by 3D vector position
		VoxelType getVoxelAt(const Vector3DUint16& v3dPos) const;

		/// Sets the number of blocks for which uncompressed data is stored.
		void setBlockCacheSize(uint16_t uBlockCacheSize);
		/// Sets the value used for voxels which are outside the volume
		void setBorderValue(const VoxelType& tBorder);
		/// Sets the voxel at an <tt>x,y,z</tt> position
		bool setVoxelAt(uint16_t uXPos, uint16_t uYPos, uint16_t uZPos, VoxelType tValue);
		/// Sets the voxel at a 3D vector position
		bool setVoxelAt(const Vector3DUint16& v3dPos, VoxelType tValue);

		void clearBlockCache(void);
		float calculateCompressionRatio(void);
		uint32_t calculateSizeInBytes(void);
		void useCompatibilityMode(void);
		/// Resizes the volume to the specified dimensions
		void resize(uint16_t uWidth, uint16_t uHeight, uint16_t uDepth, uint16_t uBlockSideLength = 32);

	private:
		Block<VoxelType>* getUncompressedBlock(uint16_t uBlockX, uint16_t uBlockY, uint16_t uBlockZ) const;

		//The block data
		mutable std::vector< Block<VoxelType> > m_pBlocks;

		//The cache of uncompressed blocks. The uncompressed block data and the timestamps are stored here rather
		//than in the Block class. This is so that in the future each VolumeIterator might to maintain its own cache
		//of blocks. However, this could mean the same block data is uncompressed and modified in more than one
		//location in memory... could be messy with threading.
		mutable std::vector< UncompressedBlock > m_vecUncompressedBlockCache;
		mutable std::vector<uint32_t> m_pUncompressedTimestamps;
		mutable uint32_t m_uTimestamper;
		uint32_t m_ulastAccessedBlockIndex;
		uint32_t m_uMaxUncompressedBlockCacheSize;

		//We don't store an actual Block for the border, just the uncompressed data. This is partly because the border
		//block does not have a position (so can't be passed to getUncompressedBlock()) and partly because there's a
		//good chance we'll often hit it anyway. It's a chunk of homogenous data (rather than a single value) so that
		//the VolumeIterator can do it's usual pointer arithmetic without needing to know it's gone outside the volume.
		VoxelType* m_pUncompressedBorderData;

		uint32_t m_uNoOfBlocksInVolume;

		uint16_t m_uWidthInBlocks;
		uint16_t m_uHeightInBlocks;
		uint16_t m_uDepthInBlocks;

		uint16_t m_uWidth;
		uint16_t m_uHeight;	
		uint16_t m_uDepth;

		uint8_t m_uBlockSideLengthPower;
		uint16_t m_uBlockSideLength;

		uint16_t m_uLongestSideLength;
		uint16_t m_uShortestSideLength;
		float m_fDiagonalLength;
	};

	//Some handy typedefs
	typedef Volume<float> FloatVolume;
	typedef Volume<uint8_t> UInt8Volume;
	typedef Volume<uint16_t> UInt16Volume;	
}

#include "Volume.inl"

#endif

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
#include "Region.h"
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
	/// A Volume is essentially a 3D array in which each element (or <i>voxel</i>) is identified by a three dimensional (x,y,z) coordinate.
	/// We use the Volume class to store our data in an efficient way, and it is the input to many of the algorithms (such as the surface
	/// extractors) which form the heart of PolyVox. The Volume class is templatised so that different types of data can be stored with each voxel.
	///
	/// <b> Basic usage</b>
	/// The following code snippet shows how to construct a volume and demonstrates basic usage:
	///
	/// Volume<Material8> volume(Region(Vector3DInt32(0,0,0), Vector3DInt32(63,127,255)));
	/// volume.setVoxelAt(15, 90, 42, Material8(5));
	/// std::cout << "Voxel at (15, 90, 42) has value: " << volume.getVoxelAt(15, 90, 42).getMaterial() << std::endl;
	/// std::cout << "Width = " << volume.getWidth() << ", Height = " << volume.getHeight() << ", Depth = " << volume.getDepth() << std::endl;
	/// 
	/// The Volume constructor takes a Region as a parameter. This specifies the valid range of voxels which can be held in the volume, so in this
	/// particular case the valid voxel positions are (0,0,0) to (63, 127, 255). Attempts to access voxels outside this range can result in a crash
	/// (or an assert() if running in debug mode). PolyVox also has support for near infinite volumes which will be discussed later.
	/// 
	/// In this particular example each voxel in the Volume is of type 'Material', as specified by the template parameter. This is one of several
	/// predefined voxel types, and it is also possible to define your own. The Material type simply holds an integer value where zero represents
	/// empty space and any other value represents a solid material.
	/// 
	/// Access to individual voxels is provided via the setVoxelAt() and getVoxelAt() member functions. Advanced users may also be interested in
	/// the VolumeSampler class for faster read-only access to a large number of voxels.
	/// 
	/// Lastly the example prints out some properties of the Volume. Note that the dimentsions width(), height(), and depth() are inclusive, such
	/// that the width is 64 when the range of valid x coordinates goes from 0 to 63.
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
	/// to achive this, the volume class stores a cache of recently used blocks and their associated uncompressed data. Each time a voxel
	/// is touched a timestamp is updated on the corresponding block, when the cache becomes full the block with the oldest timestamp is
	/// recompressed and moved out of the cache.
	///
	/// This compression scheme will typically allow you to load several billion voxels into a few hundred megabytes of memory, though the exact
	/// compression rate is highly dependant on your data. If you have more data than this then PolyVox provides a mechanism by which parts of
	/// the volume can be paged out of memory by calling user supplied callback functions. This mechanism allows a potentially unlimited amount
	/// of data to be loaded, provided the user is able to take responsibility for storing any data which PolyVox cannot fit in memory, and then
	/// returning it back to PolyVox on demand. For example, the user might choose to temporarilly store this data on disk or stream it to a
	/// remote database.
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
		/// Constructor
		Volume
		(
			polyvox_function<void(const ConstVolumeProxy<VoxelType>&, const Region&)> dataRequiredHandler,
			polyvox_function<void(const ConstVolumeProxy<VoxelType>&, const Region&)> dataOverflowHandler,
			uint16_t uBlockSideLength = 32
		);
		/// Constructor
		Volume
		(
			const Region& regValid,
			polyvox_function<void(const ConstVolumeProxy<VoxelType>&, const Region&)> dataRequiredHandler = 0,
			polyvox_function<void(const ConstVolumeProxy<VoxelType>&, const Region&)> dataOverflowHandler = 0,
			bool bPagingEnabled = false,
			uint16_t uBlockSideLength = 32
		);
		/// Deprecated constructor - do not use.
		Volume
		(
			int32_t dont_use_this_constructor_1, int32_t dont_use_this_constructor_2, int32_t dont_use_this_constructor_3
		);
		/// Destructor
		~Volume();

		/// Gets the value used for voxels which are outside the volume
		VoxelType getBorderValue(void) const;
		/// Gets a Region representing the extents of the Volume.
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
		/// Gets a voxel by <tt>x,y,z</tt> position
		VoxelType getVoxelAt(int32_t uXPos, int32_t uYPos, int32_t uZPos) const;
		/// Gets a voxel by 3D vector position
		VoxelType getVoxelAt(const Vector3DInt32& v3dPos) const;

		void setCompressionEnabled(bool bCompressionEnabled);
		/// Sets the number of blocks for which uncompressed data is stored.
		void setMaxNumberOfUncompressedBlocks(uint16_t uMaxNumberOfUncompressedBlocks);
		/// Sets the number of blocks which can be in memory before unload is called
		void setMaxNumberOfBlocksInMemory(uint16_t uMaxNumberOfBlocksInMemory);
		/// Sets the value used for voxels which are outside the volume
		void setBorderValue(const VoxelType& tBorder);
		/// Sets the voxel at an <tt>x,y,z</tt> position
		bool setVoxelAt(int32_t uXPos, int32_t uYPos, int32_t uZPos, VoxelType tValue);
		/// Sets the voxel at a 3D vector position
		bool setVoxelAt(const Vector3DInt32& v3dPos, VoxelType tValue);
		/// tries to load all voxels inside the given region
		/// if MaxNumberOfBlocksInMemory is not large enough to support the region
		/// this function will only load part of the region. it is undefined which parts will actually be loaded
		/// if all the voxels in the given region are already loaded, this function will not do anything
		/// other blocks might be unloaded to make space for the new blocks
		void prefetch(Region regPrefetch);
		/// attempts to unload all the voxels in the given region.
		/// there is no guarantee that any voxels are unloaded
		/// try unloading a region whose sidelengths are multiples of BlockSideLength
		void flush(Region regFlush);
		/// unloads all data
		void flushAll();

		void clearBlockCache(void);
		float calculateCompressionRatio(void);
		uint32_t calculateSizeInBytes(void);

		/// Deprecated - I don't think we should expose this function? Let us know if you disagree...
		void resize(const Region& regValidRegion, uint16_t uBlockSideLength);

private:
		/// gets called when a new region is allocated and needs to be filled
		/// NOTE: accessing ANY voxels outside this region during the process of this function
		/// is absolutely unsafe
		polyvox_function<void(const ConstVolumeProxy<VoxelType>&, const Region&)> m_funcDataRequiredHandler;
		/// gets called when a Region needs to be stored by the user, because Volume will erase it right after
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
		uint32_t m_uMaxNumberOfBlocksInMemory;

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
		bool m_bPagingEnabled;
	};
}

#include "Volume.inl"

#endif

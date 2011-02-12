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
	////////////////////////////////////////////////////////////////////////////////
	/// A Volume is essentially a '3D image' in which each element (voxel) is identified
	/// by a three dimensional (x,y,z) coordinate, rather than the two dimensional (x,y)
	/// coordinate which is used to identify an element (pixel) in a normal image. Within
	/// PolyVox, the Volume class is used to store and manipulate our data before we extract 
	/// our SurfaceMeshs from it.
	/// 
	/// <b>Data Representaion - feel free to skip</b>
	/// If stored carelessly, volume data can take up a huge amount of memory. For example, a
	/// volume of dimensions 1024x1024x1024 with 1 byte per voxel will require 1GB of memory
	/// if stored in an uncompressed form. Natuarally our Volume class is much more efficient
	/// than this and it is worth understanding (at least at a high level) the approach
	/// which is used.
	/// 
	/// Essentially, the Volume class stores its data as a collection of blocks. Each 
	/// of these block is much smaller than the whole volume, for example a typical size
	/// might be 32x32x32 voxels (though is is configurable by the user). In this case,
	/// a 256x512x1024 volume would contain 8x16x32 = 4096 blocks. However, it is unlikely that
	/// all these blocks actually have to be stored because usually there are duplicates
	/// in which case common data can be shared.
	/// 
	/// Identifying duplicate blocks is in general a difficult task which involves looking at pairs
	/// of blocks and comparing all the voxels. This is a time consuming task which is not amiable 
	/// to being performed when the volume is being modified in real time. However, there are two
	/// specific scenarios which are easily spotted and which PolyVox uses to identify block
	/// sharing opportunities.
	/// 
	/// -# Homogeneous blocks (those which contain just a single voxel value) are easy to
	/// spot and fairly common becuase volumes often contain large homogeous regions. Any time
	/// you change the value of a voxel you have potentially made the block which contains
	/// it homogeneous. PolyVox does not check the homogeneity immediately as this would slow
	/// down the process of modifying voxels, but you can use the tidyUpMemory() function
	/// to check for and remove duplicate homogeneous regions whenever you have spare
	/// processing time.
	/// 
	/// -# Copying a volume naturally means that all the voxels in the second volume are
	/// the same as the first. Therefore volume copying is a relatively fast operation in
	/// which all the blocks in the second volume simply reference the first volume. Future
	/// modifications to either volume will, of course, cause the blocks to become unshared.
	/// 
	/// Other advantages of breaking the volume down into blocks include enhancing data locality
	/// (i.e. voxels which are spatially near to each other are also likely to be near in
	/// memory) and the ability to load larger volumes as no large contiguous areas of
	/// memory are needed. However, these advantages are more transparent to user code
	/// so we will not dwell on them here.
	/// 
	/// <b>Usage</b>
	/// Volumes are constructed by passing the desired width, height and depth to the
	/// constructor. Note that for speed reasons only values which are a power of two
	/// are permitted for these sidelengths.
	/// 
	/// \code
	/// Volume<uint8_t> volData(g_uVolumeSideLength, g_uVolumeSideLength, g_uVolumeSideLength);
	/// \endcode
	/// 
	/// Access to specific voxels is provided by the getVoxelAt() and setVoxelAt() fuctions.
	/// Each of these has two forms so that voxels can be identified by integer triples
	/// or by Vector3DUint16%s.
	/// 
	/// \code
	/// volData.setVoxelAt(12, 186, 281, 3);
	/// uint8_t voxelValue = volData.getVoxelAt(12, 186, 281);
	/// //voxelValue is now 3
	/// \endcode
	/// 
	/// The tidyUpMemory() function should normally be called after you first populate
	/// the volume with data, and then at periodic intervals as the volume is modified.
	/// However, you don't actually <i>have</i> to call it at all. See the function's
	/// documentation for further details.
	/// 
	/// One further important point of note is that this class is templatised on the voxel
	/// type. This allows you to store volumes of data types you might not normally expect,
	/// for example the OpenGL example 'abuses' this class to store a 3D grid of pointers.
	/// However, it is not guaranteed that all functionality works correctly with non-integer
	/// voxel types.
	////////////////////////////////////////////////////////////////////////////////
	template <typename VoxelType>
	class Volume
	{
		//Make VolumeSampler a friend
		friend class VolumeSampler<VoxelType>;

		struct UncompressedBlock
		{
			uint32_t uBlockIndex;
			VoxelType* data;

		};

	public:		
		///Constructor
		Volume(uint16_t uWidth, uint16_t uHeight, uint16_t uDepth, uint16_t uBlockSideLength = 32);
		///Destructor
		~Volume();	

		///Gets the value used for voxels which are outside the volume
		VoxelType getBorderValue(void) const;
		///Gets a Region representing the extents of the Volume.
		Region getEnclosingRegion(void) const;
		///Gets the width of the volume in voxels.
		uint16_t getWidth(void) const;
		///Gets the height of the volume in voxels.
		uint16_t getHeight(void) const;
		///Gets the depth of the volume in voxels.
		uint16_t getDepth(void) const;
		///Gets the length of the longest side in voxels
		uint16_t getLongestSideLength(void) const;
		///Gets the length of the shortest side in voxels
		uint16_t getShortestSideLength(void) const;
		///Gets the length of the diagonal in voxels
		float getDiagonalLength(void) const;
		///Gets a voxel by <tt>x,y,z</tt> position
		VoxelType getVoxelAt(uint16_t uXPos, uint16_t uYPos, uint16_t uZPos) const;
		///Gets a voxel by 3D vector position
		VoxelType getVoxelAt(const Vector3DUint16& v3dPos) const;

		///Sets the value used for voxels which are outside the volume
		void setBorderValue(const VoxelType& tBorder);
		///Sets the voxel at an <tt>x,y,z</tt> position
		bool setVoxelAt(uint16_t uXPos, uint16_t uYPos, uint16_t uZPos, VoxelType tValue);
		///Sets the voxel at a 3D vector position
		bool setVoxelAt(const Vector3DUint16& v3dPos, VoxelType tValue);

		///Resizes the volume to the specified dimensions
		void resize(uint16_t uWidth, uint16_t uHeight, uint16_t uDepth, uint16_t uBlockSideLength = 32);

		void setBlockCacheSize(uint16_t uBlockCacheSize);
		void clearBlockCache(void);

		uint32_t sizeInBytes(void);

	public:
		Block<VoxelType>* getUncompressedBlock(uint16_t uBlockX, uint16_t uBlockY, uint16_t uBlockZ) const;

		//Block<VoxelType> m_pBorderBlock;
		VoxelType* m_pUncompressedBorderData;

		Block<VoxelType>* m_pBlocks;
		uint32_t* m_pUncompressedTimestamps;
		mutable std::vector< UncompressedBlock > m_vecUncompressedBlockCache;
		uint16_t m_uMaxUncompressedBlockCacheSize;
		uint32_t m_ulastAccessedBlockIndex;

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
		mutable uint64_t m_uTimestamper;

		mutable uint32_t m_uCompressions;
		mutable uint32_t m_uUncompressions;
	};

	//Some handy typedefs
	typedef Volume<float> FloatVolume;
	typedef Volume<uint8_t> UInt8Volume;
	typedef Volume<uint16_t> UInt16Volume;	
}

#include "Volume.inl"

#endif

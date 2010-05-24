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

#ifndef __PolyVox_Volume_H__
#define __PolyVox_Volume_H__

#pragma region Headers
#include "PolyVoxImpl/Block.h"
#include "PolyVoxForwardDeclarations.h"

#include <limits>
#include <map>
#include <vector>
#include <memory>

#pragma endregion

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
	/// it homogeneous. PolyVox does not check the homogeneity immediatly as this would slow
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
	/// Volumes are constructed by passing the desired width height and depth to the
	/// constructor. Note that for speed reasons only values which are a power of two
	/// are permitted for these sidelengths.
	/// 
	/// Access to specific voxels is provided by the getVoxelAt() and setVoxelAt fuctions.
	/// Each of these has two forms so that voxels can be identified by integer triples
	/// or by Vector3DUint16's.
	/// 
	/// The tidyUpMemory() function should normally be called after you first populate
	/// the volume with data, and then at periodic intervals as the volume is modified.
	/// However, you don't actually <i>have</i> to call it at all. See the functions
	/// documentation for further details.
	/// 
	/// One further important point of note is that this class is templatised on the voxel
	/// type. This allows you to store volumes of data types you might not normally expect,
	/// for example theOpenGL example 'abuses' this class to store a 3D grid of pointers.
	/// However, it is not guarentted that all functionality works correctly with non-integer
	/// voxel types.
	////////////////////////////////////////////////////////////////////////////////
	template <typename VoxelType>
	class Volume
	{
		//Make VolumeSampler a friend
		friend class VolumeSampler<VoxelType>;

	public:		
		///Constructor
		Volume(uint16_t uWidth, uint16_t uHeight, uint16_t uDepth, uint16_t uBlockSideLength = 32);
		///Destructor
		~Volume();	

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
		VoxelType getVoxelAt(uint16_t uXPos, uint16_t uYPos, uint16_t uZPos) const;
		VoxelType getVoxelAt(const Vector3DUint16& v3dPos) const;

		void setVoxelAt(uint16_t uXPos, uint16_t uYPos, uint16_t uZPos, VoxelType tValue);
		void setVoxelAt(const Vector3DUint16& v3dPos, VoxelType tValue);

		void tidyUpMemory(uint32_t uNoOfBlocksToProcess = (std::numeric_limits<uint32_t>::max)());

	private:
		std::shared_ptr< Block<VoxelType> > getHomogenousBlock(VoxelType tHomogenousValue);

		std::vector< std::shared_ptr< Block<VoxelType> > > m_pBlocks;
		std::vector<bool> m_vecBlockIsPotentiallyHomogenous;

		//Note: We were once storing weak_ptr's in this map, so that the blocks would be deleted once they
		//were not being referenced by anyone else. However, this made it difficult to know when a block was
		//shared. A call to shared_ptr::unique() from within setVoxel was not sufficient as weak_ptr's did
		//not contribute to the reference count. Instead we store shared_ptr's here, and check if they
		//are used by anyone else (i.e are non-unique) when we tidy the volume.
		std::map<VoxelType, std::shared_ptr< Block<VoxelType> > > m_pHomogenousBlock;

		uint32_t m_uNoOfBlocksInVolume;

		uint16_t m_uWidthInBlocks;
		uint16_t m_uHeightInBlocks;
		uint16_t m_uDepthInBlocks;

		uint16_t m_uWidth;
		uint8_t m_uWidthPower;		

		uint16_t m_uHeight;
		uint8_t m_uHeightPower;	

		uint16_t m_uDepth;
		uint8_t m_uDepthPower;	

		uint8_t m_uBlockSideLengthPower;
		uint16_t m_uBlockSideLength;

		uint16_t m_uLongestSideLength;
		uint16_t m_uShortestSideLength;

		float m_fDiagonalLength;

		uint32_t m_uCurrentBlockForTidying;
	};

	//Some handy typedefs
	typedef Volume<float> FloatVolume;
	typedef Volume<uint8_t> UInt8Volume;
	typedef Volume<uint16_t> UInt16Volume;	
}

#include "Volume.inl"

#endif

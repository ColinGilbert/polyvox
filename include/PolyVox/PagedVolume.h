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

#ifndef __PolyVox_PagedVolume_H__
#define __PolyVox_PagedVolume_H__

#include "BaseVolume.h"
#include "Region.h"
#include "Vector.h"

#include <limits>
#include <cstdlib> //For abort()
#include <cstring> //For memcpy
#include <unordered_map>
#include <list>
#include <map>
#include <memory>
#include <stdexcept> //For invalid_argument
#include <vector>

namespace PolyVox
{
	/// The PagedVolume class provides a memory efficient method of storing voxel data while also allowing fast access and modification.
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	/// A PagedVolume is essentially a 3D array in which each element (or <i>voxel</i>) is identified by a three dimensional (x,y,z) coordinate.
	/// We use the PagedVolume class to store our data in an efficient way, and it is the input to many of the algorithms (such as the surface
	/// extractors) which form the heart of PolyVox. The PagedVolume class is templatised so that different types of data can be stored within each voxel.
	///
	/// Basic usage
	/// -----------
	///
	/// The following code snippet shows how to construct a volume and demonstrates basic usage:
	///
	/// \code
	/// PagedVolume<int> volume(Region(Vector3DInt32(0,0,0), Vector3DInt32(63,127,255)));
	/// volume.setVoxel(15, 90, 42, int(5));
	/// std::cout << "Voxel at (15, 90, 42) has value: " << volume.getVoxel(15, 90, 42) << std::endl;
	/// std::cout << "Width = " << volume.getWidth() << ", Height = " << volume.getHeight() << ", Depth = " << volume.getDepth() << std::endl;
	/// \endcode
	/// 
	/// The PagedVolume constructor takes a Region as a parameter. This specifies the valid range of voxels which can be held in the volume, so in this
	/// particular case the valid voxel positions are (0,0,0) to (63, 127, 255). The result of attempts to access voxels outside this range will result
	/// are defined by the WrapMode). PolyVox also has support for near infinite volumes which will be discussed later.
	/// 
	/// Access to individual voxels is provided via the setVoxel() and getVoxel() member functions. Advanced users may also be interested in
	/// the Sampler nested class for faster read-only access to a large number of voxels.
	/// 
	/// Lastly the example prints out some properties of the PagedVolume. Note that the dimentsions getWidth(), getHeight(), and getDepth() are inclusive, such
	/// that the width is 64 when the range of valid x coordinates goes from 0 to 63.
	/// 
	/// Data Representaion
	/// ------------------
	/// If stored carelessly, volume data can take up a huge amount of memory. For example, a volume of dimensions 1024x1024x1024 with
	/// 1 byte per voxel will require 1GB of memory if stored in an uncompressed form. Natuarally our PagedVolume class is much more efficient
	/// than this and it is worth understanding (at least at a high level) the approach which is used.
	///
	/// Essentially, the PagedVolume class stores its data as a collection of chunks. Each of these chunk is much smaller than the whole volume,
	/// for example a typical size might be 32x32x32 voxels (though is is configurable by the user). In this case, a 256x512x1024 volume
	/// would contain 8x16x32 = 4096 chunks. Typically these chunks do not need to all be in memory all the time, and the Pager class can 
	/// be used to control how they are loaded and unloaded. This mechanism allows a
	/// potentially unlimited amount of data to be loaded, provided the user is able to take responsibility for storing any data which PolyVox
	/// cannot fit in memory, and then returning it back to PolyVox on demand. For example, the user might choose to temporarily store this data
	/// on disk or stream it to a remote database.
	///
	/// Essentially you are providing an extension to the PagedVolume class - a way for data to be stored once PolyVox has run out of memory for it. Note
	/// that you don't actually have to do anything with the data - you could simply decide that once it gets removed from memory it doesn't matter
	/// anymore.
	///
	/// Cache-aware traversal
	/// ---------------------
	/// *NOTE: This needs updating for PagedVolume rather than the old LargeVolume*
	/// You might be suprised at just how many cache misses can occur when you traverse the volume in a naive manner. Consider a 1024x1024x1024 volume
	/// with chunks of size 32x32x32. And imagine you iterate over this volume with a simple three-level for loop which iterates over x, the y, then z.
	/// If you start at position (0,0,0) then ny the time you reach position (1023,0,0) you have touched 1024 voxels along one edge of the volume and
	/// have pulled 32 chunks into the cache. By the time you reach (1023,1023,0) you have hit 1024x1024 voxels and pulled 32x32 chunks into the cache.
	/// You are now ready to touch voxel (0,0,1) which is right next to where you started, but unless your cache is at least 32x32 chunks large then this
	/// initial chunk has already been cleared from the cache.
	///
	/// Ensuring you have a large enough cache size can obviously help the above situation, but you might also consider iterating over the voxels in a
	/// different order. For example, if you replace your three-level loop with a six-level loop then you can first process all the voxels between (0,0,0)
	/// and (31,31,31), then process all the voxels between (32,0,0) and (63,0,0), and so forth. Using this approach you will have no cache misses even
	/// is your cache size is only one. Of course the logic is more complex, but writing code in such a cache-aware manner may be beneficial in some situations.
	///
	/// Threading
	/// ---------
	/// The PagedVolume class does not make any guarentees about thread safety. You should ensure that all accesses are performed from the same thread.
	/// This is true even if you are only reading data from the volume, as concurrently reading from different threads can invalidate the contents
	/// of the chunk cache (amoung other problems).
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	template <typename VoxelType>
	class PagedVolume : public BaseVolume<VoxelType>
	{
	public:
		/// The PagedVolume stores it data as a set of Chunk instances which can be loaded and unloaded as memory requirements dictate.
		class Chunk;
		/// The Pager class is responsible for the loading and unloading of Chunks, and can be overridden by the user.
		class Pager;

		class Chunk
		{
			friend class PagedVolume;

		public:
			Chunk(Vector3DInt32 v3dPosition, uint16_t uSideLength, Pager* pPager = nullptr);
			~Chunk();

			VoxelType* getData(void) const;
			uint32_t getDataSizeInBytes(void) const;

			VoxelType getVoxel(uint32_t uXPos, uint32_t uYPos, uint32_t uZPos) const;
			VoxelType getVoxel(const Vector3DUint16& v3dPos) const;

			void setVoxel(uint32_t uXPos, uint32_t uYPos, uint32_t uZPos, VoxelType tValue);
			void setVoxel(const Vector3DUint16& v3dPos, VoxelType tValue);

			void changeLinearOrderingToMorton(void);
			void changeMortonOrderingToLinear(void);

		private:
			/// Private copy constructor to prevent accisdental copying
			Chunk(const Chunk& /*rhs*/) {};

			/// Private assignment operator to prevent accisdental copying
			Chunk& operator=(const Chunk& /*rhs*/) {};

			// This is updated by the PagedVolume and used to discard the least recently used chunks.
			uint32_t m_uChunkLastAccessed;

			// This is so we can tell whether a uncompressed chunk has to be recompressed and whether
			// a compressed chunk has to be paged back to disk, or whether they can just be discarded.
			bool m_bDataModified;

			uint32_t calculateSizeInBytes(void);
			static uint32_t calculateSizeInBytes(uint32_t uSideLength);

			VoxelType* m_tData;
			uint16_t m_uSideLength;
			uint8_t m_uSideLengthPower;
			Pager* m_pPager;

			// Note: Do we really need to store this position here as well as in the block maps?
			Vector3DInt32 m_v3dChunkSpacePosition;
		};

		/**
		* Users can override this class and provide an instance of the derived class to the PagedVolume constructor. This derived class
		* could then perform tasks such as compression and decompression of the data, and read/writing it to a file, database, network,
		* or other storage as appropriate. See FilePager for a simple example of such a derived class.
		*/
		class Pager
		{
		public:
			/// Constructor
			Pager() {};
			/// Destructor
			virtual ~Pager() {};

			virtual void pageIn(const Region& region, Chunk* pChunk) = 0;
			virtual void pageOut(const Region& region, Chunk* pChunk) = 0;
		};

		//There seems to be some descrepency between Visual Studio and GCC about how the following class should be declared.
		//There is a work around (see also See http://goo.gl/qu1wn) given below which appears to work on VS2010 and GCC, but
		//which seems to cause internal compiler errors on VS2008 when building with the /Gm 'Enable Minimal Rebuild' compiler
		//option. For now it seems best to 'fix' it with the preprocessor insstead, but maybe the workaround can be reinstated
		//in the future
		//typedef Volume<VoxelType> VolumeOfVoxelType; //Workaround for GCC/VS2010 differences.
		//class Sampler : public VolumeOfVoxelType::template Sampler< PagedVolume<VoxelType> >
		#ifndef SWIG
#if defined(_MSC_VER)
		class Sampler : public BaseVolume<VoxelType>::Sampler< PagedVolume<VoxelType> > //This line works on VS2010
#else
		class Sampler : public BaseVolume<VoxelType>::template Sampler< PagedVolume<VoxelType> > //This line works on GCC
#endif
		{
		public:
			Sampler(PagedVolume<VoxelType>* volume);
			~Sampler();

			inline VoxelType getVoxel(void) const;			

			void setPosition(const Vector3DInt32& v3dNewPos);
			void setPosition(int32_t xPos, int32_t yPos, int32_t zPos);
			inline bool setVoxel(VoxelType tValue);

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
			//Other current position information
			VoxelType* mCurrentVoxel;

			uint16_t m_uXPosInChunk;
			uint16_t m_uYPosInChunk;
			uint16_t m_uZPosInChunk;

			// This should ideally be const, but that prevent automatic generation of an assignment operator (https://goo.gl/Sn7KpZ).
			// We could provide one manually, but it's currently unused so there is no real test for if it works. I'm putting
			// together a new release at the moment so I'd rathern not make 'risky' changes.
			uint16_t m_uChunkSideLengthMinusOne;
		};

		#endif // SWIG

	public:
		/// Constructor for creating a fixed size volume.
		PagedVolume(Pager* pPager, uint32_t uTargetMemoryUsageInBytes = 256 * 1024 * 1024, uint16_t uChunkSideLength = 32);
		/// Destructor
		~PagedVolume();

		/// Gets a voxel at the position given by <tt>x,y,z</tt> coordinates
		VoxelType getVoxel(int32_t uXPos, int32_t uYPos, int32_t uZPos) const;
		/// Gets a voxel at the position given by a 3D vector
		VoxelType getVoxel(const Vector3DInt32& v3dPos) const;

		/// Sets the voxel at the position given by <tt>x,y,z</tt> coordinates
		void setVoxel(int32_t uXPos, int32_t uYPos, int32_t uZPos, VoxelType tValue);
		/// Sets the voxel at the position given by a 3D vector
		void setVoxel(const Vector3DInt32& v3dPos, VoxelType tValue);

		/// Tries to ensure that the voxels within the specified Region are loaded into memory.
		void prefetch(Region regPrefetch);
		/// Ensures that any voxels within the specified Region are removed from memory.
		//void flush(Region regFlush);
		/// Removes all voxels from memory
		void flushAll();

		/// Calculates approximatly how many bytes of memory the volume is currently using.
		uint32_t calculateSizeInBytes(void);

	protected:
		/// Copy constructor
		PagedVolume(const PagedVolume& rhs);

		/// Assignment operator
		PagedVolume& operator=(const PagedVolume& rhs);

	private:
		bool canReuseLastAccessedChunk(int32_t iChunkX, int32_t iChunkY, int32_t iChunkZ) const;
		Chunk* getChunk(int32_t uChunkX, int32_t uChunkY, int32_t uChunkZ) const;

		// Storing these properties individually has proved to be faster than keeping
		// them in a Vector3DInt32 as it avoids constructions and comparison overheads.
		// They are also at the start of the class in the hope that they will be pulled
		// into cache - I've got no idea if this actually makes a difference.
		mutable int32_t m_v3dLastAccessedChunkX = 0;
		mutable int32_t m_v3dLastAccessedChunkY = 0;
		mutable int32_t m_v3dLastAccessedChunkZ = 0;
		mutable Chunk* m_pLastAccessedChunk = nullptr;

		mutable uint32_t m_uTimestamper = 0;

		uint32_t m_uChunkCountLimit = 0;

		// Chunks are stored in the following array which is used as a hash-table. Conventional wisdom is that such a hash-table
		// should not be more than half full to avoid conflicts, and a practical chunk size seems to be 64^3. With this configuration
		// there can be up to 32768*64^3 = 8 gigavoxels (with each voxel perhaps being many bytes). This should effectively make use 
		// of even high end machines. Of course, the user can choose to limit the memory usage in which case much less of the chunk 
		// array will actually be used. None-the-less, we have chosen to use a fixed size array (rather than a vector) as it appears to 
		// be slightly faster (probably due to the extra pointer indirection in a vector?) and the actual size of this array should
		// just be 1Mb or so.
		static const uint32_t uChunkArraySize = 65536;
		mutable std::unique_ptr< Chunk > m_arrayChunks[uChunkArraySize];

		// The size of the chunks
		uint16_t m_uChunkSideLength;
		uint8_t m_uChunkSideLengthPower;
		int32_t m_iChunkMask;

		Pager* m_pPager = nullptr;
	};
}

#include "PagedVolume.inl"
#include "PagedVolumeChunk.inl"
#include "PagedVolumeSampler.inl"

#endif //__PolyVox_PagedVolume_H__

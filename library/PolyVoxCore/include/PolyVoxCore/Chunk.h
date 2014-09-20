/*******************************************************************************
Copyright (c) 2005-2013 David Williams and Matthew Williams

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

#ifndef __PolyVox_Chunk_H__
#define __PolyVox_Chunk_H__

#include "PolyVoxCore/PolyVoxForwardDeclarations.h"
#include "PolyVoxCore/Vector.h"

namespace PolyVox
{
	template <typename VoxelType>
    class Chunk
    {
		friend class PagedVolume<VoxelType>;

	public:
		Chunk(Vector3DInt32 v3dPosition, uint16_t uSideLength, Pager<VoxelType>* pPager = nullptr);
		~Chunk();

		VoxelType* getData(void) const;
		uint32_t getDataSizeInBytes(void) const;

		VoxelType getVoxel(uint16_t uXPos, uint16_t uYPos, uint16_t uZPos) const;
		VoxelType getVoxel(const Vector3DUint16& v3dPos) const;

		void setVoxelAt(uint16_t uXPos, uint16_t uYPos, uint16_t uZPos, VoxelType tValue);
		void setVoxelAt(const Vector3DUint16& v3dPos, VoxelType tValue);

	private:
		/// Private copy constructor to prevent accisdental copying
		Chunk(const Chunk& /*rhs*/) {};

		/// Private assignment operator to prevent accisdental copying
		Chunk& operator=(const Chunk& /*rhs*/) {};

		// This is updated by the PagedVolume and used to discard the least recently used blocks.
		uint32_t m_uBlockLastAccessed;

		// This is so we can tell whether a uncompressed block has to be recompressed and whether
		// a compressed block has to be paged back to disk, or whether they can just be discarded.
		bool m_bDataModified;

		// Made this private for consistancy with CompressedBlock.
		// Users shouldn't really need this for Chunk anyway.
		uint32_t calculateSizeInBytes(void);
		static uint32_t calculateSizeInBytes(uint32_t uSideLength);

        VoxelType* m_tData;
        uint16_t m_uSideLength;
        uint8_t m_uSideLengthPower;
		Pager<VoxelType>* m_pPager;
		Vector3DInt32 m_v3dBlockSpacePosition;
	};
}

#include "PolyVoxCore/Chunk.inl"

#endif

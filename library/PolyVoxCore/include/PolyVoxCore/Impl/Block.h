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

#ifndef __PolyVox_Block_H__
#define __PolyVox_Block_H__

#include "PolyVoxCore/Impl/TypeDef.h"

#include "PolyVoxCore/PolyVoxForwardDeclarations.h"
#include "PolyVoxCore/Vector.h"

#include <limits>
#include <vector>

namespace PolyVox
{
	template <typename VoxelType>
	class Block
	{
		friend LargeVolume<VoxelType>;

	public:
		Block()
			:m_uBlockLastAccessed(0)
			,m_bDataModified(true)
		{
		}

	protected:
		// This is updated by the LargeVolume and used to discard the least recently used blocks.
		uint32_t m_uBlockLastAccessed;

		// This is so we can tell whether a uncompressed block has to be recompressed and whether
		// a compressed block has to be paged back to disk, or whether they can just be discarded.
		bool m_bDataModified;

	private:
		/// Private copy constructor to prevent accisdental copying
		Block(const Block& /*rhs*/) {};

		/// Private assignment operator to prevent accisdental copying
		Block& operator=(const Block& /*rhs*/) {};
	};

	template <typename VoxelType>
	class CompressedBlock : public Block<VoxelType>
	{
		friend LargeVolume<VoxelType>;

	public:
		CompressedBlock();
		~CompressedBlock();

		const uint8_t* getData(void) const;
		uint32_t getDataSizeInBytes(void) const;

		void setData(const uint8_t* const pData, uint32_t uDataSizeInBytes);

	private:
		/// Private copy constructor to prevent accisdental copying
		CompressedBlock(const CompressedBlock& /*rhs*/) {};

		/// Private assignment operator to prevent accisdental copying
		CompressedBlock& operator=(const CompressedBlock& /*rhs*/) {};

		// Made this private to avoid any confusion with getDataSizeInBytes().
		// Users shouldn't really need this for CompressedBlock anyway.
		uint32_t calculateSizeInBytes(void);

		uint8_t* m_pData;
		uint32_t m_uDataSizeInBytes;
	};

	template <typename VoxelType>
    class UncompressedBlock : public Block<VoxelType>
    {
		friend LargeVolume<VoxelType>;

	public:
		UncompressedBlock(uint16_t uSideLength);
		~UncompressedBlock();

		VoxelType* getData(void) const;
		uint32_t getDataSizeInBytes(void) const;

		VoxelType getVoxel(uint16_t uXPos, uint16_t uYPos, uint16_t uZPos) const;
		VoxelType getVoxel(const Vector3DUint16& v3dPos) const;

		void setVoxelAt(uint16_t uXPos, uint16_t uYPos, uint16_t uZPos, VoxelType tValue);
		void setVoxelAt(const Vector3DUint16& v3dPos, VoxelType tValue);

	private:
		/// Private copy constructor to prevent accisdental copying
		UncompressedBlock(const UncompressedBlock& /*rhs*/) {};

		/// Private assignment operator to prevent accisdental copying
		UncompressedBlock& operator=(const UncompressedBlock& /*rhs*/) {};

		// Made this private for consistancy with CompressedBlock.
		// Users shouldn't really need this for UncompressedBlock anyway.
		uint32_t calculateSizeInBytes(void);

        VoxelType* m_tData;
        uint16_t m_uSideLength;
        uint8_t m_uSideLengthPower;     
	};
}

#include "PolyVoxCore/Impl/Block.inl"

#endif

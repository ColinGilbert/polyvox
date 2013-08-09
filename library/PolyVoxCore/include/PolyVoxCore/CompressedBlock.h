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

#ifndef __PolyVox_CompressedBlock_H__
#define __PolyVox_CompressedBlock_H__

#include "PolyVoxCore/Block.h"

namespace PolyVox
{
	template <typename VoxelType>
	class CompressedBlock : public Block<VoxelType>
	{
		friend class LargeVolume<VoxelType>;

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
}

#include "PolyVoxCore/CompressedBlock.inl"

#endif

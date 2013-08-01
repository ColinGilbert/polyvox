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
		friend class LargeVolume<VoxelType>;

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
}

#endif

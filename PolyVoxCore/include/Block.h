#pragma region License
/******************************************************************************
This file is part of the PolyVox library
Copyright (C) 2006  David Williams

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
******************************************************************************/
#pragma endregion

#ifndef __PolyVox_Block_H__
#define __PolyVox_Block_H__

#pragma region Headers
#include "PolyVoxForwardDeclarations.h"

#include "PolyVoxCStdInt.h"
#pragma endregion

namespace PolyVox
{
	template <typename VoxelType>
	class Block
	{
		//Make BlockVolumeIterator a friend
		friend class BlockVolumeIterator<VoxelType>;
	public:
		Block(std::uint8_t uSideLengthPower);
		Block(const Block& rhs);
		~Block();

		Block& operator=(const Block& rhs);

		std::uint16_t getSideLength(void) const;
		VoxelType getVoxelAt(std::uint16_t uXPos, std::uint16_t uYPos, std::uint16_t uZPos) const;

		void setVoxelAt(std::uint16_t uXPos, std::uint16_t uYPos, std::uint16_t uZPos, VoxelType tValue);

		void fill(VoxelType tValue);

	private:
		std::uint8_t m_uSideLengthPower;
		std::uint16_t m_uSideLength;
		VoxelType* m_tData;
	};
}

#include "Block.inl"

#endif

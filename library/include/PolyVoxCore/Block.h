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
		//Make VolumeIterator a friend
		friend class VolumeIterator<VoxelType>;
	public:
		Block(uint8 uSideLengthPower);
		Block(const Block& rhs);
		~Block();

		Block& operator=(const Block& rhs);

		uint16 getSideLength(void) const;
		VoxelType getVoxelAt(uint16 uXPos, uint16 uYPos, uint16 uZPos) const;

		void setVoxelAt(uint16 uXPos, uint16 uYPos, uint16 uZPos, VoxelType tValue);

		void fill(VoxelType tValue);

	private:
		uint8 m_uSideLengthPower;
		uint16 m_uSideLength;
		VoxelType* m_tData;
	};
}

#include "Block.inl"

#endif

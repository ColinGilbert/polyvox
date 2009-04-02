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

#ifndef __PolyVox_BlockData_H__
#define __PolyVox_BlockData_H__

#pragma region Headers
#include "../PolyVoxForwardDeclarations.h"

#include "CPlusPlusZeroXSupport.h"
#pragma endregion

namespace PolyVox
{
	template <typename VoxelType>
	class BlockData
	{
		//Make VolumeIterator a friend
		friend class VolumeIterator<VoxelType>;
	public:
		BlockData(uint16_t uSideLength);
		BlockData(const BlockData& rhs);
		~BlockData();

		BlockData& operator=(const BlockData& rhs);

		uint16_t getSideLength(void) const;
		VoxelType getVoxelAt(uint16_t uXPos, uint16_t uYPos, uint16_t uZPos) const;
		VoxelType getVoxelAt(const Vector3DUint16& v3dPos) const;

		void setVoxelAt(uint16_t uXPos, uint16_t uYPos, uint16_t uZPos, VoxelType tValue);
		void setVoxelAt(const Vector3DUint16& v3dPos, VoxelType tValue);

		void fill(VoxelType tValue);
		bool isHomogeneous(void);

	private:
		uint16_t m_uSideLength;
		uint8_t m_uSideLengthPower;	
		VoxelType* m_tData;
	};
}

#include "BlockData.inl"

#endif

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

#ifndef __PolyVox_LinearVolume_H__
#define __PolyVox_LinearVolume_H__

#pragma region Headers
#include "Constants.h"
#include "PolyVoxForwardDeclarations.h"
#include "TypeDef.h"

#include "PolyVoxCStdInt.h"
#pragma endregion

namespace PolyVox
{
	template <typename VoxelType>
	class LinearVolume
	{
	public:
		LinearVolume(uint8 uSideLengthPower);
		LinearVolume(const LinearVolume& rhs);
		~LinearVolume();

		LinearVolume& operator=(const LinearVolume& rhs);

		//bool isHomogeneous(void);

		uint16 getSideLength(void);

		VoxelType getVoxelAt(const uint16 xPosition, const uint16 yPosition, const uint16 zPosition) const;
		void setVoxelAt(const uint16 xPosition, const uint16 yPosition, const uint16 zPosition, const VoxelType value);

		//void fillWithValue(const VoxelType value);

	private:
		uint32 getNoOfVoxels(void);
		uint8 m_uSideLengthPower;
		uint16 m_uSideLength;
		VoxelType* m_tData;		
	};
}

#include "LinearVolume.inl"

#endif

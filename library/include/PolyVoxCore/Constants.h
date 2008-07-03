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

#ifndef __PolyVox_Constants_H__
#define __PolyVox_Constants_H__

#include "PolyVoxForwardDeclarations.h"

namespace PolyVox
{
	//FIXME - i think we can define mod using a bitmask which flattens the upper bits. Should define that here.
	//const uint32 POLYVOX_BLOCK_SIDE_LENGTH_POWER = 5;
	//const uint32 POLYVOX_BLOCK_SIDE_LENGTH = (0x0001 << POLYVOX_BLOCK_SIDE_LENGTH_POWER);
	//const uint32 POLYVOX_NO_OF_VOXELS_IN_BLOCK = (POLYVOX_BLOCK_SIDE_LENGTH * POLYVOX_BLOCK_SIDE_LENGTH * POLYVOX_BLOCK_SIDE_LENGTH);

	const uint16 POLYVOX_VOLUME_SIDE_LENGTH_POWER = 8;
	const uint16 POLYVOX_VOLUME_SIDE_LENGTH = (0x0001 << POLYVOX_VOLUME_SIDE_LENGTH_POWER);
	//const uint32 POLYVOX_VOLUME_SIDE_LENGTH_IN_BLOCKS = (POLYVOX_VOLUME_SIDE_LENGTH >> POLYVOX_BLOCK_SIDE_LENGTH_POWER);
	//const uint32 POLYVOX_NO_OF_BLOCKS_IN_VOLUME = (POLYVOX_VOLUME_SIDE_LENGTH_IN_BLOCKS * POLYVOX_VOLUME_SIDE_LENGTH_IN_BLOCKS * POLYVOX_VOLUME_SIDE_LENGTH_IN_BLOCKS);
	//const uint32 POLYVOX_NO_OF_VOXELS_IN_VOLUME = (POLYVOX_VOLUME_SIDE_LENGTH * POLYVOX_VOLUME_SIDE_LENGTH * POLYVOX_VOLUME_SIDE_LENGTH);

	const uint16 POLYVOX_REGION_SIDE_LENGTH_POWER = 4;
	const uint16 POLYVOX_REGION_SIDE_LENGTH = (0x0001 << POLYVOX_REGION_SIDE_LENGTH_POWER);
	const uint16 POLYVOX_VOLUME_SIDE_LENGTH_IN_REGIONS = (POLYVOX_VOLUME_SIDE_LENGTH >> POLYVOX_REGION_SIDE_LENGTH_POWER);
}

#endif

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

#ifndef __PolyVoxImpl_ReferenceSurfaceExtractor_H__
#define __PolyVoxImpl_ReferenceSurfaceExtractor_H__

#pragma region Headers
#include "PolyVoxCore/Constants.h"
#include "PolyVoxCore/PolyVoxForwardDeclarations.h"
#include "PolyVoxCore/TypeDef.h"

#include "PolyVoxCore/PolyVoxCStdInt.h"
#pragma endregion

namespace PolyVox
{
	POLYVOX_API void generateReferenceMeshDataForRegionImpl(BlockVolume<uint8>* volumeData, Region region, IndexedSurfacePatch* singleMaterialPatch);
	int32 getIndexFor(const Vector3DFloat& pos, int32 vertexIndicesX[POLYVOX_REGION_SIDE_LENGTH+1][POLYVOX_REGION_SIDE_LENGTH+1][POLYVOX_REGION_SIDE_LENGTH+1], int32 vertexIndicesY[POLYVOX_REGION_SIDE_LENGTH+1][POLYVOX_REGION_SIDE_LENGTH+1][POLYVOX_REGION_SIDE_LENGTH+1], int32 vertexIndicesZ[POLYVOX_REGION_SIDE_LENGTH+1][POLYVOX_REGION_SIDE_LENGTH+1][POLYVOX_REGION_SIDE_LENGTH+1]);
	void setIndexFor(const Vector3DFloat& pos, int32 newIndex, int32 vertexIndicesX[POLYVOX_REGION_SIDE_LENGTH+1][POLYVOX_REGION_SIDE_LENGTH+1][POLYVOX_REGION_SIDE_LENGTH+1], int32 vertexIndicesY[POLYVOX_REGION_SIDE_LENGTH+1][POLYVOX_REGION_SIDE_LENGTH+1][POLYVOX_REGION_SIDE_LENGTH+1], int32 vertexIndicesZ[POLYVOX_REGION_SIDE_LENGTH+1][POLYVOX_REGION_SIDE_LENGTH+1][POLYVOX_REGION_SIDE_LENGTH+1]);
}

#endif
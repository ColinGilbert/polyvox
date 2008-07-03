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

#ifndef __PolyVox_SurfaceExtractors_H__
#define __PolyVox_SurfaceExtractors_H__

#pragma region Headers
#include "Constants.h"
#include "PolyVoxForwardDeclarations.h"
#include "TypeDef.h"

#include "PolyVoxCStdInt.h"

#include <list>
#pragma endregion

namespace PolyVox
{
	uint32 getIndex(uint32 x, uint32 y);

	POLYVOX_API void generateRoughMeshDataForRegion(BlockVolume<uint8>* volumeData, Region region, IndexedSurfacePatch* singleMaterialPatch);
	POLYVOX_API uint32 computeInitialRoughBitmaskForSlice(BlockVolumeIterator<uint8>& volIter, const Region& regSlice, const Vector3DFloat& offset, uint8 *bitmask);
	POLYVOX_API uint32 computeRoughBitmaskForSliceFromPrevious(BlockVolumeIterator<uint8>& volIter, const Region& regSlice, const Vector3DFloat& offset, uint8 *bitmask, uint8 *previousBitmask);
	POLYVOX_API void generateRoughIndicesForSlice(BlockVolumeIterator<uint8>& volIter, const Region& regSlice, IndexedSurfacePatch* singleMaterialPatch, const Vector3DFloat& offset, uint8* bitmask0, uint8* bitmask1, int32 vertexIndicesX0[],int32 vertexIndicesY0[],int32 vertexIndicesZ0[], int32 vertexIndicesX1[],int32 vertexIndicesY1[],int32 vertexIndicesZ1[]);
	POLYVOX_API void generateRoughVerticesForSlice(BlockVolumeIterator<uint8>& volIter, Region& regSlice, const Vector3DFloat& offset, uint8* bitmask, IndexedSurfacePatch* singleMaterialPatch,int32 vertexIndicesX[],int32 vertexIndicesY[],int32 vertexIndicesZ[]);

	POLYVOX_API void generateReferenceMeshDataForRegion(BlockVolume<uint8>* volumeData, Region region, IndexedSurfacePatch* singleMaterialPatch);

	int32 getIndexFor(const Vector3DFloat& pos, int32 vertexIndicesX[POLYVOX_REGION_SIDE_LENGTH+1][POLYVOX_REGION_SIDE_LENGTH+1][POLYVOX_REGION_SIDE_LENGTH+1], int32 vertexIndicesY[POLYVOX_REGION_SIDE_LENGTH+1][POLYVOX_REGION_SIDE_LENGTH+1][POLYVOX_REGION_SIDE_LENGTH+1], int32 vertexIndicesZ[POLYVOX_REGION_SIDE_LENGTH+1][POLYVOX_REGION_SIDE_LENGTH+1][POLYVOX_REGION_SIDE_LENGTH+1]);
	void setIndexFor(const Vector3DFloat& pos, int32 newIndex, int32 vertexIndicesX[POLYVOX_REGION_SIDE_LENGTH+1][POLYVOX_REGION_SIDE_LENGTH+1][POLYVOX_REGION_SIDE_LENGTH+1], int32 vertexIndicesY[POLYVOX_REGION_SIDE_LENGTH+1][POLYVOX_REGION_SIDE_LENGTH+1][POLYVOX_REGION_SIDE_LENGTH+1], int32 vertexIndicesZ[POLYVOX_REGION_SIDE_LENGTH+1][POLYVOX_REGION_SIDE_LENGTH+1][POLYVOX_REGION_SIDE_LENGTH+1]);
}

#endif

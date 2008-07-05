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

#ifndef __PolyVoxImpl_FastSurfaceExtractor_H__
#define __PolyVoxImpl_FastSurfaceExtractor_H__

#pragma region Headers
#include "PolyVoxCore/Constants.h"
#include "PolyVoxCore/PolyVoxForwardDeclarations.h"
#include "PolyVoxCore/TypeDef.h"

#include "PolyVoxCore/PolyVoxCStdInt.h"
#pragma endregion

namespace PolyVox
{
	void extractFastSurfaceImpl(BlockVolume<uint8>* volumeData, Region region, IndexedSurfacePatch* singleMaterialPatch);
	uint32 getIndex(uint32 x, uint32 y);
	uint32 computeInitialRoughBitmaskForSlice(BlockVolumeIterator<uint8>& volIter, const Region& regSlice, const Vector3DFloat& offset, uint8 *bitmask);
	uint32 computeRoughBitmaskForSliceFromPrevious(BlockVolumeIterator<uint8>& volIter, const Region& regSlice, const Vector3DFloat& offset, uint8 *bitmask, uint8 *previousBitmask);
	void generateRoughIndicesForSlice(BlockVolumeIterator<uint8>& volIter, const Region& regSlice, IndexedSurfacePatch* singleMaterialPatch, const Vector3DFloat& offset, uint8* bitmask0, uint8* bitmask1, int32 vertexIndicesX0[],int32 vertexIndicesY0[],int32 vertexIndicesZ0[], int32 vertexIndicesX1[],int32 vertexIndicesY1[],int32 vertexIndicesZ1[]);
	void generateRoughVerticesForSlice(BlockVolumeIterator<uint8>& volIter, Region& regSlice, const Vector3DFloat& offset, uint8* bitmask, IndexedSurfacePatch* singleMaterialPatch,int32 vertexIndicesX[],int32 vertexIndicesY[],int32 vertexIndicesZ[]);
}

#endif

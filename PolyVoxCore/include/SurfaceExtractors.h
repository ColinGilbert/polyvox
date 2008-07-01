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
	POLYVOX_API std::list<RegionGeometry> getChangedRegionGeometry(VolumeChangeTracker& volume);

	std::uint32_t getIndex(std::uint32_t x, std::uint32_t y);

	POLYVOX_API void generateRoughMeshDataForRegion(BlockVolume<std::uint8_t>* volumeData, Region region, IndexedSurfacePatch* singleMaterialPatch);
	POLYVOX_API std::uint32_t computeInitialRoughBitmaskForSlice(BlockVolumeIterator<std::uint8_t>& volIter, const Region& regSlice, const Vector3DFloat& offset, std::uint8_t *bitmask);
	POLYVOX_API std::uint32_t computeRoughBitmaskForSliceFromPrevious(BlockVolumeIterator<std::uint8_t>& volIter, const Region& regSlice, const Vector3DFloat& offset, std::uint8_t *bitmask, std::uint8_t *previousBitmask);
	POLYVOX_API void generateRoughIndicesForSlice(BlockVolumeIterator<std::uint8_t>& volIter, const Region& regSlice, IndexedSurfacePatch* singleMaterialPatch, const Vector3DFloat& offset, std::uint8_t* bitmask0, std::uint8_t* bitmask1, std::int32_t vertexIndicesX0[],std::int32_t vertexIndicesY0[],std::int32_t vertexIndicesZ0[], std::int32_t vertexIndicesX1[],std::int32_t vertexIndicesY1[],std::int32_t vertexIndicesZ1[]);
	POLYVOX_API void generateRoughVerticesForSlice(BlockVolumeIterator<std::uint8_t>& volIter, Region& regSlice, const Vector3DFloat& offset, std::uint8_t* bitmask, IndexedSurfacePatch* singleMaterialPatch,std::int32_t vertexIndicesX[],std::int32_t vertexIndicesY[],std::int32_t vertexIndicesZ[]);

	POLYVOX_API void generateReferenceMeshDataForRegion(BlockVolume<std::uint8_t>* volumeData, Region region, IndexedSurfacePatch* singleMaterialPatch);

	std::int32_t getIndexFor(const Vector3DFloat& pos, std::int32_t vertexIndicesX[POLYVOX_REGION_SIDE_LENGTH+1][POLYVOX_REGION_SIDE_LENGTH+1][POLYVOX_REGION_SIDE_LENGTH+1], std::int32_t vertexIndicesY[POLYVOX_REGION_SIDE_LENGTH+1][POLYVOX_REGION_SIDE_LENGTH+1][POLYVOX_REGION_SIDE_LENGTH+1], std::int32_t vertexIndicesZ[POLYVOX_REGION_SIDE_LENGTH+1][POLYVOX_REGION_SIDE_LENGTH+1][POLYVOX_REGION_SIDE_LENGTH+1]);
	void setIndexFor(const Vector3DFloat& pos, std::int32_t newIndex, std::int32_t vertexIndicesX[POLYVOX_REGION_SIDE_LENGTH+1][POLYVOX_REGION_SIDE_LENGTH+1][POLYVOX_REGION_SIDE_LENGTH+1], std::int32_t vertexIndicesY[POLYVOX_REGION_SIDE_LENGTH+1][POLYVOX_REGION_SIDE_LENGTH+1][POLYVOX_REGION_SIDE_LENGTH+1], std::int32_t vertexIndicesZ[POLYVOX_REGION_SIDE_LENGTH+1][POLYVOX_REGION_SIDE_LENGTH+1][POLYVOX_REGION_SIDE_LENGTH+1]);
}

#endif

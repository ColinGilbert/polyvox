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

#ifndef __PolyVox_SurfaceExtractorsDecimated_H__
#define __PolyVox_SurfaceExtractorsDecimated_H__

#pragma region Headers
#include "Constants.h"
#include "PolyVoxForwardDeclarations.h"
#include "TypeDef.h"

#include "boost/cstdint.hpp"

#include <list>
#pragma endregion

namespace PolyVox
{
	boost::uint32_t getDecimatedIndex(boost::uint32_t x, boost::uint32_t y);

	POLYVOX_API void generateDecimatedMeshDataForRegion(BlockVolume<boost::uint8_t>* volumeData, boost::uint8_t uLevel, Region region, IndexedSurfacePatch* singleMaterialPatch);
	POLYVOX_API boost::uint32_t computeInitialDecimatedBitmaskForSlice(BlockVolumeIterator<boost::uint8_t>& volIter, boost::uint8_t uLevel, const Region& regSlice, const Vector3DFloat& offset, boost::uint8_t *bitmask);
	POLYVOX_API boost::uint32_t computeDecimatedBitmaskForSliceFromPrevious(BlockVolumeIterator<boost::uint8_t>& volIter, boost::uint8_t uLevel, const Region& regSlice, const Vector3DFloat& offset, boost::uint8_t *bitmask, boost::uint8_t *previousBitmask);
	POLYVOX_API void generateDecimatedIndicesForSlice(BlockVolumeIterator<boost::uint8_t>& volIter, boost::uint8_t uLevel, const Region& regSlice, IndexedSurfacePatch* singleMaterialPatch, const Vector3DFloat& offset, boost::uint8_t* bitmask0, boost::uint8_t* bitmask1, boost::int32_t vertexIndicesX0[],boost::int32_t vertexIndicesY0[],boost::int32_t vertexIndicesZ0[], boost::int32_t vertexIndicesX1[],boost::int32_t vertexIndicesY1[],boost::int32_t vertexIndicesZ1[]);
	POLYVOX_API void generateDecimatedVerticesForSlice(BlockVolumeIterator<boost::uint8_t>& volIter, boost::uint8_t uLevel, Region& regSlice, const Vector3DFloat& offset, boost::uint8_t* bitmask, IndexedSurfacePatch* singleMaterialPatch,boost::int32_t vertexIndicesX[],boost::int32_t vertexIndicesY[],boost::int32_t vertexIndicesZ[]);

	POLYVOX_API void generateDecimatedMeshDataForRegionSlow(BlockVolume<boost::uint8_t>* volumeData, Region region, IndexedSurfacePatch* singleMaterialPatch);

	POLYVOX_API Vector3DFloat computeDecimatedNormal(BlockVolume<boost::uint8_t>* volumeData, const Vector3DFloat& position, NormalGenerationMethod normalGenerationMethod);
}

#endif

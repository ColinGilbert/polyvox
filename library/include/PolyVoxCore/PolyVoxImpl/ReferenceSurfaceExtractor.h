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

#include "CPlusPlusZeroXSupport.h"
#pragma endregion

#include <vector>

namespace PolyVox
{
	///A simple version of the surface extractor optimised for readability rather than speed.
	void extractReferenceSurfaceImpl(Volume<uint8_t>* volumeData, Region region, IndexedSurfacePatch* singleMaterialPatch); //FIXME - should pass variables by reference?
	///Determines whether a vertex already exists for a given edge, and if so returns it's index.
	int32_t getIndexFor(const Vector3DFloat& pos, const Vector3DInt32& regionDimensions, const std::vector<int32_t>& vertexIndicesX, const std::vector<int32_t>& vertexIndicesY, const std::vector<int32_t>& vertexIndicesZ);
	///Sets the index of an existing vertex for a given edge.
	void setIndexFor(const Vector3DFloat& pos, const Vector3DInt32& regionDimensions, int32_t newIndex, std::vector<int32_t>& vertexIndicesX, std::vector<int32_t>& vertexIndicesY, std::vector<int32_t>& vertexIndicesZ);
}

#endif
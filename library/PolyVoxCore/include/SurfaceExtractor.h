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

#ifndef __PolyVox_SurfaceExtractor_H__
#define __PolyVox_SurfaceExtractor_H__

#pragma region Headers
#include "PolyVoxForwardDeclarations.h"
#include "VolumeSampler.h"

#include "PolyVoxImpl/TypeDef.h"
#include "PolyVoxImpl/CPlusPlusZeroXSupport.h"
#pragma endregion

namespace PolyVox
{
	class POLYVOXCORE_API SurfaceExtractor
	{
	public:
		SurfaceExtractor(Volume<uint8_t>& volData);

		uint8_t getLodLevel(void);

		void setLodLevel(uint8_t uLodLevel);

		POLYVOX_SHARED_PTR<IndexedSurfacePatch> extractSurfaceForRegion(Region region);

	private:
		uint8_t m_uLodLevel;
		uint8_t m_uStepSize;

		Volume<uint8_t> m_volData;
		VolumeSampler<uint8_t> m_iterVolume;

		uint8_t* m_pPreviousBitmask;
		uint8_t* m_pCurrentBitmask;


		int32_t* m_pPreviousVertexIndicesX;
		int32_t* m_pPreviousVertexIndicesY; 
		int32_t* m_pPreviousVertexIndicesZ; 
		int32_t* m_pCurrentVertexIndicesX; 
		int32_t* m_pCurrentVertexIndicesY; 
		int32_t* m_pCurrentVertexIndicesZ; 

		uint32_t getIndex(uint32_t x, uint32_t y, uint32_t regionWidth);

		//void extractSurfaceForRegionLevel0(Volume<uint8_t>* volumeData, Region region, IndexedSurfacePatch* singleMaterialPatch);

		void extractSurfaceImpl(Volume<uint8_t>* volumeData, Region region, IndexedSurfacePatch* singleMaterialPatch);
		uint32_t computeBitmaskForSlice(VolumeSampler<uint8_t>& volIter, const Region& regSlice, const Vector3DFloat& offset);
		void generateIndicesForSlice(VolumeSampler<uint8_t>& volIter, const Region& regSlice, IndexedSurfacePatch* singleMaterialPatch, const Vector3DFloat& offset);
		void generateVerticesForSlice(VolumeSampler<uint8_t>& volIter, Region& regSlice, const Vector3DFloat& offset, IndexedSurfacePatch* singleMaterialPatch);
	};
}

#endif

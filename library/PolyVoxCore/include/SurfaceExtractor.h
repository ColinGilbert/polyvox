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
		VolumeSampler<uint8_t> m_sampVolume;

		uint8_t* m_pPreviousBitmask;
		uint8_t* m_pCurrentBitmask;

		int32_t* m_pPreviousVertexIndicesX;
		int32_t* m_pPreviousVertexIndicesY; 
		int32_t* m_pPreviousVertexIndicesZ; 
		int32_t* m_pCurrentVertexIndicesX; 
		int32_t* m_pCurrentVertexIndicesY; 
		int32_t* m_pCurrentVertexIndicesZ;

		uint8_t v000;
		uint8_t v100;
		uint8_t v010;
		uint8_t v110;
		uint8_t v001;
		uint8_t v101;
		uint8_t v011;
		uint8_t v111;

		uint16_t uXVolSpace;
		uint16_t uYVolSpace;
		uint16_t uZVolSpace;

		uint16_t uXRegSpace;
		uint16_t uYRegSpace;
		uint16_t uZRegSpace;

		uint32_t m_uNoOfOccupiedCells;

		inline uint32_t getIndex(uint32_t x, uint32_t y)
		{
			return x + (y * (m_uRegionWidth+2));
		}

		IndexedSurfacePatch* m_ispCurrent;

		Vector3DFloat m_v3dRegionOffset;

		Region regSlice0;
		Region regSlice1;

		uint16_t m_uRegionWidth;
		//uint16_t m_uRegionWidthOverStepSize;
		//uint16_t m_uRegionHeightOverStepSize;

		//void extractSurfaceForRegionLevel0(Volume<uint8_t>* volumeData, Region region, IndexedSurfacePatch* m_ispCurrent);

		template<uint8_t uLodLevel>
		void extractSurfaceImpl(Region region);

		template<bool isPrevZAvail, uint8_t uLodLevel>
		uint32_t computeBitmaskForSlice(void);

		template<bool isPrevXAvail, bool isPrevYAvail, bool isPrevZAvail, uint8_t uLodLevel>
		void computeBitmaskForCell(void);

		void generateIndicesForSlice();
		void generateVerticesForSlice();
	};
}

#endif

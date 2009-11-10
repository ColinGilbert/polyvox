#pragma region License
/*******************************************************************************
Copyright (c) 2005-2009 David Williams

This software is provided 'as-is', without any express or implied
warranty. In no event will the authors be held liable for any damages
arising from the use of this software.

Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute it
freely, subject to the following restrictions:

    1. The origin of this software must not be misrepresented; you must not
    claim that you wrote the original software. If you use this software
    in a product, an acknowledgment in the product documentation would be
    appreciated but is not required.

    2. Altered source versions must be plainly marked as such, and must not be
    misrepresented as being the original software.

    3. This notice may not be removed or altered from any source
    distribution. 	
*******************************************************************************/
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
		//Extract the surface for a particular LOD level
		template<uint8_t uLodLevel>
		void extractSurfaceImpl(void);

		//Compute the cell bitmask for a particular slice in z.
		template<bool isPrevZAvail, uint8_t uLodLevel>
		uint32_t computeBitmaskForSlice(void);

		//Compute the cell bitmask for a given cell.
		template<bool isPrevXAvail, bool isPrevYAvail, bool isPrevZAvail, uint8_t uLodLevel>
		void computeBitmaskForCell(void);

		//Use the cell bitmasks to generate all the vertices needed for that slice
		void generateVerticesForSlice();

		//Use the cell bitmasks to generate all the indices needed for that slice
		void generateIndicesForSlice();

		//Converts a position into an index for accessing scratch areas.
		inline uint32_t getIndex(uint32_t x, uint32_t y)
		{
			return x + (y * m_uScratchPadWidth);
		}

		//The lod level can step size.
		uint8_t m_uLodLevel;
		uint8_t m_uStepSize;

		//The volume data and a sampler to access it.
		Volume<uint8_t> m_volData;
		VolumeSampler<uint8_t> m_sampVolume;

		//Cell bitmasks for the current and previous slices.
		uint8_t* m_pPreviousBitmask;
		uint8_t* m_pCurrentBitmask;

		//Used to keep track of where generated vertices have been placed.
		int32_t* m_pPreviousVertexIndicesX;
		int32_t* m_pPreviousVertexIndicesY; 
		int32_t* m_pPreviousVertexIndicesZ; 
		int32_t* m_pCurrentVertexIndicesX; 
		int32_t* m_pCurrentVertexIndicesY; 
		int32_t* m_pCurrentVertexIndicesZ;

		//Holds a position in volume space.
		uint16_t uXVolSpace;
		uint16_t uYVolSpace;
		uint16_t uZVolSpace;

		//Holds a position in region space.
		uint16_t uXRegSpace;
		uint16_t uYRegSpace;
		uint16_t uZRegSpace;

		//Used to return the number of cells in a slice which contain triangles.
		uint32_t m_uNoOfOccupiedCells;

		//The surface patch we are currently filling.
		IndexedSurfacePatch* m_ispCurrent;

		//Information about the region we are currently processing
		Region m_regInputCropped;
		Region m_regInputUncropped;
		Region m_regVolumeCropped;
		Region m_regSlicePrevious;
		Region m_regSliceCurrent;

		//Store the width and height because they are frequently
		//used and have some overhead to compute.
		uint16_t m_uRegionWidth;
		uint16_t m_uRegionHeight;

		//These are used in several places so best stored.
		uint16_t m_uScratchPadWidth;
		uint16_t m_uScratchPadHeight;		
	};
}

#endif

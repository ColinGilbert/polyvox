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

#ifndef __PolyVox_IndexedSurfacePatch_H__
#define __PolyVox_IndexedSurfacePatch_H__

#include <vector>
#include <set>

#include "PolyVoxImpl/CPlusPlusZeroXSupport.h"

#include "PolyVoxForwardDeclarations.h"
#include "Region.h"
#include "SurfaceVertex.h"
#include "PolyVoxImpl/TypeDef.h"

namespace PolyVox
{
	class LodRecord
	{
	public:
		int beginIndex;
		int endIndex; //Let's put it just past the end STL style
	};

	class POLYVOXCORE_API IndexedSurfacePatch
	{
	public:
	   IndexedSurfacePatch();
	   ~IndexedSurfacePatch();	   

	   const std::vector<uint32_t>& getIndices(void) const;
	   uint32_t getNoOfIndices(void) const;
	   uint32_t getNoOfNonUniformTrianges(void) const;
	   uint32_t getNoOfUniformTrianges(void) const;
	   uint32_t getNoOfVertices(void) const;	   
	   std::vector<SurfaceVertex>& getRawVertexData(void); //FIXME - this should be removed
	   const std::vector<SurfaceVertex>& getVertices(void) const;

	   void addTriangle(uint32_t index0, uint32_t index1, uint32_t index2);
	   uint32_t addVertex(const SurfaceVertex& vertex);
	   void clear(void);
	   const bool isEmpty(void) const;

	   void smoothPositions(float fAmount, bool bIncludeEdgeVertices = false);
	   void sumNearbyNormals(bool bNormaliseResult = true);

	   POLYVOX_SHARED_PTR<IndexedSurfacePatch> extractSubset(std::set<uint8_t> setMaterials);

	   void generateAveragedFaceNormals(bool bNormalise, bool bIncludeEdgeVertices = false);

	   //Vector3DInt32 m_v3dRegionPosition; //FIXME - remove this?

	   /*void growMaterialBoundary(void);
	   int countMaterialBoundary(void);*/

	   void makeProgressiveMesh(void);

	   Region m_Region;

	   int32_t m_iTimeStamp;

	   int32_t m_iNoOfLod0Tris;
	
	public:		
		std::vector<uint32_t> m_vecTriangleIndices;
		std::vector<SurfaceVertex> m_vecVertices;

		std::vector<LodRecord> m_vecLodRecords;

		//The set of materials which are in this mesh. Only those materials
		//which cover a whole triangle are counted. Materials which only
		//exist on a material boundary do not count.
		std::set<uint8_t> m_mapUsedMaterials;
	};	
}

#endif /* __IndexedSurfacePatch_H__ */

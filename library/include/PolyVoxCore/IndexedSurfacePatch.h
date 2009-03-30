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

#include "PolyVoxImpl/CPlusPlusZeroXSupport.h"

#include "Constants.h"
#include "PolyVoxForwardDeclarations.h"
#include "SurfaceVertex.h"
#include "TypeDef.h"

namespace PolyVox
{
	class POLYVOX_API IndexedSurfacePatch
	{
	public:
	   IndexedSurfacePatch();
	   ~IndexedSurfacePatch();	   

	   const std::vector<uint32>& getIndices(void) const;
	   uint32 getNoOfIndices(void) const;
	   uint32 getNoOfNonUniformTrianges(void) const;
	   uint32 getNoOfUniformTrianges(void) const;
	   uint32 getNoOfVertices(void) const;	   
	   std::vector<SurfaceVertex>& getRawVertexData(void); //FIXME - this shoudl be removed
	   const std::vector<SurfaceVertex>& getVertices(void) const;

	   void addTriangle(uint32 index0, uint32 index1, uint32 index2);
	   uint32 addVertex(const SurfaceVertex& vertex);
	   void clear(void);
	   const bool isEmpty(void) const;

	   Vector3DInt32 m_v3dRegionPosition; //FIXME - remove this?

	   int32 m_iTimeStamp;
	
	private:		
		std::vector<uint32> m_vecTriangleIndices;
		std::vector<SurfaceVertex> m_vecVertices;
	};	

}

#endif /* __IndexedSurfacePatch_H__ */

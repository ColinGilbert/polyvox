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

#include "boost/cstdint.hpp"

#include "Constants.h"
#include "PolyVoxForwardDeclarations.h"
#include "SurfaceVertex.h"
#include "TypeDef.h"

namespace PolyVox
{
	class POLYVOX_API IndexedSurfacePatch
	{
	public:
	   IndexedSurfacePatch(bool allowDuplicateVertices);
	   ~IndexedSurfacePatch();	   

	   void addTriangle(const SurfaceVertex& v0,const SurfaceVertex& v1,const SurfaceVertex& v2);
	   void fillVertexAndIndexData(std::vector<SurfaceVertex>& vecVertices, std::vector<boost::uint32_t>& vecIndices);

	   const std::vector<SurfaceVertex>& getVertices(void) const;
	   std::vector<SurfaceVertex>& getVertices(void); //FIXME - non const version should be removed.
	   const std::vector<boost::uint32_t>& getIndices(void) const;

	   unsigned short getNoNonUniformTrianges(void);
	   unsigned short getNoUniformTrianges(void);
	
	public:		
		std::vector<boost::uint32_t> m_vecTriangleIndices;
		std::vector<SurfaceVertex> m_vecVertices;

		static boost::int32_t vertexIndicesX[POLYVOX_REGION_SIDE_LENGTH+1][POLYVOX_REGION_SIDE_LENGTH+1][POLYVOX_REGION_SIDE_LENGTH+1];
		static boost::int32_t vertexIndicesY[POLYVOX_REGION_SIDE_LENGTH+1][POLYVOX_REGION_SIDE_LENGTH+1][POLYVOX_REGION_SIDE_LENGTH+1];
		static boost::int32_t vertexIndicesZ[POLYVOX_REGION_SIDE_LENGTH+1][POLYVOX_REGION_SIDE_LENGTH+1][POLYVOX_REGION_SIDE_LENGTH+1];

		static boost::int32_t noOfVerticesSubmitted;
		static boost::int32_t noOfVerticesAccepted;
		static boost::int32_t noOfTrianglesSubmitted;

		boost::int32_t getIndexFor(const Vector3DFloat& pos);
		void setIndexFor(const Vector3DFloat& pos, boost::int32_t newIndex);

	public:
		bool m_AllowDuplicateVertices;
	};	

}

#endif /* __IndexedSurfacePatch_H__ */

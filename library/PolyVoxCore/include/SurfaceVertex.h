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

#ifndef __PolyVox_SurfaceVertex_H__
#define __PolyVox_SurfaceVertex_H__

#include "PolyVoxImpl/TypeDef.h"
#include "Vector.h"

#include <bitset>
#include <vector>

namespace PolyVox
{	
	enum POLYVOXCORE_API VertexFlags
	{
		VF_ON_MATERIAL_EDGE,
		VF_ON_GEOMETRY_EDGE_NEG_X,
		VF_ON_GEOMETRY_EDGE_POS_X ,
		VF_ON_GEOMETRY_EDGE_NEG_Y ,
		VF_ON_GEOMETRY_EDGE_POS_Y ,
		VF_ON_GEOMETRY_EDGE_NEG_Z ,
		VF_ON_GEOMETRY_EDGE_POS_Z,
		VF_NO_OF_FLAGS
	};

	class POLYVOXCORE_API SurfaceVertex
	{
	public:	
		SurfaceVertex();
		SurfaceVertex(Vector3DFloat positionToSet, float materialToSet);
		SurfaceVertex(Vector3DFloat positionToSet, Vector3DFloat normalToSet, float materialToSet);	

		float getMaterial(void) const;
		const Vector3DFloat& getNormal(void) const;
		const Vector3DFloat& getPosition(void) const;	
		//unsigned int getNoOfGeometryEdges(void) const;

		bool isOnEdge(void) const;
		bool isOnGeometryEdge(void) const;
		bool isOnGeometryEdgeNegX(void) const;
		bool isOnGeometryEdgePosX(void) const;
		bool isOnGeometryEdgeNegY(void) const;
		bool isOnGeometryEdgePosY(void) const;
		bool isOnGeometryEdgeNegZ(void) const;
		bool isOnGeometryEdgePosZ(void) const;
		bool isOnMaterialEdge(void) const;

		void setMaterial(float materialToSet);
		void setNormal(const Vector3DFloat& normalToSet);
		void setOnGeometryEdgeNegX(bool bOnRegionEdge);
		void setOnGeometryEdgePosX(bool bOnRegionEdge);
		void setOnGeometryEdgeNegY(bool bOnRegionEdge);
		void setOnGeometryEdgePosY(bool bOnRegionEdge);
		void setOnGeometryEdgeNegZ(bool bOnRegionEdge);
		void setOnGeometryEdgePosZ(bool bOnRegionEdge);
		void setOnMaterialEdge(bool bOnMaterialEdge);
		void setPosition(const Vector3DFloat& positionToSet);

	public:		
		Vector3DFloat position;
		Vector3DFloat normal;
		float material; //FIXME: This shouldn't be float on CPU?
		std::bitset<VF_NO_OF_FLAGS> m_bFlags;	
	};

	
	

	//bool operator < (const SurfaceVertexIterator& lhs, const SurfaceVertexIterator& rhs);
}

#endif

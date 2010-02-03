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

#include <sstream>

#include "SurfaceVertex.h"

namespace PolyVox
{
	SurfaceVertex::SurfaceVertex()
	{
	}

	SurfaceVertex::SurfaceVertex(Vector3DFloat positionToSet, float materialToSet)
		:position(positionToSet)
		,material(materialToSet)
	{
		
	}

	SurfaceVertex::SurfaceVertex(Vector3DFloat positionToSet, Vector3DFloat normalToSet, float materialToSet)
		:position(positionToSet)
		,normal(normalToSet)
		,material(materialToSet)
	{
	}

	float SurfaceVertex::getMaterial(void) const
	{
		return material;
	}

	const Vector3DFloat& SurfaceVertex::getNormal(void) const
	{
		return normal;
	}

	const Vector3DFloat& SurfaceVertex::getPosition(void) const
	{
		return position;
	}
	
	//Returns a value between 0-3, for how many geometry edges (X,Y,Z) this vertex is on.
	unsigned int SurfaceVertex::getNoOfGeometryEdges(void) const
	{
		unsigned int uIsOnEdgeX = static_cast<unsigned int>(isOnGeometryEdgeX());
		unsigned int uIsOnEdgeY = static_cast<unsigned int>(isOnGeometryEdgeY());
		unsigned int uIsOnEdgeZ = static_cast<unsigned int>(isOnGeometryEdgeZ());
		return uIsOnEdgeX + uIsOnEdgeY + uIsOnEdgeZ;
	}

	bool SurfaceVertex::isOnEdge(void) const
	{
		return (isOnMaterialEdge() || isOnGeometryEdge());
	}

	bool SurfaceVertex::isOnMaterialEdge(void) const
	{
		return m_bFlags[VF_ON_MATERIAL_EDGE];
	}

	bool SurfaceVertex::isOnGeometryEdge(void) const
	{
		return m_bFlags[VF_ON_GEOMETRY_EDGE_X] || m_bFlags[VF_ON_GEOMETRY_EDGE_Y] || m_bFlags[VF_ON_GEOMETRY_EDGE_Z];
	}

	bool SurfaceVertex::isOnGeometryEdgeX(void) const
	{
		return m_bFlags[VF_ON_GEOMETRY_EDGE_X];
	}

	bool SurfaceVertex::isOnGeometryEdgeY(void) const
	{
		return m_bFlags[VF_ON_GEOMETRY_EDGE_Y];
	}

	bool SurfaceVertex::isOnGeometryEdgeZ(void) const
	{
		return m_bFlags[VF_ON_GEOMETRY_EDGE_Z];
	}

	void SurfaceVertex::setMaterial(float materialToSet)
	{
		material = materialToSet;
	}

	void SurfaceVertex::setNormal(const Vector3DFloat& normalToSet)
	{
		normal = normalToSet;
	}	

	void SurfaceVertex::setOnMaterialEdge(bool bOnMaterialEdge)
	{
		m_bFlags[VF_ON_MATERIAL_EDGE] = bOnMaterialEdge;
	}

	void SurfaceVertex::setOnGeometryEdgeX(bool bOnRegionEdge)
	{
		m_bFlags[VF_ON_GEOMETRY_EDGE_X] = bOnRegionEdge;
	}

	void SurfaceVertex::setOnGeometryEdgeY(bool bOnRegionEdge)
	{
		m_bFlags[VF_ON_GEOMETRY_EDGE_Y] = bOnRegionEdge;
	}

	void SurfaceVertex::setOnGeometryEdgeZ(bool bOnRegionEdge)
	{
		m_bFlags[VF_ON_GEOMETRY_EDGE_Z] = bOnRegionEdge;
	}

	void SurfaceVertex::setPosition(const Vector3DFloat& positionToSet)
	{
		position = positionToSet;
	}
}

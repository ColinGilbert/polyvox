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

#include <sstream>

#include "VertexTypes.h"

namespace PolyVox
{
	PositionMaterialNormal::PositionMaterialNormal()
	{
	}

	PositionMaterialNormal::PositionMaterialNormal(Vector3DFloat positionToSet, float materialToSet)
		:position(positionToSet)
		,material(materialToSet)
	{
		
	}

	PositionMaterialNormal::PositionMaterialNormal(Vector3DFloat positionToSet, Vector3DFloat normalToSet, float materialToSet)
		:position(positionToSet)
		,normal(normalToSet)
		,material(materialToSet)
	{
	}

	float PositionMaterialNormal::getMaterial(void) const
	{
		return material;
	}

	const Vector3DFloat& PositionMaterialNormal::getNormal(void) const
	{
		return normal;
	}

	const Vector3DFloat& PositionMaterialNormal::getPosition(void) const
	{
		return position;
	}
	
	//Returns a value between 0-3, for how many geometry edges (X,Y,Z) this vertex is on.
	/*unsigned int PositionMaterialNormal::getNoOfGeometryEdges(void) const
	{
		unsigned int uIsOnEdgeX = static_cast<unsigned int>(isOnGeometryEdgeX());
		unsigned int uIsOnEdgeY = static_cast<unsigned int>(isOnGeometryEdgeY());
		unsigned int uIsOnEdgeZ = static_cast<unsigned int>(isOnGeometryEdgeZ());
		return uIsOnEdgeX + uIsOnEdgeY + uIsOnEdgeZ;
	}*/

	bool PositionMaterialNormal::isOnEdge(void) const
	{
		return (isOnMaterialEdge() || isOnGeometryEdge());
	}

	bool PositionMaterialNormal::isOnMaterialEdge(void) const
	{
		return m_bFlags[VF_ON_MATERIAL_EDGE];
	}

	bool PositionMaterialNormal::isOnGeometryEdge(void) const
	{
		return 
			m_bFlags [VF_ON_GEOMETRY_EDGE_NEG_X] || m_bFlags[VF_ON_GEOMETRY_EDGE_POS_X] ||
			m_bFlags [VF_ON_GEOMETRY_EDGE_NEG_Y] || m_bFlags[VF_ON_GEOMETRY_EDGE_POS_Y] ||
			m_bFlags [VF_ON_GEOMETRY_EDGE_NEG_Z] || m_bFlags[VF_ON_GEOMETRY_EDGE_POS_Z];
	}

	bool PositionMaterialNormal::isOnGeometryEdgeNegX(void) const
	{
		return m_bFlags[VF_ON_GEOMETRY_EDGE_NEG_X];
	}

	bool PositionMaterialNormal::isOnGeometryEdgePosX(void) const
	{
		return m_bFlags[VF_ON_GEOMETRY_EDGE_POS_X];
	}

	bool PositionMaterialNormal::isOnGeometryEdgeNegY(void) const
	{
		return m_bFlags[VF_ON_GEOMETRY_EDGE_NEG_Y];
	}

	bool PositionMaterialNormal::isOnGeometryEdgePosY(void) const
	{
		return m_bFlags[VF_ON_GEOMETRY_EDGE_POS_Y];
	}

	bool PositionMaterialNormal::isOnGeometryEdgeNegZ(void) const
	{
		return m_bFlags[VF_ON_GEOMETRY_EDGE_NEG_Z];
	}

	bool PositionMaterialNormal::isOnGeometryEdgePosZ(void) const
	{
		return m_bFlags[VF_ON_GEOMETRY_EDGE_POS_Z];
	}

	void PositionMaterialNormal::setMaterial(float materialToSet)
	{
		material = materialToSet;
	}

	void PositionMaterialNormal::setNormal(const Vector3DFloat& normalToSet)
	{
		normal = normalToSet;
	}	

	void PositionMaterialNormal::setOnMaterialEdge(bool bOnMaterialEdge)
	{
		m_bFlags[VF_ON_MATERIAL_EDGE] = bOnMaterialEdge;
	}

	void PositionMaterialNormal::setOnGeometryEdgeNegX(bool bOnRegionEdge)
	{
		m_bFlags[VF_ON_GEOMETRY_EDGE_NEG_X] = bOnRegionEdge;
	}

	void PositionMaterialNormal::setOnGeometryEdgePosX(bool bOnRegionEdge)
	{
		m_bFlags[VF_ON_GEOMETRY_EDGE_POS_X] = bOnRegionEdge;
	}

	void PositionMaterialNormal::setOnGeometryEdgeNegY(bool bOnRegionEdge)
	{
		m_bFlags[VF_ON_GEOMETRY_EDGE_NEG_Y] = bOnRegionEdge;
	}

	void PositionMaterialNormal::setOnGeometryEdgePosY(bool bOnRegionEdge)
	{
		m_bFlags[VF_ON_GEOMETRY_EDGE_POS_Y] = bOnRegionEdge;
	}

	void PositionMaterialNormal::setOnGeometryEdgeNegZ(bool bOnRegionEdge)
	{
		m_bFlags[VF_ON_GEOMETRY_EDGE_NEG_Z] = bOnRegionEdge;
	}

	void PositionMaterialNormal::setOnGeometryEdgePosZ(bool bOnRegionEdge)
	{
		m_bFlags[VF_ON_GEOMETRY_EDGE_POS_Z] = bOnRegionEdge;
	}

	void PositionMaterialNormal::setPosition(const Vector3DFloat& positionToSet)
	{
		position = positionToSet;
	}

	////////////////////////////////////////////////////////////////////////////////
	// PositionMaterial
	////////////////////////////////////////////////////////////////////////////////

	PositionMaterial::PositionMaterial()
	{
	}

	PositionMaterial::PositionMaterial(Vector3DFloat positionToSet, float materialToSet)
		:position(positionToSet)
		,material(materialToSet)
	{
		
	}

	float PositionMaterial::getMaterial(void) const
	{
		return material;
	}

	const Vector3DFloat& PositionMaterial::getPosition(void) const
	{
		return position;
	}

	void PositionMaterial::setMaterial(float materialToSet)
	{
		material = materialToSet;
	}

	void PositionMaterial::setPosition(const Vector3DFloat& positionToSet)
	{
		position = positionToSet;
	}
}

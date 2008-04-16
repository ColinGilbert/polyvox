#include <sstream>

#include "Constants.h"
#include "SurfaceVertex.h"
#include "SurfaceTriangle.h"
#include "SurfaceEdge.h"

namespace PolyVox
{
	SurfaceVertex::SurfaceVertex()
	{
	}

	SurfaceVertex::SurfaceVertex(Vector3DFloat positionToSet, float materialToSet, float alphaToSet)
		:material(materialToSet)
		,alpha(alphaToSet)
		,position(positionToSet)
		,m_uHash((position.x()*(POLYVOX_REGION_SIDE_LENGTH*2+1)*(POLYVOX_REGION_SIDE_LENGTH*2+1)) + (position.y()*(POLYVOX_REGION_SIDE_LENGTH*2+1)) + (position.z()))
	{
		
	}

	SurfaceVertex::SurfaceVertex(Vector3DFloat positionToSet, Vector3DFloat normalToSet)
		:position(positionToSet)
		,normal(normalToSet)
	{
		m_uHash = (position.x()*(POLYVOX_REGION_SIDE_LENGTH*2+1)*(POLYVOX_REGION_SIDE_LENGTH*2+1)) + (position.y()*(POLYVOX_REGION_SIDE_LENGTH*2+1)) + (position.z());
	}

	float SurfaceVertex::getAlpha(void) const
	{
		return alpha;
	}	

	float SurfaceVertex::getMaterial(void) const
	{
		return material;
	}

	const SurfaceEdgeIterator& SurfaceVertex::getEdge(void) const
	{
		return edge;
	}

	const Vector3DFloat& SurfaceVertex::getNormal(void) const
	{
		return normal;
	}

	const Vector3DFloat& SurfaceVertex::getPosition(void) const
	{
		return position;
	}

	void SurfaceVertex::setAlpha(float alphaToSet)
	{
		alpha = alphaToSet;
	}

	void SurfaceVertex::setMaterial(float materialToSet)
	{
		material = materialToSet;
	}

	void SurfaceVertex::setEdge(const SurfaceEdgeIterator& edgeToSet)
	{
		edge = edgeToSet;
	}

	void SurfaceVertex::setNormal(const Vector3DFloat& normalToSet)
	{
		normal = normalToSet;
		normal.normalise();
	}		

	std::string SurfaceVertex::tostring(void) const
	{
		std::stringstream ss;
		ss << "SurfaceVertex: Position = (" << position.x() << "," << position.y() << "," << position.z() << "), Normal = " << normal;
		return ss.str();
	}

	bool operator==(const SurfaceVertex& lhs, const SurfaceVertex& rhs)
	{
		return (lhs.m_uHash == rhs.m_uHash);
	}

	bool operator < (const SurfaceVertex& lhs, const SurfaceVertex& rhs)
	{
		return (lhs.m_uHash < rhs.m_uHash);	
	}	
}

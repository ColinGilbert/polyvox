#include "Constants.h"
#include "SurfaceVertex.h"
#include "SurfaceTriangle.h"
#include "SurfaceEdge.h"

#include "OgreStringConverter.h"

namespace Ogre
{
	SurfaceVertex::SurfaceVertex()
	{
	}

	SurfaceVertex::SurfaceVertex(UIntVector3 positionToSet, float alphaToSet)
		:alpha(alphaToSet)
		,position(positionToSet)
		,m_uHash((position.x*(OGRE_REGION_SIDE_LENGTH*2+1)*(OGRE_REGION_SIDE_LENGTH*2+1)) + (position.y*(OGRE_REGION_SIDE_LENGTH*2+1)) + (position.z))
	{
		
	}

	SurfaceVertex::SurfaceVertex(UIntVector3 positionToSet, Vector3 normalToSet)
		:position(positionToSet)
		,normal(normalToSet)
	{
		m_uHash = (position.x*(OGRE_REGION_SIDE_LENGTH*2+1)*(OGRE_REGION_SIDE_LENGTH*2+1)) + (position.y*(OGRE_REGION_SIDE_LENGTH*2+1)) + (position.z);
	}

	float SurfaceVertex::getAlpha(void) const
	{
		return alpha;
	}	

	const SurfaceEdgeIterator& SurfaceVertex::getEdge(void) const
	{
		return edge;
	}

	const Vector3& SurfaceVertex::getNormal(void) const
	{
		return normal;
	}

	const UIntVector3& SurfaceVertex::getPosition(void) const
	{
		return position;
	}

	void SurfaceVertex::setAlpha(float alphaToSet)
	{
		alpha = alphaToSet;
	}

	void SurfaceVertex::setEdge(const SurfaceEdgeIterator& edgeToSet)
	{
		edge = edgeToSet;
	}

	void SurfaceVertex::setNormal(const Vector3& normalToSet)
	{
		normal = normalToSet;
		normal.normalise();
	}		

	std::string SurfaceVertex::toString(void) const
	{
		std::stringstream ss;
		ss << "SurfaceVertex: Position = (" << position.x << "," << position.y << "," << position.z << "), Normal = " << StringConverter::toString(normal);
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

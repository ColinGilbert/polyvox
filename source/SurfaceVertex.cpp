#include "SurfaceVertex.h"

namespace Ogre
{
	SurfaceVertex::SurfaceVertex()
	{
	}

	SurfaceVertex::SurfaceVertex(Vector3 positionToSet)
		:position(positionToSet)
	{
	}

	SurfaceVertex::SurfaceVertex(Vector3 positionToSet, Vector3 normalToSet)
		:position(positionToSet)
		,normal(normalToSet)
	{
	}

	bool SurfaceVertex::operator==(const SurfaceVertex& rhs) const
	{
		//We dont't check the normal here as it may not have been set. But if two vertices have the same position they should have the same normal too.
		return ((position.positionEquals(rhs.position)) && (abs(alpha - rhs.alpha) < 0.01));
	}

	/*bool SurfaceVertex::operator < (const SurfaceVertex& rhs) const
	{
	if(position.z < rhs.position.z)
	{
	return true;
	}
	else
	{
	if(position.y < rhs.position.y)
	{
	return true;
	}
	else
	{
	if(position.x < rhs.position.x)
	{
	return true;
	}
	else
	{
	return false;
	}
	}
	}
	}*/
}

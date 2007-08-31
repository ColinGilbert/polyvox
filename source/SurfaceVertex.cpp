#include "SurfaceVertex.h"

namespace Ogre
{
	SurfaceVertex::SurfaceVertex()
	{
	}

	SurfaceVertex::SurfaceVertex(UIntVector3 positionToSet)
		:position(positionToSet)
	{
	}

	SurfaceVertex::SurfaceVertex(UIntVector3 positionToSet, Vector3 normalToSet)
		:position(positionToSet)
		,normal(normalToSet)
	{
	}

	bool SurfaceVertex::operator==(const SurfaceVertex& rhs) const
	{
		//We dont't check the normal here as it may not have been set. But if two vertices have the same position they should have the same normal too.
		/*return
		(
			(abs(position.x - rhs.position.x) <= 0.01) && 
			(abs(position.y - rhs.position.y) <= 0.01) &&
			(abs(position.z - rhs.position.z) <= 0.01) &&
			(abs(alpha - rhs.alpha) <= 0.01)
		);*/

		return
		(
			(position.x == rhs.position.x) && 
			(position.x == rhs.position.y) && 
			(position.x == rhs.position.z) && 
			(abs(alpha - rhs.alpha) <= 0.01)
		);
	}

	bool SurfaceVertex::operator < (const SurfaceVertex& rhs) const
	{
		if((*this) == rhs)
		{
			return false;
		}
		if(alpha < rhs.alpha)
		{
			return true;
		}
		if(position.z < rhs.position.z)
		{
			return true;
		}
		if(position.y < rhs.position.y)
		{
			return true;
		}
		if(position.x < rhs.position.x)
		{
			return true;
		}
		
		return false;
	}
}

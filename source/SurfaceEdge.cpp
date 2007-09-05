#include "SurfaceEdge.h"
#include "SurfaceTriangle.h"
#include "SurfaceVertex.h"

namespace Ogre
{
	SurfaceEdge::SurfaceEdge()
	{
	}

	bool operator == (const SurfaceEdge& lhs, const SurfaceEdge& rhs)
		{
			return
			(
				(lhs.target == rhs.target) &&
				(lhs.triangle == rhs.triangle)
			);
		}

		bool operator < (const SurfaceEdge& lhs, const SurfaceEdge& rhs)
		{
			if(lhs.target == rhs.target)
			{
				if(lhs.triangle == rhs.triangle)
				{
					return false;
				}
				return (*(lhs.triangle) < *(rhs.triangle));
			}
			return (*(lhs.target) < *(rhs.target));
		}
}

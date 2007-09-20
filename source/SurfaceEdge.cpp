#include "SurfaceEdge.h"
#include "SurfaceTriangle.h"
#include "SurfaceVertex.h"

namespace Ogre
{
	std::string SurfaceEdge::toString(void)
	{
		std::stringstream ss;
		ss << "SurfaceEdge: Target Vertex = " << target->toString() << "Source Vertex = " << otherHalfEdge->target->toString();
		return ss.str();
	}

	bool operator == (const SurfaceEdge& lhs, const SurfaceEdge& rhs)
	{
		return
		(
			(lhs.target == rhs.target) &&
			//(lhs.triangle == rhs.triangle)
			(lhs.otherHalfEdge == rhs.otherHalfEdge)
		);
	}

	/*bool operator < (const SurfaceEdge& lhs, const SurfaceEdge& rhs)
	{
		if(lhs.target == rhs.target)
		{
			if(lhs.otherHalfEdge->target == rhs.otherHalfEdge->target)
			{
				return false;
			}
			return (*(lhs.otherHalfEdge->target) < *(rhs.otherHalfEdge->target));
		}
		return (*(lhs.target) < *(rhs.target));
	}*/
}

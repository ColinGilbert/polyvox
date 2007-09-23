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
			((*lhs.target) == (*rhs.target)) &&
			((*lhs.otherHalfEdge->target) == (*rhs.otherHalfEdge->target))
		);
	}

	bool SurfaceEdge::isDegenerate(void)
	{
		return (target == otherHalfEdge->target);
	}

	bool operator < (const SurfaceEdge& lhs, const SurfaceEdge& rhs)
	{
		if ((*lhs.target) < (*rhs.target))
			return true;
		if ((*rhs.target) < (*lhs.target))
			return false;

		if ((*lhs.otherHalfEdge->target) < (*rhs.otherHalfEdge->target))
			return true;
		if ((*rhs.otherHalfEdge->target) < (*lhs.otherHalfEdge->target))
			return false;

		return false;
	}
}

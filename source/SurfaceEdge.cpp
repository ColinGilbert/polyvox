#include "SurfaceEdge.h"
#include "SurfaceTriangle.h"
#include "SurfaceVertex.h"

namespace Ogre
{
	SurfaceEdge::SurfaceEdge(const SurfaceVertexIterator& targetToSet,const SurfaceVertexIterator& sourceToSet)
	{
		target = targetToSet;
		source = sourceToSet;
	}

	std::string SurfaceEdge::toString(void)
	{
		std::stringstream ss;
		ss << "SurfaceEdge: Target Vertex = " << target->toString() << "Source Vertex = " << source->toString();
		return ss.str();
	}

	bool operator == (const SurfaceEdge& lhs, const SurfaceEdge& rhs)
	{
		return
		(
			((*lhs.target) == (*rhs.target)) &&
			((*lhs.source) == (*rhs.source))
		);
	}

	bool SurfaceEdge::isDegenerate(void)
	{
		return (target == source);
	}

	bool operator < (const SurfaceEdge& lhs, const SurfaceEdge& rhs)
	{
		if ((*lhs.target) < (*rhs.target))
			return true;
		if ((*rhs.target) < (*lhs.target))
			return false;

		if ((*lhs.source) < (*rhs.source))
			return true;
		if ((*rhs.source) < (*lhs.source))
			return false;

		return false;
	}

	const SurfaceVertexIterator& SurfaceEdge::getTarget(void)
	{
		return target;
	}

	const SurfaceVertexIterator& SurfaceEdge::getSource(void)
	{
		return source;
	}

	void SurfaceEdge::pairWithOtherHalfEdge(const SurfaceEdgeIterator& otherHalfEdgeToPair)
	{
		otherHalfEdge = otherHalfEdgeToPair;
		previousHalfEdge = otherHalfEdgeToPair;
		nextHalfEdge = otherHalfEdgeToPair;
	}

	const SurfaceEdgeIterator& SurfaceEdge::getOtherHalfEdge(void)
	{
		return otherHalfEdge;
	}

	const SurfaceEdgeIterator& SurfaceEdge::getPreviousHalfEdge(void)
	{
		return previousHalfEdge;
	}

	const SurfaceEdgeIterator& SurfaceEdge::getNextHalfEdge(void)
	{
		return nextHalfEdge;
	}

	void SurfaceEdge::setPreviousHalfEdge(const SurfaceEdgeIterator& previousHalfEdgeToSet)
	{
		previousHalfEdge = previousHalfEdgeToSet;
	}

	void SurfaceEdge::setNextHalfEdge(const SurfaceEdgeIterator& nextHalfEdgeToSet)
	{
		nextHalfEdge = nextHalfEdgeToSet;
	}
}

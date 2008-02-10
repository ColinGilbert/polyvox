#include "SurfaceEdge.h"
#include "SurfaceTriangle.h"
#include "SurfaceVertex.h"

#include <sstream>

namespace Ogre
{
	SurfaceEdge::SurfaceEdge(const SurfaceVertexIterator& targetToSet,const SurfaceVertexIterator& sourceToSet)
	{
		target = targetToSet;
		source = sourceToSet;
	}

	std::string SurfaceEdge::tostring(void)
	{
		std::stringstream ss;
		ss << "SurfaceEdge: Target Vertex = " << target->tostring() << "Source Vertex = " << source->tostring();
		return ss.str();
	}

	bool operator == (const SurfaceEdge& lhs, const SurfaceEdge& rhs)
	{
		//Vertices are unique in the set, so if the two positions are the same the 
		//two iterators must also be the same. So we just check the iterators.
		return
		(
			(lhs.target == rhs.target) &&
			(lhs.source == rhs.source)
		);
	}

	bool SurfaceEdge::isDegenerate(void)
	{
		return (target == source);
	}

	bool operator < (const SurfaceEdge& lhs, const SurfaceEdge& rhs)
	{
		//Unlike the equality operator, we can't compare iterators.
		//So dereference and compare the results.
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

	const SurfaceVertexIterator& SurfaceEdge::getTarget(void) const
	{
		return target;
	}

	const SurfaceVertexIterator& SurfaceEdge::getSource(void) const
	{
		return source;
	}

	void SurfaceEdge::pairWithOtherHalfEdge(const SurfaceEdgeIterator& otherHalfEdgeToPair)
	{
		otherHalfEdge = otherHalfEdgeToPair;
		previousHalfEdge = otherHalfEdgeToPair;
		nextHalfEdge = otherHalfEdgeToPair;
	}

	const SurfaceEdgeIterator& SurfaceEdge::getOtherHalfEdge(void) const
	{
		return otherHalfEdge;
	}

	const SurfaceEdgeIterator& SurfaceEdge::getPreviousHalfEdge(void) const
	{
		return previousHalfEdge;
	}

	const SurfaceEdgeIterator& SurfaceEdge::getNextHalfEdge(void) const
	{
		return nextHalfEdge;
	}

	const SurfaceTriangleIterator& SurfaceEdge::getTriangle(void) const
	{
		return triangle;
	}

	void SurfaceEdge::setPreviousHalfEdge(const SurfaceEdgeIterator& previousHalfEdgeToSet)
	{
		previousHalfEdge = previousHalfEdgeToSet;
	}

	void SurfaceEdge::setNextHalfEdge(const SurfaceEdgeIterator& nextHalfEdgeToSet)
	{
		nextHalfEdge = nextHalfEdgeToSet;
	}

	void SurfaceEdge::setTriangle(const SurfaceTriangleIterator& triangleToSet)
	{
		triangle = triangleToSet;
	}
}

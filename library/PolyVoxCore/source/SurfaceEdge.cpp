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

#include "SurfaceEdge.h"
#include "SurfaceTriangle.h"
#include "SurfaceVertex.h"

namespace PolyVox
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

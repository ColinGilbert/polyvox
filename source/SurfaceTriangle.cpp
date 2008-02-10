#include "SurfaceTriangle.h"
#include "SurfaceVertex.h"
#include "SurfaceEdge.h"

namespace PolyVox
{
	SurfaceTriangle::SurfaceTriangle()
	{
	}

	const SurfaceEdgeIterator& SurfaceTriangle::getEdge(void) const
	{
		return edge;
	}

	void SurfaceTriangle::setEdge(const SurfaceEdgeIterator& edgeToSet)
	{
		edge = edgeToSet;
	}

	/*std::string SurfaceTriangle::tostring(void)
	{
		std::stringstream ss;

		uint16_t ct = 0;
		SurfaceEdgeIterator edgeIter = edge;
		ss << "SurfaceTriangle:";
		do
		{
			ss << "\n        Edge " << ct << " = " << edgeIter->tostring();
			if(edgeIter->hasOtherHalfEdge)
			{
				ss << "\n    Opp Edge " << ct << " = " << edgeIter->otherHalfEdge->tostring();
			}
			else
			{
				ss << "\n    No Other Half";
			}
			edgeIter = edgeIter->nextHalfEdge;
			++ct;
		}
		while(edgeIter != edge);		
		return ss.str();
	}*/

	bool operator == (const SurfaceTriangle& lhs, const SurfaceTriangle& rhs)
	{
		//Edges are unique in the set, so if the two positions are the same the 
		//two iterators must also be the same. So we just check the iterators.
		return (lhs.edge == rhs.edge);
	}

	bool operator < (const SurfaceTriangle& lhs, const SurfaceTriangle& rhs)
	{
		//Unlike the equality operator, we can't compare iterators.
		//So dereference and compare the results.
		return (*lhs.edge < *rhs.edge);
	}
}

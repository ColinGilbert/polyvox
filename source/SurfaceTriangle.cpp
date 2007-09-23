#include "SurfaceTriangle.h"
#include "SurfaceVertex.h"
#include "SurfaceEdge.h"

namespace Ogre
{
	SurfaceTriangle::SurfaceTriangle()
	{
	}

	/*std::string SurfaceTriangle::toString(void)
	{
		std::stringstream ss;

		uint ct = 0;
		SurfaceEdgeIterator edgeIter = edge;
		ss << "SurfaceTriangle:";
		do
		{
			ss << "\n        Edge " << ct << " = " << edgeIter->toString();
			if(edgeIter->hasOtherHalfEdge)
			{
				ss << "\n    Opp Edge " << ct << " = " << edgeIter->otherHalfEdge->toString();
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
		return lhs.edge == rhs.edge;
	}

	bool operator < (const SurfaceTriangle& lhs, const SurfaceTriangle& rhs)
	{
		return (*lhs.edge < *rhs.edge);
	}

	/*bool operator < (const SurfaceTriangleIterator& lhs, const SurfaceTriangleIterator& rhs)
	{
		return (*lhs) < (*rhs);
	}*/
}

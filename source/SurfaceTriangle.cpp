#include "SurfaceTriangle.h"
#include "SurfaceVertex.h"

namespace Ogre
{
	SurfaceTriangle::SurfaceTriangle()
	{
	}

	SurfaceTriangle::SurfaceTriangle(SurfaceVertexIterator v0ToSet, SurfaceVertexIterator v1ToSet, SurfaceVertexIterator v2ToSet)
		:v0(v0ToSet)
		,v1(v1ToSet)
		,v2(v2ToSet)
	{
	}

	bool operator == (const SurfaceTriangle& lhs, const SurfaceTriangle& rhs)
		{
			return
			(
				(lhs.v0 == rhs.v0) &&
				(lhs.v1 == rhs.v1) &&
				(lhs.v2 == rhs.v2)
			);
		}

		bool operator < (const SurfaceTriangle& lhs, const SurfaceTriangle& rhs)
		{
			if(lhs.v0 == rhs.v0)
			{
				if(lhs.v1 == rhs.v1)
				{
					if(lhs.v2 == rhs.v2)
					{
						return false;
					}
					return (*(lhs.v2) < *(rhs.v2));
				}
				return (*(lhs.v1) < *(rhs.v1));
			}
			return (*(lhs.v0) < *(rhs.v0));
		}

		/*bool operator < (const SurfaceTriangleIterator& lhs, const SurfaceTriangleIterator& rhs)
		{
			return (*lhs) < (*rhs);
		}*/
}

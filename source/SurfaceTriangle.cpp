#include "SurfaceTriangle.h"

namespace Ogre
{
	SurfaceTriangle::SurfaceTriangle()
	{
	}

	SurfaceTriangle::SurfaceTriangle(std::list<SurfaceVertex>::iterator v0ToSet, std::list<SurfaceVertex>::iterator v1ToSet, std::list<SurfaceVertex>::iterator v2ToSet)
		:v0(v0ToSet)
		,v1(v1ToSet)
		,v2(v2ToSet)
	{
	}
}
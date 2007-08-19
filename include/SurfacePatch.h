#ifndef __SurfacePatch_H__
#define __SurfacePatch_H__

#include <vector>

#include "SurfaceVertex.h"
#include "SurfaceTriangle.h"

namespace Ogre
{
	class SurfacePatch
	{
	public:
	   SurfacePatch();
	   ~SurfacePatch();

	   void addTriangle(const SurfaceVertex& v0,const SurfaceVertex& v1,const SurfaceVertex& v2);

	   const std::vector<SurfaceVertex>& getVertexArray();
	   const std::vector<SurfaceTriangle>& getTriangleArray();

	private:
		std::vector<SurfaceVertex> m_vecVertices;
		std::vector<SurfaceTriangle> m_vecTriangles;
	};	
}

#endif /* __SurfacePatch_H__ */
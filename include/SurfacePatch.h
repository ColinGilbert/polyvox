#ifndef __SurfacePatch_H__
#define __SurfacePatch_H__

#include <list>
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

	   /*const std::vector<SurfaceVertex> getVertexArray();
	   const std::vector<SurfaceTriangle> getTriangleArray();*/

	   void getVertexAndIndexData(std::vector<SurfaceVertex>& vertexData, std::vector<uint>& indexData);

	private:
		std::list<SurfaceVertex> m_listVertices;
		std::list<SurfaceTriangle> m_listTriangles;

		std::vector<SurfaceVertex> m_vecVertexData;
		std::vector<uint> m_vecIndexData;
	};	
}

#endif /* __SurfacePatch_H__ */
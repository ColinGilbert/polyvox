#ifndef __SurfacePatch_H__
#define __SurfacePatch_H__

#include <set>
#include <list>

#include "IntegralVector3.h"
#include "SurfaceTypes.h"
#include "VolumeIterator.h"


namespace Ogre
{
	class SurfacePatch
	{
	public:
	   SurfacePatch();
	   ~SurfacePatch();

	   //This allow users of the class to iterate over its contents.
	   SurfaceEdgeIterator getEdgesBegin(void);
	   SurfaceEdgeIterator getEdgesEnd(void);
	   SurfaceTriangleIterator getTrianglesBegin(void);
	   SurfaceTriangleIterator getTrianglesEnd(void);
	   SurfaceVertexIterator getVerticesBegin(void);
	   SurfaceVertexIterator getVerticesEnd(void);

	   //Users of the class might want these for debugging or info purposes.
	   uint getNoOfEdges(void) const;
	   uint getNoOfTriangles(void) const;
	   uint getNoOfVertices(void) const;

	   bool canRemoveVertexFrom(SurfaceVertexIterator vertexIter, std::list<SurfaceVertexIterator> listConnectedIter, bool isEdge);
	   std::list<SurfaceVertexIterator> findConnectedVertices(SurfaceVertexIterator vertexIter, bool& isEdge);
	   uint decimate(void);
	   void triangulate(std::list<SurfaceVertexIterator> listVertices);
	   bool isPolygonConvex(std::list<SurfaceVertexIterator> listVertices, Vector3 normal);
	   void addTriangle(const SurfaceVertex& v0,const SurfaceVertex& v1,const SurfaceVertex& v2);

	   


	private:
		std::set<SurfaceVertex> m_listVertices;
		std::set<SurfaceTriangle> m_listTriangles;
		std::set<SurfaceEdge> m_listEdges;

		SurfaceEdgeIterator findEdge(const SurfaceVertexIterator& source, const SurfaceVertexIterator& target);
		SurfaceEdgeIterator findOrAddEdge(const SurfaceVertexIterator& source, const SurfaceVertexIterator& target);
		SurfaceVertexIterator findOrAddVertex(const SurfaceVertex& vertex);
	};	
}

#endif /* __SurfacePatch_H__ */

#ifndef __SurfacePatch_H__
#define __SurfacePatch_H__

#include <set>
#include <list>

#include "IntegralVector3.h"

#include "VolumeIterator.h"


namespace Ogre
{
	enum NormalGenerationMethod
	{
		SIMPLE,
		CENTRAL_DIFFERENCE,
		SOBEL
	};

	class SurfaceVertex;
	typedef std::set<SurfaceVertex>::iterator SurfaceVertexIterator;
	class SurfaceTriangle;
	typedef std::set<SurfaceTriangle>::iterator SurfaceTriangleIterator;
	class SurfaceEdge;
	typedef std::set<SurfaceEdge>::iterator SurfaceEdgeIterator;

	class SurfacePatch
	{
	public:
	   SurfacePatch();
	   ~SurfacePatch();

	   void beginDefinition(void);
	   void endDefinition(void);

	   void addTriangle(const SurfaceVertex& v0,const SurfaceVertex& v1,const SurfaceVertex& v2);	   
	   SurfaceVertexIterator findOrAddVertex(const SurfaceVertex& vertex);
	   SurfaceEdgeIterator findEdge(const SurfaceVertexIterator& source, const SurfaceVertexIterator& target);
	   SurfaceEdgeIterator findOrAddEdge(const SurfaceVertexIterator& source, const SurfaceVertexIterator& target);
	   //SurfaceVertexIterator findTriangle(const SurfaceTriangle& triangle);

	   void getVertexAndIndexData(std::vector<SurfaceVertex>& vertexData, std::vector<uint>& indexData);


	   bool canRemoveVertexFrom(SurfaceVertexIterator vertexIter, std::list<SurfaceVertexIterator> listConnectedIter, bool isEdge);
	   std::list<SurfaceVertexIterator> findConnectedVertices(SurfaceVertexIterator vertexIter, bool& isEdge);
	   uint decimate(void);
	   void triangulate(std::list<SurfaceVertexIterator> listVertices);
	   bool isPolygonConvex(std::list<SurfaceVertexIterator> listVertices, Vector3 normal);

	   SurfaceVertexIterator getVerticesBegin(void);
	   SurfaceVertexIterator getVerticesEnd(void);

	   uint getNoOfTriangles(void);


	private:
		std::set<SurfaceVertex> m_listVertices;
		std::set<SurfaceTriangle> m_listTriangles;
		std::set<SurfaceEdge> m_listEdges;

		//std::vector<SurfaceVertex> m_vecVertexData;
		//std::vector<uint> m_vecIndexData;

		uint m_uTrianglesAdded;
		uint m_uVerticesAdded;

		long int* vertexIndices;

		//UIntVector3 m_v3dOffset;
	};	
}

#endif /* __SurfacePatch_H__ */

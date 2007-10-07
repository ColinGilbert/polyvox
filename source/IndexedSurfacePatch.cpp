#include "IndexedSurfacePatch.h"

namespace Ogre
{
	IndexedSurfacePatch::IndexedSurfacePatch()
	{
	}

	IndexedSurfacePatch::~IndexedSurfacePatch()	  
	{
	}

	void IndexedSurfacePatch::addTriangle(const SurfaceVertex& v0,const SurfaceVertex& v1,const SurfaceVertex& v2)
	{
		SurfaceVertexIterator v0Iter = m_listVertices.insert(v0).first;
		SurfaceVertexIterator v1Iter = m_listVertices.insert(v1).first;
		SurfaceVertexIterator v2Iter = m_listVertices.insert(v2).first;

		m_vecTriangleIndices.push_back(v0Iter);
		m_vecTriangleIndices.push_back(v1Iter);
		m_vecTriangleIndices.push_back(v2Iter);
	}

	void IndexedSurfacePatch::fillVertexAndIndexData(std::vector<SurfaceVertex>& vecVertices, std::vector<ushort>& vecIndices)
	{
		vecVertices.resize(m_listVertices.size());
		std::copy(m_listVertices.begin(), m_listVertices.end(), vecVertices.begin());

		//vecIndices.resize(m_listTriangles.size() * 3);
		for(std::vector<SurfaceVertexIterator>::iterator iterVertices = m_vecTriangleIndices.begin(); iterVertices != m_vecTriangleIndices.end(); ++iterVertices)
		{		
			std::vector<SurfaceVertex>::iterator iterVertex;
			
			iterVertex = lower_bound(vecVertices.begin(), vecVertices.end(), **iterVertices);
			vecIndices.push_back(iterVertex - vecVertices.begin());
		}
	}
}
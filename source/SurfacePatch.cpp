#include "SurfacePatch.h"

#include <algorithm>

namespace Ogre
{
	SurfacePatch::SurfacePatch()
	{
	}

	SurfacePatch::~SurfacePatch()
	{
	}

	void SurfacePatch::addTriangle(const SurfaceVertex& v0,const SurfaceVertex& v1,const SurfaceVertex& v2)
	{
		SurfaceTriangle triangle;

		m_listVertices.push_back(v0);
		triangle.v0 = m_listVertices.end();
		triangle.v0--;

		m_listVertices.push_back(v1);
		triangle.v1 = m_listVertices.end();
		triangle.v1--;

		m_listVertices.push_back(v2);
		triangle.v2 = m_listVertices.end();
		triangle.v2--;

		m_listTriangles.push_back(triangle);

		/*m_vecVertexData.push_back(v0);
		m_vecIndexData.push_back(m_vecVertexData.size()-1);
		m_vecVertexData.push_back(v1);
		m_vecIndexData.push_back(m_vecVertexData.size()-1);
		m_vecVertexData.push_back(v2);
		m_vecIndexData.push_back(m_vecVertexData.size()-1);*/
	}

	/*const std::vector<SurfaceVertex> SurfacePatch::getVertexArray()
	{
		std::vector<SurfaceVertex> vertexArray;
		vertexArray.resize(m_listVertices.size());
		std::copy(m_listVertices.begin(), m_listVertices.end(), vertexArray.begin());
		return vertexArray;
	}

	const std::vector<SurfaceTriangle> SurfacePatch::getTriangleArray()
	{
		std::vector<SurfaceTriangle> triangleArray;
		triangleArray.resize(m_listTriangles.size());
		std::copy(m_listTriangles.begin(), m_listTriangles.end(), triangleArray.begin());
		return triangleArray;
	}*/

	void SurfacePatch::getVertexAndIndexData(std::vector<SurfaceVertex>& vertexData, std::vector<uint>& indexData)
	{
		vertexData.clear();
		indexData.clear();

		for(std::list<SurfaceTriangle>::iterator iterTriangles = m_listTriangles.begin(); iterTriangles != m_listTriangles.end(); ++iterTriangles)
		{
			vertexData.push_back(*(iterTriangles->v0));
			indexData.push_back(vertexData.size()-1);
			vertexData.push_back(*(iterTriangles->v1));
			indexData.push_back(vertexData.size()-1);
			vertexData.push_back(*(iterTriangles->v2));
			indexData.push_back(vertexData.size()-1);
		}

		/*vertexData = m_vecVertexData;
		indexData = m_vecIndexData;*/
	}
}
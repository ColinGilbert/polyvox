#include "SurfacePatch.h"

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

		m_vecVertices.push_back(v0);
		triangle.v0 = m_vecVertices.size()-1;
		m_vecVertices.push_back(v1);
		triangle.v1 = m_vecVertices.size()-1;
		m_vecVertices.push_back(v2);
		triangle.v2 = m_vecVertices.size()-1;

		m_vecTriangles.push_back(triangle);
	}

	const std::vector<SurfaceVertex>& SurfacePatch::getVertexArray()
	{
		return m_vecVertices;
	}

	const std::vector<SurfaceTriangle>& SurfacePatch::getTriangleArray()
	{
		return m_vecTriangles;
	}
}
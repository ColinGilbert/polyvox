#include "SurfacePatch.h"

#include "OgreLogManager.h"
#include "OgreStringConverter.h"

#include <algorithm>

namespace Ogre
{
	SurfacePatch::SurfacePatch()
	{
		m_listVertices.clear();
		m_listTriangles.clear();

		m_uTrianglesAdded = 0;
	}

	SurfacePatch::~SurfacePatch()
	{
	}

	void SurfacePatch::endDefinition(void)
	{
		//LogManager::getSingleton().logMessage("No of triangles added = " + StringConverter::toString(m_uTrianglesAdded)); 
		//LogManager::getSingleton().logMessage("No of triangles present = " + StringConverter::toString(m_listTriangles.size())); 
		//LogManager::getSingleton().logMessage("No of vertices present = " + StringConverter::toString(m_listVertices.size())); 
	}

	void SurfacePatch::addTriangle(const SurfaceVertex& v0,const SurfaceVertex& v1,const SurfaceVertex& v2)
	{
		m_uTrianglesAdded++;

		SurfaceTriangle triangle;
		
		triangle.v0 = std::find(m_listVertices.begin(), m_listVertices.end(), v0);
		if(triangle.v0 == m_listVertices.end()) //We have to add it
		{
			m_listVertices.push_back(v0);
			triangle.v0 = m_listVertices.end();
			triangle.v0--;
		}

		
		triangle.v1 = std::find(m_listVertices.begin(), m_listVertices.end(), v1);
		if(triangle.v1 == m_listVertices.end()) //We have to add it
		{
			m_listVertices.push_back(v1);
			triangle.v1 = m_listVertices.end();
			triangle.v1--;
		}

		
		triangle.v2 = std::find(m_listVertices.begin(), m_listVertices.end(), v2);
		if(triangle.v2 == m_listVertices.end()) //We have to add it
		{
			m_listVertices.push_back(v2);
			triangle.v2 = m_listVertices.end();
			triangle.v2--;
		}	

		m_listTriangles.push_back(triangle);

		triangle.v0->listTrianglesUsingThisVertex.push_back(m_listTriangles.end());
		triangle.v1->listTrianglesUsingThisVertex.push_back(m_listTriangles.end());
		triangle.v2->listTrianglesUsingThisVertex.push_back(m_listTriangles.end());
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

		vertexData.resize(m_listVertices.size());
		std::copy(m_listVertices.begin(), m_listVertices.end(), vertexData.begin());

		for(std::list<SurfaceTriangle>::iterator iterTriangles = m_listTriangles.begin(); iterTriangles != m_listTriangles.end(); ++iterTriangles)
		{
			/*vertexData.push_back(*(iterTriangles->v0));
			indexData.push_back(vertexData.size()-1);
			vertexData.push_back(*(iterTriangles->v1));
			indexData.push_back(vertexData.size()-1);
			vertexData.push_back(*(iterTriangles->v2));
			indexData.push_back(vertexData.size()-1);*/

			for(uint ct = 0; ct < vertexData.size(); ++ct)
			{
				if(vertexData[ct] == (*(iterTriangles->v0)))
				{
					indexData.push_back(ct);
				}
			}

			for(uint ct = 0; ct < vertexData.size(); ++ct)
			{
				if(vertexData[ct] == (*(iterTriangles->v1)))
				{
					indexData.push_back(ct);
				}
			}

			for(uint ct = 0; ct < vertexData.size(); ++ct)
			{
				if(vertexData[ct] == (*(iterTriangles->v2)))
				{
					indexData.push_back(ct);
				}
			}
		}

		/*vertexData = m_vecVertexData;
		indexData = m_vecIndexData;*/
	}
}
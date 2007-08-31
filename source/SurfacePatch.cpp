#include "SurfacePatch.h"

#include "OgreLogManager.h"
#include "OgreStringConverter.h"

#include <algorithm>

namespace Ogre
{
	SurfacePatch::SurfacePatch()
	{
		m_setVertices.clear();
		m_listTriangles.clear();

		m_uTrianglesAdded = 0;
		m_uVerticesAdded = 0;
	}

	SurfacePatch::~SurfacePatch()
	{
	}

	void SurfacePatch::beginDefinition(void)
	{
	}

	void SurfacePatch::endDefinition(void)
	{
		//LogManager::getSingleton().logMessage("No of triangles added = " + StringConverter::toString(m_uTrianglesAdded)); 
		//LogManager::getSingleton().logMessage("No of triangles present = " + StringConverter::toString(m_listTriangles.size())); 
		//LogManager::getSingleton().logMessage("No of vertices added = " + StringConverter::toString(m_uVerticesAdded)); 
		//LogManager::getSingleton().logMessage("No of vertices present = " + StringConverter::toString(m_setVertices.size())); 
	}

	void SurfacePatch::addTriangle(const SurfaceVertex& v0,const SurfaceVertex& v1,const SurfaceVertex& v2)
	{
		m_uTrianglesAdded++;

		SurfaceTriangle triangle;

		m_setVertices.insert(v0);
		m_uVerticesAdded++;
		m_setVertices.insert(v1);
		m_uVerticesAdded++;
		m_setVertices.insert(v2);
		m_uVerticesAdded++;

		triangle.v0 = std::find(m_setVertices.begin(), m_setVertices.end(), v0);
		triangle.v1 = std::find(m_setVertices.begin(), m_setVertices.end(), v1);
		triangle.v2 = std::find(m_setVertices.begin(), m_setVertices.end(), v2);
		
		/*triangle.v0 = std::find(m_listVertices.begin(), m_listVertices.end(), v0);
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
		}*/	

		m_listTriangles.push_back(triangle);

		/*triangle.v0->listTrianglesUsingThisVertex.push_back(m_listTriangles.end());
		triangle.v1->listTrianglesUsingThisVertex.push_back(m_listTriangles.end());
		triangle.v2->listTrianglesUsingThisVertex.push_back(m_listTriangles.end());*/
	}

	void SurfacePatch::getVertexAndIndexData(std::vector<SurfaceVertex>& vertexData, std::vector<uint>& indexData)
	{
		vertexData.clear();
		indexData.clear();

		vertexData.resize(m_setVertices.size());
		std::copy(m_setVertices.begin(), m_setVertices.end(), vertexData.begin());

		sort(vertexData.begin(),vertexData.end());
		//reverse(vertexData.begin(),vertexData.end());

		for(std::list<SurfaceTriangle>::iterator iterTriangles = m_listTriangles.begin(); iterTriangles != m_listTriangles.end(); ++iterTriangles)
		{
			/*vertexData.push_back(*(iterTriangles->v0));
			indexData.push_back(vertexData.size()-1);
			vertexData.push_back(*(iterTriangles->v1));
			indexData.push_back(vertexData.size()-1);
			vertexData.push_back(*(iterTriangles->v2));
			indexData.push_back(vertexData.size()-1);*/		

			/*std::set<SurfaceVertex>::iterator iterVertex = m_setVertices.find((*(iterTriangles->v0)));
			indexData.push_back(iterVertex - m_setVertices.begin());
			iterVertex = m_setVertices.find((*(iterTriangles->v1)));
			indexData.push_back(iterVertex - m_setVertices.begin());
			iterVertex = m_setVertices.find((*(iterTriangles->v2)));
			indexData.push_back(iterVertex - m_setVertices.begin());*/

			/*for(uint ct = 0; ct < vertexData.size(); ++ct)
			{
				if(vertexData[ct] == (*(iterTriangles->v0)))
				{
					indexData.push_back(ct);
					break;
				}				
			}*/

			std::vector<SurfaceVertex>::iterator iterVertex = find(vertexData.begin(), vertexData.end(),(*(iterTriangles->v0)));
			if(iterVertex == vertexData.end())
			{
				LogManager::getSingleton().logMessage("Vertex 0 Not Found");
			}
			indexData.push_back(iterVertex - vertexData.begin());

			iterVertex = find(vertexData.begin(), vertexData.end(),(*(iterTriangles->v1)));
			if(iterVertex == vertexData.end())
			{
				LogManager::getSingleton().logMessage("Vertex 1 Not Found");
			}
			indexData.push_back(iterVertex - vertexData.begin());

			iterVertex = find(vertexData.begin(), vertexData.end(),(*(iterTriangles->v2)));
			if(iterVertex == vertexData.end())
			{
				LogManager::getSingleton().logMessage("Vertex 2 Not Found");
			}
			indexData.push_back(iterVertex - vertexData.begin());

			/*for(uint ct = 0; ct < vertexData.size(); ++ct)
			{
				if(vertexData[ct] == (*(iterTriangles->v1)))
				{
					indexData.push_back(ct);
					break;
				}
			}

			for(uint ct = 0; ct < vertexData.size(); ++ct)
			{
				if(vertexData[ct] == (*(iterTriangles->v2)))
				{
					indexData.push_back(ct);
					break;
				}
			}*/
		}

		/*vertexData = m_vecVertexData;
		indexData = m_vecIndexData;*/
	}
}

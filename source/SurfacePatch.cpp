#include "SurfacePatch.h"
#include "Constants.h"

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

		vertexIndices = 0;

		beginDefinition(); //FIXME - we shouldn't really be calling this from the constructor.
	}

	SurfacePatch::~SurfacePatch()
	{		
	}

	void SurfacePatch::beginDefinition(void)
	{
		vertexIndices = new long int [(OGRE_REGION_SIDE_LENGTH*2+1)*(OGRE_REGION_SIDE_LENGTH*2+1)*(OGRE_REGION_SIDE_LENGTH*2+1)];
		memset(vertexIndices,0xFF,sizeof(long int)*(OGRE_REGION_SIDE_LENGTH*2+1)*(OGRE_REGION_SIDE_LENGTH*2+1)*(OGRE_REGION_SIDE_LENGTH*2+1)); //0xFF is -1 as two's complement - this may not be portable...
	}

	void SurfacePatch::endDefinition(void)
	{
		//LogManager::getSingleton().logMessage("No of triangles added = " + StringConverter::toString(m_uTrianglesAdded)); 
		//LogManager::getSingleton().logMessage("No of triangles present = " + StringConverter::toString(m_listTriangles.size())); 
		//LogManager::getSingleton().logMessage("No of vertices added = " + StringConverter::toString(m_uVerticesAdded)); 
		//LogManager::getSingleton().logMessage("No of vertices present = " + StringConverter::toString(m_setVertices.size())); 

		delete vertexIndices;
		vertexIndices = 0;
	}

	void SurfacePatch::addTriangle(const SurfaceVertex& v0,const SurfaceVertex& v1,const SurfaceVertex& v2)
	{
		/*m_vecVertexData.push_back(v0);
		m_vecIndexData.push_back(m_vecVertexData.size()-1);
		m_vecVertexData.push_back(v1);
		m_vecIndexData.push_back(m_vecVertexData.size()-1);
		m_vecVertexData.push_back(v2);
		m_vecIndexData.push_back(m_vecVertexData.size()-1);*/

		m_uTrianglesAdded++;

		m_uVerticesAdded += 3;

		SurfaceTriangle triangle;

		long int index;
		//If a vertex has not yet been added, it's index is -1
		index = vertexIndices[(v0.position.x*(OGRE_REGION_SIDE_LENGTH*2+1)*(OGRE_REGION_SIDE_LENGTH*2+1)) + (v0.position.y*(OGRE_REGION_SIDE_LENGTH*2+1)) + (v0.position.z)];
		if((index == -1))
		{
			//Add the vertex
			m_vecVertexData.push_back(v0);
			vertexIndices[(v0.position.x*(OGRE_REGION_SIDE_LENGTH*2+1)*(OGRE_REGION_SIDE_LENGTH*2+1)) + (v0.position.y*(OGRE_REGION_SIDE_LENGTH*2+1)) + (v0.position.z)] = m_vecVertexData.size()-1;
			m_vecIndexData.push_back(m_vecVertexData.size()-1);
		}
		else
		{
			//Just reuse the existing vertex
			m_vecIndexData.push_back(index);
		}

		//If a vertex has not yet been added, it's index is -1
		index = vertexIndices[(v1.position.x*(OGRE_REGION_SIDE_LENGTH*2+1)*(OGRE_REGION_SIDE_LENGTH*2+1)) + (v1.position.y*(OGRE_REGION_SIDE_LENGTH*2+1)) + (v1.position.z)];
		if((index == -1))
		{
			//Add the vertex
			m_vecVertexData.push_back(v1);
			vertexIndices[(v1.position.x*(OGRE_REGION_SIDE_LENGTH*2+1)*(OGRE_REGION_SIDE_LENGTH*2+1)) + (v1.position.y*(OGRE_REGION_SIDE_LENGTH*2+1)) + (v1.position.z)] = m_vecVertexData.size()-1;
			m_vecIndexData.push_back(m_vecVertexData.size()-1);
		}
		else
		{
			//Just reuse the existing vertex
			m_vecIndexData.push_back(index);
		}

		//If a vertex has not yet been added, it's index is -1
		index = vertexIndices[(v2.position.x*(OGRE_REGION_SIDE_LENGTH*2+1)*(OGRE_REGION_SIDE_LENGTH*2+1)) + (v2.position.y*(OGRE_REGION_SIDE_LENGTH*2+1)) + (v2.position.z)];
		if((index == -1))
		{
			//Add the vertex
			m_vecVertexData.push_back(v2);
			vertexIndices[(v2.position.x*(OGRE_REGION_SIDE_LENGTH*2+1)*(OGRE_REGION_SIDE_LENGTH*2+1)) + (v2.position.y*(OGRE_REGION_SIDE_LENGTH*2+1)) + (v2.position.z)] = m_vecVertexData.size()-1;
			m_vecIndexData.push_back(m_vecVertexData.size()-1);
		}
		else
		{
			//Just reuse the existing vertex
			m_vecIndexData.push_back(index);
		}




		/*m_setVertices.insert(v0);		
		m_setVertices.insert(v1);
		m_setVertices.insert(v2);

		triangle.v0 = std::find(m_setVertices.begin(), m_setVertices.end(), v0);
		triangle.v1 = std::find(m_setVertices.begin(), m_setVertices.end(), v1);
		triangle.v2 = std::find(m_setVertices.begin(), m_setVertices.end(), v2);*/			

		//m_listTriangles.push_back(triangle);

		/*triangle.v0->listTrianglesUsingThisVertex.push_back(m_listTriangles.end());
		triangle.v1->listTrianglesUsingThisVertex.push_back(m_listTriangles.end());
		triangle.v2->listTrianglesUsingThisVertex.push_back(m_listTriangles.end());*/
	}

	void SurfacePatch::getVertexAndIndexData(std::vector<SurfaceVertex>& vertexData, std::vector<uint>& indexData)
	{
		/*vertexData.clear();
		indexData.clear();*/

		vertexData = m_vecVertexData;
		indexData = m_vecIndexData;

		return;
#ifdef BLAH

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

#endif
	}
}

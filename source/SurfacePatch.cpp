#include "SurfacePatch.h"
#include "Constants.h"

#include "SurfaceVertex.h"
#include "SurfaceTriangle.h"
#include "SurfaceEdge.h"

#include "OgreLogManager.h"
#include "OgreStringConverter.h"

#include <algorithm>

namespace Ogre
{
	SurfacePatch::SurfacePatch()
	{
		m_listVertices.clear();
		m_listTriangles.clear();
		m_listEdges.clear();

		m_uTrianglesAdded = 0;
		m_uVerticesAdded = 0;

		vertexIndices = 0;

		//beginDefinition(); //FIXME - we shouldn't really be calling this from the constructor.
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
		//if(m_uTrianglesAdded > 1) return;
		//LogManager::getSingleton().logMessage("Adding Triangle " + StringConverter::toString(m_uTrianglesAdded));
		m_uTrianglesAdded++;
		m_uVerticesAdded += 3;	

		
		SurfaceVertexIterator v0Iter = find(m_listVertices.begin(), m_listVertices.end(), v0);
		if(v0Iter == m_listVertices.end())
		{		
			//LogManager::getSingleton().logMessage("Adding Vertex " + StringConverter::toString(v0.position.x) + "," + StringConverter::toString(v0.position.y) + "," + StringConverter::toString(v0.position.z));
			m_listVertices.push_back(v0);
			v0Iter = m_listVertices.end();
			v0Iter--;
		}
		//else
			//LogManager::getSingleton().logMessage("Already Exists " + StringConverter::toString(v0.position.x) + "," + StringConverter::toString(v0.position.y) + "," + StringConverter::toString(v0.position.z));

		SurfaceVertexIterator v1Iter = find(m_listVertices.begin(), m_listVertices.end(), v1);
		if(v1Iter == m_listVertices.end())
		{
			//LogManager::getSingleton().logMessage("Adding Vertex " + StringConverter::toString(v1.position.x) + "," + StringConverter::toString(v1.position.y) + "," + StringConverter::toString(v1.position.z));
			m_listVertices.push_back(v1);
			v1Iter = m_listVertices.end();
			v1Iter--;
		}
		//else
			//LogManager::getSingleton().logMessage("Already Exists " + StringConverter::toString(v1.position.x) + "," + StringConverter::toString(v1.position.y) + "," + StringConverter::toString(v1.position.z));

		SurfaceVertexIterator v2Iter = find(m_listVertices.begin(), m_listVertices.end(), v2);
		if(v2Iter == m_listVertices.end())
		{
			//LogManager::getSingleton().logMessage("Adding Vertex " + StringConverter::toString(v2.position.x) + "," + StringConverter::toString(v2.position.y) + "," + StringConverter::toString(v2.position.z));
			m_listVertices.push_back(v2);
			v2Iter = m_listVertices.end();
			v2Iter--;
		}
		//else
			//LogManager::getSingleton().logMessage("Already Exists " + StringConverter::toString(v2.position.x) + "," + StringConverter::toString(v2.position.y) + "," + StringConverter::toString(v2.position.z));

		//LogManager::getSingleton().logMessage("Creating Edges");
		SurfaceEdge v0v1;
		v0v1.target = v1Iter;
		SurfaceEdge v1v2;
		v1v2.target = v2Iter;
		SurfaceEdge v2v0;
		v2v0.target = v0Iter;

		m_listEdges.push_back(v0v1);
		SurfaceEdgeIterator v0v1Iter = m_listEdges.end();
		v0v1Iter--;
		m_listEdges.push_back(v1v2);
		SurfaceEdgeIterator v1v2Iter = m_listEdges.end();
		v1v2Iter--;
		m_listEdges.push_back(v2v0);
		SurfaceEdgeIterator v2v0Iter = m_listEdges.end();
		v2v0Iter--;

		v0Iter->edge = v0v1Iter;
		v1Iter->edge = v1v2Iter;
		v2Iter->edge = v2v0Iter;

		v0v1Iter->nextHalfEdge = v1v2Iter;
		v1v2Iter->nextHalfEdge = v2v0Iter;
		v2v0Iter->nextHalfEdge = v0v1Iter;

		SurfaceTriangle triangle;

		triangle.edge = v0v1Iter;	

		m_listTriangles.push_back(triangle);
		SurfaceTriangleIterator iterTriangle = m_listTriangles.end();
		iterTriangle--;

		v0v1Iter->triangle = iterTriangle;
		v1v2Iter->triangle = iterTriangle;
		v2v0Iter->triangle = iterTriangle;
	}

	void SurfacePatch::computeNormalsFromVolume(VolumeIterator volIter)
	{
		//LogManager::getSingleton().logMessage("In SurfacePatch::computeNormalsFromVolume");
		for(SurfaceVertexIterator vertexIter = m_listVertices.begin(); vertexIter != m_listVertices.end(); ++vertexIter)
		{
			//LogManager::getSingleton().logMessage("In Loop");
			const float posX = (vertexIter->position.x + m_v3dOffset.x) / 2.0f;
			const float posY = (vertexIter->position.y + m_v3dOffset.y) / 2.0f;
			const float posZ = (vertexIter->position.z + m_v3dOffset.z) / 2.0f;

			const uint floorX = static_cast<uint>(posX);
			const uint floorY = static_cast<uint>(posY);
			const uint floorZ = static_cast<uint>(posZ);

			NormalGenerationMethod normalGenerationMethod = CENTRAL_DIFFERENCE;

			switch(normalGenerationMethod)
			{
			case SIMPLE:
				{
					volIter.setPosition(static_cast<uint>(posX),static_cast<uint>(posY),static_cast<uint>(posZ));
					const uchar uFloor = volIter.getVoxel() > 0 ? 1 : 0;
					if((posX - floorX) > 0.25) //The result should be 0.0 or 0.5
					{					
						uchar uCeil = volIter.peekVoxel1px0py0pz() > 0 ? 1 : 0;
						vertexIter->normal = Vector3(uFloor - uCeil,0.0,0.0);
					}
					else if((posY - floorY) > 0.25) //The result should be 0.0 or 0.5
					{
						uchar uCeil = volIter.peekVoxel0px1py0pz() > 0 ? 1 : 0;
						vertexIter->normal = Vector3(0.0,uFloor - uCeil,0.0);
					}
					else if((posZ - floorZ) > 0.25) //The result should be 0.0 or 0.5
					{
						uchar uCeil = volIter.peekVoxel0px0py1pz() > 0 ? 1 : 0;
						vertexIter->normal = Vector3(0.0, 0.0,uFloor - uCeil);					
					}
					vertexIter->normal.normalise();
					break;
				}
			case CENTRAL_DIFFERENCE:
				{
					volIter.setPosition(static_cast<uint>(posX),static_cast<uint>(posY),static_cast<uint>(posZ));
					const Vector3 gradFloor = volIter.getCentralDifferenceGradient();
					if((posX - floorX) > 0.25) //The result should be 0.0 or 0.5
					{			
						volIter.setPosition(static_cast<uint>(posX+1.0),static_cast<uint>(posY),static_cast<uint>(posZ));
					}
					if((posY - floorY) > 0.25) //The result should be 0.0 or 0.5
					{			
						volIter.setPosition(static_cast<uint>(posX),static_cast<uint>(posY+1.0),static_cast<uint>(posZ));
					}
					if((posZ - floorZ) > 0.25) //The result should be 0.0 or 0.5
					{			
						volIter.setPosition(static_cast<uint>(posX),static_cast<uint>(posY),static_cast<uint>(posZ+1.0));					
					}
					const Vector3 gradCeil = volIter.getCentralDifferenceGradient();
					vertexIter->normal = gradFloor + gradCeil;
					vertexIter->normal *= -1;
					vertexIter->normal.normalise();
					break;
				}
			case SOBEL:
				{
					volIter.setPosition(static_cast<uint>(posX),static_cast<uint>(posY),static_cast<uint>(posZ));
					const Vector3 gradFloor = volIter.getSobelGradient();
					if((posX - floorX) > 0.25) //The result should be 0.0 or 0.5
					{			
						volIter.setPosition(static_cast<uint>(posX+1.0),static_cast<uint>(posY),static_cast<uint>(posZ));
					}
					if((posY - floorY) > 0.25) //The result should be 0.0 or 0.5
					{			
						volIter.setPosition(static_cast<uint>(posX),static_cast<uint>(posY+1.0),static_cast<uint>(posZ));
					}
					if((posZ - floorZ) > 0.25) //The result should be 0.0 or 0.5
					{			
						volIter.setPosition(static_cast<uint>(posX),static_cast<uint>(posY),static_cast<uint>(posZ+1.0));					
					}
					const Vector3 gradCeil = volIter.getSobelGradient();
					vertexIter->normal = gradFloor + gradCeil;
					vertexIter->normal *= -1;
					vertexIter->normal.normalise();
					break;
				}
			}
		}
	}

	void SurfacePatch::getVertexAndIndexData(std::vector<SurfaceVertex>& vertexData, std::vector<uint>& indexData)
	{
		vertexData.clear();
		indexData.clear();

		vertexData.resize(m_listVertices.size());
		std::copy(m_listVertices.begin(), m_listVertices.end(), vertexData.begin());

		/*LogManager::getSingleton().logMessage("----------Vertex Data----------");
		for(std::vector<SurfaceVertex>::iterator vertexIter = vertexData.begin(); vertexIter != vertexData.end(); ++vertexIter)
		{
			LogManager::getSingleton().logMessage(StringConverter::toString(vertexIter->position.x) + "," + StringConverter::toString(vertexIter->position.y) + "," + StringConverter::toString(vertexIter->position.z));
		}
		LogManager::getSingleton().logMessage("----------End Vertex Data----------");*/

		for(SurfaceTriangleIterator iterTriangles = m_listTriangles.begin(); iterTriangles != m_listTriangles.end(); ++iterTriangles)
		{		
			//LogManager::getSingleton().logMessage("Begin Triangle:");
			std::vector<SurfaceVertex>::iterator iterVertex;
			SurfaceEdgeIterator edgeIter;
			
			edgeIter = iterTriangles->edge;
			//LogManager::getSingleton().logMessage("Edge Target " + StringConverter::toString(edgeIter->target->position.x) + "," + StringConverter::toString(edgeIter->target->position.y) + "," + StringConverter::toString(edgeIter->target->position.z));
			iterVertex = find(vertexData.begin(), vertexData.end(), *(edgeIter->target));
			LogManager::getSingleton().logMessage("    " + StringConverter::toString(iterVertex->position.x) + "," + StringConverter::toString(iterVertex->position.y) + "," + StringConverter::toString(iterVertex->position.z));
			indexData.push_back(iterVertex - vertexData.begin());

			edgeIter = edgeIter->nextHalfEdge;
			iterVertex = find(vertexData.begin(), vertexData.end(), *(edgeIter->target));
			//LogManager::getSingleton().logMessage("    " + StringConverter::toString(iterVertex->position.x) + "," + StringConverter::toString(iterVertex->position.y) + "," + StringConverter::toString(iterVertex->position.z));
			indexData.push_back(iterVertex - vertexData.begin());

			edgeIter = edgeIter->nextHalfEdge;
			iterVertex = find(vertexData.begin(), vertexData.end(), *(edgeIter->target));
			//LogManager::getSingleton().logMessage("    " + StringConverter::toString(iterVertex->position.x) + "," + StringConverter::toString(iterVertex->position.y) + "," + StringConverter::toString(iterVertex->position.z));
			indexData.push_back(iterVertex - vertexData.begin());

			//LogManager::getSingleton().logMessage("End Triangle");
		}
	}

	/*void SurfacePatch::getVertexAndIndexData(std::vector<SurfaceVertex>& vertexData, std::vector<uint>& indexData)
	{
		vertexData.clear();
		indexData.clear();

		vertexData.resize(m_setVertices.size());
		std::copy(m_setVertices.begin(), m_setVertices.end(), vertexData.begin());

		for(SurfaceTriangleIterator iterTriangles = m_setTriangles.begin(); iterTriangles != m_setTriangles.end(); ++iterTriangles)
		{
			std::vector<SurfaceVertex>::iterator iterVertex;

			iterVertex = lower_bound(vertexData.begin(), vertexData.end(),(*(iterTriangles->v0)));
			indexData.push_back(iterVertex - vertexData.begin());

			iterVertex = lower_bound(vertexData.begin(), vertexData.end(),(*(iterTriangles->v1)));
			indexData.push_back(iterVertex - vertexData.begin());

			iterVertex = lower_bound(vertexData.begin(), vertexData.end(),(*(iterTriangles->v2)));
			indexData.push_back(iterVertex - vertexData.begin());			
		}
	}*/

#ifdef BLAH

	void SurfacePatch::decimate(void)
	{
		LogManager::getSingleton().logMessage("Vertices before decimation = " + StringConverter::toString(m_setVertices.size()));
		LogManager::getSingleton().logMessage("Triangles before decimation = " + StringConverter::toString(m_setTriangles.size())); 		

		//Build the lists of connected vertices
		/*for(SurfaceVertexIterator vertexIter = m_setVertices.begin(); vertexIter != m_setVertices.end(); ++vertexIter)
		{
			vertexIter->listConnectedVertices.clear();

			for(std::list<SurfaceTriangleIterator>::iterator triangleIter = vertexIter->listTrianglesUsingThisVertex.begin(); triangleIter != vertexIter->listTrianglesUsingThisVertex.end(); ++triangleIter) 
			{
				SurfaceVertexIterator connectedVertex;

				connectedVertex = (*triangleIter)->v0;
				if(find(vertexIter->listConnectedVertices.begin(),vertexIter->listConnectedVertices.end(),connectedVertex) == vertexIter->listConnectedVertices.end())
					vertexIter->listConnectedVertices.push_back(connectedVertex);

				connectedVertex = (*triangleIter)->v1;
				if(find(vertexIter->listConnectedVertices.begin(),vertexIter->listConnectedVertices.end(),connectedVertex) == vertexIter->listConnectedVertices.end())
					vertexIter->listConnectedVertices.push_back(connectedVertex);

				connectedVertex = (*triangleIter)->v2;
				if(find(vertexIter->listConnectedVertices.begin(),vertexIter->listConnectedVertices.end(),connectedVertex) == vertexIter->listConnectedVertices.end())
					vertexIter->listConnectedVertices.push_back(connectedVertex);
			}

			//Remove self from own connected vertex list
			vertexIter->listConnectedVertices.remove(vertexIter);
		}*/

		//do the vertex merging
		//for(SurfaceVertexIterator vertexIter = m_setVertices.begin(); vertexIter != m_setVertices.end(); ++vertexIter)
		for(uint ct = 0; ct < 500; ++ct)
		{
			SurfaceVertexIterator currentVertexIter;
			for(currentVertexIter = m_setVertices.begin(); currentVertexIter != m_setVertices.end(); ++currentVertexIter)
			{

				//LogManager::getSingleton().logMessage("Vertex Pos = " + StringConverter::toString(vertexIter->position.x) + "," + StringConverter::toString(vertexIter->position.y) + "," + StringConverter::toString(vertexIter->position.z) + " No of connected vertices = " + StringConverter::toString(vertexIter->listConnectedVertices.size()));

				/*if(vertexIter->listConnectedVertices.size() == 6)
					break;*/
				if(currentVertexIter->flags == 0)
					break;
				if(currentVertexIter->flags == 0x01)
					break;
				if(currentVertexIter->flags == 0x02)
					break;
				if(currentVertexIter->flags == 0x04)
					break;
				if(currentVertexIter->flags == 0x08)
					break;
			}
			if(currentVertexIter == m_setVertices.end())
				break;

			SurfaceVertex& currentVertex = *currentVertexIter;

			//Find a vertex to merge with
			/*std::list<SurfaceVertexIterator>::iterator firstConnectedVertexIter = currentVertex.listConnectedVertices.begin();
			SurfaceVertex& vertexToMergeWith = (*(*firstConnectedVertexIter));
			SurfaceVertexIterator vertexToMergeWithIter = m_setVertices.find(vertexToMergeWith);*/
			
			SurfaceVertexIterator vertexToMergeWithIter = (*(currentVertex.listTrianglesUsingThisVertex.begin()))->v0;
			SurfaceVertex vertexToMergeWith = *vertexToMergeWithIter;
			if(currentVertex == vertexToMergeWith)
			{
				vertexToMergeWithIter = (*(currentVertex.listTrianglesUsingThisVertex.begin()))->v1;
				vertexToMergeWith = *vertexToMergeWithIter;
			}
			for(std::list<SurfaceTriangleIterator>::iterator triangleIterIter = currentVertex.listTrianglesUsingThisVertex.begin(); triangleIterIter != currentVertex.listTrianglesUsingThisVertex.end(); ++triangleIterIter)
			{
				SurfaceVertex connectedVertex = (*(*triangleIterIter)->v0);
			}
			

			//Change triangles to use new vertex
			for(SurfaceTriangleIterator iterTriangles = m_setTriangles.begin(); iterTriangles != m_setTriangles.end(); ++iterTriangles)
			{
				if(*(iterTriangles->v0) == currentVertex)
					iterTriangles->v0 = vertexToMergeWithIter;
				if(*(iterTriangles->v1) == currentVertex)
					iterTriangles->v1 = vertexToMergeWithIter;
				if(*(iterTriangles->v2) == currentVertex)
					iterTriangles->v2 = vertexToMergeWithIter;
			}

			//Remove the vertex from other connected vertex lists
			/*for(std::list<SurfaceVertexIterator>::iterator connectedVertexIter = currentVertex.listConnectedVertices.begin(); connectedVertexIter != currentVertex.listConnectedVertices.end(); ++connectedVertexIter)
			{
				for(std::list<SurfaceVertexIterator>::iterator secondLevelConnectedIter = (*connectedVertexIter)->listConnectedVertices.begin(); secondLevelConnectedIter != (*connectedVertexIter)->listConnectedVertices.end(); ++secondLevelConnectedIter)
				{
					if((*(*secondLevelConnectedIter)) == currentVertex)
					{
						(*connectedVertexIter)->listConnectedVertices.remove(secondLevelConnected);
						break;
					}
				}
			}*/

			//Now remove the vertex as nothing should point to it.
			m_setVertices.erase(currentVertexIter);
		}
	}

	/*bool SurfacePatch::verticesArePlanar(SurfaceVertexIterator iterCurrentVertex)
	{
		//FIXME - specially handle the case where they are all the same.
		//This is happening a lot after many vertices have been moved round?
		bool allXMatch = true;
		bool allYMatch = true;
		bool allZMatch = true;
		bool allNormalsMatch = true;

		//FIXME - reorder come of these tests based on likelyness to fail?

		//std::set<uint>::iterator iterConnectedVertices;

		std::list<SurfaceVertexIterator> listConnectedVertices = iterCurrentVertex->listConnectedVertices;
		std::list<SurfaceVertexIterator>::iterator iterConnectedVertices;
		for(iterConnectedVertices = listConnectedVertices.begin(); iterConnectedVertices != listConnectedVertices.end(); ++iterConnectedVertices)
		{
			if(iterCurrentVertex->position.x != (*iterConnectedVertices)->position.x)
			{
				allXMatch = false;
			}
			if(iterCurrentVertex->position.y != (*iterConnectedVertices)->position.y)
			{
				allYMatch = false;
			}
			if(iterCurrentVertex->position.z != (*iterConnectedVertices)->position.z)
			{
				allZMatch = false;
			}
			//FIXME - are these already normalised? We should make sure they are...
			if(iterCurrentVertex->normal.normalisedCopy().dotProduct((*iterConnectedVertices)->normal.normalisedCopy()) < 0.99)
			{
				return false;				
			}
		}

		return allXMatch || allYMatch || allZMatch;
	}*/
#endif

}

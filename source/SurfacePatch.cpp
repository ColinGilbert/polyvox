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

		computeOtherHalfEdges();

	}

	void SurfacePatch::addTriangle(const SurfaceVertex& v0,const SurfaceVertex& v1,const SurfaceVertex& v2)
	{
		if(v0.position.x > 8)
			return;
		if(v0.position.y > 8)
			return;
		if(v1.position.x > 8)
			return;
		if(v1.position.y > 8)
			return;
		if(v2.position.x > 8)
			return;
		if(v2.position.y > 8)
			return;


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

		/*SurfaceEdgeIterator v0v1Iter = m_listEdges.end();
		SurfaceEdgeIterator v1v2Iter = m_listEdges.end();
		SurfaceEdgeIterator v2v0Iter = m_listEdges.end();
		SurfaceEdgeIterator v1v0Iter = m_listEdges.end();
		SurfaceEdgeIterator v2v1Iter = m_listEdges.end();
		SurfaceEdgeIterator v0v2Iter = m_listEdges.end();
		for(SurfaceEdgeIterator edgeIter = m_listEdges.begin(); edgeIter != m_listEdges.end(); ++edgeIter)
		{
			if((edgeIter->otherHalfEdge->target == v0) && (edgeIter->target == v1))
				v0v1Iter = edgeIter;
			if((edgeIter->otherHalfEdge->target == v1) && (edgeIter->target == v2))
				v1v2Iter = edgeIter;
			if((edgeIter->otherHalfEdge->target == v2) && (edgeIter->target == v0))
				v2v0Iter = edgeIter;
			if((edgeIter->otherHalfEdge->target == v1) && (edgeIter->target == v0))
				v1v0Iter = edgeIter;
			if((edgeIter->otherHalfEdge->target == v2) && (edgeIter->target == v1))
				v2v1Iter = edgeIter;
			if((edgeIter->otherHalfEdge->target == v0) && (edgeIter->target == v2))
				v0v2Iter = edgeIter;
		}

		if(v0v1Iter == m_listEdges.end())
		{
			SurfaceEdge v0v1;
			m_listEdges.push_back(v0v1);
			SurfaceEdgeIterator v0v1ToAdd;
		}*/

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

		v0v1Iter->previousHalfEdge = v2v0Iter;
		v1v2Iter->previousHalfEdge = v0v1Iter;
		v2v0Iter->previousHalfEdge = v1v2Iter;

		SurfaceTriangle triangle;

		triangle.edge = v0v1Iter;	

		m_listTriangles.push_back(triangle);
		SurfaceTriangleIterator iterTriangle = m_listTriangles.end();
		iterTriangle--;

		v0v1Iter->triangle = iterTriangle;
		v1v2Iter->triangle = iterTriangle;
		v2v0Iter->triangle = iterTriangle;

		v0v1Iter->hasTriangle = true;
		v1v2Iter->hasTriangle = true;
		v2v0Iter->hasTriangle = true;
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
			//LogManager::getSingleton().logMessage("    " + StringConverter::toString(iterVertex->position.x) + "," + StringConverter::toString(iterVertex->position.y) + "," + StringConverter::toString(iterVertex->position.z));
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

	void SurfacePatch::computeOtherHalfEdges(void)
	{
		LogManager::getSingleton().logMessage("Computing other half edges");
		//Clear all other edges
		for(SurfaceEdgeIterator edgeIter = m_listEdges.begin(); edgeIter != m_listEdges.end(); ++edgeIter)
		{
			edgeIter->otherHalfEdge = m_listEdges.end();
		}

		//FIXME - speed this up by storing edges in a container which sorts by edge 'target'.

		std::list<SurfaceEdge> listAddedEdges;

		//Assign all other edges
		for(SurfaceEdgeIterator outerEdgeIter = m_listEdges.begin(); outerEdgeIter != m_listEdges.end(); ++outerEdgeIter)
		{
			bool foundOtherHalf = false;
			for(SurfaceEdgeIterator innerEdgeIter = m_listEdges.begin(); innerEdgeIter != m_listEdges.end(); ++innerEdgeIter)
			{
				if((innerEdgeIter->target == outerEdgeIter->previousHalfEdge->target) && (outerEdgeIter->target == innerEdgeIter->previousHalfEdge->target))
				{
					innerEdgeIter->otherHalfEdge = outerEdgeIter;
					outerEdgeIter->otherHalfEdge = innerEdgeIter;

					foundOtherHalf = true;
				}
			}
			if(!foundOtherHalf)
			{				
				SurfaceEdge tempEdge;

				tempEdge.otherHalfEdge = outerEdgeIter;
				tempEdge.target = outerEdgeIter->nextHalfEdge->nextHalfEdge->target;

				listAddedEdges.push_back(tempEdge);
			}
		}

		LogManager::getSingleton().logMessage("No of added edges = " + StringConverter::toString(listAddedEdges.size()));

		for(SurfaceEdgeIterator addedEdgeIter = listAddedEdges.begin(); addedEdgeIter != listAddedEdges.end(); ++addedEdgeIter)
		{
			LogManager::getSingleton().logMessage("Adding new edge");
			m_listEdges.push_back(*addedEdgeIter);
			SurfaceEdgeIterator justAddedIter = m_listEdges.end();
			--justAddedIter;

			justAddedIter->otherHalfEdge->otherHalfEdge = justAddedIter;
			LogManager::getSingleton().logMessage("Done adding new edge");
		}

		LogManager::getSingleton().logMessage("Done computing other half edges");
	}

	bool SurfacePatch::decimate3(void)
	{
		bool didDecimation = false;
		LogManager::getSingleton().logMessage("Performing decimation");
		LogManager::getSingleton().logMessage("No of triangles = " + StringConverter::toString(m_listTriangles.size()));
		//int fixed = 0;
		//int movable = 0;
		for(SurfaceVertexIterator vertexIter = m_listVertices.begin(); vertexIter != m_listVertices.end(); ++vertexIter)
		{
			LogManager::getSingleton().logMessage("Examining vertex");
			std::list<SurfaceVertexIterator> listConnectedVertices;

			//listConnectedVertices.push_back(vertexIter);
			
			SurfaceEdgeIterator firstEdge = vertexIter->edge;
			SurfaceVertexIterator firstVertex = firstEdge->target;

			SurfaceEdgeIterator nextEdge = firstEdge;
			do
			{
				if(nextEdge->hasTriangle == false)
				{
					break;
				}
				listConnectedVertices.push_back(nextEdge->target);
				nextEdge = nextEdge->nextHalfEdge->nextHalfEdge->otherHalfEdge;
			}while((nextEdge != firstEdge) && (nextEdge != m_listEdges.end()));

			if(nextEdge->hasTriangle == false)
			{
				continue;
			}

			bool allXMatch = true;
			bool allYMatch = true;
			bool allZMatch = true;
			LogManager::getSingleton().logMessage("Checking connected vertices");
			for(std::list<SurfaceVertexIterator>::iterator connectedVertexIter = listConnectedVertices.begin(); connectedVertexIter != listConnectedVertices.end(); ++connectedVertexIter)
			{
				if((*connectedVertexIter)->position.x != vertexIter->position.x)
				{
					allXMatch = false;
				}
				if((*connectedVertexIter)->position.y != vertexIter->position.y)
				{
					allYMatch = false;
				}
				if((*connectedVertexIter)->position.z != vertexIter->position.z)
				{
					allZMatch = false;
				}
			}

			if((allXMatch) || (allYMatch) || (allZMatch))
			{
				LogManager::getSingleton().logMessage("    All flat");
				/*for(SurfaceVertexIterator innerVertexIter = m_listVertices.begin(); innerVertexIter != m_listVertices.end(); ++innerVertexIter)
				{
					if(innerVertexIter->position == vertexIter->position)
					{
						innerVertexIter->position = (*listConnectedVertices.begin())->position;
					}
				}*/
				nextEdge = firstEdge;
				//std::list<SurfaceVertexIterator> verticesFormingPolygon;
				do
				{
					//verticesFormingPolygon.push_back(nextEdge->target);
					LogManager::getSingleton().logMessage("Removing triangle");
					m_listTriangles.erase(nextEdge->triangle);
					nextEdge = nextEdge->nextHalfEdge->nextHalfEdge->otherHalfEdge;
				}while(nextEdge != firstEdge);

				//nextEdge = firstEdge;
				std::list<SurfaceEdgeIterator> edgesToRemove;
				std::list<SurfaceEdgeIterator> edgesFormingPolygon;
				do
				{
					LogManager::getSingleton().logMessage("Adding Edges To Remove");
					edgesToRemove.push_back(nextEdge);
					nextEdge = nextEdge->nextHalfEdge;
					edgesFormingPolygon.push_back(nextEdge);
					nextEdge = nextEdge->nextHalfEdge;
					edgesToRemove.push_back(nextEdge);
					nextEdge = nextEdge->otherHalfEdge;
				}while(nextEdge != firstEdge);

				for(std::list<SurfaceEdgeIterator>::iterator edgesToRemoveIter = edgesToRemove.begin(); edgesToRemoveIter != edgesToRemove.end(); ++edgesToRemoveIter)
				{
					m_listEdges.erase(*edgesToRemoveIter);
				}

				m_listVertices.erase(vertexIter);

				//Now triangulate...
				LogManager::getSingleton().logMessage("Doing triangulation");
				//std::list<SurfaceEdgeIterator>::iterator firstEdgeIter = edgesFormingPolygon.begin();
				std::list<SurfaceEdgeIterator>::iterator secondEdgeIter = edgesFormingPolygon.begin();
				SurfaceEdgeIterator lastAddedEdge = (*edgesFormingPolygon.begin());

				//++firstEdgeIter;
				//++secondEdgeIter;
				++secondEdgeIter;

				std::list<SurfaceEdgeIterator>::iterator endEdgeIter = edgesFormingPolygon.end();
				--endEdgeIter;
				--endEdgeIter;
				while(secondEdgeIter != endEdgeIter)
				{
					SurfaceEdge newEdge;
					newEdge.target = (lastAddedEdge)->otherHalfEdge->target;
					newEdge.nextHalfEdge = (lastAddedEdge);
					newEdge.previousHalfEdge = (*secondEdgeIter);
					m_listEdges.push_back(newEdge);

					SurfaceEdgeIterator newEdgeIter = m_listEdges.end();
					--newEdgeIter;

					(lastAddedEdge)->nextHalfEdge = (*secondEdgeIter);
					(lastAddedEdge)->previousHalfEdge = (newEdgeIter);
					(*secondEdgeIter)->nextHalfEdge = (newEdgeIter);
					(*secondEdgeIter)->previousHalfEdge = (lastAddedEdge);

					SurfaceEdge otherNewEdge;
					m_listEdges.push_back(otherNewEdge);
					SurfaceEdgeIterator otherNewEdgeIter = m_listEdges.end();
					--otherNewEdgeIter;

					otherNewEdgeIter->target = (*secondEdgeIter)->target;
					otherNewEdgeIter->otherHalfEdge = newEdgeIter;
					newEdgeIter->otherHalfEdge = otherNewEdgeIter;

					SurfaceTriangle triangle;
					m_listTriangles.push_back(triangle);
					SurfaceTriangleIterator iterTriangle = m_listTriangles.end();
					iterTriangle--;

					iterTriangle->edge = lastAddedEdge;
					lastAddedEdge->triangle = iterTriangle;
					(*secondEdgeIter)->triangle = iterTriangle;
					newEdgeIter->triangle = iterTriangle;

					lastAddedEdge->hasTriangle = true;
					(*secondEdgeIter)->hasTriangle = true;
					newEdgeIter->hasTriangle = true;

					//++firstEdgeIter;
					++secondEdgeIter;
					lastAddedEdge = otherNewEdgeIter;
				}

				++endEdgeIter;
				lastAddedEdge->nextHalfEdge = (*secondEdgeIter);
				(*secondEdgeIter)->nextHalfEdge = (*endEdgeIter);
				(*endEdgeIter)->nextHalfEdge = lastAddedEdge;

				lastAddedEdge->previousHalfEdge = (*endEdgeIter);
				(*endEdgeIter)->previousHalfEdge = (*secondEdgeIter);
				(*secondEdgeIter)->previousHalfEdge = lastAddedEdge;

				SurfaceTriangle triangle;
				m_listTriangles.push_back(triangle);
				SurfaceTriangleIterator iterTriangle = m_listTriangles.end();
				iterTriangle--;

				iterTriangle->edge = lastAddedEdge;
				lastAddedEdge->triangle = iterTriangle;
				(*secondEdgeIter)->triangle = iterTriangle;
				(*endEdgeIter)->triangle = iterTriangle;

				lastAddedEdge->hasTriangle = true;
				(*secondEdgeIter)->hasTriangle = true;
				(*endEdgeIter)->hasTriangle = true;

				didDecimation = true;
				break;
			}			
		}

		//LogManager::getSingleton().logMessage("Fixed = " + StringConverter::toString(fixed) + " Movable = "  + StringConverter::toString(movable));
		LogManager::getSingleton().logMessage("Done decimation");
		LogManager::getSingleton().logMessage("No of triangles = " + StringConverter::toString(m_listTriangles.size()));

		return didDecimation;
	}


#ifdef BLAH
	bool SurfacePatch::decimate(void)
	{
		bool removedEdge = false;
		LogManager::getSingleton().logMessage("Performing decimation");
		LogManager::getSingleton().logMessage("No of triangles = " + StringConverter::toString(m_listTriangles.size()));

		for(SurfaceEdgeIterator edgeIter = m_listEdges.begin(); edgeIter != m_listEdges.end(); ++edgeIter)
		{						
			LogManager::getSingleton().logMessage("Examining Edge " + edgeIter->toString());

			SurfaceVertexIterator targetVertexIter = edgeIter->target;
			SurfaceVertexIterator otherVertexIter = edgeIter->nextHalfEdge->nextHalfEdge->target;

			LogManager::getSingleton().logMessage("Target Vertex = " + targetVertexIter->toString());
			LogManager::getSingleton().logMessage("Other  Vertex = " + otherVertexIter->toString());

			//if((targetVertexIter->flags == 0) /*&& (otherVertexIter->flags == 0)*/)
			if(canCollapseEdge(*targetVertexIter,*otherVertexIter))
			{
				collapseEdge(edgeIter);
				removedEdge = true;

				break;
			}
			else
			{
				LogManager::getSingleton().logMessage("    Not Collapsing Edge");
				//LogManager::getSingleton().logMessage("Edge Target Vertex = " + StringConverter::toString(edgeIter->target->position.toOgreVector3()));
				//LogManager::getSingleton().logMessage("Other Edge Non-Existant");
			}
		}
		LogManager::getSingleton().logMessage("Done decimation");
		LogManager::getSingleton().logMessage("No of triangles = " + StringConverter::toString(m_listTriangles.size()));

		return removedEdge;
	}

	bool SurfacePatch::canCollapseEdge(SurfaceVertex target, SurfaceVertex other)
	{
		if((target.flags == 0) /*&& (other.flags == 0)*/)
		{
			if((other.flags == 0))
			{
				LogManager::getSingleton().logMessage("Both Zero");
				return true;
			}
			else
			{
				LogManager::getSingleton().logMessage("Target Zero");
				return false;
			}
			//return true;
		}
		else
			return false;
	}

	void SurfacePatch::collapseEdge(SurfaceEdgeIterator edgeIter)
	{
		SurfaceVertexIterator targetVertexIter = edgeIter->target;
		SurfaceVertexIterator otherVertexIter = edgeIter->nextHalfEdge->nextHalfEdge->target;

		LogManager::getSingleton().logMessage("    Collapsing Edge");
		for(SurfaceEdgeIterator innerEdgeIter = m_listEdges.begin(); innerEdgeIter != m_listEdges.end(); ++innerEdgeIter)
		{
			if((innerEdgeIter->target == targetVertexIter) && (innerEdgeIter != edgeIter))
			{
				LogManager::getSingleton().logMessage("    Reset Edge Target");
				innerEdgeIter->target = otherVertexIter;
			}
		}
		
		if(edgeIter->hasOtherHalfEdge)
		{
			LogManager::getSingleton().logMessage("    Has Other Edge");
			SurfaceEdgeIterator otherEdgeIter = edgeIter->otherHalfEdge;

			/*SurfaceTriangleIterator otherTriangleIter = otherEdgeIter->triangle;
			m_listTriangles.erase(otherTriangleIter);*/


			LogManager::getSingleton().logMessage("    Removing Other Edges");
			SurfaceTriangleIterator otherTriangleIter = otherEdgeIter->triangle;
			SurfaceEdgeIterator currentIter = otherTriangleIter->edge;
			for(uint ct = 0; ct < 3; ++ct)
			{
				SurfaceEdgeIterator previousIter = currentIter;
				currentIter = currentIter->nextHalfEdge;

				if(previousIter->hasOtherHalfEdge)
				{
					previousIter->otherHalfEdge->hasOtherHalfEdge = false;
				}
				for(SurfaceVertexIterator vertexIter = m_listVertices.begin(); vertexIter != m_listVertices.end(); ++vertexIter)
				{
					if(vertexIter->edge == previousIter)
					{
						LogManager::getSingleton().logMessage("Error! Vertex points to dead edge!");
						for(SurfaceEdgeIterator potentialNewEdge = m_listEdges.begin(); potentialNewEdge != m_listEdges.end(); ++potentialNewEdge)
						{
							if(potentialNewEdge->hasOtherHalfEdge)
							{
								if(potentialNewEdge->otherHalfEdge->target == vertexIter)
								{
									vertexIter->edge = potentialNewEdge;
									LogManager::getSingleton().logMessage("    Fixed");
									break;
								}
							}
						}
					}
				}
				m_listEdges.erase(previousIter);
			}
			LogManager::getSingleton().logMessage("    Removing Other Triangle");
			m_listTriangles.erase(otherTriangleIter);
		}
		else
		{
			LogManager::getSingleton().logMessage("    Does Not Have Other Edge");
		}

		LogManager::getSingleton().logMessage("    Removing Edges");
		SurfaceTriangleIterator triangleIter = edgeIter->triangle;
		SurfaceEdgeIterator currentIter = triangleIter->edge;
		for(uint ct = 0; ct < 3; ++ct)
		{
			SurfaceEdgeIterator previousIter = currentIter;
			currentIter = currentIter->nextHalfEdge;

			if(previousIter->hasOtherHalfEdge)
			{
				previousIter->otherHalfEdge->hasOtherHalfEdge = false;
			}
			for(SurfaceVertexIterator vertexIter = m_listVertices.begin(); vertexIter != m_listVertices.end(); ++vertexIter)
			{
				if(vertexIter->edge == previousIter)
				{
					LogManager::getSingleton().logMessage("Error! Vertex points to dead edge!");
					for(SurfaceEdgeIterator potentialNewEdge = m_listEdges.begin(); potentialNewEdge != m_listEdges.end(); ++potentialNewEdge)
						{
							if(potentialNewEdge->hasOtherHalfEdge)
							{
								if(potentialNewEdge->otherHalfEdge->target == vertexIter)
								{
									vertexIter->edge = potentialNewEdge;
									LogManager::getSingleton().logMessage("    Fixed");
									break;
								}
							}
						}
				}
			}
			m_listEdges.erase(previousIter);
		}
		LogManager::getSingleton().logMessage("    Removing Triangle");
		m_listTriangles.erase(triangleIter);

		LogManager::getSingleton().logMessage("    Removing Vertex");
		m_listVertices.erase(targetVertexIter);
	}
#endif

#ifdef BLAH2

	bool SurfacePatch::decimate2(void)
	{
		bool didDecimate = false;
		//return false;
		LogManager::getSingleton().logMessage("Performing decimation");
		LogManager::getSingleton().logMessage("No of triangles = " + StringConverter::toString(m_listTriangles.size()));

		std::list<SurfaceEdgeIterator> edgeList;

		for(SurfaceVertexIterator vertexIter = m_listVertices.begin(); vertexIter != m_listVertices.end(); ++vertexIter)
		{
			vertexIter->fixed = false;
		}

		for(SurfaceEdgeIterator edgeIter = m_listEdges.begin(); edgeIter != m_listEdges.end(); ++edgeIter)
		{						
			LogManager::getSingleton().logMessage("Examining Edge " + edgeIter->toString());

			SurfaceVertexIterator targetVertexIter = edgeIter->target;
			SurfaceVertexIterator otherVertexIter = edgeIter->nextHalfEdge->nextHalfEdge->target;

			LogManager::getSingleton().logMessage("Target Vertex = " + targetVertexIter->toString());
			LogManager::getSingleton().logMessage("Other  Vertex = " + otherVertexIter->toString());

			if(canCollapseEdge2(*targetVertexIter,*otherVertexIter))
			{
				LogManager::getSingleton().logMessage("    Collapsing Edge");
				edgeList.push_back(edgeIter);
				//collapseEdge2(edgeIter);
				/*for(SurfaceVertexIterator vertexIter = m_listVertices.begin(); vertexIter != m_listVertices.end(); ++vertexIter)
				{
					if(vertexIter->position == targetVertexIter->position)
					{
						vertexIter->position = otherVertexIter->position;
						vertexIter->flags = otherVertexIter->flags;

						vertexIter->fixed = true;
					}
				}*/
			}
			else
			{
				LogManager::getSingleton().logMessage("    Not Collapsing Edge");
				//LogManager::getSingleton().logMessage("Edge Target Vertex = " + StringConverter::toString(edgeIter->target->position.toOgreVector3()));
				//LogManager::getSingleton().logMessage("Other Edge Non-Existant");
			}
		}

		for(std::list<SurfaceEdgeIterator>::iterator edgeIter = edgeList.begin(); edgeIter != edgeList.end(); ++edgeIter)
		{
			SurfaceVertexIterator targetVertexIter = (*edgeIter)->target;
			SurfaceVertexIterator otherVertexIter = (*edgeIter)->nextHalfEdge->nextHalfEdge->target;

			if(targetVertexIter->fixed == true)
				continue;

			for(SurfaceVertexIterator vertexIter = m_listVertices.begin(); vertexIter != m_listVertices.end(); ++vertexIter)
			{
				if(vertexIter->position == targetVertexIter->position)
				{
					vertexIter->position = otherVertexIter->position;
					vertexIter->flags = otherVertexIter->flags;

					vertexIter->noOfUses = otherVertexIter->noOfUses;

					vertexIter->fixed = true;

					didDecimate = true;
				}
			}
		}
		

		LogManager::getSingleton().logMessage("Done decimation");
		LogManager::getSingleton().logMessage("No of triangles = " + StringConverter::toString(m_listTriangles.size()));

		return didDecimate;
	}

	bool SurfacePatch::canCollapseEdge2(SurfaceVertex target, SurfaceVertex other)
	{
		if(target.position == other.position)
			return false;

		if(target.fixed)
			return false;

		//FIXME - are these already normalised? We should make sure they are...
		if(target.normal.normalisedCopy().dotProduct(other.normal.normalisedCopy()) < 0.999)
		{
			return false;				
		}

		/*if((target.flags == 0) && (other.flags != 0))
			return true;

		return false;*/

		/*if(target.flags == other.flags)
			return true;

		return false;*/

		/*if(target.position == UIntVector3(0,0,15))
			return false;
		if(target.position == UIntVector3(0,16,15))
			return false;
		if(target.position == UIntVector3(16,0,15))
			return false;
		if(target.position == UIntVector3(16,16,15))
			return false;*/

		/*if(target.position.x < 3)
			return false;
		if(target.position.y < 3)
			return false;
		if(target.position.x > 13)
			return false;
		if(target.position.y > 13)
			return false;*/		

		//return true;

		if((target.noOfUses == 6) && (other.noOfUses != 6))
			return true;

		/*if((target.noOfUses == 6) && (other.noOfUses == 3))
			return true;*/

		return false;

		/*if((matchesAll(other.flags, target.flags)) && (!matchesAll(target.flags, other.flags)))
		{
			return false;
		}
		return true;*/
	}

	bool SurfacePatch::matchesAll(uchar target, uchar other)
	{
		if(target & 1)
		{
			if(other & 1)
			{
			}
			else
			{
				return false;
			}
		}

		if(target & 2)
		{
			if(other & 2)
			{
			}
			else
			{
				return false;
			}
		}

		if(target & 4)
		{
			if(other & 4)
			{
			}
			else
			{
				return false;
			}
		}

		if(target & 8)
		{
			if(other & 8)
			{
			}
			else
			{
				return false;
			}
		}

		if(target & 16)
		{
			if(other & 16)
			{
			}
			else
			{
				return false;
			}
		}

		if(target & 32)
		{
			if(other & 32)
			{
			}
			else
			{
				return false;
			}
		}
	}

	void SurfacePatch::collapseEdge2(SurfaceEdgeIterator edgeIter)
	{
		SurfaceVertexIterator targetVertexIter = edgeIter->target;
		SurfaceVertexIterator otherVertexIter = edgeIter->nextHalfEdge->nextHalfEdge->target;

		for(SurfaceVertexIterator vertexIter = m_listVertices.begin(); vertexIter != m_listVertices.end(); ++vertexIter)
		{
			if(vertexIter->position == targetVertexIter->position)
			{
				vertexIter->position = otherVertexIter->position;
			}
		}
	}
#endif

}

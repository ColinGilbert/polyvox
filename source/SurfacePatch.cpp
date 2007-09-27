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

		//computeOtherHalfEdges();

	}

	void SurfacePatch::addTriangle(const SurfaceVertex& v0,const SurfaceVertex& v1,const SurfaceVertex& v2)
	{
		/*if(v0.getPosition().x > 4)
			return;
		if(v0.getPosition().y > 4)
			return;
		if(v1.getPosition().x > 4)
			return;
		if(v1.getPosition().y > 4)
			return;
		if(v2.getPosition().x > 4)
			return;
		if(v2.getPosition().y > 4)
			return;*/


		m_uTrianglesAdded++;
		m_uVerticesAdded += 3;			
		
		SurfaceVertexIterator v0Iter = findOrAddVertex(v0);
		SurfaceVertexIterator v1Iter = findOrAddVertex(v1);
		SurfaceVertexIterator v2Iter = findOrAddVertex(v2);
		

		SurfaceEdgeIterator v0v1Iter = findOrAddEdge(v0Iter,v1Iter);
		SurfaceEdgeIterator v1v2Iter = findOrAddEdge(v1Iter,v2Iter);
		SurfaceEdgeIterator v2v0Iter = findOrAddEdge(v2Iter,v0Iter);

		v0Iter->setEdge(v0v1Iter);
		v1Iter->setEdge(v1v2Iter);
		v2Iter->setEdge(v2v0Iter);

		v0v1Iter->setNextHalfEdge(v1v2Iter);
		v1v2Iter->setNextHalfEdge(v2v0Iter);
		v2v0Iter->setNextHalfEdge(v0v1Iter);

		v0v1Iter->setPreviousHalfEdge(v2v0Iter);
		v1v2Iter->setPreviousHalfEdge(v0v1Iter);
		v2v0Iter->setPreviousHalfEdge(v1v2Iter);

		SurfaceTriangle triangle;

		triangle.setEdge(v0v1Iter);	

		//m_listTriangles.push_back(triangle);
		//SurfaceTriangleIterator iterTriangle = m_listTriangles.end();
		//iterTriangle--;

		SurfaceTriangleIterator iterTriangle = m_listTriangles.insert(triangle).first;

		v0v1Iter->setTriangle(iterTriangle);
		v1v2Iter->setTriangle(iterTriangle);
		v2v0Iter->setTriangle(iterTriangle);
	}

	SurfaceVertexIterator SurfacePatch::findOrAddVertex(const SurfaceVertex& vertex)
	{
		/*SurfaceVertexIterator vertexIter = find(m_listVertices.begin(), m_listVertices.end(), vertex);
		if(vertexIter == m_listVertices.end())
		{		
			//LogManager::getSingleton().logMessage("Adding Vertex " + StringConverter::toString(v0.position.x) + "," + StringConverter::toString(v0.position.y) + "," + StringConverter::toString(v0.position.z));
			m_listVertices.push_back(vertex);
			vertexIter = m_listVertices.end();
			vertexIter--;
		}
		return vertexIter;*/

		return m_listVertices.insert(vertex).first;
	}

	SurfaceEdgeIterator SurfacePatch::findEdge(const SurfaceVertexIterator& source, const SurfaceVertexIterator& target)
	{
		/*LogManager::getSingleton().logMessage("In findEdge()");
		LogManager::getSingleton().logMessage("Input: source = " + source->toString() + " target = " + target->toString());
		for(SurfaceEdgeIterator edgeIter = m_listEdges.begin(); edgeIter != m_listEdges.end(); ++edgeIter)
		{
			LogManager::getSingleton().logMessage("Current: source = " + edgeIter->getSource()->toString() + " target = " + edgeIter->getTarget()->toString());
			if((edgeIter->getTarget() == target) && (edgeIter->getSource() == source))
			{
				return edgeIter;
			}
		}

		//Not found - return end.
		return m_listEdges.end();*/

		SurfaceEdge edgeToFind(target,source);
		return m_listEdges.find(edgeToFind);
	}

	SurfaceEdgeIterator SurfacePatch::findOrAddEdge(const SurfaceVertexIterator& source, const SurfaceVertexIterator& target)
	{
		SurfaceEdge edge(target,source);
		std::pair<SurfaceEdgeIterator, bool> insertionResult = m_listEdges.insert(edge);

		if(insertionResult.second == false)
		{
			//Edge was already in there, so other edge is too.
			return insertionResult.first;
		}
		SurfaceEdgeIterator edgeIter = insertionResult.first;

		SurfaceEdge otherEdge(source, target);		
		SurfaceEdgeIterator otherEdgeIter = m_listEdges.insert(otherEdge).first;

		edgeIter->pairWithOtherHalfEdge(otherEdgeIter);
		otherEdgeIter->pairWithOtherHalfEdge(edgeIter);

		return edgeIter;
	}

	void SurfacePatch::computeNormalsFromVolume(VolumeIterator volIter)
	{
		//LogManager::getSingleton().logMessage("In SurfacePatch::computeNormalsFromVolume");
		for(SurfaceVertexIterator vertexIter = m_listVertices.begin(); vertexIter != m_listVertices.end(); ++vertexIter)
		{
			//LogManager::getSingleton().logMessage("In Loop");
			const float posX = (vertexIter->getPosition().x + m_v3dOffset.x) / 2.0f;
			const float posY = (vertexIter->getPosition().y + m_v3dOffset.y) / 2.0f;
			const float posZ = (vertexIter->getPosition().z + m_v3dOffset.z) / 2.0f;

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
						vertexIter->setNormal(Vector3(uFloor - uCeil,0.0,0.0));
					}
					else if((posY - floorY) > 0.25) //The result should be 0.0 or 0.5
					{
						uchar uCeil = volIter.peekVoxel0px1py0pz() > 0 ? 1 : 0;
						vertexIter->setNormal(Vector3(0.0,uFloor - uCeil,0.0));
					}
					else if((posZ - floorZ) > 0.25) //The result should be 0.0 or 0.5
					{
						uchar uCeil = volIter.peekVoxel0px0py1pz() > 0 ? 1 : 0;
						vertexIter->setNormal(Vector3(0.0, 0.0,uFloor - uCeil));					
					}
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
					vertexIter->setNormal((gradFloor + gradCeil) * -1.0);
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
					vertexIter->setNormal((gradFloor + gradCeil) * -1.0);
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
			LogManager::getSingleton().logMessage(StringConverter::toString(vertexIter->getPosition().x) + "," + StringConverter::toString(vertexIter->getPosition().y) + "," + StringConverter::toString(vertexIter->getPosition().z));
		}
		LogManager::getSingleton().logMessage("----------End Vertex Data----------");*/

		for(SurfaceTriangleIterator iterTriangles = m_listTriangles.begin(); iterTriangles != m_listTriangles.end(); ++iterTriangles)
		{		
			//LogManager::getSingleton().logMessage("Begin Triangle:");
			std::vector<SurfaceVertex>::iterator iterVertex;
			SurfaceEdgeIterator edgeIter;
			
			edgeIter = iterTriangles->getEdge();
			//LogManager::getSingleton().logMessage("Edge Target " + StringConverter::toString(edgeIter->target->position.x) + "," + StringConverter::toString(edgeIter->target->position.y) + "," + StringConverter::toString(edgeIter->target->position.z));
			iterVertex = find(vertexData.begin(), vertexData.end(), *(edgeIter->getTarget()));
			//LogManager::getSingleton().logMessage("");
			//LogManager::getSingleton().logMessage("    " + StringConverter::toString(iterVertex->getPosition().x) + "," + StringConverter::toString(iterVertex->getPosition().y) + "," + StringConverter::toString(iterVertex->getPosition().z));
			indexData.push_back(iterVertex - vertexData.begin());

			edgeIter = edgeIter->getNextHalfEdge();
			iterVertex = find(vertexData.begin(), vertexData.end(), *(edgeIter->getTarget()));
			//LogManager::getSingleton().logMessage("    " + StringConverter::toString(iterVertex->getPosition().x) + "," + StringConverter::toString(iterVertex->getPosition().y) + "," + StringConverter::toString(iterVertex->getPosition().z));
			indexData.push_back(iterVertex - vertexData.begin());

			edgeIter = edgeIter->getNextHalfEdge();
			iterVertex = find(vertexData.begin(), vertexData.end(), *(edgeIter->getTarget()));
			//LogManager::getSingleton().logMessage("    " + StringConverter::toString(iterVertex->getPosition().x) + "," + StringConverter::toString(iterVertex->getPosition().y) + "," + StringConverter::toString(iterVertex->getPosition().z));
			indexData.push_back(iterVertex - vertexData.begin());	

			//LogManager::getSingleton().logMessage("End Triangle");
		}
	}

	bool SurfacePatch::canRemoveVertexFrom(SurfaceVertexIterator vertexIter, std::list<SurfaceVertexIterator> listConnectedIter, bool isEdge)
	{
		bool allXMatch = true;
		bool allYMatch = true;
		bool allZMatch = true;
		bool allNormalsMatch = true;
		bool twoEdgesMatch = true;

		for(std::list<SurfaceVertexIterator>::iterator connectedIter = listConnectedIter.begin(); connectedIter != listConnectedIter.end(); ++connectedIter)
		{
			if((*connectedIter)->getPosition().x != vertexIter->getPosition().x)
			{
				allXMatch = false;
			}
			if((*connectedIter)->getPosition().y != vertexIter->getPosition().y)
			{
				allYMatch = false;
			}
			if((*connectedIter)->getPosition().z != vertexIter->getPosition().z)
			{
				allZMatch = false;
			}
			if((*connectedIter)->getNormal().dotProduct(vertexIter->getNormal()) < 0.99)
			{
				return false;				
			}
		}

		if(isEdge)
		{
			SurfaceVertexIterator firstExtreme = *(listConnectedIter.begin());
			SurfaceVertexIterator secondExtreme = *(--listConnectedIter.end());

			bool edgeXMatch = (firstExtreme->getPosition().x == vertexIter->getPosition().x) && (secondExtreme->getPosition().x == vertexIter->getPosition().x);
			bool edgeYMatch = (firstExtreme->getPosition().y == vertexIter->getPosition().y) && (secondExtreme->getPosition().y == vertexIter->getPosition().y);
			bool edgeZMatch = (firstExtreme->getPosition().z == vertexIter->getPosition().z) && (secondExtreme->getPosition().z == vertexIter->getPosition().z);

			twoEdgesMatch = ((edgeXMatch&&edgeYMatch) || (edgeXMatch&&edgeZMatch) || (edgeYMatch&&edgeZMatch));
		}		

		return (allXMatch || allYMatch || allZMatch)
			&& (twoEdgesMatch);
	}

	std::list<SurfaceVertexIterator> SurfacePatch::findConnectedVertices(SurfaceVertexIterator vertexIter, bool& isEdge)
	{
		isEdge = false;
		std::list<SurfaceVertexIterator> result;
		//LogManager::getSingleton().logMessage("findConnectedVertices " + vertexIter->toString());

		SurfaceEdgeIterator firstEdge = vertexIter->getEdge();
		SurfaceEdgeIterator nextEdge = firstEdge;
		SurfaceEdgeIterator previousEdge = firstEdge;
		int ct = 0;
		do
		{
			ct++;
			//LogManager::getSingleton().logMessage("ct = " + StringConverter::toString(ct));
			if(ct > 100)
			{
				LogManager::getSingleton().logMessage("ct too big!!! Aborting decimation");
				exit(1);
			}

			result.push_back(nextEdge->getTarget());

			previousEdge = nextEdge;
			nextEdge = nextEdge->getPreviousHalfEdge()->getOtherHalfEdge();
		}while((nextEdge != firstEdge) && (nextEdge != previousEdge));

		if(nextEdge == previousEdge)
		{
			//LogManager::getSingleton().logMessage("Is edge");
			//In this case vertexIter is on an edge/

			isEdge = true;

			nextEdge = firstEdge;
			previousEdge = firstEdge;

			previousEdge = nextEdge;
			nextEdge = nextEdge->getOtherHalfEdge()->getNextHalfEdge();

			int ct2 = 0;
			do
			{
				ct2++;
				//LogManager::getSingleton().logMessage("ct2 = " + StringConverter::toString(ct2));
				if(ct2 > 100)
				{
					LogManager::getSingleton().logMessage("ct2 too big!!! Aborting decimation");
					exit(1);
				}

				result.push_front(nextEdge->getTarget());

				previousEdge = nextEdge;
				nextEdge = nextEdge->getOtherHalfEdge()->getNextHalfEdge();
			}while(nextEdge != previousEdge);
		}

		//LogManager::getSingleton().logMessage("Done find");

		return result;
	}

	uint SurfacePatch::decimate(void)
	{
		uint uNoRemoved = 0;
		//LogManager::getSingleton().logMessage("\n\nPerforming decimation");
		//LogManager::getSingleton().logMessage("No of triangles at start = " + StringConverter::toString(m_listTriangles.size()));
		//LogManager::getSingleton().logMessage("No of edges at start = " + StringConverter::toString(m_listEdges.size()));
		//int fixed = 0;
		//int movable = 0;

		std::vector<SurfaceVertexIterator> listVertexIterators;
		//listVertexIterators.resize(m_listVertices.size());
		for(SurfaceVertexIterator vertexIter = m_listVertices.begin(); vertexIter != m_listVertices.end(); ++vertexIter)
		{
			listVertexIterators.push_back(vertexIter);
		}
		//random_shuffle(listVertexIterators.begin(),listVertexIterators.end());

		for(std::vector<SurfaceVertexIterator>::iterator vertexIterIter = listVertexIterators.begin(); vertexIterIter != listVertexIterators.end(); ++vertexIterIter)
		//for(SurfaceVertexIterator vertexIter = m_listVertices.begin(); vertexIter != m_listVertices.end(); ++vertexIter)
		{
			//LogManager::getSingleton().logMessage("Examining vertex " + vertexIter->toString());	

			SurfaceVertexIterator vertexIter = *vertexIterIter;

			bool isEdge;
			std::list<SurfaceVertexIterator> listConnectedVertices = findConnectedVertices(vertexIter,isEdge);		
			listConnectedVertices.remove(vertexIter);
			listConnectedVertices.unique();
			
			/*LogManager::getSingleton().logMessage("No of connected vertices = " + StringConverter::toString(listConnectedVertices.size()));
			for(std::list<SurfaceVertexIterator>::iterator iter = listConnectedVertices.begin(); iter != listConnectedVertices.end(); ++iter)
			{
				LogManager::getSingleton().logMessage("    Connected vertex = " + (*iter)->toString());
			}*/

			if(canRemoveVertexFrom(vertexIter, listConnectedVertices, isEdge) == false)
			{
				continue;
			}

			if(isPolygonConvex(listConnectedVertices, vertexIter->getNormal()) == false)
			{
				continue;
			}

			//LogManager::getSingleton().logMessage("Vertex can be removed");

			//std::list<SurfaceEdgeIterator> edgesToRemove;

			for(std::list<SurfaceVertexIterator>::iterator iter = listConnectedVertices.begin(); iter != listConnectedVertices.end(); ++iter)
			{
				SurfaceEdgeIterator edgeToDelete = findEdge(vertexIter, *iter);
				/*if(edgeToDelete == m_listEdges.end())
				{
					LogManager::getSingleton().logMessage("Error - Failed to find");
				}*/
				SurfaceEdgeIterator otherEdgeToDelete = edgeToDelete->getOtherHalfEdge();

				if(edgeToDelete->getNextHalfEdge() != edgeToDelete->getOtherHalfEdge())
				{
					m_listTriangles.erase(edgeToDelete->getTriangle());
				}

				//LogManager::getSingleton().logMessage("Removing edge " + edgeToDelete->toString());				
				m_listEdges.erase(edgeToDelete);
				//LogManager::getSingleton().logMessage("Removing edge " + otherEdgeToDelete->toString());
				m_listEdges.erase(otherEdgeToDelete);
				//edgesToRemove.push_back(edgeToDelete);
				//edgesToRemove.push_back(otherEdgeToDelete);
			}

			/*LogManager::getSingleton().logMessage("Now removing edges");
			for(std::list<SurfaceEdgeIterator>::iterator iter = edgesToRemove.begin(); iter != edgesToRemove.end(); ++iter)
			{
				m_listEdges.erase(*iter);
			}*/

			//LogManager::getSingleton().logMessage("Removing vertex " + vertexIter->toString());
			m_listVertices.erase(vertexIter);

			//Now triangulate...
			//LogManager::getSingleton().logMessage("Doing triangulation");
			triangulate(listConnectedVertices);

			++uNoRemoved;
			//break;	
		}

		//LogManager::getSingleton().logMessage("Fixed = " + StringConverter::toString(fixed) + " Movable = "  + StringConverter::toString(movable));
		//LogManager::getSingleton().logMessage("Done decimation");
		//LogManager::getSingleton().logMessage("No of triangles at end = " + StringConverter::toString(m_listTriangles.size()));

		/*std::vector<SurfaceVertex> vertexDataTemp;
		std::vector<uint> indexDataTemp;
		getVertexAndIndexData(vertexDataTemp, indexDataTemp);*/

		return uNoRemoved;
	}

	void SurfacePatch::triangulate(std::list<SurfaceVertexIterator> listVertices)
	{
		std::list<SurfaceVertexIterator>::iterator v0IterIter = listVertices.begin();
		std::list<SurfaceVertexIterator>::iterator v1IterIter = listVertices.begin();
		std::list<SurfaceVertexIterator>::iterator v2IterIter = listVertices.begin();
		++v1IterIter;
		++v2IterIter;
		++v2IterIter;
		while(v2IterIter != listVertices.end())
		{
			SurfaceVertexIterator v0Iter = *v0IterIter;
			SurfaceVertexIterator v1Iter = *v1IterIter;
			SurfaceVertexIterator v2Iter = *v2IterIter;

			addTriangle(*v0Iter, *v1Iter, *v2Iter);

			++v1IterIter;
			++v2IterIter;
		}
	}

	bool SurfacePatch::isPolygonConvex(std::list<SurfaceVertexIterator> listVertices, Vector3 normal)
	{
		std::list<SurfaceVertexIterator>::iterator v0IterIter = listVertices.begin();
		std::list<SurfaceVertexIterator>::iterator v1IterIter = listVertices.begin();
		std::list<SurfaceVertexIterator>::iterator v2IterIter = listVertices.begin();
		++v1IterIter;
		++v2IterIter;
		++v2IterIter;
		while(v2IterIter != listVertices.end())
		{
			SurfaceVertexIterator v0Iter = *v0IterIter;
			SurfaceVertexIterator v1Iter = *v1IterIter;
			SurfaceVertexIterator v2Iter = *v2IterIter;

			Vector3 v1tov0(v0Iter->getPosition().toOgreVector3() - v1Iter->getPosition().toOgreVector3());
			Vector3 v1tov2(v2Iter->getPosition().toOgreVector3() - v1Iter->getPosition().toOgreVector3());
			Vector3 cross = (v1tov2).crossProduct(v1tov0);
			cross.normalise();
			
			//LogManager::getSingleton().logMessage("Cross = " + StringConverter::toString(cross));

			if(cross.dotProduct(normal) < 0.99)
			{
				return false;
			}

			++v1IterIter;
			++v2IterIter;
		}

		return true;
	}
}

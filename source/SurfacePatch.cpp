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
	}

	SurfacePatch::~SurfacePatch()
	{		
	}

	SurfaceEdgeIterator SurfacePatch::getEdgesBegin(void)
	{
		return m_listEdges.begin();
	}

	SurfaceEdgeIterator SurfacePatch::getEdgesEnd(void)
	{
		return m_listEdges.end();
	}

	SurfaceTriangleIterator SurfacePatch::getTrianglesBegin(void)
	{
		return m_listTriangles.begin();
	}

	SurfaceTriangleIterator SurfacePatch::getTrianglesEnd(void)
	{
		return m_listTriangles.end();
	}

	SurfaceVertexIterator SurfacePatch::getVerticesBegin(void)
	{
		return m_listVertices.begin();
	}

	SurfaceVertexIterator SurfacePatch::getVerticesEnd(void)
	{
		return m_listVertices.end();
	}

	uint SurfacePatch::getNoOfEdges(void) const
	{
		return m_listEdges.size();
	}

	uint SurfacePatch::getNoOfTriangles(void) const
	{
		return m_listTriangles.size();
	}

	uint SurfacePatch::getNoOfVertices(void) const
	{
		return m_listVertices.size();
	}

	void SurfacePatch::addTriangle(const SurfaceVertex& v0,const SurfaceVertex& v1,const SurfaceVertex& v2)
	{	
		SurfaceVertexIterator v0Iter = findOrAddVertex(v0);
		SurfaceVertexIterator v1Iter = findOrAddVertex(v1);
		SurfaceVertexIterator v2Iter = findOrAddVertex(v2);		

		SurfaceEdgeIterator v0v1Iter = findOrAddEdge(v0Iter,v1Iter);
		SurfaceEdgeIterator v1v2Iter = findOrAddEdge(v1Iter,v2Iter);
		SurfaceEdgeIterator v2v0Iter = findOrAddEdge(v2Iter,v0Iter);

		const_cast<SurfaceVertex&>(*v0Iter).setEdge(v0v1Iter);
		const_cast<SurfaceVertex&>(*v1Iter).setEdge(v1v2Iter);
		const_cast<SurfaceVertex&>(*v2Iter).setEdge(v2v0Iter);

		const_cast<SurfaceEdge&>(*v0v1Iter).setNextHalfEdge(v1v2Iter);
		const_cast<SurfaceEdge&>(*v1v2Iter).setNextHalfEdge(v2v0Iter);
		const_cast<SurfaceEdge&>(*v2v0Iter).setNextHalfEdge(v0v1Iter);

		const_cast<SurfaceEdge&>(*v0v1Iter).setPreviousHalfEdge(v2v0Iter);
		const_cast<SurfaceEdge&>(*v1v2Iter).setPreviousHalfEdge(v0v1Iter);
		const_cast<SurfaceEdge&>(*v2v0Iter).setPreviousHalfEdge(v1v2Iter);

		SurfaceTriangle triangle;

		triangle.setEdge(v0v1Iter);	

		SurfaceTriangleIterator iterTriangle = m_listTriangles.insert(triangle).first;

		const_cast<SurfaceEdge&>(*v0v1Iter).setTriangle(iterTriangle);
		const_cast<SurfaceEdge&>(*v1v2Iter).setTriangle(iterTriangle);
		const_cast<SurfaceEdge&>(*v2v0Iter).setTriangle(iterTriangle);
	}

	SurfaceVertexIterator SurfacePatch::findOrAddVertex(const SurfaceVertex& vertex)
	{
		return m_listVertices.insert(vertex).first;
	}

	SurfaceEdgeIterator SurfacePatch::findEdge(const SurfaceVertexIterator& source, const SurfaceVertexIterator& target)
	{
		return m_listEdges.find(SurfaceEdge(target,source));
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

		const_cast<SurfaceEdge&>(*edgeIter).pairWithOtherHalfEdge(otherEdgeIter);
		const_cast<SurfaceEdge&>(*otherEdgeIter).pairWithOtherHalfEdge(edgeIter);

		return edgeIter;
	}

	bool SurfacePatch::canRemoveVertexFrom(SurfaceVertexIterator vertexIter, std::list<SurfaceVertexIterator> listConnectedIter, bool isEdge)
	{
		bool allXMatch = true;
		bool allYMatch = true;
		bool allZMatch = true;
		bool allAlphaMatch = true;
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
			if((*connectedIter)->getAlpha() != vertexIter->getAlpha())
			{
				allAlphaMatch = false;
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
			&& (allAlphaMatch)
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

	void SurfacePatch::fillVertexAndIndexData(std::vector<SurfaceVertex>& vecVertices, std::vector<ushort>& vecIndices)
	{
		vecVertices.resize(m_listVertices.size());
		std::copy(m_listVertices.begin(), m_listVertices.end(), vecVertices.begin());

		//vecIndices.resize(m_listTriangles.size() * 3);
		for(SurfaceTriangleConstIterator iterTriangles = m_listTriangles.begin(); iterTriangles != m_listTriangles.end(); ++iterTriangles)
		{		
			std::vector<SurfaceVertex>::iterator iterVertex;
			SurfaceEdgeIterator edgeIter;
			
			edgeIter = iterTriangles->getEdge();
			iterVertex = lower_bound(vecVertices.begin(), vecVertices.end(), *(edgeIter->getTarget()));
			vecIndices.push_back(iterVertex - vecVertices.begin());

			edgeIter = edgeIter->getNextHalfEdge();
			iterVertex = lower_bound(vecVertices.begin(), vecVertices.end(), *(edgeIter->getTarget()));
			vecIndices.push_back(iterVertex - vecVertices.begin());

			edgeIter = edgeIter->getNextHalfEdge();
			iterVertex = lower_bound(vecVertices.begin(), vecVertices.end(), *(edgeIter->getTarget()));
			vecIndices.push_back(iterVertex - vecVertices.begin());
		}
	}
}

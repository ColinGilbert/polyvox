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


		m_uTrianglesAdded++;
		m_uVerticesAdded += 3;			
		
		SurfaceVertexIterator v0Iter = findVertex(v0);
		SurfaceVertexIterator v1Iter = findVertex(v1);
		SurfaceVertexIterator v2Iter = findVertex(v2);
		

		SurfaceEdgeIterator v0v1Iter = findEdge(v0Iter,v1Iter);
		SurfaceEdgeIterator v1v2Iter = findEdge(v1Iter,v2Iter);
		SurfaceEdgeIterator v2v0Iter = findEdge(v2Iter,v0Iter);

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
	}

	SurfaceVertexIterator SurfacePatch::findVertex(const SurfaceVertex& vertex)
	{
		SurfaceVertexIterator vertexIter = find(m_listVertices.begin(), m_listVertices.end(), vertex);
		if(vertexIter == m_listVertices.end())
		{		
			//LogManager::getSingleton().logMessage("Adding Vertex " + StringConverter::toString(v0.position.x) + "," + StringConverter::toString(v0.position.y) + "," + StringConverter::toString(v0.position.z));
			m_listVertices.push_back(vertex);
			vertexIter = m_listVertices.end();
			vertexIter--;
		}
		return vertexIter;
	}

	SurfaceEdgeIterator SurfacePatch::findEdge(const SurfaceVertexIterator& source, const SurfaceVertexIterator& target)
	{
		for(SurfaceEdgeIterator edgeIter = m_listEdges.begin(); edgeIter != m_listEdges.end(); ++edgeIter)
		{
			if((edgeIter->target == target) && (edgeIter->otherHalfEdge->target == source))
			{
				return edgeIter;
			}
		}

		//Not found - add it.
		SurfaceEdge edge;
		edge.target = target;
		SurfaceEdge otherEdge;
		otherEdge.target = source;

		m_listEdges.push_back(edge);
		SurfaceEdgeIterator edgeIter = m_listEdges.end();
		edgeIter--;

		m_listEdges.push_back(otherEdge);
		SurfaceEdgeIterator otherEdgeIter = m_listEdges.end();
		otherEdgeIter--;

		edgeIter->otherHalfEdge = otherEdgeIter;
		edgeIter->nextHalfEdge = otherEdgeIter;
		edgeIter->previousHalfEdge = otherEdgeIter;

		otherEdgeIter->otherHalfEdge = edgeIter;
		otherEdgeIter->nextHalfEdge = edgeIter;
		otherEdgeIter->previousHalfEdge = edgeIter;

		/*edgeIter->hasTriangle = false;
		otherEdgeIter->hasTriangle = false;*/

		return edgeIter;
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

		LogManager::getSingleton().logMessage("----------Vertex Data----------");
		for(std::vector<SurfaceVertex>::iterator vertexIter = vertexData.begin(); vertexIter != vertexData.end(); ++vertexIter)
		{
			LogManager::getSingleton().logMessage(StringConverter::toString(vertexIter->position.x) + "," + StringConverter::toString(vertexIter->position.y) + "," + StringConverter::toString(vertexIter->position.z));
		}
		LogManager::getSingleton().logMessage("----------End Vertex Data----------");

		for(SurfaceTriangleIterator iterTriangles = m_listTriangles.begin(); iterTriangles != m_listTriangles.end(); ++iterTriangles)
		{		
			//LogManager::getSingleton().logMessage("Begin Triangle:");
			std::vector<SurfaceVertex>::iterator iterVertex;
			SurfaceEdgeIterator edgeIter;
			
			edgeIter = iterTriangles->edge;
			//LogManager::getSingleton().logMessage("Edge Target " + StringConverter::toString(edgeIter->target->position.x) + "," + StringConverter::toString(edgeIter->target->position.y) + "," + StringConverter::toString(edgeIter->target->position.z));
			iterVertex = find(vertexData.begin(), vertexData.end(), *(edgeIter->target));
			LogManager::getSingleton().logMessage("");
			LogManager::getSingleton().logMessage("    " + StringConverter::toString(iterVertex->position.x) + "," + StringConverter::toString(iterVertex->position.y) + "," + StringConverter::toString(iterVertex->position.z));
			indexData.push_back(iterVertex - vertexData.begin());

			edgeIter = edgeIter->nextHalfEdge;
			iterVertex = find(vertexData.begin(), vertexData.end(), *(edgeIter->target));
			LogManager::getSingleton().logMessage("    " + StringConverter::toString(iterVertex->position.x) + "," + StringConverter::toString(iterVertex->position.y) + "," + StringConverter::toString(iterVertex->position.z));
			indexData.push_back(iterVertex - vertexData.begin());

			edgeIter = edgeIter->nextHalfEdge;
			iterVertex = find(vertexData.begin(), vertexData.end(), *(edgeIter->target));
			LogManager::getSingleton().logMessage("    " + StringConverter::toString(iterVertex->position.x) + "," + StringConverter::toString(iterVertex->position.y) + "," + StringConverter::toString(iterVertex->position.z));
			indexData.push_back(iterVertex - vertexData.begin());

			//LogManager::getSingleton().logMessage("End Triangle");
		}
	}

	bool SurfacePatch::canRemoveVertex(SurfaceVertexIterator vertexIter)
	{
		//LogManager::getSingleton().logMessage("Checking to remove vertex " + vertexIter->toString());
		//FIXME - use bit flags
		bool allXMatch = true;
		bool allYMatch = true;
		bool allZMatch = true;

		SurfaceEdgeIterator firstEdge = vertexIter->edge;
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

			LogManager::getSingleton().logMessage("Edge Target = " + nextEdge->target->toString() + " Edge Source = " + nextEdge->otherHalfEdge->target->toString());

			if(nextEdge->target->position.x != vertexIter->position.x)
			{
				allXMatch = false;
			}
			if(nextEdge->target->position.y != vertexIter->position.y)
			{
				allYMatch = false;
			}
			if(nextEdge->target->position.z != vertexIter->position.z)
			{
				allZMatch = false;
			}

			previousEdge = nextEdge;
			nextEdge = nextEdge->previousHalfEdge->otherHalfEdge;
		}while((nextEdge != firstEdge) && (nextEdge != previousEdge));

		if(nextEdge == previousEdge)
		{
			LogManager::getSingleton().logMessage("Is edge");
			//In this case vertexIter is on an edge
			//return false;
			SurfaceVertexIterator firstExtreme = nextEdge->target;

			nextEdge = firstEdge;
			previousEdge = firstEdge;

			previousEdge = nextEdge;
			nextEdge = nextEdge->otherHalfEdge->nextHalfEdge;

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

				LogManager::getSingleton().logMessage("Edge Target = " + nextEdge->target->toString() + " Edge Source = " + nextEdge->otherHalfEdge->target->toString());
				if(nextEdge->isDegenerate())
				{
					LogManager::getSingleton().logMessage("Degenerate Edge Here!!");
				}

				if(nextEdge->target->position.x != vertexIter->position.x)
				{
					allXMatch = false;
				}
				if(nextEdge->target->position.y != vertexIter->position.y)
				{
					allYMatch = false;
				}
				if(nextEdge->target->position.z != vertexIter->position.z)
				{
					allZMatch = false;
				}

				previousEdge = nextEdge;
				nextEdge = nextEdge->otherHalfEdge->nextHalfEdge;
			}while(nextEdge != previousEdge);

			SurfaceVertexIterator secondExtreme = nextEdge->target;

			LogManager::getSingleton().logMessage("FirstExtreme = " + firstExtreme->toString() + ", SecondExtreme = " + secondExtreme->toString());

			bool edgeXMatch = (firstExtreme->position.x == vertexIter->position.x) && (secondExtreme->position.x == vertexIter->position.x);
			bool edgeYMatch = (firstExtreme->position.y == vertexIter->position.y) && (secondExtreme->position.y == vertexIter->position.y);
			bool edgeZMatch = (firstExtreme->position.z == vertexIter->position.z) && (secondExtreme->position.z == vertexIter->position.z);

			bool twoEdgesMatch = ((edgeXMatch&&edgeYMatch) || (edgeXMatch&&edgeZMatch) || (edgeYMatch&&edgeZMatch));
			
			//LogManager::getSingleton().logMessage("Done checking (edge)");

			return (allXMatch || allYMatch || allZMatch)
				&& (twoEdgesMatch);
		}

		//LogManager::getSingleton().logMessage("Done checking (not edge)");

		return allXMatch || allYMatch || allZMatch;
	}

	std::list<SurfaceVertexIterator> SurfacePatch::findConnectedVertices(SurfaceVertexIterator vertexIter)
	{
		std::list<SurfaceVertexIterator> result;
		//LogManager::getSingleton().logMessage("findConnectedVertices " + vertexIter->toString());

		SurfaceEdgeIterator firstEdge = vertexIter->edge;
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

			result.push_back(nextEdge->target);

			previousEdge = nextEdge;
			nextEdge = nextEdge->previousHalfEdge->otherHalfEdge;
		}while((nextEdge != firstEdge) && (nextEdge != previousEdge));

		if(nextEdge == previousEdge)
		{
			//LogManager::getSingleton().logMessage("Is edge");
			//In this case vertexIter is on an edge/

			nextEdge = firstEdge;
			previousEdge = firstEdge;

			previousEdge = nextEdge;
			nextEdge = nextEdge->otherHalfEdge->nextHalfEdge;

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

				result.push_front(nextEdge->target);

				previousEdge = nextEdge;
				nextEdge = nextEdge->otherHalfEdge->nextHalfEdge;
			}while(nextEdge != previousEdge);
		}

		//LogManager::getSingleton().logMessage("Done find");

		return result;
	}

	std::list<SurfaceEdgeIterator> SurfacePatch::removeTrianglesAndFindEdges(SurfaceVertexIterator vertexIter)
	{
		std::list<SurfaceEdgeIterator> result;
		//LogManager::getSingleton().logMessage("removeTrianglesAndFindEdges " + vertexIter->toString());

		SurfaceEdgeIterator firstEdge = vertexIter->edge;
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

			if(nextEdge->nextHalfEdge != nextEdge->otherHalfEdge)
			{
				m_listTriangles.erase(nextEdge->triangle);
			}

			result.push_back(nextEdge);
			result.push_back(nextEdge->otherHalfEdge);
			previousEdge = nextEdge;
			nextEdge = nextEdge->previousHalfEdge->otherHalfEdge;

		}while((nextEdge != firstEdge) && (nextEdge != previousEdge));

		if(nextEdge == previousEdge)
		{
			//LogManager::getSingleton().logMessage("Is edge");
			//In this case vertexIter is on an edge
			//return false;

			nextEdge = firstEdge;
			previousEdge = firstEdge;

			previousEdge = nextEdge;
			nextEdge = nextEdge->otherHalfEdge->nextHalfEdge;

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

				m_listTriangles.erase(nextEdge->triangle);

				result.push_back(nextEdge);
				result.push_back(nextEdge->otherHalfEdge);
				previousEdge = nextEdge;
				nextEdge = nextEdge->otherHalfEdge->nextHalfEdge;

			}while(nextEdge != previousEdge);
		}

		//LogManager::getSingleton().logMessage("Done find");

		return result;
	}

	bool SurfacePatch::decimate3(void)
	{
		bool didDecimation = false;
		LogManager::getSingleton().logMessage("\n\nPerforming decimation");
		LogManager::getSingleton().logMessage("No of triangles at start = " + StringConverter::toString(m_listTriangles.size()));
		//int fixed = 0;
		//int movable = 0;
		for(SurfaceVertexIterator vertexIter = m_listVertices.begin(); vertexIter != m_listVertices.end(); ++vertexIter)
		{
			LogManager::getSingleton().logMessage("Examining vertex " + vertexIter->toString());
			if(canRemoveVertex(vertexIter) == false)
			{
				continue;
			}
			LogManager::getSingleton().logMessage("Vertex can be removed");

			std::list<SurfaceVertexIterator> listConnectedVertices = findConnectedVertices(vertexIter);		
			listConnectedVertices.remove(vertexIter);
			listConnectedVertices.unique();

			SurfaceEdgeIterator firstEdge = vertexIter->edge;
			SurfaceEdgeIterator nextEdge = firstEdge;

			
			LogManager::getSingleton().logMessage("No of connected vertices = " + StringConverter::toString(listConnectedVertices.size()));
			for(std::list<SurfaceVertexIterator>::iterator iter = listConnectedVertices.begin(); iter != listConnectedVertices.end(); ++iter)
			{
				LogManager::getSingleton().logMessage("    Connected vertex = " + (*iter)->toString());
			}
			
			nextEdge = firstEdge;
			std::list<SurfaceEdgeIterator> edgesToRemove = removeTrianglesAndFindEdges(vertexIter);
			/*do
			{
				LogManager::getSingleton().logMessage("Removing triangle");
				m_listTriangles.erase(nextEdge->triangle);

				edgesToRemove.push_back(nextEdge);
				nextEdge = nextEdge->previousHalfEdge;
				edgesToRemove.push_back(nextEdge);
				nextEdge = nextEdge->otherHalfEdge;
			}while(nextEdge != firstEdge);*/

			LogManager::getSingleton().logMessage("Removing edges " + vertexIter->toString());
			for(std::list<SurfaceEdgeIterator>::iterator edgesToRemoveIter = edgesToRemove.begin(); edgesToRemoveIter != edgesToRemove.end(); ++edgesToRemoveIter)
			{
				m_listEdges.erase(*edgesToRemoveIter);
			}

			LogManager::getSingleton().logMessage("Removing vertex " + vertexIter->toString());
			m_listVertices.erase(vertexIter);

			//Now triangulate...
			LogManager::getSingleton().logMessage("Doing triangulation");

			std::list<SurfaceVertexIterator>::iterator v0IterIter = listConnectedVertices.begin();
			std::list<SurfaceVertexIterator>::iterator v1IterIter = listConnectedVertices.begin();
			std::list<SurfaceVertexIterator>::iterator v2IterIter = listConnectedVertices.begin();
			++v1IterIter;
			++v2IterIter;
			++v2IterIter;
			while(v2IterIter != listConnectedVertices.end())
			{
				LogManager::getSingleton().logMessage("Dereferenceing");
				SurfaceVertexIterator v0Iter = *v0IterIter;
				SurfaceVertexIterator v1Iter = *v1IterIter;
				SurfaceVertexIterator v2Iter = *v2IterIter;

				LogManager::getSingleton().logMessage("Adding Triangle");
				addTriangle(*v0Iter, *v1Iter, *v2Iter);

				++v1IterIter;
				++v2IterIter;
			}

			didDecimation = true;
			break;	
		}

		for(SurfaceEdgeIterator edgeIter = m_listEdges.begin(); edgeIter != m_listEdges.end(); ++edgeIter)
		{
			if(edgeIter->isDegenerate())
			{
				LogManager::getSingleton().logMessage("Error - found degenerate edge");
			}
		}

		//LogManager::getSingleton().logMessage("Fixed = " + StringConverter::toString(fixed) + " Movable = "  + StringConverter::toString(movable));
		LogManager::getSingleton().logMessage("Done decimation");
		LogManager::getSingleton().logMessage("No of triangles at end = " + StringConverter::toString(m_listTriangles.size()));

		std::vector<SurfaceVertex> vertexDataTemp;
		std::vector<uint> indexDataTemp;
		getVertexAndIndexData(vertexDataTemp, indexDataTemp);

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

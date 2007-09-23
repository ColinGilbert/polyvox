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
		if(v0.position.x > 16)
			return;
		if(v0.position.y > 16)
			return;
		if(v1.position.x > 16)
			return;
		if(v1.position.y > 16)
			return;
		if(v2.position.x > 16)
			return;
		if(v2.position.y > 16)
			return;


		m_uTrianglesAdded++;
		m_uVerticesAdded += 3;			
		
		SurfaceVertexIterator v0Iter = findOrAddVertex(v0);
		SurfaceVertexIterator v1Iter = findOrAddVertex(v1);
		SurfaceVertexIterator v2Iter = findOrAddVertex(v2);
		

		SurfaceEdgeIterator v0v1Iter = findOrAddEdge(v0Iter,v1Iter);
		SurfaceEdgeIterator v1v2Iter = findOrAddEdge(v1Iter,v2Iter);
		SurfaceEdgeIterator v2v0Iter = findOrAddEdge(v2Iter,v0Iter);

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

	SurfaceVertexIterator SurfacePatch::findOrAddVertex(const SurfaceVertex& vertex)
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

		//Not found - return end.
		return m_listEdges.end();
	}

	SurfaceEdgeIterator SurfacePatch::findOrAddEdge(const SurfaceVertexIterator& source, const SurfaceVertexIterator& target)
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
			Vector3 v0 = (*(edgeIter->target)).position.toOgreVector3();
			LogManager::getSingleton().logMessage("");
			LogManager::getSingleton().logMessage("    " + StringConverter::toString(iterVertex->position.x) + "," + StringConverter::toString(iterVertex->position.y) + "," + StringConverter::toString(iterVertex->position.z));
			indexData.push_back(iterVertex - vertexData.begin());

			edgeIter = edgeIter->nextHalfEdge;
			iterVertex = find(vertexData.begin(), vertexData.end(), *(edgeIter->target));
			Vector3 v1 = (*(edgeIter->target)).position.toOgreVector3();
			LogManager::getSingleton().logMessage("    " + StringConverter::toString(iterVertex->position.x) + "," + StringConverter::toString(iterVertex->position.y) + "," + StringConverter::toString(iterVertex->position.z));
			indexData.push_back(iterVertex - vertexData.begin());

			edgeIter = edgeIter->nextHalfEdge;
			iterVertex = find(vertexData.begin(), vertexData.end(), *(edgeIter->target));
			Vector3 v2 = (*(edgeIter->target)).position.toOgreVector3();
			LogManager::getSingleton().logMessage("    " + StringConverter::toString(iterVertex->position.x) + "," + StringConverter::toString(iterVertex->position.y) + "," + StringConverter::toString(iterVertex->position.z));
			indexData.push_back(iterVertex - vertexData.begin());	

			Vector3 cross = (v0-v1).crossProduct(v2-v1);
			LogManager::getSingleton().logMessage("Cross = " + StringConverter::toString(cross));

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
			if((*connectedIter)->position.x != vertexIter->position.x)
			{
				allXMatch = false;
			}
			if((*connectedIter)->position.y != vertexIter->position.y)
			{
				allYMatch = false;
			}
			if((*connectedIter)->position.z != vertexIter->position.z)
			{
				allZMatch = false;
			}
			//FIXME - already normalised?
			if((*connectedIter)->normal.normalisedCopy().dotProduct(vertexIter->normal.normalisedCopy()) < 0.99)
			{
				return false;				
			}
		}

		if(isEdge)
		{
			SurfaceVertexIterator firstExtreme = *(listConnectedIter.begin());
			SurfaceVertexIterator secondExtreme = *(--listConnectedIter.end());

			bool edgeXMatch = (firstExtreme->position.x == vertexIter->position.x) && (secondExtreme->position.x == vertexIter->position.x);
			bool edgeYMatch = (firstExtreme->position.y == vertexIter->position.y) && (secondExtreme->position.y == vertexIter->position.y);
			bool edgeZMatch = (firstExtreme->position.z == vertexIter->position.z) && (secondExtreme->position.z == vertexIter->position.z);

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

			isEdge = true;

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

	bool SurfacePatch::decimateOneVertex(void)
	{
		bool didDecimation = false;
		LogManager::getSingleton().logMessage("\n\nPerforming decimation");
		LogManager::getSingleton().logMessage("No of triangles at start = " + StringConverter::toString(m_listTriangles.size()));
		//int fixed = 0;
		//int movable = 0;
		for(SurfaceVertexIterator vertexIter = m_listVertices.begin(); vertexIter != m_listVertices.end(); ++vertexIter)
		{
			LogManager::getSingleton().logMessage("Examining vertex " + vertexIter->toString());			

			bool isEdge;
			std::list<SurfaceVertexIterator> listConnectedVertices = findConnectedVertices(vertexIter,isEdge);		
			listConnectedVertices.remove(vertexIter);
			listConnectedVertices.unique();
			
			LogManager::getSingleton().logMessage("No of connected vertices = " + StringConverter::toString(listConnectedVertices.size()));
			for(std::list<SurfaceVertexIterator>::iterator iter = listConnectedVertices.begin(); iter != listConnectedVertices.end(); ++iter)
			{
				LogManager::getSingleton().logMessage("    Connected vertex = " + (*iter)->toString());
			}

			if(canRemoveVertexFrom(vertexIter, listConnectedVertices, isEdge) == false)
			{
				continue;
			}

			if(isPolygonConvex(listConnectedVertices) == false)
			{
				return false;
			}

			LogManager::getSingleton().logMessage("Vertex can be removed");

			for(std::list<SurfaceVertexIterator>::iterator iter = listConnectedVertices.begin(); iter != listConnectedVertices.end(); ++iter)
			{
				SurfaceEdgeIterator edgeToDelete = findEdge(vertexIter, *iter);
				SurfaceEdgeIterator otherEdgeToDelete = edgeToDelete->otherHalfEdge;

				if(edgeToDelete->nextHalfEdge != edgeToDelete->otherHalfEdge)
				{
					m_listTriangles.erase(edgeToDelete->triangle);
				}

				m_listEdges.erase(edgeToDelete);
				m_listEdges.erase(otherEdgeToDelete);
			}

			LogManager::getSingleton().logMessage("Removing vertex " + vertexIter->toString());
			m_listVertices.erase(vertexIter);

			//Now triangulate...
			LogManager::getSingleton().logMessage("Doing triangulation");
			triangulate(listConnectedVertices);

			didDecimation = true;
			break;	
		}

		//LogManager::getSingleton().logMessage("Fixed = " + StringConverter::toString(fixed) + " Movable = "  + StringConverter::toString(movable));
		LogManager::getSingleton().logMessage("Done decimation");
		LogManager::getSingleton().logMessage("No of triangles at end = " + StringConverter::toString(m_listTriangles.size()));

		/*std::vector<SurfaceVertex> vertexDataTemp;
		std::vector<uint> indexDataTemp;
		getVertexAndIndexData(vertexDataTemp, indexDataTemp);*/

		return didDecimation;
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

	bool SurfacePatch::isPolygonConvex(std::list<SurfaceVertexIterator> listVertices)
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

			Vector3 v1tov0(v0Iter->position.toOgreVector3() -v1Iter->position.toOgreVector3());
			Vector3 v1tov2(v2Iter->position.toOgreVector3() -v1Iter->position.toOgreVector3());
			Vector3 cross = (v1tov0).crossProduct(v1tov2);
			LogManager::getSingleton().logMessage("Cross = " + StringConverter::toString(cross));

			if(cross.z > 0.0)
			{
				return false;
			}

			++v1IterIter;
			++v2IterIter;
		}

		return true;
	}

	/*bool SurfacePatch::isPolygonConvex(std::list<SurfaceVertexIterator> listVertices)
	{
		std::list<SurfaceVertexIterator>::iterator v0IterIter = listVertices.begin();
		std::list<SurfaceVertexIterator>::iterator v1IterIter = listVertices.begin();
		++v1IterIter;

		while(v1IterIter != listVertices.end())
		{
			SurfaceVertexIterator v0Iter = *v0IterIter;
			SurfaceVertexIterator v1Iter = *v1IterIter;

			Vector3 currentEdge = v1Iter->position.toOgreVector3() - v0Iter->position.toOgreVector3();
			for(std::list<SurfaceVertexIterator>::iterator currentVertex = listVertices.begin(); currentVertex != listVertices.end(); ++currentVertex)
			{
				if((currentVertex == v0IterIter) || (currentVertex == v1IterIter))
				{
					continue;
				}
				Vector3 v0ToCurrent = (*currentVertex)->position.toOgreVector3() - v0Iter->position.toOgreVector3();
				if(currentEdge.)
			}

			++v0IterIter;
			++v1IterIter;
		}

		return true;
	}*/

	/*bool SurfacePatch::isPolygonConvex(std::list<SurfaceVertexIterator> listVertices)
	{
		if(listVertices.size() < 3)
		{
			LogManager::getSingleton().logMessage("Not enough vertices!");
			return false;
		}

		std::list<SurfaceVertexIterator>::iterator v0IterIter = listVertices.begin();
		std::list<SurfaceVertexIterator>::iterator v1IterIter = listVertices.begin();
		std::list<SurfaceVertexIterator>::iterator v2IterIter = listVertices.begin();
		++v1IterIter;
		++v2IterIter;
		++v2IterIter;

		SurfaceVertexIterator v0Iter = *v0IterIter;
		SurfaceVertexIterator v1Iter = *v1IterIter;
		SurfaceVertexIterator v2Iter = *v2IterIter;

		Vector3 v0tov1 = v1Iter->position.toOgreVector3() - v0Iter->position.toOgreVector3();
		Vector3 v1tov2 = v2Iter->position.toOgreVector3() - v1Iter->position.toOgreVector3();

		Vector3 firstCrossProduct = v0tov1.crossProduct(v1tov2);
		firstCrossProduct.normalise();

		++v0IterIter;
		++v1IterIter;
		++v2IterIter;


		while(v2IterIter != listVertices.end())
		{
			v0Iter = *v0IterIter;
			v1Iter = *v1IterIter;
			v2Iter = *v2IterIter;

			v0tov1 = v1Iter->position.toOgreVector3() - v0Iter->position.toOgreVector3();
			v1tov2 = v2Iter->position.toOgreVector3() - v1Iter->position.toOgreVector3();

			Vector3 currentCrossProduct = v0tov1.crossProduct(v1tov2);
			currentCrossProduct.normalise();
			
			if(firstCrossProduct.dotProduct(currentCrossProduct) < 0.99)
			{
				return false;
			}

			++v0IterIter;
			++v1IterIter;
			++v2IterIter;
		}

		return true;
	}*/
}

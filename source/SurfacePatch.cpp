#include "SurfacePatch.h"
#include "Constants.h"

#include "SurfaceVertex.h"
#include "SurfaceTriangle.h"

#include "OgreLogManager.h"
#include "OgreStringConverter.h"

#include <algorithm>

namespace Ogre
{
	SurfacePatch::SurfacePatch()
	{
		m_setVertices.clear();
		m_setTriangles.clear();

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
		m_uTrianglesAdded++;

		m_uVerticesAdded += 3;		

		SurfaceTriangle triangle;

		triangle.v0 = m_setVertices.insert(v0).first;		
		triangle.v1 = m_setVertices.insert(v1).first;
		triangle.v2 = m_setVertices.insert(v2).first;	

		SurfaceTriangleIterator iterTriangle = m_setTriangles.insert(triangle).first;

		triangle.v0->listTrianglesUsingThisVertex.push_back(iterTriangle);
		triangle.v1->listTrianglesUsingThisVertex.push_back(iterTriangle);
		triangle.v2->listTrianglesUsingThisVertex.push_back(iterTriangle);
	}

	void SurfacePatch::computeNormalsFromVolume(VolumeIterator volIter)
	{
		//LogManager::getSingleton().logMessage("In SurfacePatch::computeNormalsFromVolume");
		for(SurfaceVertexIterator vertexIter = m_setVertices.begin(); vertexIter != m_setVertices.end(); ++vertexIter)
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
	}

	void SurfacePatch::decimate(void)
	{
		//Build the lists of connected vertices
		for(SurfaceVertexIterator vertexIter = m_setVertices.begin(); vertexIter != m_setVertices.end(); ++vertexIter)
		{
			for(std::list<SurfaceTriangleIterator>::iterator triangleIter = vertexIter->listTrianglesUsingThisVertex.begin(); triangleIter != vertexIter->listTrianglesUsingThisVertex.end(); ++triangleIter) 
			{
				if(find(vertexIter->listConnectedVertices.begin(),vertexIter->listConnectedVertices.end(),(*triangleIter)->v0) == vertexIter->listConnectedVertices.end())
					vertexIter->listConnectedVertices.push_back((*triangleIter)->v0);
				if(find(vertexIter->listConnectedVertices.begin(),vertexIter->listConnectedVertices.end(),(*triangleIter)->v1) == vertexIter->listConnectedVertices.end())
					vertexIter->listConnectedVertices.push_back((*triangleIter)->v1);
				if(find(vertexIter->listConnectedVertices.begin(),vertexIter->listConnectedVertices.end(),(*triangleIter)->v2) == vertexIter->listConnectedVertices.end())
					vertexIter->listConnectedVertices.push_back((*triangleIter)->v2);
			}
		}

		//do the vertex merging
		for(SurfaceVertexIterator vertexIter = m_setVertices.begin(); vertexIter != m_setVertices.end(); ++vertexIter)
		{
			if(vertexIter->alpha < 0.9)
				continue;

			if(true/*verticesArePlanar(vertexIter)*/)
			{
				//Find a vertex to merge with
				SurfaceVertexIterator vertexToMergeWith = (*(vertexIter->listTrianglesUsingThisVertex.begin()))->v0;
				if(vertexIter == vertexToMergeWith)
				{
					vertexToMergeWith = (*(vertexIter->listTrianglesUsingThisVertex.begin()))->v1;
				}

				//Change triangles to use new vertex
				for(SurfaceTriangleIterator iterTriangles = m_setTriangles.begin(); iterTriangles != m_setTriangles.end(); ++iterTriangles)
				{
					if(iterTriangles->v0 == vertexIter)
						iterTriangles->v0 = vertexToMergeWith;
					if(iterTriangles->v1 == vertexIter)
						iterTriangles->v1 = vertexToMergeWith;
					if(iterTriangles->v2 == vertexIter)
						iterTriangles->v2 = vertexToMergeWith;
				}

				//Change connected vertices to use new vertex
			}
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
}

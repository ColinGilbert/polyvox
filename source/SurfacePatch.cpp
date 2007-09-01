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

		m_listTriangles.push_back(triangle);

		triangle.v0->listTrianglesUsingThisVertex.push_back(m_listTriangles.end());
		triangle.v1->listTrianglesUsingThisVertex.push_back(m_listTriangles.end());
		triangle.v2->listTrianglesUsingThisVertex.push_back(m_listTriangles.end());
	}

	void SurfacePatch::computeNormalsFromVolume(uint regionX, uint regionY, uint regionZ, VolumeIterator volIter)
	{
		//LogManager::getSingleton().logMessage("In SurfacePatch::computeNormalsFromVolume");
		for(std::set<SurfaceVertex>::iterator vertexIter = m_setVertices.begin(); vertexIter != m_setVertices.end(); ++vertexIter)
		{
			//LogManager::getSingleton().logMessage("In Loop");
			const float posX = vertexIter->position.x/2.0f + static_cast<float>(regionX * OGRE_REGION_SIDE_LENGTH);
			const float posY = vertexIter->position.y/2.0f + static_cast<float>(regionY * OGRE_REGION_SIDE_LENGTH);
			const float posZ = vertexIter->position.z/2.0f + static_cast<float>(regionZ * OGRE_REGION_SIDE_LENGTH);

			const uint floorX = static_cast<uint>(vertexIter->position.x/2.0f) + regionX * OGRE_REGION_SIDE_LENGTH;
			const uint floorY = static_cast<uint>(vertexIter->position.y/2.0f) + regionY * OGRE_REGION_SIDE_LENGTH;
			const uint floorZ = static_cast<uint>(vertexIter->position.z/2.0f) + regionZ * OGRE_REGION_SIDE_LENGTH;

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

		for(std::list<SurfaceTriangle>::iterator iterTriangles = m_listTriangles.begin(); iterTriangles != m_listTriangles.end(); ++iterTriangles)
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
}

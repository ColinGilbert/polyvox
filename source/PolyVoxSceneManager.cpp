/******************************************************************************
This file is part of a voxel plugin for OGRE
Copyright (C) 2006  David Williams

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
******************************************************************************/

#include "MarchingCubesTables.h"
#include "MaterialMapManager.h"
#include "SurfaceVertex.h"
#include "SurfaceEdge.h"
#include "PolyVoxSceneManager.h"
#include "VolumeIterator.h"
#include "VolumeManager.h"

#include "OgreManualObject.h"
#include "OgreStringConverter.h"
#include "OgreLogManager.h"

//Tempory...
#include "OgreMeshManager.h"
#include "OgreEntity.h"

namespace Ogre
{

	//////////////////////////////////////////////////////////////////////////
	// PolyVoxSceneManagerFactory
	//////////////////////////////////////////////////////////////////////////
	const String PolyVoxSceneManagerFactory::FACTORY_TYPE_NAME = "PolyVoxSceneManager";

	SceneManager* PolyVoxSceneManagerFactory::createInstance(
		const String& instanceName)
	{
		return new PolyVoxSceneManager(instanceName);
	}

	void PolyVoxSceneManagerFactory::destroyInstance(SceneManager* instance)
	{
		delete instance;
	}

	void PolyVoxSceneManagerFactory::initMetaData(void) const
	{
		mMetaData.typeName = FACTORY_TYPE_NAME;
		mMetaData.description = "A voxel based scene manager";
		mMetaData.sceneTypeMask = ST_GENERIC;
		mMetaData.worldGeometrySupported = false;
	}

	//////////////////////////////////////////////////////////////////////////
	// PolyVoxSceneManager
	//////////////////////////////////////////////////////////////////////////
	PolyVoxSceneManager::PolyVoxSceneManager(const String& name)
		: SceneManager(name)
		,volumeData(0)
		,useNormalSmoothing(false)
		,normalSmoothingFilterSize(1)
		,m_normalGenerationMethod(SOBEL)
		,m_bHaveGeneratedMeshes(false)
	{	
		/*for(uint regionZ = 0; regionZ < OGRE_VOLUME_SIDE_LENGTH_IN_REGIONS; ++regionZ)
		{		
			for(uint regionY = 0; regionY < OGRE_VOLUME_SIDE_LENGTH_IN_REGIONS; ++regionY)
			{
				for(uint regionX = 0; regionX < OGRE_VOLUME_SIDE_LENGTH_IN_REGIONS; ++regionX)
				{
					sceneNodes[regionX][regionY][regionZ] = 0; 
				}
			}
		}*/
		sceneNodes.clear();
	}

	PolyVoxSceneManager::~PolyVoxSceneManager()
	{
	}

	const String& PolyVoxSceneManager::getTypeName(void) const
	{
		return PolyVoxSceneManagerFactory::FACTORY_TYPE_NAME;
	}

	bool PolyVoxSceneManager::loadScene(const String& filename)
	{
		/*volumeData = new Volume;
		
		if(filename.empty())
		{
			generateLevelVolume();
		}
		else
		{
			volumeData->load(filename);
		}	*/

		//volumeData = VolumeManager::getSingletonPtr()->getByName(filename + ".volume");
		volumeData = VolumeManager::getSingletonPtr()->load(filename + ".volume", "General");
		if(volumeData.isNull())
		{
			LogManager::getSingleton().logMessage("Generating default volume");
			generateLevelVolume();
			LogManager::getSingleton().logMessage("Done generating default volume");
		}

		volumeData->tidy();

		//Load material map
		materialMap = MaterialMapManager::getSingletonPtr()->load(filename + ".materialmap", "General");

		LogManager::getSingleton().logMessage("HERE");
		VolumeIterator volIter1(*volumeData);
		for(uint z = 0; z < OGRE_VOLUME_SIDE_LENGTH; z += 10)
		{
			for(uint y = 0; y < OGRE_VOLUME_SIDE_LENGTH; y += 10)
			{
				for(uint x = 0; x < OGRE_VOLUME_SIDE_LENGTH; x += 10)
				{
					volIter1.setPosition(x,y,z);
					uchar value = volIter1.getVoxel();
					//LogManager::getSingleton().logMessage("Value is " + StringConverter::toString(int(value)));
				}
			}
		}
		LogManager::getSingleton().logMessage("DONE");

		for(uint blockZ = 0; blockZ < OGRE_VOLUME_SIDE_LENGTH_IN_REGIONS; ++blockZ)
		{
			for(uint blockY = 0; blockY < OGRE_VOLUME_SIDE_LENGTH_IN_REGIONS; ++blockY)
			{
				for(uint blockX = 0; blockX < OGRE_VOLUME_SIDE_LENGTH_IN_REGIONS; ++blockX)
				{					
					surfaceUpToDate[blockX][blockY][blockZ] = false;

					for(std::map<uchar,SurfacePatchRenderable*>::iterator iterSurfaces = m_mapSurfaces[blockX][blockY][blockZ].begin(); iterSurfaces != m_mapSurfaces[blockX][blockY][blockZ].end(); ++iterSurfaces)
					{
						delete iterSurfaces->second;
					}
					m_mapSurfaces[blockX][blockY][blockZ].clear();
				}
			}
		}

		getRootSceneNode()->removeAndDestroyAllChildren();
		

		//Create surrounding box
		//FIXME - should do this using indices
		/*ManualObject* metalBox = createManualObject("MetalBox");

		metalBox->begin("Examples/BumpyMetal",RenderOperation::OT_TRIANGLE_STRIP);
		metalBox->position(0,0,0);
		metalBox->textureCoord(0,0);
		metalBox->normal(0,0,1);
		metalBox->position(512,0,0);
		metalBox->textureCoord(1,0);
		metalBox->normal(0,0,1);
		metalBox->position(0,512,0);
		metalBox->textureCoord(0,1);
		metalBox->normal(0,0,1);
		metalBox->position(512,512,0);
		metalBox->textureCoord(1,1);
		metalBox->normal(0,0,1);
		metalBox->end();*/

		/*metalBox->begin("Examples/BumpyMetal",RenderOperation::OT_TRIANGLE_STRIP);
		metalBox->position(0,0,0);
		metalBox->textureCoord(0,0);
		metalBox->normal(0,1,0);
		metalBox->position(0,0,512);
		metalBox->textureCoord(0,1);
		metalBox->normal(0,1,0);
		metalBox->position(512,0,0);
		metalBox->textureCoord(1,0);
		metalBox->normal(0,1,0);
		metalBox->position(512,0,512);
		metalBox->textureCoord(1,1);
		metalBox->normal(0,1,0);
		metalBox->end();*/

		/*metalBox->begin("Examples/BumpyMetal",RenderOperation::OT_TRIANGLE_STRIP);
		metalBox->position(0,0,0);
		metalBox->textureCoord(0,0);
		metalBox->normal(1,0,0);
		metalBox->position(0,512,0);
		metalBox->textureCoord(1,0);
		metalBox->normal(1,0,0);
		metalBox->position(0,0,512);
		metalBox->textureCoord(0,1);
		metalBox->normal(1,0,0);
		metalBox->position(0,512,512);
		metalBox->textureCoord(1,1);
		metalBox->normal(1,0,0);
		metalBox->end();

		metalBox->begin("Examples/BumpyMetal",RenderOperation::OT_TRIANGLE_STRIP);
		metalBox->position(0,0,512);
		metalBox->textureCoord(0,0);	
		metalBox->normal(0,0,-1);
		metalBox->position(0,512,512);
		metalBox->textureCoord(0,1);
		metalBox->normal(0,0,-1);
		metalBox->position(512,0,512);
		metalBox->textureCoord(1,0);
		metalBox->normal(0,0,-1);
		metalBox->position(512,512,512);
		metalBox->textureCoord(1,1);
		metalBox->normal(0,0,-1);
		metalBox->end();

		metalBox->begin("Examples/BumpyMetal",RenderOperation::OT_TRIANGLE_STRIP);
		metalBox->position(0,512,0);
		metalBox->textureCoord(0,0);	
		metalBox->normal(0,-1,0);
		metalBox->position(512,512,0);
		metalBox->textureCoord(1,0);
		metalBox->normal(0,-1,0);
		metalBox->position(0,512,512);
		metalBox->textureCoord(0,1);
		metalBox->normal(0,-1,0);
		metalBox->position(512,512,512);
		metalBox->textureCoord(1,1);
		metalBox->normal(0,-1,0);
		metalBox->end();

		metalBox->begin("Examples/BumpyMetal",RenderOperation::OT_TRIANGLE_STRIP);
		metalBox->position(512,0,0);
		metalBox->textureCoord(0,0);
		metalBox->normal(-1,0,0);
		metalBox->position(512,0,512);
		metalBox->textureCoord(0,1);
		metalBox->normal(-1,0,0);
		metalBox->position(512,512,0);
		metalBox->textureCoord(1,0);
		metalBox->normal(-1,0,0);
		metalBox->position(512,512,512);
		metalBox->textureCoord(1,1);
		metalBox->normal(-1,0,0);
		metalBox->end();

		SceneNode* boxSceneNode = getRootSceneNode()->createChildSceneNode();
		boxSceneNode->attachObject(metalBox);*/

		/*ManualObject* water = createManualObject("WaterPlane");

		water->begin("DavidWater",RenderOperation::OT_TRIANGLE_STRIP);
		water->position(-1024,100,-1024);
		water->textureCoord(0,0);
		water->normal(0,1,0);
		water->position(-1024,100,1024);
		water->textureCoord(0,1);
		water->normal(0,1,0);
		water->position(1024,100,-1024);
		water->textureCoord(1,0);
		water->normal(0,1,0);
		water->position(1024,100,1024);
		water->textureCoord(1,1);
		water->normal(0,1,0);

		water->end();

		SceneNode* waterSceneNode = getRootSceneNode()->createChildSceneNode();
		waterSceneNode->attachObject(water);*/

	return true;
	}

	void PolyVoxSceneManager::_findVisibleObjects(Camera* cam, VisibleObjectsBoundsInfo *  visibleBounds,  bool onlyShadowCasters)
	{
		if(!volumeData.isNull())
		{
			//Do burning
			//FIXME - can make this more efficient
			ulong uNoOfVoxelsToBurn = m_queueVoxelsToBurn.size();
			uNoOfVoxelsToBurn = (std::min)(uNoOfVoxelsToBurn, OGRE_MAX_VOXELS_TO_BURN_PER_FRAME);
			VolumeIterator volIter(*volumeData);
			for(ulong ct = 0; ct < uNoOfVoxelsToBurn; ++ct)
			{
				UIntVector3 pos = m_queueVoxelsToBurn.front();
				m_queueVoxelsToBurn.pop();
				if((pos.x >= 0) && (pos.y >= 0) && (pos.z >= 0) && (pos.x < OGRE_VOLUME_SIDE_LENGTH) && (pos.y < OGRE_VOLUME_SIDE_LENGTH) && (pos.z < OGRE_VOLUME_SIDE_LENGTH))
				{
					volIter.setPosition(pos.x,pos.y,pos.z);
					if(volIter.getVoxel() != 0)
					{
						volIter.setVoxelAt(pos.x,pos.y,pos.z,0);
						markVoxelChanged(pos.x,pos.y,pos.z);

						m_queueVoxelsToBurn.push(UIntVector3(pos.x  ,pos.y  ,pos.z-1));
						m_queueVoxelsToBurn.push(UIntVector3(pos.x  ,pos.y  ,pos.z+1));
						m_queueVoxelsToBurn.push(UIntVector3(pos.x  ,pos.y-1,pos.z  ));
						m_queueVoxelsToBurn.push(UIntVector3(pos.x  ,pos.y+1,pos.z  ));
						m_queueVoxelsToBurn.push(UIntVector3(pos.x-1,pos.y  ,pos.z  ));
						m_queueVoxelsToBurn.push(UIntVector3(pos.x+1,pos.y  ,pos.z  ));
					}
				}			

				//LogManager::getSingleton().logMessage("Burnt voxel at x = " + StringConverter::toString(pos.x) + " y = " + StringConverter::toString(pos.y) + " z = " + StringConverter::toString(pos.z));

				/*if((pos.x > 0) && (pos.y > 0) && (pos.z > 0) && (pos.x < OGRE_VOLUME_SIDE_LENGTH-1) && (pos.y < OGRE_VOLUME_SIDE_LENGTH-1) && (pos.z < OGRE_VOLUME_SIDE_LENGTH-1))
				{*/
					//In this case we can definatly move in all direction
					
				/*}
				else
				{
					//In this case we need to check more carefully.
				}*/
			}

			//Regenerate meshes.
			for(uint regionZ = 0; regionZ < OGRE_VOLUME_SIDE_LENGTH_IN_REGIONS; ++regionZ)
			{		
				//LogManager::getSingleton().logMessage("regionZ = " + StringConverter::toString(regionZ));
				for(uint regionY = 0; regionY < OGRE_VOLUME_SIDE_LENGTH_IN_REGIONS; ++regionY)
				{
					//LogManager::getSingleton().logMessage("regionY = " + StringConverter::toString(regionY));
					for(uint regionX = 0; regionX < OGRE_VOLUME_SIDE_LENGTH_IN_REGIONS; ++regionX)
					{
						//LogManager::getSingleton().logMessage("regionX = " + StringConverter::toString(regionX));
						if(surfaceUpToDate[regionX][regionY][regionZ] == false)
						{
							//Generate the surface
							//std::vector< std::vector<SurfaceVertex> > vertexData;
							//std::vector< std::vector<SurfaceTriangle> > indexData;
							std::map<uchar, SurfacePatch> mapSurfacePatch = generateMeshDataForRegion(regionX,regionY,regionZ);

							//If a SceneNode doesn't exist in this position then create one.
							std::map<UIntVector3, SceneNode*>::iterator iterSceneNode = sceneNodes.find(UIntVector3(regionX,regionY,regionZ));
							SceneNode* sceneNode;
							if(iterSceneNode == sceneNodes.end())
							{
								sceneNode = getRootSceneNode()->createChildSceneNode(Vector3(regionX*OGRE_REGION_SIDE_LENGTH,regionY*OGRE_REGION_SIDE_LENGTH,regionZ*OGRE_REGION_SIDE_LENGTH));
								sceneNodes.insert(std::make_pair(UIntVector3(regionX,regionY,regionZ),sceneNode));
							}
							else
							{
								sceneNode = iterSceneNode->second;
								sceneNode->detachAllObjects();
							}

							//For each surface attach it to the scene node.
							//for(uint meshCt = 1; meshCt < 256; ++meshCt)
							for(std::map<uchar, SurfacePatch>::iterator iterSurfacePatch = mapSurfacePatch.begin(); iterSurfacePatch != mapSurfacePatch.end(); ++iterSurfacePatch)
							{
								std::vector<SurfaceVertex> vertexData;
								std::vector<uint> indexData;
								iterSurfacePatch->second.getVertexAndIndexData(vertexData, indexData);
								
								std::map<uchar,SurfacePatchRenderable*>::iterator iterSurface = m_mapSurfaces[regionX][regionY][regionZ].find(iterSurfacePatch->first);
								if(iterSurface == m_mapSurfaces[regionX][regionY][regionZ].end())
								{
									//We have to create the surface
									SurfacePatchRenderable* surface = new SurfacePatchRenderable(materialMap->getMaterialAtIndex(iterSurfacePatch->first));
									//surface->setGeometry(vertexData[meshCt],indexData[meshCt]);
									surface->setGeometry(vertexData, indexData);

									m_mapSurfaces[regionX][regionY][regionZ].insert(std::make_pair(iterSurfacePatch->first,surface));

									sceneNode->attachObject(surface);
								}
								else
								{
									//We just update the existing surface
									//iterSurface->second->setGeometry(vertexData[meshCt],indexData[meshCt]);
									iterSurface->second->setGeometry(vertexData, indexData);
									sceneNode->attachObject(iterSurface->second);
								}
							}
							//sceneNode->showBoundingBox(true);
							surfaceUpToDate[regionX][regionY][regionZ] = true;
						}
					}
				}
			}
		}
		//showBoundingBoxes(true);
		//Now call the base class to do the actual visibility determination...
		SceneManager::_findVisibleObjects(cam, visibleBounds, onlyShadowCasters);
	}

	void PolyVoxSceneManager::setAllUpToDateFalse(void)
	{
		for(uint blockZ = 0; blockZ < OGRE_VOLUME_SIDE_LENGTH_IN_REGIONS; ++blockZ)
		{
			for(uint blockY = 0; blockY < OGRE_VOLUME_SIDE_LENGTH_IN_REGIONS; ++blockY)
			{
				for(uint blockX = 0; blockX < OGRE_VOLUME_SIDE_LENGTH_IN_REGIONS; ++blockX)
				{
					surfaceUpToDate[blockX][blockY][blockZ] = false;
				}
			}
		}
	}

	void PolyVoxSceneManager::createSphereAt(Vector3 centre, Real radius, uchar value, bool painting)
	{
		int firstX = static_cast<int>(std::floor(centre.x - radius));
		int firstY = static_cast<int>(std::floor(centre.y - radius));
		int firstZ = static_cast<int>(std::floor(centre.z - radius));

		int lastX = static_cast<int>(std::ceil(centre.x + radius));
		int lastY = static_cast<int>(std::ceil(centre.y + radius));
		int lastZ = static_cast<int>(std::ceil(centre.z + radius));

		Real radiusSquared = radius * radius;

		//Check bounds
		firstX = std::max(firstX,0);
		firstY = std::max(firstY,0);
		firstZ = std::max(firstZ,0);

		lastX = std::min(lastX,int(OGRE_VOLUME_SIDE_LENGTH-1));
		lastY = std::min(lastY,int(OGRE_VOLUME_SIDE_LENGTH-1));
		lastZ = std::min(lastZ,int(OGRE_VOLUME_SIDE_LENGTH-1));

		VolumeIterator volIter(*volumeData);
		volIter.setValidRegion(firstX,firstY,firstZ,lastX,lastY,lastZ);
		volIter.setPosition(firstX,firstY,firstZ);
		while(volIter.isValidForRegion())
		{
			//if((volIter.getPosX()*volIter.getPosX()+volIter.getPosY()*volIter.getPosY()+volIter.getPosZ()*volIter.getPosZ()) < radiusSquared)
			if((centre - Vector3(volIter.getPosX(),volIter.getPosY(),volIter.getPosZ())).squaredLength() <= radiusSquared)
			{
				if(painting)
				{
					if(volIter.getVoxel() != 0)
					{
						volIter.setVoxel(value);
						//volIter.setVoxelAt(volIter.getPosX(),volIter.getPosY(),volIter.getPosZ(),value);
					}
				}
				else
				{
					volIter.setVoxel(value);
					//volIter.setVoxelAt(volIter.getPosX(),volIter.getPosY(),volIter.getPosZ(),value);
				}
				//markVoxelChanged(volIter.getPosX(),volIter.getPosY(),volIter.getPosZ()); //FIXME - create a version of this function to mark larger regions at a time.
			}
			volIter.moveForwardInRegion();
		}
		markRegionChanged(firstX,firstY,firstZ,lastX,lastY,lastZ);
	}

	void PolyVoxSceneManager::generateLevelVolume(void)
	{
		volumeData = VolumePtr(new Volume);
		VolumeIterator volIter(*volumeData);
		for(uint z = 0; z < OGRE_VOLUME_SIDE_LENGTH; ++z)
		{
			for(uint y = 0; y < OGRE_VOLUME_SIDE_LENGTH; ++y)
			{
				for(uint x = 0; x < OGRE_VOLUME_SIDE_LENGTH; ++x)
				{
					if((x/16+y/16+z/16)%2 == 0)
						volIter.setVoxelAt(x,y,z,4);
					else
						volIter.setVoxelAt(x,y,z,8);
				}
			}
		}		

		for(uint z = 0; z < OGRE_VOLUME_SIDE_LENGTH; ++z)
		{
			for(uint y = 0; y < OGRE_VOLUME_SIDE_LENGTH; ++y)
			{
				for(uint x = 0; x < OGRE_VOLUME_SIDE_LENGTH; ++x)
				{
					if(
						(z<62)||
						(z>193)||
						(y<78)||
						(y>177)||
						(x<30)||
						(x>225)
						)
					{
						volIter.setVoxelAt(x,y,z,2);
					}
				}
			}
		}		

		//Rooms
		Vector3 centre(128,128,128);
		Vector3 v3dSize(192,96,128);

		uint uHalfX = static_cast<uint>(v3dSize.x / 2);
		uint uHalfY = static_cast<uint>(v3dSize.y / 2);
		uint uHalfZ = static_cast<uint>(v3dSize.z / 2);

		for(uint z = static_cast<uint>(centre.z) - uHalfZ; z < static_cast<uint>(centre.z) + uHalfZ; z++)
		{
			for(uint y = static_cast<uint>(centre.y) - uHalfY; y < static_cast<uint>(centre.y) + uHalfY; y++)
			{
				for(uint x = static_cast<uint>(centre.x) - uHalfX; x < static_cast<uint>(centre.x) + uHalfX; x++)
				{
					volIter.setVoxelAt(x,y,z,0);
				}
			}
		}

		for(uint z = 0; z < OGRE_VOLUME_SIDE_LENGTH; ++z)
		{
			for(uint y = 0; y < OGRE_VOLUME_SIDE_LENGTH; ++y)
			{
				for(uint x = 0; x < OGRE_VOLUME_SIDE_LENGTH; ++x)
				{
					if(
						(x%64 < 8) &&
						(y < 128) &&
						(z>=62)&&
						(z<=193)&&
						(y>=78)&&
						(y<=177)&&
						(x>=30)&&
						(x<=225)
						)
					{
						volIter.setVoxelAt(x,y,z,1);
					}
				}
			}
		}
	}

	std::map<uchar, SurfacePatch> PolyVoxSceneManager::generateMeshDataForRegion(const uint regionX, const uint regionY, const uint regionZ) const
	{	
		//LogManager::getSingleton().logMessage("Generating Mesh Data");
		/*LogManager::getSingleton().logMessage("HERE");
		VolumeIterator volIter1(volumeData);
		for(uint z = 0; z < OGRE_VOLUME_SIDE_LENGTH; z += 10)
		{
			for(uint y = 0; y < OGRE_VOLUME_SIDE_LENGTH; y += 10)
			{
				for(uint x = 0; x < OGRE_VOLUME_SIDE_LENGTH; x += 10)
				{
					volIter1.setPosition(x,y,z);
					uchar value = volIter1.getVoxel();
					LogManager::getSingleton().logMessage("Value is " + StringConverter::toString(int(value)));
				}
			}
		}
		LogManager::getSingleton().logMessage("DONE");*/
		//The vertex and index data
		std::vector< std::vector<SurfaceVertex> > vertexData;
		std::vector< std::vector<SurfaceTriangle> > indexData;

		vertexData.resize(256);
		indexData.resize(256);

		std::map<uchar, SurfacePatch> result;

		//Used later to check if vertex has already been added
		//long int vertexIndices[OGRE_REGION_SIDE_LENGTH*2+1][OGRE_REGION_SIDE_LENGTH*2+1][OGRE_REGION_SIDE_LENGTH*2+1][10];
		//uchar materialAtPosition[OGRE_REGION_SIDE_LENGTH*2+1][OGRE_REGION_SIDE_LENGTH*2+1][OGRE_REGION_SIDE_LENGTH*2+1]; //FIXME - do we really need this? Can't we just get it from the volume...
		//bool isVertexSharedBetweenMaterials[OGRE_REGION_SIDE_LENGTH*2+1][OGRE_REGION_SIDE_LENGTH*2+1][OGRE_REGION_SIDE_LENGTH*2+1];
		//memset(vertexIndices,0xFF,sizeof(vertexIndices)); //0xFF is -1 as two's complement - this may not be portable...
		//memset(materialAtPosition,0x00,sizeof(materialAtPosition));
		//memset(isVertexSharedBetweenMaterials,0x00,sizeof(isVertexSharedBetweenMaterials));

		//tracks triangles using each vertex
		//std::set<uint> trianglesUsingVertex[OGRE_REGION_SIDE_LENGTH*2+1][OGRE_REGION_SIDE_LENGTH*2+1][OGRE_REGION_SIDE_LENGTH*2+1];
		//FIXME - is this loop necessary or is a default set empty?
		/*for(uint z = 0; z < OGRE_REGION_SIDE_LENGTH*2+1; z += 10)
		{
			for(uint y = 0; y < OGRE_REGION_SIDE_LENGTH*2+1; y += 10)
			{
				for(uint x = 0; x < OGRE_REGION_SIDE_LENGTH*2+1; x += 10)
				{
					trianglesUsingVertex[x][y][z].clear();
				}
			}
		}*/

		//First and last voxels in the region
		const uint firstX = regionX * OGRE_REGION_SIDE_LENGTH;
		const uint firstY = regionY * OGRE_REGION_SIDE_LENGTH;
		const uint firstZ = regionZ * OGRE_REGION_SIDE_LENGTH;
		const uint lastX = (std::min)(firstX + OGRE_REGION_SIDE_LENGTH-1,static_cast<uint>(OGRE_VOLUME_SIDE_LENGTH-2));
		const uint lastY = (std::min)(firstY + OGRE_REGION_SIDE_LENGTH-1,static_cast<uint>(OGRE_VOLUME_SIDE_LENGTH-2));
		const uint lastZ = (std::min)(firstZ + OGRE_REGION_SIDE_LENGTH-1,static_cast<uint>(OGRE_VOLUME_SIDE_LENGTH-2));

		//Offset from lower block corner
		const UIntVector3 offset(firstX*2,firstY*2,firstZ*2);

		UIntVector3 vertlist[12];
		uchar vertMaterials[12];
		VolumeIterator volIter(*volumeData);
		volIter.setValidRegion(firstX,firstY,firstZ,lastX,lastY,lastZ);

		//////////////////////////////////////////////////////////////////////////
		//Get mesh data
		//////////////////////////////////////////////////////////////////////////

		//Iterate over each cell in the region
		for(volIter.setPosition(firstX,firstY,firstZ);volIter.isValidForRegion();volIter.moveForwardInRegion())
		{		
			//Current position
			const uint x = volIter.getPosX();
			const uint y = volIter.getPosY();
			const uint z = volIter.getPosZ();

			//LogManager::getSingleton().logMessage("x = " + StringConverter::toString(int(x)) + " y = " + StringConverter::toString(int(y)) + " z = " + StringConverter::toString(int(z)));

			//Voxels values
			const uchar v000 = volIter.getVoxel();
			const uchar v100 = volIter.peekVoxel1px0py0pz();
			const uchar v010 = volIter.peekVoxel0px1py0pz();
			const uchar v110 = volIter.peekVoxel1px1py0pz();
			const uchar v001 = volIter.peekVoxel0px0py1pz();
			const uchar v101 = volIter.peekVoxel1px0py1pz();
			const uchar v011 = volIter.peekVoxel0px1py1pz();
			const uchar v111 = volIter.peekVoxel1px1py1pz();
			
			//Determine the index into the edge table which tells us which vertices are inside of the surface
			uchar iCubeIndex = 0;

			if (v000 == 0) iCubeIndex |= 1;
			if (v100 == 0) iCubeIndex |= 2;
			if (v110 == 0) iCubeIndex |= 4;
			if (v010 == 0) iCubeIndex |= 8;
			if (v001 == 0) iCubeIndex |= 16;
			if (v101 == 0) iCubeIndex |= 32;
			if (v111 == 0) iCubeIndex |= 64;
			if (v011 == 0) iCubeIndex |= 128;

			/* Cube is entirely in/out of the surface */
			if (edgeTable[iCubeIndex] == 0)
			{
				continue;
			}

			/* Find the vertices where the surface intersects the cube */
			if (edgeTable[iCubeIndex] & 1)
			{
				vertlist[0].x = 2*x + 1;
				vertlist[0].y = 2*y;
				vertlist[0].z = 2*z;
				vertMaterials[0] = (std::max)(v000,v100); //FIXME - faster way?
			}
			if (edgeTable[iCubeIndex] & 2)
			{
				vertlist[1].x = 2*x + 2;
				vertlist[1].y = 2*y + 1;
				vertlist[1].z = 2*z;
				vertMaterials[1] = (std::max)(v100,v110);
			}
			if (edgeTable[iCubeIndex] & 4)
			{
				vertlist[2].x = 2*x + 1;
				vertlist[2].y = 2*y + 2;
				vertlist[2].z = 2*z;
				vertMaterials[2] = (std::max)(v010,v110);
			}
			if (edgeTable[iCubeIndex] & 8)
			{
				vertlist[3].x = 2*x;
				vertlist[3].y = 2*y + 1;
				vertlist[3].z = 2*z;
				vertMaterials[3] = (std::max)(v000,v010);
			}
			if (edgeTable[iCubeIndex] & 16)
			{
				vertlist[4].x = 2*x + 1;
				vertlist[4].y = 2*y;
				vertlist[4].z = 2*z + 2;
				vertMaterials[4] = (std::max)(v001,v101);
			}
			if (edgeTable[iCubeIndex] & 32)
			{
				vertlist[5].x = 2*x + 2;
				vertlist[5].y = 2*y + 1;
				vertlist[5].z = 2*z + 2;
				vertMaterials[5] = (std::max)(v101,v111);
			}
			if (edgeTable[iCubeIndex] & 64)
			{
				vertlist[6].x = 2*x + 1;
				vertlist[6].y = 2*y + 2;
				vertlist[6].z = 2*z + 2;
				vertMaterials[6] = (std::max)(v011,v111);
			}
			if (edgeTable[iCubeIndex] & 128)
			{
				vertlist[7].x = 2*x;
				vertlist[7].y = 2*y + 1;
				vertlist[7].z = 2*z + 2;
				vertMaterials[7] = (std::max)(v001,v011);
			}
			if (edgeTable[iCubeIndex] & 256)
			{
				vertlist[8].x = 2*x;
				vertlist[8].y = 2*y;
				vertlist[8].z = 2*z + 1;
				vertMaterials[8] = (std::max)(v000,v001);
			}
			if (edgeTable[iCubeIndex] & 512)
			{
				vertlist[9].x = 2*x + 2;
				vertlist[9].y = 2*y;
				vertlist[9].z = 2*z + 1;
				vertMaterials[9] = (std::max)(v100,v101);
			}
			if (edgeTable[iCubeIndex] & 1024)
			{
				vertlist[10].x = 2*x + 2;
				vertlist[10].y = 2*y + 2;
				vertlist[10].z = 2*z + 1;
				vertMaterials[10] = (std::max)(v110,v111);
			}
			if (edgeTable[iCubeIndex] & 2048)
			{
				vertlist[11].x = 2*x;
				vertlist[11].y = 2*y + 2;
				vertlist[11].z = 2*z + 1;
				vertMaterials[11] = (std::max)(v010,v011);
			}

			for (int i=0;triTable[iCubeIndex][i]!=-1;i+=3)
			{
				//The three vertices forming a triangle
				const UIntVector3 vertex0 = vertlist[triTable[iCubeIndex][i  ]] - offset;
				const UIntVector3 vertex1 = vertlist[triTable[iCubeIndex][i+1]] - offset;
				const UIntVector3 vertex2 = vertlist[triTable[iCubeIndex][i+2]] - offset;

				const uchar material0 = vertMaterials[triTable[iCubeIndex][i  ]];
				const uchar material1 = vertMaterials[triTable[iCubeIndex][i+1]];
				const uchar material2 = vertMaterials[triTable[iCubeIndex][i+2]];

				//FIXME - for the time being the material is the highest vertex
				//Need to think about this...
				/*uchar material = std::max(v000,v001);
				material = std::max(material,v010);
				material = std::max(material,v011);
				material = std::max(material,v100);
				material = std::max(material,v101);
				material = std::max(material,v110);
				material = std::max(material,v111);	*/

				std::set<uchar> materials; //FIXME - set::set is pretty slow for this as it only holds up to 3 vertices.
				materials.insert(material0);
				materials.insert(material1);
				materials.insert(material2);

				//vertexScaled values are always integers and so can be used as indices.
				//FIXME - these integer values can just be obtained by using floor()?
				/*long int index;
				unsigned int vertexScaledX;
				unsigned int vertexScaledY;
				unsigned int vertexScaledZ;*/

				//SurfaceTriangle triangle; //Triangle to be created...

				for(std::set<uchar>::iterator materialsIter = materials.begin(); materialsIter != materials.end(); ++materialsIter)
				{
					uchar material = *materialsIter;

					SurfaceVertex surfaceVertex0(vertex0);
					if(material0 == material)
						surfaceVertex0.alpha = 1.0;
					else
						surfaceVertex0.alpha = 0.0;
					//surfaceVertex0.normal = Vector3(1.0,1.0,1.0);
					surfaceVertex0.normal = Vector3(0.0,0.0,0.0);

					SurfaceVertex surfaceVertex1(vertex1);
					if(material1 == material)
						surfaceVertex1.alpha = 1.0;
					else
						surfaceVertex1.alpha = 0.0;
					//surfaceVertex1.normal = Vector3(1.0,1.0,1.0);
					surfaceVertex1.normal = Vector3(0.0,0.0,0.0);

					SurfaceVertex surfaceVertex2(vertex2);
					if(material2 == material)
						surfaceVertex2.alpha = 1.0;
					else
						surfaceVertex2.alpha = 0.0;
					//surfaceVertex2.normal = Vector3(1.0,1.0,1.0);
					surfaceVertex2.normal = Vector3(0.0,0.0,0.0);

					result[material].addTriangle(surfaceVertex0, surfaceVertex1, surfaceVertex2);

					//Get scaled values for vertex 0
					/*vertexScaledX = static_cast<unsigned int>((vertex0.x * 2.0) + 0.5);
					vertexScaledY = static_cast<unsigned int>((vertex0.y * 2.0) + 0.5);
					vertexScaledZ = static_cast<unsigned int>((vertex0.z * 2.0) + 0.5);
					vertexScaledX %= OGRE_REGION_SIDE_LENGTH*2+1;
					vertexScaledY %= OGRE_REGION_SIDE_LENGTH*2+1;
					vertexScaledZ %= OGRE_REGION_SIDE_LENGTH*2+1;
					//If a vertex has not yet been added, it's index is -1
					index = vertexIndices[vertexScaledX][vertexScaledY][vertexScaledZ][material];
					if((index == -1))
					{
						//Add the vertex
						SurfaceVertex vertex(vertex0);
						if(material0 == material)
							vertex.alpha = 1.0;
						else
							vertex.alpha = 0.0;
						vertexData[material].push_back(vertex);
						triangle.v0 = vertexData[material].size()-1;
						vertexIndices[vertexScaledX][vertexScaledY][vertexScaledZ][material] = vertexData[material].size()-1;
					}
					else
					{
						//Just reuse the existing vertex
						triangle.v0 = index;
					}

					//Get scaled values for vertex 1
					vertexScaledX = static_cast<unsigned int>((vertex1.x * 2.0) + 0.5);
					vertexScaledY = static_cast<unsigned int>((vertex1.y * 2.0) + 0.5);
					vertexScaledZ = static_cast<unsigned int>((vertex1.z * 2.0) + 0.5);
					vertexScaledX %= OGRE_REGION_SIDE_LENGTH*2+1;
					vertexScaledY %= OGRE_REGION_SIDE_LENGTH*2+1;
					vertexScaledZ %= OGRE_REGION_SIDE_LENGTH*2+1;
					//If a vertex has not yet been added, it's index is -1
					index = vertexIndices[vertexScaledX][vertexScaledY][vertexScaledZ][material];
					if((index == -1))
					{
						//Add the vertex
						SurfaceVertex vertex(vertex1);
						if(material1 == material)
							vertex.alpha = 1.0;
						else
							vertex.alpha = 0.0;
						vertexData[material].push_back(vertex);
						triangle.v1 = vertexData[material].size()-1;
						vertexIndices[vertexScaledX][vertexScaledY][vertexScaledZ][material] = vertexData[material].size()-1;
					}
					else
					{
						//Just reuse the existing vertex
						triangle.v1 = index;
					}

					//Get scaled values for vertex 2
					vertexScaledX = static_cast<unsigned int>((vertex2.x * 2.0) + 0.5);
					vertexScaledY = static_cast<unsigned int>((vertex2.y * 2.0) + 0.5);
					vertexScaledZ = static_cast<unsigned int>((vertex2.z * 2.0) + 0.5);
					vertexScaledX %= OGRE_REGION_SIDE_LENGTH*2+1;
					vertexScaledY %= OGRE_REGION_SIDE_LENGTH*2+1;
					vertexScaledZ %= OGRE_REGION_SIDE_LENGTH*2+1;
					//If a vertex has not yet been added, it's index is -1
					index = vertexIndices[vertexScaledX][vertexScaledY][vertexScaledZ][material];
					if((index == -1))
					{
						//Add the vertex
						SurfaceVertex vertex(vertex2);
						if(material2 == material)
							vertex.alpha = 1.0;
						else
							vertex.alpha = 0.0;
						vertexData[material].push_back(vertex);
						triangle.v2 = vertexData[material].size()-1;
						vertexIndices[vertexScaledX][vertexScaledY][vertexScaledZ][material] = vertexData[material].size()-1;
					}
					else
					{
						//Just reuse the existing vertex
						triangle.v2 = index;
					}

					//Add the triangle
					indexData[material].push_back(triangle);*/
				}
			}
		}	

		//FIXME - can it happen that we have no vertices or triangles? Should exit early?

		
		//////////////////////////////////////////////////////////////////////////
		//Compute normals
		//////////////////////////////////////////////////////////////////////////

		/*for(uint materialCt = 0; materialCt < 256; materialCt++)
		{
			for(uint vertexCt = 0; vertexCt < vertexData[materialCt].size(); ++vertexCt)
			{
				const Vector3 vertexPosition = vertexData[materialCt][vertexCt].position;

				const float posX = vertexPosition.x + static_cast<float>(regionX * OGRE_REGION_SIDE_LENGTH);
				const float posY = vertexPosition.y + static_cast<float>(regionY * OGRE_REGION_SIDE_LENGTH);
				const float posZ = vertexPosition.z + static_cast<float>(regionZ * OGRE_REGION_SIDE_LENGTH);

				const uint floorX = static_cast<uint>(vertexPosition.x) + regionX * OGRE_REGION_SIDE_LENGTH;
				const uint floorY = static_cast<uint>(vertexPosition.y) + regionY * OGRE_REGION_SIDE_LENGTH;
				const uint floorZ = static_cast<uint>(vertexPosition.z) + regionZ * OGRE_REGION_SIDE_LENGTH;

				switch(m_normalGenerationMethod)
				{
				case SIMPLE:
					{
						volIter.setPosition(static_cast<uint>(posX),static_cast<uint>(posY),static_cast<uint>(posZ));
						const uchar uFloor = volIter.getVoxel() > 0 ? 1 : 0;
						if((posX - floorX) > 0.25) //The result should be 0.0 or 0.5
						{					
							uchar uCeil = volIter.peekVoxel1px0py0pz() > 0 ? 1 : 0;
							vertexData[materialCt][vertexCt].normal = Vector3(uFloor - uCeil,0.0,0.0);
						}
						else if((posY - floorY) > 0.25) //The result should be 0.0 or 0.5
						{
							uchar uCeil = volIter.peekVoxel0px1py0pz() > 0 ? 1 : 0;
							vertexData[materialCt][vertexCt].normal = Vector3(0.0,uFloor - uCeil,0.0);
						}
						else if((posZ - floorZ) > 0.25) //The result should be 0.0 or 0.5
						{
							uchar uCeil = volIter.peekVoxel0px0py1pz() > 0 ? 1 : 0;
							vertexData[materialCt][vertexCt].normal = Vector3(0.0, 0.0,uFloor - uCeil);					
						}
						vertexData[materialCt][vertexCt].normal.normalise();
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
						vertexData[materialCt][vertexCt].normal = gradFloor + gradCeil;
						vertexData[materialCt][vertexCt].normal *= -1;
						vertexData[materialCt][vertexCt].normal.normalise();
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
						vertexData[materialCt][vertexCt].normal = gradFloor + gradCeil;
						vertexData[materialCt][vertexCt].normal *= -1;
						vertexData[materialCt][vertexCt].normal.normalise();
						break;
					}
				}
			}
		}*/

		//////////////////////////////////////////////////////////////////////////
		//Decimate mesh
		//////////////////////////////////////////////////////////////////////////

		//Ogre::LogManager::getSingleton().logMessage("Before merge: vertices = " + Ogre::StringConverter::toString(vertexData[4].size()) + ", triangles = " + Ogre::StringConverter::toString(indexData[4].size()/3));
		//mergeVertices6(vertexData, indexData);
		//Ogre::LogManager::getSingleton().logMessage("After merge:  vertices = " + Ogre::StringConverter::toString(vertexData[4].size()) + ", triangles = " + Ogre::StringConverter::toString(indexData[4].size()/3));

		/*std::map<uchar, SurfacePatch> result;
		for(uint materialCt = 1; materialCt < 256; materialCt++)
		{
			if(indexData[materialCt].size() == 0)
			{
				continue;
			}

			SurfacePatch surfacePatch;
			for(uint indexCt = 0; indexCt < indexData[materialCt].size(); indexCt++)
			{
				surfacePatch.addTriangle
				(
					vertexData[materialCt][indexData[materialCt][indexCt].v0],
					vertexData[materialCt][indexData[materialCt][indexCt].v1],
					vertexData[materialCt][indexData[materialCt][indexCt].v2]
				);
			}

			result.insert(std::make_pair(materialCt, surfacePatch));
		}*/

		for(std::map<uchar, SurfacePatch>::iterator iterPatch = result.begin(); iterPatch != result.end(); ++iterPatch)
		{
			iterPatch->second.m_v3dOffset = offset;
			iterPatch->second.computeNormalsFromVolume(volIter);
			iterPatch->second.endDefinition();
			bool removedVertex = false;
			//do
			//{
				removedVertex = iterPatch->second.decimate2();
			//}
			//while(removedVertex);
		}

		//LogManager::getSingleton().logMessage("Finished Generating Mesh Data");

		return result;
	}

#ifdef BLAH

	void PolyVoxSceneManager::mergeVertices6(std::vector< std::vector<SurfaceVertex> >& vertexData, std::vector< std::vector<SurfaceTriangle> >& indexData) const
	{
		for(uint material = 1; material < 256; ++material)
		{
			if(vertexData[material].empty())
				continue;

			//Ogre::LogManager::getSingleton().logMessage("Material = " + Ogre::StringConverter::toString(material) + " No of vertices = " + Ogre::StringConverter::toString(vertexData[material].size()));

			std::vector< std::set<uint> > connectedVertices;
			connectedVertices.resize(vertexData[material].size());
			for(uint vertexCt = 0; vertexCt < vertexData[material].size(); vertexCt++)
			{
				for(uint triangleCt = 0; triangleCt < indexData[material].size(); ++triangleCt)
				{
					if((indexData[material][triangleCt].v0 == vertexCt) || (indexData[material][triangleCt].v1 == vertexCt) || (indexData[material][triangleCt].v2 == vertexCt))
					{
						connectedVertices[vertexCt].insert(indexData[material][triangleCt].v0);
						connectedVertices[vertexCt].insert(indexData[material][triangleCt].v1);
						connectedVertices[vertexCt].insert(indexData[material][triangleCt].v2);
					}
				}
				connectedVertices[vertexCt].erase(vertexCt);
			}

			std::vector< uint > initialConnections;
			initialConnections.resize(vertexData[material].size());
			for(uint vertexCt = 0; vertexCt < vertexData[material].size(); vertexCt++)
			{
				initialConnections[vertexCt] = connectedVertices[vertexCt].size();
			}

			/*std::vector<uint> noOfEdges; //0 means not on ege, 1 means edge, 2 or more means corner
			noOfEdges.resize(vertexData[material].size());
			for(uint vertexCt = 0; vertexCt < vertexData[material].size(); ++vertexCt)
			{
				noOfEdges[vertexCt] = 0;
			}
			for(uint vertexCt = 0; vertexCt < vertexData[material].size(); ++vertexCt)
			{
				if(vertexData[material][vertexCt].position.x < 0.25)
					 noOfEdges[vertexCt] |= 1;
				if(vertexData[material][vertexCt].position.y < 0.25)
					noOfEdges[vertexCt] |= 2;
				if(vertexData[material][vertexCt].position.z < 0.25)
					noOfEdges[vertexCt] |= 4;
				if(vertexData[material][vertexCt].position.x > OGRE_REGION_SIDE_LENGTH-0.25)
					noOfEdges[vertexCt] |= 8;
				if(vertexData[material][vertexCt].position.y > OGRE_REGION_SIDE_LENGTH-0.25)
					noOfEdges[vertexCt] |= 16;
				if(vertexData[material][vertexCt].position.z > OGRE_REGION_SIDE_LENGTH-0.25)
					noOfEdges[vertexCt] |= 32;
			}*/

			for(uint u = 0; u < 50; u++)
			{
				//FIXME - this is innefficient! iterating over ever vertex, even though one material might just use a few of them.
				for(uint vertexCt = 0; vertexCt < vertexData[material].size(); vertexCt++)
				{	
					//if((initialConnections[vertexCt] == 6) || (initialConnections[vertexCt] == initialConnections[*connectedIter]))
					{
						if(verticesArePlanar3(vertexCt, connectedVertices[vertexCt], vertexData[material]))
						{		
							std::set<uint>::iterator connectedIter = connectedVertices[vertexCt].begin();
							/*for(uint triCt = 0; triCt < indexData[material].size(); triCt++)
							{							
								if(indexData[material][triCt].v0 == vertexCt)
								{								
									indexData[material][triCt].v0 = *connectedIter;
								}
								if(indexData[material][triCt].v1 == vertexCt)
								{
									indexData[material][triCt].v1 = *connectedIter;
								}
								if(indexData[material][triCt].v2 == vertexCt)
								{
									indexData[material][triCt].v2 = *connectedIter;
								}
							}*/
							for(uint innerVertexCt = 0; innerVertexCt < vertexData[material].size(); innerVertexCt++)
							{
								if(vertexData[material][innerVertexCt].position.distance(vertexData[material][vertexCt].position) < 0.1)
								{
									if((initialConnections[innerVertexCt] == 6) || (initialConnections[innerVertexCt] == initialConnections[*connectedIter]))
										vertexData[material][innerVertexCt].position = vertexData[material][*connectedIter].position;
								}
							}
						}
					}
				}
			}

			//Delete degenerate triangles
			std::vector<SurfaceVertex> resultingVertexData;
			std::vector<SurfaceTriangle> resultingIndexData;
			for(uint triCt = 0; triCt < indexData[material].size(); triCt++)
			{
				if((indexData[material][triCt].v0 != indexData[material][triCt].v1) || (indexData[material][triCt].v1 != indexData[material][triCt].v2))
				{
					uint pos = resultingVertexData.size();

					resultingVertexData.push_back(vertexData[material][indexData[material][triCt].v0]);
					resultingVertexData.push_back(vertexData[material][indexData[material][triCt].v1]);
					resultingVertexData.push_back(vertexData[material][indexData[material][triCt].v2]);

					SurfaceTriangle triangle(pos, pos+1, pos+2);
					resultingIndexData.push_back(triangle);
				}
			}
			vertexData[material] = resultingVertexData;
			indexData[material] = resultingIndexData;
		}			
	}

#endif


	bool PolyVoxSceneManager::verticesArePlanar3(uint uCurrentVertex, std::set<uint> setConnectedVertices, std::vector<SurfaceVertex>& vertexData) const
	{
		//FIXME - specially handle the case where they are all the same.
		//This is happening a lot after many vertices have been moved round?
		bool allXMatch = true;
		bool allYMatch = true;
		bool allZMatch = true;
		bool allNormalsMatch = true;

		//FIXME - reorder come of these tests based on likelyness to fail?

		std::set<uint>::iterator iterConnectedVertices;
		for(iterConnectedVertices = setConnectedVertices.begin(); iterConnectedVertices != setConnectedVertices.end(); ++iterConnectedVertices)
		{
			if(vertexData[uCurrentVertex].position.x != vertexData[*iterConnectedVertices].position.x)
			{
				allXMatch = false;
			}
			if(vertexData[uCurrentVertex].position.y != vertexData[*iterConnectedVertices].position.y)
			{
				allYMatch = false;
			}
			if(vertexData[uCurrentVertex].position.z != vertexData[*iterConnectedVertices].position.z)
			{
				allZMatch = false;
			}
			//FIXME - are these already normalised? We should make sure they are...
			if(vertexData[uCurrentVertex].normal.normalisedCopy().dotProduct(vertexData[*iterConnectedVertices].normal.normalisedCopy()) < 0.99)
			{
				return false;				
			}
		}

		return allXMatch || allYMatch || allZMatch;
	}

	void PolyVoxSceneManager::markVoxelChanged(uint x, uint y, uint z)
	{
		//If we are not on a boundary, just mark one region.
		if((x % OGRE_REGION_SIDE_LENGTH != 0) &&
			(x % OGRE_REGION_SIDE_LENGTH != OGRE_REGION_SIDE_LENGTH-1) &&
			(y % OGRE_REGION_SIDE_LENGTH != 0) &&
			(y % OGRE_REGION_SIDE_LENGTH != OGRE_REGION_SIDE_LENGTH-1) &&
			(z % OGRE_REGION_SIDE_LENGTH != 0) &&
			(z % OGRE_REGION_SIDE_LENGTH != OGRE_REGION_SIDE_LENGTH-1))
		{
			surfaceUpToDate[x >> OGRE_REGION_SIDE_LENGTH_POWER][y >> OGRE_REGION_SIDE_LENGTH_POWER][z >> OGRE_REGION_SIDE_LENGTH_POWER] = false;
		}
		else //Mark surrounding block as well
		{
			const uint regionX = x >> OGRE_REGION_SIDE_LENGTH_POWER;
			const uint regionY = y >> OGRE_REGION_SIDE_LENGTH_POWER;
			const uint regionZ = z >> OGRE_REGION_SIDE_LENGTH_POWER;

			const uint minRegionX = (std::max)(uint(0),regionX-1);
			const uint minRegionY = (std::max)(uint(0),regionY-1);
			const uint minRegionZ = (std::max)(uint(0),regionZ-1);

			const uint maxRegionX = (std::min)(uint(OGRE_VOLUME_SIDE_LENGTH_IN_REGIONS-1),regionX+1);
			const uint maxRegionY = (std::min)(uint(OGRE_VOLUME_SIDE_LENGTH_IN_REGIONS-1),regionY+1);
			const uint maxRegionZ = (std::min)(uint(OGRE_VOLUME_SIDE_LENGTH_IN_REGIONS-1),regionZ+1);

			for(uint zCt = minRegionZ; zCt <= maxRegionZ; zCt++)
			{
				for(uint yCt = minRegionY; yCt <= maxRegionY; yCt++)
				{
					for(uint xCt = minRegionX; xCt <= maxRegionX; xCt++)
					{
						surfaceUpToDate[xCt][yCt][zCt] = false;
					}
				}
			}
		}
	}

	void PolyVoxSceneManager::markRegionChanged(uint firstX, uint firstY, uint firstZ, uint lastX, uint lastY, uint lastZ)
	{
		const uint firstRegionX = firstX >> OGRE_REGION_SIDE_LENGTH_POWER;
		const uint firstRegionY = firstY >> OGRE_REGION_SIDE_LENGTH_POWER;
		const uint firstRegionZ = firstZ >> OGRE_REGION_SIDE_LENGTH_POWER;

		const uint lastRegionX = lastX >> OGRE_REGION_SIDE_LENGTH_POWER;
		const uint lastRegionY = lastY >> OGRE_REGION_SIDE_LENGTH_POWER;
		const uint lastRegionZ = lastZ >> OGRE_REGION_SIDE_LENGTH_POWER;

		for(uint zCt = firstRegionZ; zCt <= lastRegionZ; zCt++)
		{
			for(uint yCt = firstRegionY; yCt <= lastRegionY; yCt++)
			{
				for(uint xCt = firstRegionX; xCt <= lastRegionX; xCt++)
				{
					surfaceUpToDate[xCt][yCt][zCt] = false;
				}
			}
		}
	}

	void PolyVoxSceneManager::doRegionGrowing(uint xStart, uint yStart, uint zStart, uchar value)
	{
		volumeData->regionGrow(xStart,yStart,zStart,value);
		//FIXME - keep track of what has changed...
		markRegionChanged(0,0,0,OGRE_VOLUME_SIDE_LENGTH-1,OGRE_VOLUME_SIDE_LENGTH-1,OGRE_VOLUME_SIDE_LENGTH-1);
	}

	bool PolyVoxSceneManager::saveScene(const String& filename)
	{
		volumeData->saveToFile(filename);
		return true; //FIXME - check for error...
	}

	uint PolyVoxSceneManager::getSideLength(void)
	{
		return OGRE_VOLUME_SIDE_LENGTH;
	}

	uchar PolyVoxSceneManager::getMaterialIndexAt(uint uX, uint uY, uint uZ)
	{
		if(volumeData->containsPoint(IntVector3(uX,uY,uZ),0))
		{
			VolumeIterator volIter(*volumeData);
			return volIter.getVoxelAt(uX,uY,uZ);
		}
		else
		{
			return 0;
		}
	}

	void PolyVoxSceneManager::setNormalGenerationMethod(NormalGenerationMethod method)
	{
		m_normalGenerationMethod = method;
	}

	/*void PolyVoxSceneManager::setMaterialNameForIndex(uchar uIndex, std::string sMaterialName)
	{
		m_aMaterialNames[uIndex] = sMaterialName;
	}*/

	bool PolyVoxSceneManager::containsPoint(Vector3 pos, float boundary)
	{
		return volumeData->containsPoint(pos, boundary);
	}

	bool PolyVoxSceneManager::containsPoint(IntVector3 pos, uint boundary)
	{
		return volumeData->containsPoint(pos, boundary);
	}

	void PolyVoxSceneManager::igniteVoxel(UIntVector3 voxelToIgnite)
	{
		m_queueVoxelsToBurn.push(voxelToIgnite);
	}
}

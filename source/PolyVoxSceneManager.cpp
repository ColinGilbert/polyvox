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
#include "SurfaceVertex.h"
#include "SurfaceEdge.h"
#include "IndexedSurfacePatch.h"
#include "PolyVoxSceneManager.h"
#include "VolumeIterator.h"

#include <list>

using namespace boost;

namespace Ogre
{

	//////////////////////////////////////////////////////////////////////////
	// PolyVoxSceneManager
	//////////////////////////////////////////////////////////////////////////
	PolyVoxSceneManager::PolyVoxSceneManager()
		:volumeData(0)
		,useNormalSmoothing(false)
		,normalSmoothingFilterSize(1)
		,m_normalGenerationMethod(SOBEL)
		,m_bHaveGeneratedMeshes(false)
	{	
		//sceneNodes.clear();
	}

	PolyVoxSceneManager::~PolyVoxSceneManager()
	{
	}

	std::list<RegionGeometry> PolyVoxSceneManager::getChangedRegionGeometry(void)
	{
		std::list<RegionGeometry> listChangedRegionGeometry;

		//Regenerate meshes.
		for(uint16_t regionZ = 0; regionZ < OGRE_VOLUME_SIDE_LENGTH_IN_REGIONS; ++regionZ)
		{		
			for(uint16_t regionY = 0; regionY < OGRE_VOLUME_SIDE_LENGTH_IN_REGIONS; ++regionY)
			{
				for(uint16_t regionX = 0; regionX < OGRE_VOLUME_SIDE_LENGTH_IN_REGIONS; ++regionX)
				{
					if(surfaceUpToDate[regionX][regionY][regionZ] == false)
					{
						//Generate the surface
						RegionGeometry regionGeometry;
						regionGeometry.m_patchSingleMaterial = new IndexedSurfacePatch(false);
						regionGeometry.m_patchMultiMaterial = new IndexedSurfacePatch(true);
						regionGeometry.m_v3dRegionPosition.setData(regionX, regionY, regionZ);

						generateMeshDataForRegion(regionX,regionY,regionZ, regionGeometry.m_patchSingleMaterial, regionGeometry.m_patchMultiMaterial);

						regionGeometry.m_bContainsSingleMaterialPatch = regionGeometry.m_patchSingleMaterial->m_vecVertices.size() > 0;
						regionGeometry.m_bContainsMultiMaterialPatch = regionGeometry.m_patchMultiMaterial->m_vecVertices.size() > 0;
						regionGeometry.m_bIsEmpty = ((regionGeometry.m_patchSingleMaterial->m_vecVertices.size() == 0) && (regionGeometry.m_patchMultiMaterial->m_vecTriangleIndices.size() == 0));

						listChangedRegionGeometry.push_back(regionGeometry);
					}
				}
			}
		}

		return listChangedRegionGeometry;
	}

	void PolyVoxSceneManager::setAllUpToDateFlagsTo(bool newUpToDateValue)
	{
		for(uint16_t blockZ = 0; blockZ < OGRE_VOLUME_SIDE_LENGTH_IN_REGIONS; ++blockZ)
		{
			for(uint16_t blockY = 0; blockY < OGRE_VOLUME_SIDE_LENGTH_IN_REGIONS; ++blockY)
			{
				for(uint16_t blockX = 0; blockX < OGRE_VOLUME_SIDE_LENGTH_IN_REGIONS; ++blockX)
				{
					surfaceUpToDate[blockX][blockY][blockZ] = newUpToDateValue;
				}
			}
		}
	}

	void PolyVoxSceneManager::createSphereAt(Vector3 centre, Real radius, uint8_t value, bool painting)
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
		//volumeData = VolumePtr(new Volume);
		volumeData = new Volume();
		VolumeIterator volIter(*volumeData);
		for(uint16_t z = 0; z < OGRE_VOLUME_SIDE_LENGTH; ++z)
		{
			for(uint16_t y = 0; y < OGRE_VOLUME_SIDE_LENGTH; ++y)
			{
				for(uint16_t x = 0; x < OGRE_VOLUME_SIDE_LENGTH; ++x)
				{
					if((x/16+y/16+z/16)%2 == 0)
						volIter.setVoxelAt(x,y,z,4);
					else
						volIter.setVoxelAt(x,y,z,8);
				}
			}
		}		

		for(uint16_t z = 0; z < OGRE_VOLUME_SIDE_LENGTH; ++z)
		{
			for(uint16_t y = 0; y < OGRE_VOLUME_SIDE_LENGTH; ++y)
			{
				for(uint16_t x = 0; x < OGRE_VOLUME_SIDE_LENGTH; ++x)
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

		uint16_t uHalfX = static_cast<uint16_t>(v3dSize.x / 2);
		uint16_t uHalfY = static_cast<uint16_t>(v3dSize.y / 2);
		uint16_t uHalfZ = static_cast<uint16_t>(v3dSize.z / 2);

		for(uint16_t z = static_cast<uint16_t>(centre.z) - uHalfZ; z < static_cast<uint16_t>(centre.z) + uHalfZ; z++)
		{
			for(uint16_t y = static_cast<uint16_t>(centre.y) - uHalfY; y < static_cast<uint16_t>(centre.y) + uHalfY; y++)
			{
				for(uint16_t x = static_cast<uint16_t>(centre.x) - uHalfX; x < static_cast<uint16_t>(centre.x) + uHalfX; x++)
				{
					volIter.setVoxelAt(x,y,z,0);
				}
			}
		}

		for(uint16_t z = 0; z < OGRE_VOLUME_SIDE_LENGTH; ++z)
		{
			for(uint16_t y = 0; y < OGRE_VOLUME_SIDE_LENGTH; ++y)
			{
				for(uint16_t x = 0; x < OGRE_VOLUME_SIDE_LENGTH; ++x)
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

	void PolyVoxSceneManager::generateMeshDataForRegion(const uint16_t regionX, const uint16_t regionY, const uint16_t regionZ, IndexedSurfacePatch* singleMaterialPatch, IndexedSurfacePatch* multiMaterialPatch) const
	{	
		//IndexedSurfacePatch* surfacePatchResult = new IndexedSurfacePatch;

		//First and last voxels in the region
		const uint16_t firstX = regionX * OGRE_REGION_SIDE_LENGTH;
		const uint16_t firstY = regionY * OGRE_REGION_SIDE_LENGTH;
		const uint16_t firstZ = regionZ * OGRE_REGION_SIDE_LENGTH;
		const uint16_t lastX = (std::min)(firstX + OGRE_REGION_SIDE_LENGTH-1,static_cast<uint32_t>(OGRE_VOLUME_SIDE_LENGTH-2));
		const uint16_t lastY = (std::min)(firstY + OGRE_REGION_SIDE_LENGTH-1,static_cast<uint32_t>(OGRE_VOLUME_SIDE_LENGTH-2));
		const uint16_t lastZ = (std::min)(firstZ + OGRE_REGION_SIDE_LENGTH-1,static_cast<uint32_t>(OGRE_VOLUME_SIDE_LENGTH-2));

		//Offset from lower block corner
		const UIntVector3 offset(firstX*2,firstY*2,firstZ*2);

		UIntVector3 vertlist[12];
		uint8_t vertMaterials[12];
		VolumeIterator volIter(*volumeData);
		volIter.setValidRegion(firstX,firstY,firstZ,lastX,lastY,lastZ);

		//////////////////////////////////////////////////////////////////////////
		//Get mesh data
		//////////////////////////////////////////////////////////////////////////

		//Iterate over each cell in the region
		for(volIter.setPosition(firstX,firstY,firstZ);volIter.isValidForRegion();volIter.moveForwardInRegion())
		{		
			//Current position
			const uint16_t x = volIter.getPosX();
			const uint16_t y = volIter.getPosY();
			const uint16_t z = volIter.getPosZ();

			//Voxels values
			const uint8_t v000 = volIter.getVoxel();
			const uint8_t v100 = volIter.peekVoxel1px0py0pz();
			const uint8_t v010 = volIter.peekVoxel0px1py0pz();
			const uint8_t v110 = volIter.peekVoxel1px1py0pz();
			const uint8_t v001 = volIter.peekVoxel0px0py1pz();
			const uint8_t v101 = volIter.peekVoxel1px0py1pz();
			const uint8_t v011 = volIter.peekVoxel0px1py1pz();
			const uint8_t v111 = volIter.peekVoxel1px1py1pz();

			//Determine the index into the edge table which tells us which vertices are inside of the surface
			uint8_t iCubeIndex = 0;

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
				vertMaterials[0] = v000 | v100; //Because one of these is 0, the or operation takes the max.
			}
			if (edgeTable[iCubeIndex] & 2)
			{
				vertlist[1].x = 2*x + 2;
				vertlist[1].y = 2*y + 1;
				vertlist[1].z = 2*z;
				vertMaterials[1] = v100 | v110;
			}
			if (edgeTable[iCubeIndex] & 4)
			{
				vertlist[2].x = 2*x + 1;
				vertlist[2].y = 2*y + 2;
				vertlist[2].z = 2*z;
				vertMaterials[2] = v010 | v110;
			}
			if (edgeTable[iCubeIndex] & 8)
			{
				vertlist[3].x = 2*x;
				vertlist[3].y = 2*y + 1;
				vertlist[3].z = 2*z;
				vertMaterials[3] = v000 | v010;
			}
			if (edgeTable[iCubeIndex] & 16)
			{
				vertlist[4].x = 2*x + 1;
				vertlist[4].y = 2*y;
				vertlist[4].z = 2*z + 2;
				vertMaterials[4] = v001 | v101;
			}
			if (edgeTable[iCubeIndex] & 32)
			{
				vertlist[5].x = 2*x + 2;
				vertlist[5].y = 2*y + 1;
				vertlist[5].z = 2*z + 2;
				vertMaterials[5] = v101 | v111;
			}
			if (edgeTable[iCubeIndex] & 64)
			{
				vertlist[6].x = 2*x + 1;
				vertlist[6].y = 2*y + 2;
				vertlist[6].z = 2*z + 2;
				vertMaterials[6] = v011 | v111;
			}
			if (edgeTable[iCubeIndex] & 128)
			{
				vertlist[7].x = 2*x;
				vertlist[7].y = 2*y + 1;
				vertlist[7].z = 2*z + 2;
				vertMaterials[7] = v001 | v011;
			}
			if (edgeTable[iCubeIndex] & 256)
			{
				vertlist[8].x = 2*x;
				vertlist[8].y = 2*y;
				vertlist[8].z = 2*z + 1;
				vertMaterials[8] = v000 | v001;
			}
			if (edgeTable[iCubeIndex] & 512)
			{
				vertlist[9].x = 2*x + 2;
				vertlist[9].y = 2*y;
				vertlist[9].z = 2*z + 1;
				vertMaterials[9] = v100 | v101;
			}
			if (edgeTable[iCubeIndex] & 1024)
			{
				vertlist[10].x = 2*x + 2;
				vertlist[10].y = 2*y + 2;
				vertlist[10].z = 2*z + 1;
				vertMaterials[10] = v110 | v111;
			}
			if (edgeTable[iCubeIndex] & 2048)
			{
				vertlist[11].x = 2*x;
				vertlist[11].y = 2*y + 2;
				vertlist[11].z = 2*z + 1;
				vertMaterials[11] = v010 | v011;
			}

			for (int i=0;triTable[iCubeIndex][i]!=-1;i+=3)
			{
				//The three vertices forming a triangle
				const UIntVector3 vertex0 = vertlist[triTable[iCubeIndex][i  ]] - offset;
				const UIntVector3 vertex1 = vertlist[triTable[iCubeIndex][i+1]] - offset;
				const UIntVector3 vertex2 = vertlist[triTable[iCubeIndex][i+2]] - offset;

				const uint8_t material0 = vertMaterials[triTable[iCubeIndex][i  ]];
				const uint8_t material1 = vertMaterials[triTable[iCubeIndex][i+1]];
				const uint8_t material2 = vertMaterials[triTable[iCubeIndex][i+2]];


				//If all the materials are the same, we just need one triangle for that material with all the alphas set high.
				if((material0 == material1) && (material1 == material2))
				{
					SurfaceVertex surfaceVertex0Alpha1(vertex0,material0 + 0.1,1.0);
					SurfaceVertex surfaceVertex1Alpha1(vertex1,material1 + 0.1,1.0);
					SurfaceVertex surfaceVertex2Alpha1(vertex2,material2 + 0.1,1.0);
					singleMaterialPatch->addTriangle(surfaceVertex0Alpha1, surfaceVertex1Alpha1, surfaceVertex2Alpha1);
				}
				else if(material0 == material1)
				{
					{
						SurfaceVertex surfaceVertex0Alpha1(vertex0,material0 + 0.1,1.0);
						SurfaceVertex surfaceVertex1Alpha1(vertex1,material0 + 0.1,1.0);
						SurfaceVertex surfaceVertex2Alpha1(vertex2,material0 + 0.1,0.0);
						multiMaterialPatch->addTriangle(surfaceVertex0Alpha1, surfaceVertex1Alpha1, surfaceVertex2Alpha1);
					}

					{
						SurfaceVertex surfaceVertex0Alpha1(vertex0,material2 + 0.1,0.0);
						SurfaceVertex surfaceVertex1Alpha1(vertex1,material2 + 0.1,0.0);
						SurfaceVertex surfaceVertex2Alpha1(vertex2,material2 + 0.1,1.0);
						multiMaterialPatch->addTriangle(surfaceVertex0Alpha1, surfaceVertex1Alpha1, surfaceVertex2Alpha1);
					}
				}
				else if(material0 == material2)
				{
					{
						SurfaceVertex surfaceVertex0Alpha1(vertex0,material0 + 0.1,1.0);
						SurfaceVertex surfaceVertex1Alpha1(vertex1,material0 + 0.1,0.0);
						SurfaceVertex surfaceVertex2Alpha1(vertex2,material0 + 0.1,1.0);
						multiMaterialPatch->addTriangle(surfaceVertex0Alpha1, surfaceVertex1Alpha1, surfaceVertex2Alpha1);
					}

					{
						SurfaceVertex surfaceVertex0Alpha1(vertex0,material1 + 0.1,0.0);
						SurfaceVertex surfaceVertex1Alpha1(vertex1,material1 + 0.1,1.0);
						SurfaceVertex surfaceVertex2Alpha1(vertex2,material1 + 0.1,0.0);
						multiMaterialPatch->addTriangle(surfaceVertex0Alpha1, surfaceVertex1Alpha1, surfaceVertex2Alpha1);
					}
				}
				else if(material1 == material2)
				{
					{
						SurfaceVertex surfaceVertex0Alpha1(vertex0,material1 + 0.1,0.0);
						SurfaceVertex surfaceVertex1Alpha1(vertex1,material1 + 0.1,1.0);
						SurfaceVertex surfaceVertex2Alpha1(vertex2,material1 + 0.1,1.0);
						multiMaterialPatch->addTriangle(surfaceVertex0Alpha1, surfaceVertex1Alpha1, surfaceVertex2Alpha1);
					}

					{
						SurfaceVertex surfaceVertex0Alpha1(vertex0,material0 + 0.1,1.0);
						SurfaceVertex surfaceVertex1Alpha1(vertex1,material0 + 0.1,0.0);
						SurfaceVertex surfaceVertex2Alpha1(vertex2,material0 + 0.1,0.0);
						multiMaterialPatch->addTriangle(surfaceVertex0Alpha1, surfaceVertex1Alpha1, surfaceVertex2Alpha1);
					}
				}
				else
				{
					{
						SurfaceVertex surfaceVertex0Alpha1(vertex0,material0 + 0.1,1.0);
						SurfaceVertex surfaceVertex1Alpha1(vertex1,material0 + 0.1,0.0);
						SurfaceVertex surfaceVertex2Alpha1(vertex2,material0 + 0.1,0.0);
						multiMaterialPatch->addTriangle(surfaceVertex0Alpha1, surfaceVertex1Alpha1, surfaceVertex2Alpha1);
					}

					{
						SurfaceVertex surfaceVertex0Alpha1(vertex0,material1 + 0.1,0.0);
						SurfaceVertex surfaceVertex1Alpha1(vertex1,material1 + 0.1,1.0);
						SurfaceVertex surfaceVertex2Alpha1(vertex2,material1 + 0.1,0.0);
						multiMaterialPatch->addTriangle(surfaceVertex0Alpha1, surfaceVertex1Alpha1, surfaceVertex2Alpha1);
					}

					{
						SurfaceVertex surfaceVertex0Alpha1(vertex0,material2 + 0.1,0.0);
						SurfaceVertex surfaceVertex1Alpha1(vertex1,material2 + 0.1,0.0);
						SurfaceVertex surfaceVertex2Alpha1(vertex2,material2 + 0.1,1.0);
						multiMaterialPatch->addTriangle(surfaceVertex0Alpha1, surfaceVertex1Alpha1, surfaceVertex2Alpha1);
					}
				}
				//If there not all the same, we need one triangle for each unique material.
				//We'll also need some vertices with low alphas for blending.
				/*else 
				{
				SurfaceVertex surfaceVertex0Alpha0(vertex0,0.0);
				SurfaceVertex surfaceVertex1Alpha0(vertex1,0.0);
				SurfaceVertex surfaceVertex2Alpha0(vertex2,0.0);

				if(material0 == material1)
				{
				surfacePatchMapResult[material0]->addTriangle(surfaceVertex0Alpha1, surfaceVertex1Alpha1, surfaceVertex2Alpha0);
				surfacePatchMapResult[material2]->addTriangle(surfaceVertex0Alpha0, surfaceVertex1Alpha0, surfaceVertex2Alpha1);
				}
				else if(material1 == material2)
				{
				surfacePatchMapResult[material1]->addTriangle(surfaceVertex0Alpha0, surfaceVertex1Alpha1, surfaceVertex2Alpha1);
				surfacePatchMapResult[material0]->addTriangle(surfaceVertex0Alpha1, surfaceVertex1Alpha0, surfaceVertex2Alpha0);
				}
				else if(material2 == material0)
				{
				surfacePatchMapResult[material0]->addTriangle(surfaceVertex0Alpha1, surfaceVertex1Alpha0, surfaceVertex2Alpha1);
				surfacePatchMapResult[material1]->addTriangle(surfaceVertex0Alpha0, surfaceVertex1Alpha1, surfaceVertex2Alpha0);
				}
				else
				{
				surfacePatchMapResult[material0]->addTriangle(surfaceVertex0Alpha1, surfaceVertex1Alpha0, surfaceVertex2Alpha0);
				surfacePatchMapResult[material1]->addTriangle(surfaceVertex0Alpha0, surfaceVertex1Alpha1, surfaceVertex2Alpha0);
				surfacePatchMapResult[material2]->addTriangle(surfaceVertex0Alpha0, surfaceVertex1Alpha0, surfaceVertex2Alpha1);
				}
				}*/
			}//For each triangle
		}//For each cell

		//FIXME - can it happen that we have no vertices or triangles? Should exit early?


		//for(std::map<uint8_t, IndexedSurfacePatch*>::iterator iterPatch = surfacePatchMapResult.begin(); iterPatch != surfacePatchMapResult.end(); ++iterPatch)
		{

			std::vector<SurfaceVertex>::iterator iterSurfaceVertex = singleMaterialPatch->m_vecVertices.begin();
			while(iterSurfaceVertex != singleMaterialPatch->m_vecVertices.end())
			{
				Vector3 tempNormal = computeNormal((iterSurfaceVertex->getPosition() + offset).toOgreVector3()/2.0f, CENTRAL_DIFFERENCE);
				const_cast<SurfaceVertex&>(*iterSurfaceVertex).setNormal(tempNormal);
				++iterSurfaceVertex;
			}

			iterSurfaceVertex = multiMaterialPatch->m_vecVertices.begin();
			while(iterSurfaceVertex != multiMaterialPatch->m_vecVertices.end())
			{
				Vector3 tempNormal = computeNormal((iterSurfaceVertex->getPosition() + offset).toOgreVector3()/2.0f, CENTRAL_DIFFERENCE);
				const_cast<SurfaceVertex&>(*iterSurfaceVertex).setNormal(tempNormal);
				++iterSurfaceVertex;
			}

			uint16_t noOfRemovedVertices = 0;
			//do
			{
				//noOfRemovedVertices = iterPatch->second.decimate();
			}
			//while(noOfRemovedVertices > 10); //We don't worry about the last few vertices - it's not worth the overhead of calling the function.
		}

		//return singleMaterialPatch;
	}

	Vector3 PolyVoxSceneManager::computeNormal(const Vector3& position, NormalGenerationMethod normalGenerationMethod) const
	{
		VolumeIterator volIter(*volumeData); //FIXME - save this somewhere - could be expensive to create?

		const float posX = position.x;
		const float posY = position.y;
		const float posZ = position.z;

		const uint16_t floorX = static_cast<uint16_t>(posX);
		const uint16_t floorY = static_cast<uint16_t>(posY);
		const uint16_t floorZ = static_cast<uint16_t>(posZ);

		Vector3 result;


		if(normalGenerationMethod == SOBEL)
		{
			volIter.setPosition(static_cast<uint16_t>(posX),static_cast<uint16_t>(posY),static_cast<uint16_t>(posZ));
			const Vector3 gradFloor = volIter.getSobelGradient();
			if((posX - floorX) > 0.25) //The result should be 0.0 or 0.5
			{			
				volIter.setPosition(static_cast<uint16_t>(posX+1.0),static_cast<uint16_t>(posY),static_cast<uint16_t>(posZ));
			}
			if((posY - floorY) > 0.25) //The result should be 0.0 or 0.5
			{			
				volIter.setPosition(static_cast<uint16_t>(posX),static_cast<uint16_t>(posY+1.0),static_cast<uint16_t>(posZ));
			}
			if((posZ - floorZ) > 0.25) //The result should be 0.0 or 0.5
			{			
				volIter.setPosition(static_cast<uint16_t>(posX),static_cast<uint16_t>(posY),static_cast<uint16_t>(posZ+1.0));					
			}
			const Vector3 gradCeil = volIter.getSobelGradient();
			result = ((gradFloor + gradCeil) * -1.0);
			if(result.squaredLength() < 0.0001)
			{
				//Operation failed - fall back on simple gradient estimation
				normalGenerationMethod = SIMPLE;
			}
		}
		if(normalGenerationMethod == CENTRAL_DIFFERENCE)
		{
			volIter.setPosition(static_cast<uint16_t>(posX),static_cast<uint16_t>(posY),static_cast<uint16_t>(posZ));
			const Vector3 gradFloor = volIter.getCentralDifferenceGradient();
			if((posX - floorX) > 0.25) //The result should be 0.0 or 0.5
			{			
				volIter.setPosition(static_cast<uint16_t>(posX+1.0),static_cast<uint16_t>(posY),static_cast<uint16_t>(posZ));
			}
			if((posY - floorY) > 0.25) //The result should be 0.0 or 0.5
			{			
				volIter.setPosition(static_cast<uint16_t>(posX),static_cast<uint16_t>(posY+1.0),static_cast<uint16_t>(posZ));
			}
			if((posZ - floorZ) > 0.25) //The result should be 0.0 or 0.5
			{			
				volIter.setPosition(static_cast<uint16_t>(posX),static_cast<uint16_t>(posY),static_cast<uint16_t>(posZ+1.0));					
			}
			const Vector3 gradCeil = volIter.getCentralDifferenceGradient();
			result = ((gradFloor + gradCeil) * -1.0);
			if(result.squaredLength() < 0.0001)
			{
				//Operation failed - fall back on simple gradient estimation
				normalGenerationMethod = SIMPLE;
			}
		}
		if(normalGenerationMethod == SIMPLE)
		{
			volIter.setPosition(static_cast<uint16_t>(posX),static_cast<uint16_t>(posY),static_cast<uint16_t>(posZ));
			const uint8_t uFloor = volIter.getVoxel() > 0 ? 1 : 0;
			if((posX - floorX) > 0.25) //The result should be 0.0 or 0.5
			{					
				uint8_t uCeil = volIter.peekVoxel1px0py0pz() > 0 ? 1 : 0;
				result = Vector3(uFloor - uCeil,0.0,0.0);
			}
			else if((posY - floorY) > 0.25) //The result should be 0.0 or 0.5
			{
				uint8_t uCeil = volIter.peekVoxel0px1py0pz() > 0 ? 1 : 0;
				result = Vector3(0.0,uFloor - uCeil,0.0);
			}
			else if((posZ - floorZ) > 0.25) //The result should be 0.0 or 0.5
			{
				uint8_t uCeil = volIter.peekVoxel0px0py1pz() > 0 ? 1 : 0;
				result = Vector3(0.0, 0.0,uFloor - uCeil);					
			}
		}
		return result;
	}

	void PolyVoxSceneManager::markVoxelChanged(uint16_t x, uint16_t y, uint16_t z)
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
			const uint16_t regionX = x >> OGRE_REGION_SIDE_LENGTH_POWER;
			const uint16_t regionY = y >> OGRE_REGION_SIDE_LENGTH_POWER;
			const uint16_t regionZ = z >> OGRE_REGION_SIDE_LENGTH_POWER;

			const uint16_t minRegionX = (std::max)(uint16_t(0),uint16_t(regionX-1));
			const uint16_t minRegionY = (std::max)(uint16_t(0),uint16_t(regionY-1));
			const uint16_t minRegionZ = (std::max)(uint16_t(0),uint16_t(regionZ-1));

			const uint16_t maxRegionX = (std::min)(uint16_t(OGRE_VOLUME_SIDE_LENGTH_IN_REGIONS-1),uint16_t(regionX+1));
			const uint16_t maxRegionY = (std::min)(uint16_t(OGRE_VOLUME_SIDE_LENGTH_IN_REGIONS-1),uint16_t(regionY+1));
			const uint16_t maxRegionZ = (std::min)(uint16_t(OGRE_VOLUME_SIDE_LENGTH_IN_REGIONS-1),uint16_t(regionZ+1));

			for(uint16_t zCt = minRegionZ; zCt <= maxRegionZ; zCt++)
			{
				for(uint16_t yCt = minRegionY; yCt <= maxRegionY; yCt++)
				{
					for(uint16_t xCt = minRegionX; xCt <= maxRegionX; xCt++)
					{
						surfaceUpToDate[xCt][yCt][zCt] = false;
					}
				}
			}
		}
	}

	void PolyVoxSceneManager::markRegionChanged(uint16_t firstX, uint16_t firstY, uint16_t firstZ, uint16_t lastX, uint16_t lastY, uint16_t lastZ)
	{
		const uint16_t firstRegionX = firstX >> OGRE_REGION_SIDE_LENGTH_POWER;
		const uint16_t firstRegionY = firstY >> OGRE_REGION_SIDE_LENGTH_POWER;
		const uint16_t firstRegionZ = firstZ >> OGRE_REGION_SIDE_LENGTH_POWER;

		const uint16_t lastRegionX = lastX >> OGRE_REGION_SIDE_LENGTH_POWER;
		const uint16_t lastRegionY = lastY >> OGRE_REGION_SIDE_LENGTH_POWER;
		const uint16_t lastRegionZ = lastZ >> OGRE_REGION_SIDE_LENGTH_POWER;

		for(uint16_t zCt = firstRegionZ; zCt <= lastRegionZ; zCt++)
		{
			for(uint16_t yCt = firstRegionY; yCt <= lastRegionY; yCt++)
			{
				for(uint16_t xCt = firstRegionX; xCt <= lastRegionX; xCt++)
				{
					surfaceUpToDate[xCt][yCt][zCt] = false;
				}
			}
		}
	}

	uint16_t PolyVoxSceneManager::getSideLength(void)
	{
		return OGRE_VOLUME_SIDE_LENGTH;
	}

	uint8_t PolyVoxSceneManager::getMaterialIndexAt(uint16_t uX, uint16_t uY, uint16_t uZ)
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

	bool PolyVoxSceneManager::containsPoint(Vector3 pos, float boundary)
	{
		return volumeData->containsPoint(pos, boundary);
	}

	bool PolyVoxSceneManager::containsPoint(IntVector3 pos, uint16_t boundary)
	{
		return volumeData->containsPoint(pos, boundary);
	}

	/*

	void PolyVoxSceneManager::setAxisVisible(bool visible)
	{
		if(m_axisNode)
			m_axisNode->setVisible(visible);
	}*/
}

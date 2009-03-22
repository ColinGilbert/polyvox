#pragma region License
/******************************************************************************
This file is part of the PolyVox library
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
#pragma endregion

#include "PolyVoxCore/PolyVoxImpl/ReferenceSurfaceExtractor.h"

#include "PolyVoxCore/Volume.h"
#include "PolyVoxCore/VolumeIterator.h"
#include "PolyVoxCore/IndexedSurfacePatch.h"
#include "PolyVoxCore/MarchingCubesTables.h"
#include "PolyVoxCore/Region.h"
#include "PolyVoxCore/SurfaceVertex.h"
#include "PolyVoxCore/Vector.h"

namespace PolyVox
{
	void extractReferenceSurfaceImpl(Volume<uint8>* volumeData, Region region, IndexedSurfacePatch* singleMaterialPatch)
	{	
		/*static int32 vertexIndicesX[POLYVOX_REGION_SIDE_LENGTH+1][POLYVOX_REGION_SIDE_LENGTH+1][POLYVOX_REGION_SIDE_LENGTH+1];
		static int32 vertexIndicesY[POLYVOX_REGION_SIDE_LENGTH+1][POLYVOX_REGION_SIDE_LENGTH+1][POLYVOX_REGION_SIDE_LENGTH+1];
		static int32 vertexIndicesZ[POLYVOX_REGION_SIDE_LENGTH+1][POLYVOX_REGION_SIDE_LENGTH+1][POLYVOX_REGION_SIDE_LENGTH+1];*/

		Vector3DInt32 regionDimensions = region.dimensions() + Vector3DInt32(1,1,1);
		std::vector<int32> vertexIndicesX(regionDimensions.getX() * regionDimensions.getY() * regionDimensions.getZ());
		std::vector<int32> vertexIndicesY(regionDimensions.getX() * regionDimensions.getY() * regionDimensions.getZ());
		std::vector<int32> vertexIndicesZ(regionDimensions.getX() * regionDimensions.getY() * regionDimensions.getZ());

		memset(&vertexIndicesX[0],0xFF,sizeof(vertexIndicesX[0]) * vertexIndicesX.size()); //0xFF is -1 as two's complement - this may not be portable...
		memset(&vertexIndicesY[0],0xFF,sizeof(vertexIndicesY[0]) * vertexIndicesY.size()); //FIXME - can we just use sizeof(vertexIndicesY)?
		memset(&vertexIndicesZ[0],0xFF,sizeof(vertexIndicesZ[0]) * vertexIndicesZ.size());

		//When generating the mesh for a region we actually look one voxel outside it in the
		// back, bottom, right direction. Protect against access violations by cropping region here
		Region regVolume = volumeData->getEnclosingRegion();
		//regVolume.setUpperCorner(regVolume.getUpperCorner() - Vector3DInt32(1,1,1));
		region.cropTo(regVolume);
		region.setUpperCorner(region.getUpperCorner() - Vector3DInt32(1,1,1));

		//Offset from lower block corner
		const Vector3DFloat offset = static_cast<Vector3DFloat>(region.getLowerCorner());

		Vector3DFloat vertlist[12];
		Vector3DFloat normlist[12];
		uint8 vertMaterials[12];
		VolumeIterator<uint8> volIter(*volumeData);
		volIter.setValidRegion(region);

		//////////////////////////////////////////////////////////////////////////
		//Get mesh data
		//////////////////////////////////////////////////////////////////////////

		//Iterate over each cell in the region
		volIter.setPosition(region.getLowerCorner().getX(),region.getLowerCorner().getY(), region.getLowerCorner().getZ());
		while(volIter.moveForwardInRegionXYZ())
		{		
			//Current position
			const uint16 x = volIter.getPosX();
			const uint16 y = volIter.getPosY();
			const uint16 z = volIter.getPosZ();

			//Voxels values
			const uint8 v000 = volIter.getVoxel();
			const uint8 v100 = volIter.peekVoxel1px0py0pz();
			const uint8 v010 = volIter.peekVoxel0px1py0pz();
			const uint8 v110 = volIter.peekVoxel1px1py0pz();
			const uint8 v001 = volIter.peekVoxel0px0py1pz();
			const uint8 v101 = volIter.peekVoxel1px0py1pz();
			const uint8 v011 = volIter.peekVoxel0px1py1pz();
			const uint8 v111 = volIter.peekVoxel1px1py1pz();

			//Determine the index into the edge table which tells us which vertices are inside of the surface
			uint8 iCubeIndex = 0;

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
				vertlist[0].setX(x + 0.5f);
				vertlist[0].setY(y);
				vertlist[0].setZ(z);
				normlist[0].setX(v000 > v100 ? 1.0f : -1.0f);
				normlist[0].setY(0.0f);
				normlist[0].setZ(0.0f);
				vertMaterials[0] = v000 | v100; //Because one of these is 0, the or operation takes the max.
			}
			if (edgeTable[iCubeIndex] & 2)
			{
				vertlist[1].setX(x + 1.0f);
				vertlist[1].setY(y + 0.5f);
				vertlist[1].setZ(z);
				normlist[1].setX(0.0f);
				normlist[1].setY(v100 > v110 ? 1.0f : -1.0f);
				normlist[1].setZ(0.0f);
				vertMaterials[1] = v100 | v110;
			}
			if (edgeTable[iCubeIndex] & 4)
			{
				vertlist[2].setX(x + 0.5f);
				vertlist[2].setY(y + 1.0f);
				vertlist[2].setZ(z);
				normlist[2].setX(v010 > v110 ? 1.0f : -1.0f);
				normlist[2].setY(0.0f);
				normlist[2].setZ(0.0f);
				vertMaterials[2] = v010 | v110;
			}
			if (edgeTable[iCubeIndex] & 8)
			{
				vertlist[3].setX(x);
				vertlist[3].setY(y + 0.5f);
				vertlist[3].setZ(z);
				normlist[3].setX(0.0f);
				normlist[3].setY(v000 > v010 ? 1.0f : -1.0f);
				normlist[3].setZ(0.0f);
				vertMaterials[3] = v000 | v010;
			}
			if (edgeTable[iCubeIndex] & 16)
			{
				vertlist[4].setX(x + 0.5f);
				vertlist[4].setY(y);
				vertlist[4].setZ(z + 1.0f);
				normlist[4].setX(v001 > v101 ? 1.0f : -1.0f);
				normlist[4].setY(0.0f);
				normlist[4].setZ(0.0f);
				vertMaterials[4] = v001 | v101;
			}
			if (edgeTable[iCubeIndex] & 32)
			{
				vertlist[5].setX(x + 1.0f);
				vertlist[5].setY(y + 0.5f);
				vertlist[5].setZ(z + 1.0f);
				normlist[5].setX(0.0f);
				normlist[5].setY(v101 > v111 ? 1.0f : -1.0f);
				normlist[5].setZ(0.0f);
				vertMaterials[5] = v101 | v111;
			}
			if (edgeTable[iCubeIndex] & 64)
			{
				vertlist[6].setX(x + 0.5f);
				vertlist[6].setY(y + 1.0f);
				vertlist[6].setZ(z + 1.0f);
				normlist[6].setX(v011 > v111 ? 1.0f : -1.0f);
				normlist[6].setY(0.0f);
				normlist[6].setZ(0.0f);
				vertMaterials[6] = v011 | v111;
			}
			if (edgeTable[iCubeIndex] & 128)
			{
				vertlist[7].setX(x);
				vertlist[7].setY(y + 0.5f);
				vertlist[7].setZ(z + 1.0f);
				normlist[7].setX(0.0f);
				normlist[7].setY(v001 > v011 ? 1.0f : -1.0f);
				normlist[7].setZ(0.0f);
				vertMaterials[7] = v001 | v011;
			}
			if (edgeTable[iCubeIndex] & 256)
			{
				vertlist[8].setX(x);
				vertlist[8].setY(y);
				vertlist[8].setZ(z + 0.5f);
				normlist[8].setX(0.0f);
				normlist[8].setY(0.0f);
				normlist[8].setZ(v000 > v001 ? 1.0f : -1.0f);
				vertMaterials[8] = v000 | v001;
			}
			if (edgeTable[iCubeIndex] & 512)
			{
				vertlist[9].setX(x + 1.0f);
				vertlist[9].setY(y);
				vertlist[9].setZ(z + 0.5f);
				normlist[9].setX(0.0f);
				normlist[9].setY(0.0f);
				normlist[9].setZ(v100 > v101 ? 1.0f : -1.0f);
				vertMaterials[9] = v100 | v101;
			}
			if (edgeTable[iCubeIndex] & 1024)
			{
				vertlist[10].setX(x + 1.0f);
				vertlist[10].setY(y + 1.0f);
				vertlist[10].setZ(z + 0.5f);
				normlist[10].setX(0.0f);
				normlist[10].setY(0.0f);
				normlist[10].setZ(v110 > v111 ? 1.0f : -1.0f);
				vertMaterials[10] = v110 | v111;
			}
			if (edgeTable[iCubeIndex] & 2048)
			{
				vertlist[11].setX(x);
				vertlist[11].setY(y + 1.0f);
				vertlist[11].setZ(z + 0.5f);
				normlist[11].setX(0.0f);
				normlist[11].setY(0.0f);
				normlist[11].setZ(v010 > v011 ? 1.0f : -1.0f);
				vertMaterials[11] = v010 | v011;
			}

			for (int i=0;triTable[iCubeIndex][i]!=-1;i+=3)
			{
				//The three vertices forming a triangle
				const Vector3DFloat vertex0 = vertlist[triTable[iCubeIndex][i  ]] - offset;
				const Vector3DFloat vertex1 = vertlist[triTable[iCubeIndex][i+1]] - offset;
				const Vector3DFloat vertex2 = vertlist[triTable[iCubeIndex][i+2]] - offset;

				const Vector3DFloat normal0 = normlist[triTable[iCubeIndex][i  ]];
				const Vector3DFloat normal1 = normlist[triTable[iCubeIndex][i+1]];
				const Vector3DFloat normal2 = normlist[triTable[iCubeIndex][i+2]];

				//Cast to floats and divide by two.
				//const Vector3DFloat vertex0AsFloat = (static_cast<Vector3DFloat>(vertex0) / 2.0f) - offset;
				//const Vector3DFloat vertex1AsFloat = (static_cast<Vector3DFloat>(vertex1) / 2.0f) - offset;
				//const Vector3DFloat vertex2AsFloat = (static_cast<Vector3DFloat>(vertex2) / 2.0f) - offset;

				const uint8 material0 = vertMaterials[triTable[iCubeIndex][i  ]];
				const uint8 material1 = vertMaterials[triTable[iCubeIndex][i+1]];
				const uint8 material2 = vertMaterials[triTable[iCubeIndex][i+2]];

				//If all the materials are the same, we just need one triangle for that material with all the alphas set high.
				SurfaceVertex v0(vertex0, normal0, material0 + 0.1f);
				SurfaceVertex v1(vertex1, normal1, material1 + 0.1f);
				SurfaceVertex v2(vertex2, normal2, material2 + 0.1f);

				int32 index0 = getIndexFor(v0.getPosition(), regionDimensions, vertexIndicesX, vertexIndicesY, vertexIndicesZ);
				if(index0 == -1)
				{
					index0 = singleMaterialPatch->addVertex(v0);
					setIndexFor(v0.getPosition(), regionDimensions, index0, vertexIndicesX, vertexIndicesY, vertexIndicesZ);
				}

				int32 index1 = getIndexFor(v1.getPosition(), regionDimensions, vertexIndicesX, vertexIndicesY, vertexIndicesZ);
				if(index1 == -1)
				{
					index1 = singleMaterialPatch->addVertex(v1);
					setIndexFor(v1.getPosition(), regionDimensions, index1, vertexIndicesX, vertexIndicesY, vertexIndicesZ);
				}

				int32 index2 = getIndexFor(v2.getPosition(), regionDimensions, vertexIndicesX, vertexIndicesY, vertexIndicesZ);
				if(index2 == -1)
				{
					index2 = singleMaterialPatch->addVertex(v2);
					setIndexFor(v2.getPosition(), regionDimensions, index2, vertexIndicesX, vertexIndicesY, vertexIndicesZ);
				}

				singleMaterialPatch->addTriangle(index0, index1, index2);
			}//For each triangle
		}//For each cell
	}

	int32 getIndexFor(const Vector3DFloat& pos, const Vector3DInt32& regionDimensions, const std::vector<int32>& vertexIndicesX, const std::vector<int32>& vertexIndicesY, const std::vector<int32>& vertexIndicesZ)
	{
		assert(pos.getX() >= 0.0f);
		assert(pos.getY() >= 0.0f);
		assert(pos.getZ() >= 0.0f);
		assert(pos.getX() <= POLYVOX_REGION_SIDE_LENGTH);
		assert(pos.getY() <= POLYVOX_REGION_SIDE_LENGTH);
		assert(pos.getZ() <= POLYVOX_REGION_SIDE_LENGTH);

		float xIntPartAsFloat;
		float xFracPart = std::modf(pos.getX(), &xIntPartAsFloat);
		float yIntPartAsFloat;
		float yFracPart = std::modf(pos.getY(), &yIntPartAsFloat);
		float zIntPartAsFloat;
		float zFracPart = std::modf(pos.getZ(), &zIntPartAsFloat);

		uint16 xIntPart = static_cast<uint16>(xIntPartAsFloat);
		uint16 yIntPart = static_cast<uint16>(yIntPartAsFloat);
		uint16 zIntPart = static_cast<uint16>(zIntPartAsFloat);

		//Of all the fractional parts, two should be zero and one should be 0.5.
		if(xFracPart > 0.25f)
		{
			return vertexIndicesX[xIntPart + yIntPart * regionDimensions.getX() + zIntPart * regionDimensions.getX() * regionDimensions.getY()];
		}
		if(yFracPart > 0.25f)
		{
			return vertexIndicesY[xIntPart + yIntPart * regionDimensions.getX() + zIntPart * regionDimensions.getX() * regionDimensions.getY()];
		}
		if(zFracPart > 0.25f)
		{
			return vertexIndicesZ[xIntPart + yIntPart * regionDimensions.getX() + zIntPart * regionDimensions.getX() * regionDimensions.getY()];
		}
	}

	void setIndexFor(const Vector3DFloat& pos, const Vector3DInt32& regionDimensions, int32 newIndex, std::vector<int32>& vertexIndicesX, std::vector<int32>& vertexIndicesY, std::vector<int32>& vertexIndicesZ)
	{
		assert(pos.getX() >= 0.0f);
		assert(pos.getY() >= 0.0f);
		assert(pos.getZ() >= 0.0f);
		assert(pos.getX() <= POLYVOX_REGION_SIDE_LENGTH);
		assert(pos.getY() <= POLYVOX_REGION_SIDE_LENGTH);
		assert(pos.getZ() <= POLYVOX_REGION_SIDE_LENGTH);

		float xIntPartAsFloat;
		float xFracPart = std::modf(pos.getX(), &xIntPartAsFloat);
		float yIntPartAsFloat;
		float yFracPart = std::modf(pos.getY(), &yIntPartAsFloat);
		float zIntPartAsFloat;
		float zFracPart = std::modf(pos.getZ(), &zIntPartAsFloat);

		uint16 xIntPart = static_cast<uint16>(xIntPartAsFloat);
		uint16 yIntPart = static_cast<uint16>(yIntPartAsFloat);
		uint16 zIntPart = static_cast<uint16>(zIntPartAsFloat);

		//Of all the fractional parts, two should be zero and one should be 0.5.
		if(xFracPart > 0.25f)
		{
			vertexIndicesX[xIntPart + yIntPart * regionDimensions.getX() + zIntPart * regionDimensions.getX() * regionDimensions.getY()] = newIndex;
		}
		if(yFracPart > 0.25f)
		{
			vertexIndicesY[xIntPart + yIntPart * regionDimensions.getX() + zIntPart * regionDimensions.getX() * regionDimensions.getY()] = newIndex;
		}
		if(zFracPart > 0.25f)
		{
			vertexIndicesZ[xIntPart + yIntPart * regionDimensions.getX() + zIntPart * regionDimensions.getX() * regionDimensions.getY()] = newIndex;
		}
	}
}
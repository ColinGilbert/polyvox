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

#include "PolyVoxCore/PolyVoxImpl/FastSurfaceExtractor.h"

#include "PolyVoxCore/VolumeIterator.h"
#include "PolyVoxCore/IndexedSurfacePatch.h"
#include "PolyVoxCore/PolyVoxImpl/MarchingCubesTables.h"
#include "PolyVoxCore/SurfaceVertex.h"

namespace PolyVox
{

	void extractFastSurfaceImpl(Volume<uint8_t>* volumeData, Region region, IndexedSurfacePatch* singleMaterialPatch)
	{	
		singleMaterialPatch->clear();

		//For edge indices
		int32_t* vertexIndicesX0 = new int32_t[(region.width()+2) * (region.height()+2)];
		int32_t* vertexIndicesY0 = new int32_t[(region.width()+2) * (region.height()+2)];
		int32_t* vertexIndicesZ0 = new int32_t[(region.width()+2) * (region.height()+2)];
		int32_t* vertexIndicesX1 = new int32_t[(region.width()+2) * (region.height()+2)];
		int32_t* vertexIndicesY1 = new int32_t[(region.width()+2) * (region.height()+2)];
		int32_t* vertexIndicesZ1 = new int32_t[(region.width()+2) * (region.height()+2)];

		//Cell bitmasks
		uint8_t* bitmask0 = new uint8_t[(region.width()+2) * (region.height()+2)];
		uint8_t* bitmask1 = new uint8_t[(region.width()+2) * (region.height()+2)];

		//When generating the mesh for a region we actually look one voxel outside it in the
		// back, bottom, right direction. Protect against access violations by cropping region here
		Region regVolume = volumeData->getEnclosingRegion();
		regVolume.setUpperCorner(regVolume.getUpperCorner() - Vector3DInt32(1,1,1));
		region.cropTo(regVolume);

		//Offset from volume corner
		const Vector3DFloat offset = static_cast<Vector3DFloat>(region.getLowerCorner());

		//Create a region corresponding to the first slice
		Region regSlice0(region);
		regSlice0.setUpperCorner(Vector3DInt32(regSlice0.getUpperCorner().getX(),regSlice0.getUpperCorner().getY(),regSlice0.getLowerCorner().getZ()));
		
		//Iterator to access the volume data
		VolumeIterator<uint8_t> volIter(*volumeData);		

		//Compute bitmask for initial slice
		uint32_t uNoOfNonEmptyCellsForSlice0 = computeInitialRoughBitmaskForSlice(volIter, regSlice0, offset, bitmask0);		
		if(uNoOfNonEmptyCellsForSlice0 != 0)
		{
			//If there were some non-empty cells then generate initial slice vertices for them
			generateRoughVerticesForSlice(volIter,regSlice0, offset, bitmask0, singleMaterialPatch, vertexIndicesX0, vertexIndicesY0, vertexIndicesZ0);
		}

		for(uint32_t uSlice = 0; ((uSlice <= region.depth()-1) && (uSlice + offset.getZ() < region.getUpperCorner().getZ())); ++uSlice)
		{
			Region regSlice1(regSlice0);
			regSlice1.shift(Vector3DInt32(0,0,1));

			uint32_t uNoOfNonEmptyCellsForSlice1 = computeRoughBitmaskForSliceFromPrevious(volIter, regSlice1, offset, bitmask1, bitmask0);

			if(uNoOfNonEmptyCellsForSlice1 != 0)
			{
				generateRoughVerticesForSlice(volIter,regSlice1, offset, bitmask1, singleMaterialPatch, vertexIndicesX1, vertexIndicesY1, vertexIndicesZ1);				
			}

			if((uNoOfNonEmptyCellsForSlice0 != 0) || (uNoOfNonEmptyCellsForSlice1 != 0))
			{
				generateRoughIndicesForSlice(volIter, regSlice0, singleMaterialPatch, offset, bitmask0, bitmask1, vertexIndicesX0, vertexIndicesY0, vertexIndicesZ0, vertexIndicesX1, vertexIndicesY1, vertexIndicesZ1);
			}

			std::swap(uNoOfNonEmptyCellsForSlice0, uNoOfNonEmptyCellsForSlice1);
			std::swap(bitmask0, bitmask1);
			std::swap(vertexIndicesX0, vertexIndicesX1);
			std::swap(vertexIndicesY0, vertexIndicesY1);
			std::swap(vertexIndicesZ0, vertexIndicesZ1);

			regSlice0 = regSlice1;
		}

		delete[] bitmask0;
		delete[] bitmask1;
		delete[] vertexIndicesX0;
		delete[] vertexIndicesX1;
		delete[] vertexIndicesY0;
		delete[] vertexIndicesY1;
		delete[] vertexIndicesZ0;
		delete[] vertexIndicesZ1;
	}

	uint32_t getIndex(uint32_t x, uint32_t y, uint32_t regionWidth)
	{
		return x + (y * (regionWidth+1));
	}

	uint32_t computeInitialRoughBitmaskForSlice(VolumeIterator<uint8_t>& volIter, const Region& regSlice, const Vector3DFloat& offset, uint8_t* bitmask)
	{
		uint32_t uNoOfNonEmptyCells = 0;

		//Iterate over each cell in the region
		volIter.setPosition(regSlice.getLowerCorner().getX(),regSlice.getLowerCorner().getY(), regSlice.getLowerCorner().getZ());
		volIter.setValidRegion(regSlice);
		do
		{		
			//Current position
			const uint16_t x = volIter.getPosX() - offset.getX();
			const uint16_t y = volIter.getPosY() - offset.getY();

			//Determine the index into the edge table which tells us which vertices are inside of the surface
			uint8_t iCubeIndex = 0;

			if((x==0) && (y==0))
			{
				const uint8_t v000 = volIter.getVoxel();
				const uint8_t v100 = volIter.peekVoxel1px0py0pz();
				const uint8_t v010 = volIter.peekVoxel0px1py0pz();
				const uint8_t v110 = volIter.peekVoxel1px1py0pz();

				const uint8_t v001 = volIter.peekVoxel0px0py1pz();
				const uint8_t v101 = volIter.peekVoxel1px0py1pz();
				const uint8_t v011 = volIter.peekVoxel0px1py1pz();
				const uint8_t v111 = volIter.peekVoxel1px1py1pz();			

				if (v000 == 0) iCubeIndex |= 1;
				if (v100 == 0) iCubeIndex |= 2;
				if (v110 == 0) iCubeIndex |= 4;
				if (v010 == 0) iCubeIndex |= 8;
				if (v001 == 0) iCubeIndex |= 16;
				if (v101 == 0) iCubeIndex |= 32;
				if (v111 == 0) iCubeIndex |= 64;
				if (v011 == 0) iCubeIndex |= 128;
			}
			else if((x>0) && y==0)
			{
				const uint8_t v100 = volIter.peekVoxel1px0py0pz();
				const uint8_t v110 = volIter.peekVoxel1px1py0pz();

				const uint8_t v101 = volIter.peekVoxel1px0py1pz();
				const uint8_t v111 = volIter.peekVoxel1px1py1pz();			

				//x
				uint8_t iPreviousCubeIndexX = bitmask[getIndex(x-1,y, regSlice.width()+1)];
				uint8_t srcBit6 = iPreviousCubeIndexX & 64;
				uint8_t destBit7 = srcBit6 << 1;
				
				uint8_t srcBit5 = iPreviousCubeIndexX & 32;
				uint8_t destBit4 = srcBit5 >> 1;

				uint8_t srcBit2 = iPreviousCubeIndexX & 4;
				uint8_t destBit3 = srcBit2 << 1;
				
				uint8_t srcBit1 = iPreviousCubeIndexX & 2;
				uint8_t destBit0 = srcBit1 >> 1;

				iCubeIndex |= destBit0;
				if (v100 == 0) iCubeIndex |= 2;
				if (v110 == 0) iCubeIndex |= 4;
				iCubeIndex |= destBit3;
				iCubeIndex |= destBit4;
				if (v101 == 0) iCubeIndex |= 32;
				if (v111 == 0) iCubeIndex |= 64;
				iCubeIndex |= destBit7;
			}
			else if((x==0) && (y>0))
			{
				const uint8_t v010 = volIter.peekVoxel0px1py0pz();
				const uint8_t v110 = volIter.peekVoxel1px1py0pz();

				const uint8_t v011 = volIter.peekVoxel0px1py1pz();
				const uint8_t v111 = volIter.peekVoxel1px1py1pz();

				//y
				uint8_t iPreviousCubeIndexY = bitmask[getIndex(x,y-1, regSlice.width()+1)];
				uint8_t srcBit7 = iPreviousCubeIndexY & 128;
				uint8_t destBit4 = srcBit7 >> 3;
				
				uint8_t srcBit6 = iPreviousCubeIndexY & 64;
				uint8_t destBit5 = srcBit6 >> 1;

				uint8_t srcBit3 = iPreviousCubeIndexY & 8;
				uint8_t destBit0 = srcBit3 >> 3;
				
				uint8_t srcBit2 = iPreviousCubeIndexY & 4;
				uint8_t destBit1 = srcBit2 >> 1;

				iCubeIndex |= destBit0;
				iCubeIndex |= destBit1;
				if (v110 == 0) iCubeIndex |= 4;
				if (v010 == 0) iCubeIndex |= 8;
				iCubeIndex |= destBit4;
				iCubeIndex |= destBit5;
				if (v111 == 0) iCubeIndex |= 64;
				if (v011 == 0) iCubeIndex |= 128;
			}
			else
			{
				const uint8_t v110 = volIter.peekVoxel1px1py0pz();

				const uint8_t v111 = volIter.peekVoxel1px1py1pz();

				//y
				uint8_t iPreviousCubeIndexY = bitmask[getIndex(x,y-1, regSlice.width()+1)];
				uint8_t srcBit7 = iPreviousCubeIndexY & 128;
				uint8_t destBit4 = srcBit7 >> 3;
				
				uint8_t srcBit6 = iPreviousCubeIndexY & 64;
				uint8_t destBit5 = srcBit6 >> 1;

				uint8_t srcBit3 = iPreviousCubeIndexY & 8;
				uint8_t destBit0 = srcBit3 >> 3;
				
				uint8_t srcBit2 = iPreviousCubeIndexY & 4;
				uint8_t destBit1 = srcBit2 >> 1;

				//x
				uint8_t iPreviousCubeIndexX = bitmask[getIndex(x-1,y, regSlice.width()+1)];
				srcBit6 = iPreviousCubeIndexX & 64;
				uint8_t destBit7 = srcBit6 << 1;

				srcBit2 = iPreviousCubeIndexX & 4;
				uint8_t destBit3 = srcBit2 << 1;

				iCubeIndex |= destBit0;
				iCubeIndex |= destBit1;
				if (v110 == 0) iCubeIndex |= 4;
				iCubeIndex |= destBit3;
				iCubeIndex |= destBit4;
				iCubeIndex |= destBit5;
				if (v111 == 0) iCubeIndex |= 64;
				iCubeIndex |= destBit7;
			}

			//Save the bitmask
			bitmask[getIndex(x,y, regSlice.width()+1)] = iCubeIndex;

			if(edgeTable[iCubeIndex] != 0)
			{
				++uNoOfNonEmptyCells;
			}
			
		}while(volIter.moveForwardInRegionXYZ());//For each cell

		return uNoOfNonEmptyCells;
	}

	uint32_t computeRoughBitmaskForSliceFromPrevious(VolumeIterator<uint8_t>& volIter, const Region& regSlice, const Vector3DFloat& offset, uint8_t* bitmask, uint8_t* previousBitmask)
	{
		uint32_t uNoOfNonEmptyCells = 0;

		//Iterate over each cell in the region
		volIter.setPosition(regSlice.getLowerCorner().getX(),regSlice.getLowerCorner().getY(), regSlice.getLowerCorner().getZ());
		volIter.setValidRegion(regSlice);
		do
		{		
			//Current position
			const uint16_t x = volIter.getPosX() - offset.getX();
			const uint16_t y = volIter.getPosY() - offset.getY();

			//Determine the index into the edge table which tells us which vertices are inside of the surface
			uint8_t iCubeIndex = 0;

			if((x==0) && (y==0))
			{
				const uint8_t v001 = volIter.peekVoxel0px0py1pz();
				const uint8_t v101 = volIter.peekVoxel1px0py1pz();
				const uint8_t v011 = volIter.peekVoxel0px1py1pz();
				const uint8_t v111 = volIter.peekVoxel1px1py1pz();			

				//z
				uint8_t iPreviousCubeIndexZ = previousBitmask[getIndex(x,y, regSlice.width()+1)];
				iCubeIndex = iPreviousCubeIndexZ >> 4;

				if (v001 == 0) iCubeIndex |= 16;
				if (v101 == 0) iCubeIndex |= 32;
				if (v111 == 0) iCubeIndex |= 64;
				if (v011 == 0) iCubeIndex |= 128;
			}
			else if((x>0) && y==0)
			{
				const uint8_t v101 = volIter.peekVoxel1px0py1pz();
				const uint8_t v111 = volIter.peekVoxel1px1py1pz();			

				//z
				uint8_t iPreviousCubeIndexZ = previousBitmask[getIndex(x,y, regSlice.width()+1)];
				iCubeIndex = iPreviousCubeIndexZ >> 4;

				//x
				uint8_t iPreviousCubeIndexX = bitmask[getIndex(x-1,y, regSlice.width()+1)];
				uint8_t srcBit6 = iPreviousCubeIndexX & 64;
				uint8_t destBit7 = srcBit6 << 1;
				
				uint8_t srcBit5 = iPreviousCubeIndexX & 32;
				uint8_t destBit4 = srcBit5 >> 1;

				iCubeIndex |= destBit4;
				if (v101 == 0) iCubeIndex |= 32;
				if (v111 == 0) iCubeIndex |= 64;
				iCubeIndex |= destBit7;
			}
			else if((x==0) && (y>0))
			{
				const uint8_t v011 = volIter.peekVoxel0px1py1pz();
				const uint8_t v111 = volIter.peekVoxel1px1py1pz();

				//z
				uint8_t iPreviousCubeIndexZ = previousBitmask[getIndex(x,y, regSlice.width()+1)];
				iCubeIndex = iPreviousCubeIndexZ >> 4;

				//y
				uint8_t iPreviousCubeIndexY = bitmask[getIndex(x,y-1, regSlice.width()+1)];
				uint8_t srcBit7 = iPreviousCubeIndexY & 128;
				uint8_t destBit4 = srcBit7 >> 3;
				
				uint8_t srcBit6 = iPreviousCubeIndexY & 64;
				uint8_t destBit5 = srcBit6 >> 1;

				iCubeIndex |= destBit4;
				iCubeIndex |= destBit5;
				if (v111 == 0) iCubeIndex |= 64;
				if (v011 == 0) iCubeIndex |= 128;
			}
			else
			{
				const uint8_t v111 = volIter.peekVoxel1px1py1pz();			

				//z
				uint8_t iPreviousCubeIndexZ = previousBitmask[getIndex(x,y, regSlice.width()+1)];
				iCubeIndex = iPreviousCubeIndexZ >> 4;

				//y
				uint8_t iPreviousCubeIndexY = bitmask[getIndex(x,y-1, regSlice.width()+1)];
				uint8_t srcBit7 = iPreviousCubeIndexY & 128;
				uint8_t destBit4 = srcBit7 >> 3;
				
				uint8_t srcBit6 = iPreviousCubeIndexY & 64;
				uint8_t destBit5 = srcBit6 >> 1;

				//x
				uint8_t iPreviousCubeIndexX = bitmask[getIndex(x-1,y, regSlice.width()+1)];
				srcBit6 = iPreviousCubeIndexX & 64;
				uint8_t destBit7 = srcBit6 << 1;

				iCubeIndex |= destBit4;
				iCubeIndex |= destBit5;
				if (v111 == 0) iCubeIndex |= 64;
				iCubeIndex |= destBit7;
			}

			//Save the bitmask
			bitmask[getIndex(x,y, regSlice.width()+1)] = iCubeIndex;

			if(edgeTable[iCubeIndex] != 0)
			{
				++uNoOfNonEmptyCells;
			}
			
		}while(volIter.moveForwardInRegionXYZ());//For each cell

		return uNoOfNonEmptyCells;
	}

	void generateRoughVerticesForSlice(VolumeIterator<uint8_t>& volIter, Region& regSlice, const Vector3DFloat& offset, uint8_t* bitmask, IndexedSurfacePatch* singleMaterialPatch,int32_t vertexIndicesX[],int32_t vertexIndicesY[],int32_t vertexIndicesZ[])
	{
		//Iterate over each cell in the region
		volIter.setPosition(regSlice.getLowerCorner().getX(),regSlice.getLowerCorner().getY(), regSlice.getLowerCorner().getZ());
		volIter.setValidRegion(regSlice);
		//while(volIter.moveForwardInRegionXYZ())
		do
		{		
			//Current position
			const uint16_t x = volIter.getPosX() - offset.getX();
			const uint16_t y = volIter.getPosY() - offset.getY();
			const uint16_t z = volIter.getPosZ() - offset.getZ();

			const uint8_t v000 = volIter.getVoxel();

			//Determine the index into the edge table which tells us which vertices are inside of the surface
			uint8_t iCubeIndex = bitmask[getIndex(x,y, regSlice.width()+1)];

			/* Cube is entirely in/out of the surface */
			if (edgeTable[iCubeIndex] == 0)
			{
				continue;
			}

			/* Find the vertices where the surface intersects the cube */
			if (edgeTable[iCubeIndex] & 1)
			{
				if((x + offset.getX()) != regSlice.getUpperCorner().getX())
				{
					const uint8_t v100 = volIter.peekVoxel1px0py0pz();
					const Vector3DFloat v3dPosition(x + 0.5f, y, z);
					const Vector3DFloat v3dNormal(v000 > v100 ? 1.0f : -1.0f, 0.0f, 0.0f);					
					const uint8_t uMaterial = v000 | v100; //Because one of these is 0, the or operation takes the max.
					const SurfaceVertex surfaceVertex(v3dPosition, v3dNormal, uMaterial);
					uint32_t uLastVertexIndex = singleMaterialPatch->addVertex(surfaceVertex);
					vertexIndicesX[getIndex(x,y, regSlice.width()+1)] = uLastVertexIndex;
				}
			}
			if (edgeTable[iCubeIndex] & 8)
			{
				if((y + offset.getY()) != regSlice.getUpperCorner().getY())
				{
					const uint8_t v010 = volIter.peekVoxel0px1py0pz();
					const Vector3DFloat v3dPosition(x, y + 0.5f, z);
					const Vector3DFloat v3dNormal(0.0f, v000 > v010 ? 1.0f : -1.0f, 0.0f);
					const uint8_t uMaterial = v000 | v010;
					SurfaceVertex surfaceVertex(v3dPosition, v3dNormal, uMaterial);
					uint32_t uLastVertexIndex = singleMaterialPatch->addVertex(surfaceVertex);
					vertexIndicesY[getIndex(x,y, regSlice.width()+1)] = uLastVertexIndex;
				}
			}
			if (edgeTable[iCubeIndex] & 256)
			{
				//if((z + offset.getZ()) != upperCorner.getZ())
				{
					const uint8_t v001 = volIter.peekVoxel0px0py1pz();
					const Vector3DFloat v3dPosition(x, y, z + 0.5f);
					const Vector3DFloat v3dNormal(0.0f, 0.0f, v000 > v001 ? 1.0f : -1.0f);
					const uint8_t uMaterial = v000 | v001;
					SurfaceVertex surfaceVertex(v3dPosition, v3dNormal, uMaterial);
					uint32_t uLastVertexIndex = singleMaterialPatch->addVertex(surfaceVertex);
					vertexIndicesZ[getIndex(x,y, regSlice.width()+1)] = uLastVertexIndex;
				}
			}
		}while(volIter.moveForwardInRegionXYZ());//For each cell
	}

	void generateRoughIndicesForSlice(VolumeIterator<uint8_t>& volIter, const Region& regSlice, IndexedSurfacePatch* singleMaterialPatch, const Vector3DFloat& offset, uint8_t* bitmask0, uint8_t* bitmask1, int32_t vertexIndicesX0[],int32_t vertexIndicesY0[],int32_t vertexIndicesZ0[], int32_t vertexIndicesX1[],int32_t vertexIndicesY1[],int32_t vertexIndicesZ1[])
	{
		uint32_t indlist[12];

		Region regCroppedSlice(regSlice);		
		regCroppedSlice.setUpperCorner(regCroppedSlice.getUpperCorner() - Vector3DInt32(1,1,0));

		volIter.setPosition(regCroppedSlice.getLowerCorner().getX(),regCroppedSlice.getLowerCorner().getY(), regCroppedSlice.getLowerCorner().getZ());
		volIter.setValidRegion(regCroppedSlice);
		do
		{		
			//Current position
			const uint16_t x = volIter.getPosX() - offset.getX();
			const uint16_t y = volIter.getPosY() - offset.getY();
			const uint16_t z = volIter.getPosZ() - offset.getZ();

			//Determine the index into the edge table which tells us which vertices are inside of the surface
			uint8_t iCubeIndex = bitmask0[getIndex(x,y, regSlice.width()+1)];

			/* Cube is entirely in/out of the surface */
			if (edgeTable[iCubeIndex] == 0)
			{
				continue;
			}

			/* Find the vertices where the surface intersects the cube */
			if (edgeTable[iCubeIndex] & 1)
			{
				indlist[0] = vertexIndicesX0[getIndex(x,y, regSlice.width()+1)];
				assert(indlist[0] != -1);
			}
			if (edgeTable[iCubeIndex] & 2)
			{
				indlist[1] = vertexIndicesY0[getIndex(x+1,y, regSlice.width()+1)];
				assert(indlist[1] != -1);
			}
			if (edgeTable[iCubeIndex] & 4)
			{
				indlist[2] = vertexIndicesX0[getIndex(x,y+1, regSlice.width()+1)];
				assert(indlist[2] != -1);
			}
			if (edgeTable[iCubeIndex] & 8)
			{
				indlist[3] = vertexIndicesY0[getIndex(x,y, regSlice.width()+1)];
				assert(indlist[3] != -1);
			}
			if (edgeTable[iCubeIndex] & 16)
			{
				indlist[4] = vertexIndicesX1[getIndex(x,y, regSlice.width()+1)];
				assert(indlist[4] != -1);
			}
			if (edgeTable[iCubeIndex] & 32)
			{
				indlist[5] = vertexIndicesY1[getIndex(x+1,y, regSlice.width()+1)];
				assert(indlist[5] != -1);
			}
			if (edgeTable[iCubeIndex] & 64)
			{
				indlist[6] = vertexIndicesX1[getIndex(x,y+1, regSlice.width()+1)];
				assert(indlist[6] != -1);
			}
			if (edgeTable[iCubeIndex] & 128)
			{
				indlist[7] = vertexIndicesY1[getIndex(x,y, regSlice.width()+1)];
				assert(indlist[7] != -1);
			}
			if (edgeTable[iCubeIndex] & 256)
			{
				indlist[8] = vertexIndicesZ0[getIndex(x,y, regSlice.width()+1)];
				assert(indlist[8] != -1);
			}
			if (edgeTable[iCubeIndex] & 512)
			{
				indlist[9] = vertexIndicesZ0[getIndex(x+1,y, regSlice.width()+1)];
				assert(indlist[9] != -1);
			}
			if (edgeTable[iCubeIndex] & 1024)
			{
				indlist[10] = vertexIndicesZ0[getIndex(x+1,y+1, regSlice.width()+1)];
				assert(indlist[10] != -1);
			}
			if (edgeTable[iCubeIndex] & 2048)
			{
				indlist[11] = vertexIndicesZ0[getIndex(x,y+1, regSlice.width()+1)];
				assert(indlist[11] != -1);
			}

			for (int i=0;triTable[iCubeIndex][i]!=-1;i+=3)
			{
				uint32_t ind0 = indlist[triTable[iCubeIndex][i  ]];
				uint32_t ind1 = indlist[triTable[iCubeIndex][i+1]];
				uint32_t ind2 = indlist[triTable[iCubeIndex][i+2]];

				singleMaterialPatch->addTriangle(ind0, ind1, ind2);
			}//For each triangle
		}while(volIter.moveForwardInRegionXYZ());//For each cell
	}
}
#include "SurfaceExtractors.h"

#include "BlockVolume.h"
#include "GradientEstimators.h"
#include "IndexedSurfacePatch.h"
#include "MarchingCubesTables.h"
#include "Region.h"
#include "RegionGeometry.h"
#include "SurfaceAdjusters.h"
#include "SurfaceExtractorsDecimated.h"
#include "VolumeChangeTracker.h"
#include "BlockVolumeIterator.h"

#include <algorithm>

using namespace std;

namespace PolyVox
{
	std::list<RegionGeometry> getChangedRegionGeometry(VolumeChangeTracker& volume)
	{
		std::list<Region> listChangedRegions;
		volume.getChangedRegions(listChangedRegions);

		std::list<RegionGeometry> listChangedRegionGeometry;
		for(std::list<Region>::const_iterator iterChangedRegions = listChangedRegions.begin(); iterChangedRegions != listChangedRegions.end(); ++iterChangedRegions)
		{
			//Generate the surface
			RegionGeometry regionGeometry;
			regionGeometry.m_patchSingleMaterial = new IndexedSurfacePatch();
			regionGeometry.m_v3dRegionPosition = iterChangedRegions->getLowerCorner();

			//generateDecimatedMeshDataForRegion(volume.getVolumeData(), 1, *iterChangedRegions, regionGeometry.m_patchSingleMaterial);

			generateReferenceMeshDataForRegion(volume.getVolumeData(), *iterChangedRegions, regionGeometry.m_patchSingleMaterial);
		
			//for(int ct = 0; ct < 2; ct++)
			Vector3DInt32 temp = regionGeometry.m_v3dRegionPosition;
			//temp /= 16;
			/*if(temp.getY() % 32 == 0)
			{
				//smoothRegionGeometry(volume.getVolumeData(), regionGeometry);
				generateDecimatedMeshDataForRegion(volume.getVolumeData(), 0, *iterChangedRegions, regionGeometry.m_patchSingleMaterial);
			}
			else
			{
				generateDecimatedMeshDataForRegion(volume.getVolumeData(), 1, *iterChangedRegions, regionGeometry.m_patchSingleMaterial);
				//adjustDecimatedGeometry(volume.getVolumeData(), regionGeometry, 1);
			}*/

			//computeNormalsForVertices(volume.getVolumeData(), regionGeometry, CENTRAL_DIFFERENCE);

			//genMultiFromSingle(regionGeometry.m_patchSingleMaterial, regionGeometry.m_patchMultiMaterial);

			regionGeometry.m_bContainsSingleMaterialPatch = regionGeometry.m_patchSingleMaterial->getVertices().size() > 0;
			regionGeometry.m_bIsEmpty = (regionGeometry.m_patchSingleMaterial->getVertices().size() == 0) || (regionGeometry.m_patchSingleMaterial->getIndices().size() == 0);

			listChangedRegionGeometry.push_back(regionGeometry);
		}

		return listChangedRegionGeometry;
	}

	uint32 getIndex(uint32 x, uint32 y)
	{
		return x + (y * (POLYVOX_REGION_SIDE_LENGTH+1));
	}

	void generateRoughMeshDataForRegion(BlockVolume<uint8>* volumeData, Region region, IndexedSurfacePatch* singleMaterialPatch)
	{	
		singleMaterialPatch->m_vecVertices.clear();
		singleMaterialPatch->m_vecTriangleIndices.clear();

		//For edge indices
		int32* vertexIndicesX0 = new int32[(POLYVOX_REGION_SIDE_LENGTH+1) * (POLYVOX_REGION_SIDE_LENGTH+1)];
		int32* vertexIndicesY0 = new int32[(POLYVOX_REGION_SIDE_LENGTH+1) * (POLYVOX_REGION_SIDE_LENGTH+1)];
		int32* vertexIndicesZ0 = new int32[(POLYVOX_REGION_SIDE_LENGTH+1) * (POLYVOX_REGION_SIDE_LENGTH+1)];
		int32* vertexIndicesX1 = new int32[(POLYVOX_REGION_SIDE_LENGTH+1) * (POLYVOX_REGION_SIDE_LENGTH+1)];
		int32* vertexIndicesY1 = new int32[(POLYVOX_REGION_SIDE_LENGTH+1) * (POLYVOX_REGION_SIDE_LENGTH+1)];
		int32* vertexIndicesZ1 = new int32[(POLYVOX_REGION_SIDE_LENGTH+1) * (POLYVOX_REGION_SIDE_LENGTH+1)];

		//Cell bitmasks
		uint8* bitmask0 = new uint8[(POLYVOX_REGION_SIDE_LENGTH+1) * (POLYVOX_REGION_SIDE_LENGTH+1)];
		uint8* bitmask1 = new uint8[(POLYVOX_REGION_SIDE_LENGTH+1) * (POLYVOX_REGION_SIDE_LENGTH+1)];

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
		BlockVolumeIterator<uint8> volIter(*volumeData);		

		//Compute bitmask for initial slice
		uint32 uNoOfNonEmptyCellsForSlice0 = computeInitialRoughBitmaskForSlice(volIter, regSlice0, offset, bitmask0);		
		if(uNoOfNonEmptyCellsForSlice0 != 0)
		{
			//If there were some non-empty cells then generate initial slice vertices for them
			generateRoughVerticesForSlice(volIter,regSlice0, offset, bitmask0, singleMaterialPatch, vertexIndicesX0, vertexIndicesY0, vertexIndicesZ0);
		}

		for(uint32 uSlice = 0; ((uSlice <= POLYVOX_REGION_SIDE_LENGTH-1) && (uSlice + offset.getZ() < region.getUpperCorner().getZ())); ++uSlice)
		{
			Region regSlice1(regSlice0);
			regSlice1.shift(Vector3DInt32(0,0,1));

			uint32 uNoOfNonEmptyCellsForSlice1 = computeRoughBitmaskForSliceFromPrevious(volIter, regSlice1, offset, bitmask1, bitmask0);

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

	uint32 computeInitialRoughBitmaskForSlice(BlockVolumeIterator<uint8>& volIter, const Region& regSlice, const Vector3DFloat& offset, uint8* bitmask)
	{
		uint32 uNoOfNonEmptyCells = 0;

		//Iterate over each cell in the region
		volIter.setPosition(regSlice.getLowerCorner().getX(),regSlice.getLowerCorner().getY(), regSlice.getLowerCorner().getZ());
		volIter.setValidRegion(regSlice);
		do
		{		
			//Current position
			const uint16 x = volIter.getPosX() - offset.getX();
			const uint16 y = volIter.getPosY() - offset.getY();

			//Determine the index into the edge table which tells us which vertices are inside of the surface
			uint8 iCubeIndex = 0;

			if((x==0) && (y==0))
			{
				const uint8 v000 = volIter.getVoxel();
				const uint8 v100 = volIter.peekVoxel1px0py0pz();
				const uint8 v010 = volIter.peekVoxel0px1py0pz();
				const uint8 v110 = volIter.peekVoxel1px1py0pz();

				const uint8 v001 = volIter.peekVoxel0px0py1pz();
				const uint8 v101 = volIter.peekVoxel1px0py1pz();
				const uint8 v011 = volIter.peekVoxel0px1py1pz();
				const uint8 v111 = volIter.peekVoxel1px1py1pz();			

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
				const uint8 v100 = volIter.peekVoxel1px0py0pz();
				const uint8 v110 = volIter.peekVoxel1px1py0pz();

				const uint8 v101 = volIter.peekVoxel1px0py1pz();
				const uint8 v111 = volIter.peekVoxel1px1py1pz();			

				//x
				uint8 iPreviousCubeIndexX = bitmask[getIndex(x-1,y)];
				uint8 srcBit6 = iPreviousCubeIndexX & 64;
				uint8 destBit7 = srcBit6 << 1;
				
				uint8 srcBit5 = iPreviousCubeIndexX & 32;
				uint8 destBit4 = srcBit5 >> 1;

				uint8 srcBit2 = iPreviousCubeIndexX & 4;
				uint8 destBit3 = srcBit2 << 1;
				
				uint8 srcBit1 = iPreviousCubeIndexX & 2;
				uint8 destBit0 = srcBit1 >> 1;

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
				const uint8 v010 = volIter.peekVoxel0px1py0pz();
				const uint8 v110 = volIter.peekVoxel1px1py0pz();

				const uint8 v011 = volIter.peekVoxel0px1py1pz();
				const uint8 v111 = volIter.peekVoxel1px1py1pz();

				//y
				uint8 iPreviousCubeIndexY = bitmask[getIndex(x,y-1)];
				uint8 srcBit7 = iPreviousCubeIndexY & 128;
				uint8 destBit4 = srcBit7 >> 3;
				
				uint8 srcBit6 = iPreviousCubeIndexY & 64;
				uint8 destBit5 = srcBit6 >> 1;

				uint8 srcBit3 = iPreviousCubeIndexY & 8;
				uint8 destBit0 = srcBit3 >> 3;
				
				uint8 srcBit2 = iPreviousCubeIndexY & 4;
				uint8 destBit1 = srcBit2 >> 1;

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
				const uint8 v110 = volIter.peekVoxel1px1py0pz();

				const uint8 v111 = volIter.peekVoxel1px1py1pz();

				//y
				uint8 iPreviousCubeIndexY = bitmask[getIndex(x,y-1)];
				uint8 srcBit7 = iPreviousCubeIndexY & 128;
				uint8 destBit4 = srcBit7 >> 3;
				
				uint8 srcBit6 = iPreviousCubeIndexY & 64;
				uint8 destBit5 = srcBit6 >> 1;

				uint8 srcBit3 = iPreviousCubeIndexY & 8;
				uint8 destBit0 = srcBit3 >> 3;
				
				uint8 srcBit2 = iPreviousCubeIndexY & 4;
				uint8 destBit1 = srcBit2 >> 1;

				//x
				uint8 iPreviousCubeIndexX = bitmask[getIndex(x-1,y)];
				srcBit6 = iPreviousCubeIndexX & 64;
				uint8 destBit7 = srcBit6 << 1;

				srcBit2 = iPreviousCubeIndexX & 4;
				uint8 destBit3 = srcBit2 << 1;

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
			bitmask[getIndex(x,y)] = iCubeIndex;

			if(edgeTable[iCubeIndex] != 0)
			{
				++uNoOfNonEmptyCells;
			}
			
		}while(volIter.moveForwardInRegionXYZ());//For each cell

		return uNoOfNonEmptyCells;
	}

	uint32 computeRoughBitmaskForSliceFromPrevious(BlockVolumeIterator<uint8>& volIter, const Region& regSlice, const Vector3DFloat& offset, uint8* bitmask, uint8* previousBitmask)
	{
		uint32 uNoOfNonEmptyCells = 0;

		//Iterate over each cell in the region
		volIter.setPosition(regSlice.getLowerCorner().getX(),regSlice.getLowerCorner().getY(), regSlice.getLowerCorner().getZ());
		volIter.setValidRegion(regSlice);
		do
		{		
			//Current position
			const uint16 x = volIter.getPosX() - offset.getX();
			const uint16 y = volIter.getPosY() - offset.getY();

			//Determine the index into the edge table which tells us which vertices are inside of the surface
			uint8 iCubeIndex = 0;

			if((x==0) && (y==0))
			{
				const uint8 v001 = volIter.peekVoxel0px0py1pz();
				const uint8 v101 = volIter.peekVoxel1px0py1pz();
				const uint8 v011 = volIter.peekVoxel0px1py1pz();
				const uint8 v111 = volIter.peekVoxel1px1py1pz();			

				//z
				uint8 iPreviousCubeIndexZ = previousBitmask[getIndex(x,y)];
				iCubeIndex = iPreviousCubeIndexZ >> 4;

				if (v001 == 0) iCubeIndex |= 16;
				if (v101 == 0) iCubeIndex |= 32;
				if (v111 == 0) iCubeIndex |= 64;
				if (v011 == 0) iCubeIndex |= 128;
			}
			else if((x>0) && y==0)
			{
				const uint8 v101 = volIter.peekVoxel1px0py1pz();
				const uint8 v111 = volIter.peekVoxel1px1py1pz();			

				//z
				uint8 iPreviousCubeIndexZ = previousBitmask[getIndex(x,y)];
				iCubeIndex = iPreviousCubeIndexZ >> 4;

				//x
				uint8 iPreviousCubeIndexX = bitmask[getIndex(x-1,y)];
				uint8 srcBit6 = iPreviousCubeIndexX & 64;
				uint8 destBit7 = srcBit6 << 1;
				
				uint8 srcBit5 = iPreviousCubeIndexX & 32;
				uint8 destBit4 = srcBit5 >> 1;

				iCubeIndex |= destBit4;
				if (v101 == 0) iCubeIndex |= 32;
				if (v111 == 0) iCubeIndex |= 64;
				iCubeIndex |= destBit7;
			}
			else if((x==0) && (y>0))
			{
				const uint8 v011 = volIter.peekVoxel0px1py1pz();
				const uint8 v111 = volIter.peekVoxel1px1py1pz();

				//z
				uint8 iPreviousCubeIndexZ = previousBitmask[getIndex(x,y)];
				iCubeIndex = iPreviousCubeIndexZ >> 4;

				//y
				uint8 iPreviousCubeIndexY = bitmask[getIndex(x,y-1)];
				uint8 srcBit7 = iPreviousCubeIndexY & 128;
				uint8 destBit4 = srcBit7 >> 3;
				
				uint8 srcBit6 = iPreviousCubeIndexY & 64;
				uint8 destBit5 = srcBit6 >> 1;

				iCubeIndex |= destBit4;
				iCubeIndex |= destBit5;
				if (v111 == 0) iCubeIndex |= 64;
				if (v011 == 0) iCubeIndex |= 128;
			}
			else
			{
				const uint8 v111 = volIter.peekVoxel1px1py1pz();			

				//z
				uint8 iPreviousCubeIndexZ = previousBitmask[getIndex(x,y)];
				iCubeIndex = iPreviousCubeIndexZ >> 4;

				//y
				uint8 iPreviousCubeIndexY = bitmask[getIndex(x,y-1)];
				uint8 srcBit7 = iPreviousCubeIndexY & 128;
				uint8 destBit4 = srcBit7 >> 3;
				
				uint8 srcBit6 = iPreviousCubeIndexY & 64;
				uint8 destBit5 = srcBit6 >> 1;

				//x
				uint8 iPreviousCubeIndexX = bitmask[getIndex(x-1,y)];
				srcBit6 = iPreviousCubeIndexX & 64;
				uint8 destBit7 = srcBit6 << 1;

				iCubeIndex |= destBit4;
				iCubeIndex |= destBit5;
				if (v111 == 0) iCubeIndex |= 64;
				iCubeIndex |= destBit7;
			}

			//Save the bitmask
			bitmask[getIndex(x,y)] = iCubeIndex;

			if(edgeTable[iCubeIndex] != 0)
			{
				++uNoOfNonEmptyCells;
			}
			
		}while(volIter.moveForwardInRegionXYZ());//For each cell

		return uNoOfNonEmptyCells;
	}

	void generateRoughVerticesForSlice(BlockVolumeIterator<uint8>& volIter, Region& regSlice, const Vector3DFloat& offset, uint8* bitmask, IndexedSurfacePatch* singleMaterialPatch,int32 vertexIndicesX[],int32 vertexIndicesY[],int32 vertexIndicesZ[])
	{
		//Iterate over each cell in the region
		volIter.setPosition(regSlice.getLowerCorner().getX(),regSlice.getLowerCorner().getY(), regSlice.getLowerCorner().getZ());
		volIter.setValidRegion(regSlice);
		//while(volIter.moveForwardInRegionXYZ())
		do
		{		
			//Current position
			const uint16 x = volIter.getPosX() - offset.getX();
			const uint16 y = volIter.getPosY() - offset.getY();
			const uint16 z = volIter.getPosZ() - offset.getZ();

			const uint8 v000 = volIter.getVoxel();

			//Determine the index into the edge table which tells us which vertices are inside of the surface
			uint8 iCubeIndex = bitmask[getIndex(x,y)];

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
					const uint8 v100 = volIter.peekVoxel1px0py0pz();
					const Vector3DFloat v3dPosition(x + 0.5f, y, z);
					const Vector3DFloat v3dNormal(v000 > v100 ? 1.0f : -1.0f, 0.0f, 0.0f);					
					const uint8 uMaterial = v000 | v100; //Because one of these is 0, the or operation takes the max.
					const SurfaceVertex surfaceVertex(v3dPosition, v3dNormal, uMaterial);
					singleMaterialPatch->m_vecVertices.push_back(surfaceVertex);
					vertexIndicesX[getIndex(x,y)] = singleMaterialPatch->m_vecVertices.size()-1;
				}
			}
			if (edgeTable[iCubeIndex] & 8)
			{
				if((y + offset.getY()) != regSlice.getUpperCorner().getY())
				{
					const uint8 v010 = volIter.peekVoxel0px1py0pz();
					const Vector3DFloat v3dPosition(x, y + 0.5f, z);
					const Vector3DFloat v3dNormal(0.0f, v000 > v010 ? 1.0f : -1.0f, 0.0f);
					const uint8 uMaterial = v000 | v010;
					SurfaceVertex surfaceVertex(v3dPosition, v3dNormal, uMaterial);
					singleMaterialPatch->m_vecVertices.push_back(surfaceVertex);
					vertexIndicesY[getIndex(x,y)] = singleMaterialPatch->m_vecVertices.size()-1;
				}
			}
			if (edgeTable[iCubeIndex] & 256)
			{
				//if((z + offset.getZ()) != upperCorner.getZ())
				{
					const uint8 v001 = volIter.peekVoxel0px0py1pz();
					const Vector3DFloat v3dPosition(x, y, z + 0.5f);
					const Vector3DFloat v3dNormal(0.0f, 0.0f, v000 > v001 ? 1.0f : -1.0f);
					const uint8 uMaterial = v000 | v001;
					SurfaceVertex surfaceVertex(v3dPosition, v3dNormal, uMaterial);
					singleMaterialPatch->m_vecVertices.push_back(surfaceVertex);
					vertexIndicesZ[getIndex(x,y)] = singleMaterialPatch->m_vecVertices.size()-1;
				}
			}
		}while(volIter.moveForwardInRegionXYZ());//For each cell
	}

	void generateRoughIndicesForSlice(BlockVolumeIterator<uint8>& volIter, const Region& regSlice, IndexedSurfacePatch* singleMaterialPatch, const Vector3DFloat& offset, uint8* bitmask0, uint8* bitmask1, int32 vertexIndicesX0[],int32 vertexIndicesY0[],int32 vertexIndicesZ0[], int32 vertexIndicesX1[],int32 vertexIndicesY1[],int32 vertexIndicesZ1[])
	{
		uint32 indlist[12];

		Region regCroppedSlice(regSlice);		
		regCroppedSlice.setUpperCorner(regCroppedSlice.getUpperCorner() - Vector3DInt32(1,1,0));

		volIter.setPosition(regCroppedSlice.getLowerCorner().getX(),regCroppedSlice.getLowerCorner().getY(), regCroppedSlice.getLowerCorner().getZ());
		volIter.setValidRegion(regCroppedSlice);
		do
		{		
			//Current position
			const uint16 x = volIter.getPosX() - offset.getX();
			const uint16 y = volIter.getPosY() - offset.getY();
			const uint16 z = volIter.getPosZ() - offset.getZ();

			//Determine the index into the edge table which tells us which vertices are inside of the surface
			uint8 iCubeIndex = bitmask0[getIndex(x,y)];

			/* Cube is entirely in/out of the surface */
			if (edgeTable[iCubeIndex] == 0)
			{
				continue;
			}

			/* Find the vertices where the surface intersects the cube */
			if (edgeTable[iCubeIndex] & 1)
			{
				indlist[0] = vertexIndicesX0[getIndex(x,y)];
				assert(indlist[0] != -1);
			}
			if (edgeTable[iCubeIndex] & 2)
			{
				indlist[1] = vertexIndicesY0[getIndex(x+1,y)];
				assert(indlist[1] != -1);
			}
			if (edgeTable[iCubeIndex] & 4)
			{
				indlist[2] = vertexIndicesX0[getIndex(x,y+1)];
				assert(indlist[2] != -1);
			}
			if (edgeTable[iCubeIndex] & 8)
			{
				indlist[3] = vertexIndicesY0[getIndex(x,y)];
				assert(indlist[3] != -1);
			}
			if (edgeTable[iCubeIndex] & 16)
			{
				indlist[4] = vertexIndicesX1[getIndex(x,y)];
				assert(indlist[4] != -1);
			}
			if (edgeTable[iCubeIndex] & 32)
			{
				indlist[5] = vertexIndicesY1[getIndex(x+1,y)];
				assert(indlist[5] != -1);
			}
			if (edgeTable[iCubeIndex] & 64)
			{
				indlist[6] = vertexIndicesX1[getIndex(x,y+1)];
				assert(indlist[6] != -1);
			}
			if (edgeTable[iCubeIndex] & 128)
			{
				indlist[7] = vertexIndicesY1[getIndex(x,y)];
				assert(indlist[7] != -1);
			}
			if (edgeTable[iCubeIndex] & 256)
			{
				indlist[8] = vertexIndicesZ0[getIndex(x,y)];
				assert(indlist[8] != -1);
			}
			if (edgeTable[iCubeIndex] & 512)
			{
				indlist[9] = vertexIndicesZ0[getIndex(x+1,y)];
				assert(indlist[9] != -1);
			}
			if (edgeTable[iCubeIndex] & 1024)
			{
				indlist[10] = vertexIndicesZ0[getIndex(x+1,y+1)];
				assert(indlist[10] != -1);
			}
			if (edgeTable[iCubeIndex] & 2048)
			{
				indlist[11] = vertexIndicesZ0[getIndex(x,y+1)];
				assert(indlist[11] != -1);
			}

			for (int i=0;triTable[iCubeIndex][i]!=-1;i+=3)
			{
				uint32 ind0 = indlist[triTable[iCubeIndex][i  ]];
				uint32 ind1 = indlist[triTable[iCubeIndex][i+1]];
				uint32 ind2 = indlist[triTable[iCubeIndex][i+2]];

				singleMaterialPatch->m_vecTriangleIndices.push_back(ind0);
				singleMaterialPatch->m_vecTriangleIndices.push_back(ind1);
				singleMaterialPatch->m_vecTriangleIndices.push_back(ind2);
			}//For each triangle
		}while(volIter.moveForwardInRegionXYZ());//For each cell
	}

	void generateReferenceMeshDataForRegion(BlockVolume<uint8>* volumeData, Region region, IndexedSurfacePatch* singleMaterialPatch)
	{	
		static int32 vertexIndicesX[POLYVOX_REGION_SIDE_LENGTH+1][POLYVOX_REGION_SIDE_LENGTH+1][POLYVOX_REGION_SIDE_LENGTH+1];
		static int32 vertexIndicesY[POLYVOX_REGION_SIDE_LENGTH+1][POLYVOX_REGION_SIDE_LENGTH+1][POLYVOX_REGION_SIDE_LENGTH+1];
		static int32 vertexIndicesZ[POLYVOX_REGION_SIDE_LENGTH+1][POLYVOX_REGION_SIDE_LENGTH+1][POLYVOX_REGION_SIDE_LENGTH+1];

		memset(vertexIndicesX,0xFF,sizeof(vertexIndicesX)); //0xFF is -1 as two's complement - this may not be portable...
		memset(vertexIndicesY,0xFF,sizeof(vertexIndicesY));
		memset(vertexIndicesZ,0xFF,sizeof(vertexIndicesZ));

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
		BlockVolumeIterator<uint8> volIter(*volumeData);
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

				//singleMaterialPatch->addTriangle(surfaceVertex0Alpha1, surfaceVertex1Alpha1, surfaceVertex2Alpha1);

				int32 index = getIndexFor(v0.getPosition(), vertexIndicesX, vertexIndicesY, vertexIndicesZ);
				if(index == -1)
				{
					singleMaterialPatch->m_vecVertices.push_back(v0);
					singleMaterialPatch->m_vecTriangleIndices.push_back(singleMaterialPatch->m_vecVertices.size()-1);
					setIndexFor(v0.getPosition(), singleMaterialPatch->m_vecVertices.size()-1, vertexIndicesX, vertexIndicesY, vertexIndicesZ);
				}
				else
				{
					singleMaterialPatch->m_vecTriangleIndices.push_back(index);
				}

				index = getIndexFor(v1.getPosition(), vertexIndicesX, vertexIndicesY, vertexIndicesZ);
				if(index == -1)
				{
					singleMaterialPatch->m_vecVertices.push_back(v1);
					singleMaterialPatch->m_vecTriangleIndices.push_back(singleMaterialPatch->m_vecVertices.size()-1);
					setIndexFor(v1.getPosition(), singleMaterialPatch->m_vecVertices.size()-1, vertexIndicesX, vertexIndicesY, vertexIndicesZ);
				}
				else
				{
					singleMaterialPatch->m_vecTriangleIndices.push_back(index);
				}

				index = getIndexFor(v2.getPosition(), vertexIndicesX, vertexIndicesY, vertexIndicesZ);
				if(index == -1)
				{
					singleMaterialPatch->m_vecVertices.push_back(v2);
					singleMaterialPatch->m_vecTriangleIndices.push_back(singleMaterialPatch->m_vecVertices.size()-1);
					setIndexFor(v2.getPosition(), singleMaterialPatch->m_vecVertices.size()-1, vertexIndicesX, vertexIndicesY, vertexIndicesZ);
				}
				else
				{
					singleMaterialPatch->m_vecTriangleIndices.push_back(index);
				}
			}//For each triangle
		}//For each cell
	}

	int32 getIndexFor(const Vector3DFloat& pos, int32 vertexIndicesX[POLYVOX_REGION_SIDE_LENGTH+1][POLYVOX_REGION_SIDE_LENGTH+1][POLYVOX_REGION_SIDE_LENGTH+1], int32 vertexIndicesY[POLYVOX_REGION_SIDE_LENGTH+1][POLYVOX_REGION_SIDE_LENGTH+1][POLYVOX_REGION_SIDE_LENGTH+1], int32 vertexIndicesZ[POLYVOX_REGION_SIDE_LENGTH+1][POLYVOX_REGION_SIDE_LENGTH+1][POLYVOX_REGION_SIDE_LENGTH+1])
	{
		assert(pos.getX() >= 0.0f);
		assert(pos.getY() >= 0.0f);
		assert(pos.getZ() >= 0.0f);
		assert(pos.getX() <= POLYVOX_REGION_SIDE_LENGTH);
		assert(pos.getY() <= POLYVOX_REGION_SIDE_LENGTH);
		assert(pos.getZ() <= POLYVOX_REGION_SIDE_LENGTH);

		float xIntPart;
		float xFracPart = std::modf(pos.getX(), &xIntPart);
		float yIntPart;
		float yFracPart = std::modf(pos.getY(), &yIntPart);
		float zIntPart;
		float zFracPart = std::modf(pos.getZ(), &zIntPart);

		//Of all the fractional parts, two should be zero and one should have a value.
		if(xFracPart > 0.000001f)
		{
			return vertexIndicesX[static_cast<uint16>(xIntPart)][static_cast<uint16>(yIntPart)][static_cast<uint16>(zIntPart)];
		}
		if(yFracPart > 0.000001f)
		{
			return vertexIndicesY[static_cast<uint16>(xIntPart)][static_cast<uint16>(yIntPart)][static_cast<uint16>(zIntPart)];
		}
		if(zFracPart > 0.000001f)
		{
			return vertexIndicesZ[static_cast<uint16>(xIntPart)][static_cast<uint16>(yIntPart)][static_cast<uint16>(zIntPart)];
		}
		while(true);
	}

	void setIndexFor(const Vector3DFloat& pos, int32 newIndex, int32 vertexIndicesX[POLYVOX_REGION_SIDE_LENGTH+1][POLYVOX_REGION_SIDE_LENGTH+1][POLYVOX_REGION_SIDE_LENGTH+1], int32 vertexIndicesY[POLYVOX_REGION_SIDE_LENGTH+1][POLYVOX_REGION_SIDE_LENGTH+1][POLYVOX_REGION_SIDE_LENGTH+1], int32 vertexIndicesZ[POLYVOX_REGION_SIDE_LENGTH+1][POLYVOX_REGION_SIDE_LENGTH+1][POLYVOX_REGION_SIDE_LENGTH+1])
	{
		assert(pos.getX() >= 0.0f);
		assert(pos.getY() >= 0.0f);
		assert(pos.getZ() >= 0.0f);
		assert(pos.getX() <= POLYVOX_REGION_SIDE_LENGTH);
		assert(pos.getY() <= POLYVOX_REGION_SIDE_LENGTH);
		assert(pos.getZ() <= POLYVOX_REGION_SIDE_LENGTH);

		assert(newIndex < 10000);

		float xIntPart;
		float xFracPart = std::modf(pos.getX(), &xIntPart);
		float yIntPart;
		float yFracPart = std::modf(pos.getY(), &yIntPart);
		float zIntPart;
		float zFracPart = std::modf(pos.getZ(), &zIntPart);

		//Of all the fractional parts, two should be zero and one should have a value.
		if(xFracPart > 0.000001f)
		{
			vertexIndicesX[static_cast<uint16>(xIntPart)][static_cast<uint16>(yIntPart)][static_cast<uint16>(zIntPart)] = newIndex;
		}
		if(yFracPart > 0.000001f)
		{
			vertexIndicesY[static_cast<uint16>(xIntPart)][static_cast<uint16>(yIntPart)][static_cast<uint16>(zIntPart)] = newIndex;
		}
		if(zFracPart > 0.000001f)
		{
			vertexIndicesZ[static_cast<uint16>(xIntPart)][static_cast<uint16>(yIntPart)][static_cast<uint16>(zIntPart)] = newIndex;
		}
	}
}

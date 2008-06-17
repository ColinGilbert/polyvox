#include "SurfaceExtractorsDecimated.h"

#include "BlockVolume.h"
#include "GradientEstimators.h"
#include "IndexedSurfacePatch.h"
#include "MarchingCubesTables.h"
#include "Region.h"
#include "RegionGeometry.h"
#include "VolumeChangeTracker.h"
#include "BlockVolumeIterator.h"

#include <algorithm>

using namespace boost;

namespace PolyVox
{
	boost::uint32_t getDecimatedIndex(boost::uint32_t x, boost::uint32_t y)
	{
		return x + (y * (POLYVOX_REGION_SIDE_LENGTH+1));
	}

	void generateDecimatedMeshDataForRegion(BlockVolume<uint8_t>* volumeData, uint8_t uLevel, Region region, IndexedSurfacePatch* singleMaterialPatch)
	{	
		singleMaterialPatch->m_vecVertices.clear();
		singleMaterialPatch->m_vecTriangleIndices.clear();

		//For edge indices
		boost::int32_t* vertexIndicesX0 = new boost::int32_t[(POLYVOX_REGION_SIDE_LENGTH+1) * (POLYVOX_REGION_SIDE_LENGTH+1)];
		boost::int32_t* vertexIndicesY0 = new boost::int32_t[(POLYVOX_REGION_SIDE_LENGTH+1) * (POLYVOX_REGION_SIDE_LENGTH+1)];
		boost::int32_t* vertexIndicesZ0 = new boost::int32_t[(POLYVOX_REGION_SIDE_LENGTH+1) * (POLYVOX_REGION_SIDE_LENGTH+1)];
		boost::int32_t* vertexIndicesX1 = new boost::int32_t[(POLYVOX_REGION_SIDE_LENGTH+1) * (POLYVOX_REGION_SIDE_LENGTH+1)];
		boost::int32_t* vertexIndicesY1 = new boost::int32_t[(POLYVOX_REGION_SIDE_LENGTH+1) * (POLYVOX_REGION_SIDE_LENGTH+1)];
		boost::int32_t* vertexIndicesZ1 = new boost::int32_t[(POLYVOX_REGION_SIDE_LENGTH+1) * (POLYVOX_REGION_SIDE_LENGTH+1)];

		//Cell bitmasks
		boost::uint8_t* bitmask0 = new boost::uint8_t[(POLYVOX_REGION_SIDE_LENGTH+1) * (POLYVOX_REGION_SIDE_LENGTH+1)];
		boost::uint8_t* bitmask1 = new boost::uint8_t[(POLYVOX_REGION_SIDE_LENGTH+1) * (POLYVOX_REGION_SIDE_LENGTH+1)];

		const uint8_t uStepSize = uLevel == 0 ? 1 : 1 << uLevel;

		//When generating the mesh for a region we actually look outside it in the
		// back, bottom, right direction. Protect against access violations by cropping region here
		Region regVolume = volumeData->getEnclosingRegion();
		regVolume.setUpperCorner(regVolume.getUpperCorner() - Vector3DInt32(2*uStepSize-1,2*uStepSize-1,2*uStepSize-1));
		region.cropTo(regVolume);

		//Offset from volume corner
		const Vector3DFloat offset = static_cast<Vector3DFloat>(region.getLowerCorner());

		//Create a region corresponding to the first slice
		Region regSlice0(region);
		Vector3DInt32 v3dUpperCorner = regSlice0.getUpperCorner();
		v3dUpperCorner.setZ(regSlice0.getLowerCorner().getZ()); //Set the upper z to the lower z to make it one slice thick.
		regSlice0.setUpperCorner(v3dUpperCorner);
		
		//Iterator to access the volume data
		BlockVolumeIterator<boost::uint8_t> volIter(*volumeData);		

		//Compute bitmask for initial slice
		boost::uint32_t uNoOfNonEmptyCellsForSlice0 = computeInitialDecimatedBitmaskForSlice(volIter, uLevel, regSlice0, offset, bitmask0);		
		if(uNoOfNonEmptyCellsForSlice0 != 0)
		{
			//If there were some non-empty cells then generate initial slice vertices for them
			generateDecimatedVerticesForSlice(volIter, uLevel, regSlice0, offset, bitmask0, singleMaterialPatch, vertexIndicesX0, vertexIndicesY0, vertexIndicesZ0);
		}

		for(boost::uint32_t uSlice = 1; ((uSlice <= POLYVOX_REGION_SIDE_LENGTH) && (uSlice + offset.getZ() <= regVolume.getUpperCorner().getZ())); uSlice += uStepSize)
		{
			Region regSlice1(regSlice0);
			regSlice1.shift(Vector3DInt32(0,0,uStepSize));

			boost::uint32_t uNoOfNonEmptyCellsForSlice1 = computeDecimatedBitmaskForSliceFromPrevious(volIter, uLevel, regSlice1, offset, bitmask1, bitmask0);

			if(uNoOfNonEmptyCellsForSlice1 != 0)
			{
				generateDecimatedVerticesForSlice(volIter, uLevel, regSlice1, offset, bitmask1, singleMaterialPatch, vertexIndicesX1, vertexIndicesY1, vertexIndicesZ1);				
			}

			if((uNoOfNonEmptyCellsForSlice0 != 0) || (uNoOfNonEmptyCellsForSlice1 != 0))
			{
				generateDecimatedIndicesForSlice(volIter, uLevel, regSlice0, singleMaterialPatch, offset, bitmask0, bitmask1, vertexIndicesX0, vertexIndicesY0, vertexIndicesZ0, vertexIndicesX1, vertexIndicesY1, vertexIndicesZ1);
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


		/*std::vector<SurfaceVertex>::iterator iterSurfaceVertex = singleMaterialPatch->getVertices().begin();
		while(iterSurfaceVertex != singleMaterialPatch->getVertices().end())
		{
			Vector3DFloat tempNormal = computeDecimatedNormal(volumeData, static_cast<Vector3DFloat>(iterSurfaceVertex->getPosition() + offset), CENTRAL_DIFFERENCE);
			const_cast<SurfaceVertex&>(*iterSurfaceVertex).setNormal(tempNormal);
			++iterSurfaceVertex;
		}*/
	}

	boost::uint32_t computeInitialDecimatedBitmaskForSlice(BlockVolumeIterator<uint8_t>& volIter, uint8_t uLevel,  const Region& regSlice, const Vector3DFloat& offset, uint8_t* bitmask)
	{
		const uint8_t uStepSize = uLevel == 0 ? 1 : 1 << uLevel;
		boost::uint32_t uNoOfNonEmptyCells = 0;

		//Iterate over each cell in the region
		for(uint16_t y = regSlice.getLowerCorner().getY(); y <= regSlice.getUpperCorner().getY(); y += uStepSize)
		{
			for(uint16_t x = regSlice.getLowerCorner().getX(); x <= regSlice.getUpperCorner().getX(); x += uStepSize)
			{		
				//Current position
				volIter.setPosition(x,y,regSlice.getLowerCorner().getZ());

				//Determine the index into the edge table which tells us which vertices are inside of the surface
				uint8_t iCubeIndex = 0;

				if((x==regSlice.getLowerCorner().getX()) && (y==regSlice.getLowerCorner().getY()))
				{
					volIter.setPosition(x,y,regSlice.getLowerCorner().getZ());
					const uint8_t v000 = volIter.getMaxedVoxel(uLevel);
					volIter.setPosition(x+uStepSize,y,regSlice.getLowerCorner().getZ());
					const uint8_t v100 = volIter.getMaxedVoxel(uLevel);
					volIter.setPosition(x,y+uStepSize,regSlice.getLowerCorner().getZ());
					const uint8_t v010 = volIter.getMaxedVoxel(uLevel);
					volIter.setPosition(x+uStepSize,y+uStepSize,regSlice.getLowerCorner().getZ());
					const uint8_t v110 = volIter.getMaxedVoxel(uLevel);

					volIter.setPosition(x,y,regSlice.getLowerCorner().getZ()+uStepSize);
					const uint8_t v001 = volIter.getMaxedVoxel(uLevel);
					volIter.setPosition(x+uStepSize,y,regSlice.getLowerCorner().getZ()+uStepSize);
					const uint8_t v101 = volIter.getMaxedVoxel(uLevel);
					volIter.setPosition(x,y+uStepSize,regSlice.getLowerCorner().getZ()+uStepSize);
					const uint8_t v011 = volIter.getMaxedVoxel(uLevel);
					volIter.setPosition(x+uStepSize,y+uStepSize,regSlice.getLowerCorner().getZ()+uStepSize);
					const uint8_t v111 = volIter.getMaxedVoxel(uLevel);		

					if (v000 == 0) iCubeIndex |= 1;
					if (v100 == 0) iCubeIndex |= 2;
					if (v110 == 0) iCubeIndex |= 4;
					if (v010 == 0) iCubeIndex |= 8;
					if (v001 == 0) iCubeIndex |= 16;
					if (v101 == 0) iCubeIndex |= 32;
					if (v111 == 0) iCubeIndex |= 64;
					if (v011 == 0) iCubeIndex |= 128;
				}
				else if((x>regSlice.getLowerCorner().getX()) && y==regSlice.getLowerCorner().getY())
				{
					volIter.setPosition(x+uStepSize,y,regSlice.getLowerCorner().getZ());
					const uint8_t v100 = volIter.getMaxedVoxel(uLevel);
					volIter.setPosition(x+uStepSize,y+uStepSize,regSlice.getLowerCorner().getZ());
					const uint8_t v110 = volIter.getMaxedVoxel(uLevel);

					volIter.setPosition(x+uStepSize,y,regSlice.getLowerCorner().getZ()+uStepSize);
					const uint8_t v101 = volIter.getMaxedVoxel(uLevel);
					volIter.setPosition(x+uStepSize,y+uStepSize,regSlice.getLowerCorner().getZ()+uStepSize);
					const uint8_t v111 = volIter.getMaxedVoxel(uLevel);

					//x
					uint8_t iPreviousCubeIndexX = bitmask[getDecimatedIndex(x- offset.getX()-uStepSize,y- offset.getY())];
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
				else if((x==regSlice.getLowerCorner().getX()) && (y>regSlice.getLowerCorner().getY()))
				{
					volIter.setPosition(x,y+uStepSize,regSlice.getLowerCorner().getZ());
					const uint8_t v010 = volIter.getMaxedVoxel(uLevel);
					volIter.setPosition(x+uStepSize,y+uStepSize,regSlice.getLowerCorner().getZ());
					const uint8_t v110 = volIter.getMaxedVoxel(uLevel);

					volIter.setPosition(x,y+uStepSize,regSlice.getLowerCorner().getZ()+uStepSize);
					const uint8_t v011 = volIter.getMaxedVoxel(uLevel);
					volIter.setPosition(x+uStepSize,y+uStepSize,regSlice.getLowerCorner().getZ()+uStepSize);
					const uint8_t v111 = volIter.getMaxedVoxel(uLevel);

					//y
					uint8_t iPreviousCubeIndexY = bitmask[getDecimatedIndex(x- offset.getX(),y- offset.getY()-uStepSize)];
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
					volIter.setPosition(x+uStepSize,y+uStepSize,regSlice.getLowerCorner().getZ());
					const uint8_t v110 = volIter.getMaxedVoxel(uLevel);

					volIter.setPosition(x+uStepSize,y+uStepSize,regSlice.getLowerCorner().getZ()+uStepSize);
					const uint8_t v111 = volIter.getMaxedVoxel(uLevel);

					//y
					uint8_t iPreviousCubeIndexY = bitmask[getDecimatedIndex(x- offset.getX(),y- offset.getY()-uStepSize)];
					uint8_t srcBit7 = iPreviousCubeIndexY & 128;
					uint8_t destBit4 = srcBit7 >> 3;
					
					uint8_t srcBit6 = iPreviousCubeIndexY & 64;
					uint8_t destBit5 = srcBit6 >> 1;

					uint8_t srcBit3 = iPreviousCubeIndexY & 8;
					uint8_t destBit0 = srcBit3 >> 3;
					
					uint8_t srcBit2 = iPreviousCubeIndexY & 4;
					uint8_t destBit1 = srcBit2 >> 1;

					//x
					uint8_t iPreviousCubeIndexX = bitmask[getDecimatedIndex(x- offset.getX()-uStepSize,y- offset.getY())];
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
				bitmask[getDecimatedIndex(x- offset.getX(),y- offset.getY())] = iCubeIndex;

				if(edgeTable[iCubeIndex] != 0)
				{
					++uNoOfNonEmptyCells;
				}
				
			}
		}

		return uNoOfNonEmptyCells;
	}

	boost::uint32_t computeDecimatedBitmaskForSliceFromPrevious(BlockVolumeIterator<uint8_t>& volIter, uint8_t uLevel, const Region& regSlice, const Vector3DFloat& offset, uint8_t* bitmask, uint8_t* previousBitmask)
	{
		const uint8_t uStepSize = uLevel == 0 ? 1 : 1 << uLevel;
		boost::uint32_t uNoOfNonEmptyCells = 0;

		//Iterate over each cell in the region
		for(uint16_t y = regSlice.getLowerCorner().getY(); y <= regSlice.getUpperCorner().getY(); y += uStepSize)
		{
			for(uint16_t x = regSlice.getLowerCorner().getX(); x <= regSlice.getUpperCorner().getX(); x += uStepSize)
			{	
				//Current position
				volIter.setPosition(x,y,regSlice.getLowerCorner().getZ());

				//Determine the index into the edge table which tells us which vertices are inside of the surface
				uint8_t iCubeIndex = 0;

				if((x==regSlice.getLowerCorner().getX()) && (y==regSlice.getLowerCorner().getY()))
				{
					volIter.setPosition(x,y,regSlice.getLowerCorner().getZ()+uStepSize);
					const uint8_t v001 = volIter.getMaxedVoxel(uLevel);
					volIter.setPosition(x+uStepSize,y,regSlice.getLowerCorner().getZ()+uStepSize);
					const uint8_t v101 = volIter.getMaxedVoxel(uLevel);
					volIter.setPosition(x,y+uStepSize,regSlice.getLowerCorner().getZ()+uStepSize);
					const uint8_t v011 = volIter.getMaxedVoxel(uLevel);
					volIter.setPosition(x+uStepSize,y+uStepSize,regSlice.getLowerCorner().getZ()+uStepSize);
					const uint8_t v111 = volIter.getMaxedVoxel(uLevel);	

					//z
					uint8_t iPreviousCubeIndexZ = previousBitmask[getDecimatedIndex(x- offset.getX(),y- offset.getY())];
					iCubeIndex = iPreviousCubeIndexZ >> 4;

					if (v001 == 0) iCubeIndex |= 16;
					if (v101 == 0) iCubeIndex |= 32;
					if (v111 == 0) iCubeIndex |= 64;
					if (v011 == 0) iCubeIndex |= 128;
				}
				else if((x>regSlice.getLowerCorner().getX()) && y==regSlice.getLowerCorner().getY())
				{
					volIter.setPosition(x+uStepSize,y,regSlice.getLowerCorner().getZ()+uStepSize);
					const uint8_t v101 = volIter.getMaxedVoxel(uLevel);
					volIter.setPosition(x+uStepSize,y+uStepSize,regSlice.getLowerCorner().getZ()+uStepSize);
					const uint8_t v111 = volIter.getMaxedVoxel(uLevel);	

					//z
					uint8_t iPreviousCubeIndexZ = previousBitmask[getDecimatedIndex(x- offset.getX(),y- offset.getY())];
					iCubeIndex = iPreviousCubeIndexZ >> 4;

					//x
					uint8_t iPreviousCubeIndexX = bitmask[getDecimatedIndex(x- offset.getX()-uStepSize,y- offset.getY())];
					uint8_t srcBit6 = iPreviousCubeIndexX & 64;
					uint8_t destBit7 = srcBit6 << 1;
					
					uint8_t srcBit5 = iPreviousCubeIndexX & 32;
					uint8_t destBit4 = srcBit5 >> 1;

					iCubeIndex |= destBit4;
					if (v101 == 0) iCubeIndex |= 32;
					if (v111 == 0) iCubeIndex |= 64;
					iCubeIndex |= destBit7;
				}
				else if((x==regSlice.getLowerCorner().getX()) && (y>regSlice.getLowerCorner().getY()))
				{
					volIter.setPosition(x,y+uStepSize,regSlice.getLowerCorner().getZ()+uStepSize);
					const uint8_t v011 = volIter.getMaxedVoxel(uLevel);
					volIter.setPosition(x+uStepSize,y+uStepSize,regSlice.getLowerCorner().getZ()+uStepSize);
					const uint8_t v111 = volIter.getMaxedVoxel(uLevel);	

					//z
					uint8_t iPreviousCubeIndexZ = previousBitmask[getDecimatedIndex(x- offset.getX(),y- offset.getY())];
					iCubeIndex = iPreviousCubeIndexZ >> 4;

					//y
					uint8_t iPreviousCubeIndexY = bitmask[getDecimatedIndex(x- offset.getX(),y- offset.getY()-uStepSize)];
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
					volIter.setPosition(x+uStepSize,y+uStepSize,regSlice.getLowerCorner().getZ()+uStepSize);
					const uint8_t v111 = volIter.getMaxedVoxel(uLevel);	

					//z
					uint8_t iPreviousCubeIndexZ = previousBitmask[getDecimatedIndex(x- offset.getX(),y- offset.getY())];
					iCubeIndex = iPreviousCubeIndexZ >> 4;

					//y
					uint8_t iPreviousCubeIndexY = bitmask[getDecimatedIndex(x- offset.getX(),y- offset.getY()-uStepSize)];
					uint8_t srcBit7 = iPreviousCubeIndexY & 128;
					uint8_t destBit4 = srcBit7 >> 3;
					
					uint8_t srcBit6 = iPreviousCubeIndexY & 64;
					uint8_t destBit5 = srcBit6 >> 1;

					//x
					uint8_t iPreviousCubeIndexX = bitmask[getDecimatedIndex(x- offset.getX()-uStepSize,y- offset.getY())];
					srcBit6 = iPreviousCubeIndexX & 64;
					uint8_t destBit7 = srcBit6 << 1;

					iCubeIndex |= destBit4;
					iCubeIndex |= destBit5;
					if (v111 == 0) iCubeIndex |= 64;
					iCubeIndex |= destBit7;
				}

				//Save the bitmask
				bitmask[getDecimatedIndex(x- offset.getX(),y- offset.getY())] = iCubeIndex;

				if(edgeTable[iCubeIndex] != 0)
				{
					++uNoOfNonEmptyCells;
				}
				
			}//For each cell
		}

		return uNoOfNonEmptyCells;
	}

	void generateDecimatedVerticesForSlice(BlockVolumeIterator<uint8_t>& volIter, uint8_t uLevel, Region& regSlice, const Vector3DFloat& offset, uint8_t* bitmask, IndexedSurfacePatch* singleMaterialPatch,boost::int32_t vertexIndicesX[],boost::int32_t vertexIndicesY[],boost::int32_t vertexIndicesZ[])
	{
		const uint8_t uStepSize = uLevel == 0 ? 1 : 1 << uLevel;

		//Iterate over each cell in the region
		for(uint16_t y = regSlice.getLowerCorner().getY(); y <= regSlice.getUpperCorner().getY(); y += uStepSize)
		{
			for(uint16_t x = regSlice.getLowerCorner().getX(); x <= regSlice.getUpperCorner().getX(); x += uStepSize)
			{		
				//Current position
				const uint16_t z = regSlice.getLowerCorner().getZ();

				volIter.setPosition(x,y,z);
				const uint8_t v000 = volIter.getMaxedVoxel(uLevel);

				//Determine the index into the edge table which tells us which vertices are inside of the surface
				uint8_t iCubeIndex = bitmask[getDecimatedIndex(x - offset.getX(),y - offset.getY())];

				/* Cube is entirely in/out of the surface */
				if (edgeTable[iCubeIndex] == 0)
				{
					continue;
				}

				/* Find the vertices where the surface intersects the cube */
				if (edgeTable[iCubeIndex] & 1)
				{
					if(x != regSlice.getUpperCorner().getX())
					{
						volIter.setPosition(x + uStepSize,y,z);
						const uint8_t v100 = volIter.getMaxedVoxel(uLevel);
						const Vector3DFloat v3dPosition(x - offset.getX() + 0.5f * uStepSize, y - offset.getY(), z - offset.getZ());
						const Vector3DFloat v3dNormal(v000 > v100 ? 1.0f : -1.0f,0.0,0.0);
						const uint8_t uMaterial = v000 | v100; //Because one of these is 0, the or operation takes the max.
						SurfaceVertex surfaceVertex(v3dPosition, v3dNormal, uMaterial, 1.0);
						singleMaterialPatch->m_vecVertices.push_back(surfaceVertex);
						vertexIndicesX[getDecimatedIndex(x - offset.getX(),y - offset.getY())] = singleMaterialPatch->m_vecVertices.size()-1;
					}
				}
				if (edgeTable[iCubeIndex] & 8)
				{
					if(y != regSlice.getUpperCorner().getY())
					{
						volIter.setPosition(x,y + uStepSize,z);
						const uint8_t v010 = volIter.getMaxedVoxel(uLevel);
						const Vector3DFloat v3dPosition(x - offset.getX(), y - offset.getY() + 0.5f * uStepSize, z - offset.getZ());
						const Vector3DFloat v3dNormal(0.0,v000 > v010 ? 1.0f : -1.0f,0.0);
						const uint8_t uMaterial = v000 | v010; //Because one of these is 0, the or operation takes the max.
						SurfaceVertex surfaceVertex(v3dPosition, v3dNormal, uMaterial, 1.0);
						singleMaterialPatch->m_vecVertices.push_back(surfaceVertex);
						vertexIndicesY[getDecimatedIndex(x - offset.getX(),y - offset.getY())] = singleMaterialPatch->m_vecVertices.size()-1;
					}
				}
				if (edgeTable[iCubeIndex] & 256)
				{
					//if(z != regSlice.getUpperCorner.getZ())
					{
						volIter.setPosition(x,y,z + uStepSize);
						const uint8_t v001 = volIter.getMaxedVoxel(uLevel);
						const Vector3DFloat v3dPosition(x - offset.getX(), y - offset.getY(), z - offset.getZ() + 0.5f * uStepSize);
						const Vector3DFloat v3dNormal(0.0,0.0,v000 > v001 ? 1.0f : -1.0f);
						const uint8_t uMaterial = v000 | v001; //Because one of these is 0, the or operation takes the max.
						const SurfaceVertex surfaceVertex(v3dPosition, v3dNormal, uMaterial, 1.0);
						singleMaterialPatch->m_vecVertices.push_back(surfaceVertex);
						vertexIndicesZ[getDecimatedIndex(x - offset.getX(),y - offset.getY())] = singleMaterialPatch->m_vecVertices.size()-1;
					}
				}
			}//For each cell
		}
	}

	void generateDecimatedIndicesForSlice(BlockVolumeIterator<uint8_t>& volIter, uint8_t uLevel, const Region& regSlice, IndexedSurfacePatch* singleMaterialPatch, const Vector3DFloat& offset, uint8_t* bitmask0, uint8_t* bitmask1, boost::int32_t vertexIndicesX0[],boost::int32_t vertexIndicesY0[],boost::int32_t vertexIndicesZ0[], boost::int32_t vertexIndicesX1[],boost::int32_t vertexIndicesY1[],boost::int32_t vertexIndicesZ1[])
	{
		const uint8_t uStepSize = uLevel == 0 ? 1 : 1 << uLevel;
		boost::uint32_t indlist[12];

		for(uint16_t y = regSlice.getLowerCorner().getY() - offset.getY(); y < regSlice.getUpperCorner().getY() - offset.getY(); y += uStepSize)
		{
			for(uint16_t x = regSlice.getLowerCorner().getX() - offset.getX(); x < regSlice.getUpperCorner().getX() - offset.getX(); x += uStepSize)
			{		
				//Current position
				const uint16_t z = regSlice.getLowerCorner().getZ() - offset.getZ();

				//Determine the index into the edge table which tells us which vertices are inside of the surface
				uint8_t iCubeIndex = bitmask0[getDecimatedIndex(x,y)];

				/* Cube is entirely in/out of the surface */
				if (edgeTable[iCubeIndex] == 0)
				{
					continue;
				}

				/* Find the vertices where the surface intersects the cube */
				if (edgeTable[iCubeIndex] & 1)
				{
					indlist[0] = vertexIndicesX0[getDecimatedIndex(x,y)];
					assert(indlist[0] != -1);
				}
				if (edgeTable[iCubeIndex] & 2)
				{
					indlist[1] = vertexIndicesY0[getDecimatedIndex(x+uStepSize,y)];
					assert(indlist[1] != -1);
				}
				if (edgeTable[iCubeIndex] & 4)
				{
					indlist[2] = vertexIndicesX0[getDecimatedIndex(x,y+uStepSize)];
					assert(indlist[2] != -1);
				}
				if (edgeTable[iCubeIndex] & 8)
				{
					indlist[3] = vertexIndicesY0[getDecimatedIndex(x,y)];
					assert(indlist[3] != -1);
				}
				if (edgeTable[iCubeIndex] & 16)
				{
					indlist[4] = vertexIndicesX1[getDecimatedIndex(x,y)];
					assert(indlist[4] != -1);
				}
				if (edgeTable[iCubeIndex] & 32)
				{
					indlist[5] = vertexIndicesY1[getDecimatedIndex(x+uStepSize,y)];
					assert(indlist[5] != -1);
				}
				if (edgeTable[iCubeIndex] & 64)
				{
					indlist[6] = vertexIndicesX1[getDecimatedIndex(x,y+uStepSize)];
					assert(indlist[6] != -1);
				}
				if (edgeTable[iCubeIndex] & 128)
				{
					indlist[7] = vertexIndicesY1[getDecimatedIndex(x,y)];
					assert(indlist[7] != -1);
				}
				if (edgeTable[iCubeIndex] & 256)
				{
					indlist[8] = vertexIndicesZ0[getDecimatedIndex(x,y)];
					assert(indlist[8] != -1);
				}
				if (edgeTable[iCubeIndex] & 512)
				{
					indlist[9] = vertexIndicesZ0[getDecimatedIndex(x+uStepSize,y)];
					assert(indlist[9] != -1);
				}
				if (edgeTable[iCubeIndex] & 1024)
				{
					indlist[10] = vertexIndicesZ0[getDecimatedIndex(x+uStepSize,y+uStepSize)];
					assert(indlist[10] != -1);
				}
				if (edgeTable[iCubeIndex] & 2048)
				{
					indlist[11] = vertexIndicesZ0[getDecimatedIndex(x,y+uStepSize)];
					assert(indlist[11] != -1);
				}

				for (int i=0;triTable[iCubeIndex][i]!=-1;i+=3)
				{
					boost::uint32_t ind0 = indlist[triTable[iCubeIndex][i  ]];
					boost::uint32_t ind1 = indlist[triTable[iCubeIndex][i+1]];
					boost::uint32_t ind2 = indlist[triTable[iCubeIndex][i+2]];

					singleMaterialPatch->m_vecTriangleIndices.push_back(ind0);
					singleMaterialPatch->m_vecTriangleIndices.push_back(ind1);
					singleMaterialPatch->m_vecTriangleIndices.push_back(ind2);
				}//For each triangle
			}//For each cell
		}
	}

	void generateDecimatedMeshDataForRegionSlow(BlockVolume<uint8_t>* volumeData, Region region, IndexedSurfacePatch* singleMaterialPatch)
	{	
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
		uint8_t vertMaterials[12];
		BlockVolumeIterator<boost::uint8_t> volIter(*volumeData);
		volIter.setValidRegion(region);

		//////////////////////////////////////////////////////////////////////////
		//Get mesh data
		//////////////////////////////////////////////////////////////////////////

		//Iterate over each cell in the region
		//volIter.setPosition(region.getLowerCorner().getX(),region.getLowerCorner().getY(), region.getLowerCorner().getZ());
		for(uint16_t z = region.getLowerCorner().getZ(); z <= region.getUpperCorner().getZ(); z += 2)
		{
		for(uint16_t y = region.getLowerCorner().getY(); y <= region.getUpperCorner().getY(); y += 2)
		{
		for(uint16_t x = region.getLowerCorner().getX(); x <= region.getUpperCorner().getX(); x += 2)
		{
		//while(volIter.moveForwardInRegionXYZ())
		//{		
			volIter.setPosition(x,y,z);
			const uint8_t v000 = volIter.getMaxedVoxel(1);
			volIter.setPosition(x+2,y,z);
			const uint8_t v100 = volIter.getMaxedVoxel(1);
			volIter.setPosition(x,y+2,z);
			const uint8_t v010 = volIter.getMaxedVoxel(1);
			volIter.setPosition(x+2,y+2,z);
			const uint8_t v110 = volIter.getMaxedVoxel(1);
			volIter.setPosition(x,y,z+2);
			const uint8_t v001 = volIter.getMaxedVoxel(1);
			volIter.setPosition(x+2,y,z+2);
			const uint8_t v101 = volIter.getMaxedVoxel(1);
			volIter.setPosition(x,y+2,z+2);
			const uint8_t v011 = volIter.getMaxedVoxel(1);
			volIter.setPosition(x+2,y+2,z+2);
			const uint8_t v111 = volIter.getMaxedVoxel(1);

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
				vertlist[0].setX(x + 0.5f * 2.0f);
				vertlist[0].setY(y);
				vertlist[0].setZ(z);
				normlist[0] = Vector3DFloat(v000 - v100,0.0,0.0);
				vertMaterials[0] = v000 | v100; //Because one of these is 0, the or operation takes the max.
			}
			if (edgeTable[iCubeIndex] & 2)
			{
				vertlist[1].setX(x + 1.0f * 2.0f);
				vertlist[1].setY(y + 0.5f * 2.0f);
				vertlist[1].setZ(z);
				vertMaterials[1] = v100 | v110;
				normlist[1] = Vector3DFloat(0.0,v100 - v110,0.0);
			}
			if (edgeTable[iCubeIndex] & 4)
			{
				vertlist[2].setX(x + 0.5f * 2.0f);
				vertlist[2].setY(y + 1.0f * 2.0f);
				vertlist[2].setZ(z);
				vertMaterials[2] = v010 | v110;
				normlist[2] = Vector3DFloat(v010 - v110,0.0,0.0);
			}
			if (edgeTable[iCubeIndex] & 8)
			{
				vertlist[3].setX(x);
				vertlist[3].setY(y + 0.5f * 2.0f);
				vertlist[3].setZ(z);
				vertMaterials[3] = v000 | v010;
				normlist[3] = Vector3DFloat(0.0,v000 - v010,0.0);
			}
			if (edgeTable[iCubeIndex] & 16)
			{
				vertlist[4].setX(x + 0.5f * 2.0f);
				vertlist[4].setY(y);
				vertlist[4].setZ(z + 1.0f * 2.0f);
				vertMaterials[4] = v001 | v101;
				normlist[4] = Vector3DFloat(v001 - v101,0.0,0.0);
			}
			if (edgeTable[iCubeIndex] & 32)
			{
				vertlist[5].setX(x + 1.0f * 2.0f);
				vertlist[5].setY(y + 0.5f * 2.0f);
				vertlist[5].setZ(z + 1.0f * 2.0f);
				vertMaterials[5] = v101 | v111;
				normlist[5] = Vector3DFloat(0.0,v101 - v111,0.0);
			}
			if (edgeTable[iCubeIndex] & 64)
			{
				vertlist[6].setX(x + 0.5f * 2.0f);
				vertlist[6].setY(y + 1.0f * 2.0f);
				vertlist[6].setZ(z + 1.0f * 2.0f);
				vertMaterials[6] = v011 | v111;
				normlist[6] = Vector3DFloat(v011 - v111,0.0,0.0);
			}
			if (edgeTable[iCubeIndex] & 128)
			{
				vertlist[7].setX(x);
				vertlist[7].setY(y + 0.5f * 2.0f);
				vertlist[7].setZ(z + 1.0f * 2.0f);
				vertMaterials[7] = v001 | v011;
				normlist[7] = Vector3DFloat(0.0,v001 - v011,0.0);
			}
			if (edgeTable[iCubeIndex] & 256)
			{
				vertlist[8].setX(x);
				vertlist[8].setY(y);
				vertlist[8].setZ(z + 0.5f * 2.0f);
				vertMaterials[8] = v000 | v001;
				normlist[8] = Vector3DFloat(0.0,0.0,v000 - v001);
			}
			if (edgeTable[iCubeIndex] & 512)
			{
				vertlist[9].setX(x + 1.0f * 2.0f);
				vertlist[9].setY(y);
				vertlist[9].setZ(z + 0.5f * 2.0f);
				vertMaterials[9] = v100 | v101;
				normlist[9] = Vector3DFloat(0.0,0.0,v100 - v101);
			}
			if (edgeTable[iCubeIndex] & 1024)
			{
				vertlist[10].setX(x + 1.0f * 2.0f);
				vertlist[10].setY(y + 1.0f * 2.0f);
				vertlist[10].setZ(z + 0.5f * 2.0f);
				vertMaterials[10] = v110 | v111;
				normlist[10] = Vector3DFloat(0.0,0.0,v110 - v111);
			}
			if (edgeTable[iCubeIndex] & 2048)
			{
				vertlist[11].setX(x);
				vertlist[11].setY(y + 1.0f * 2.0f);
				vertlist[11].setZ(z + 0.5f * 2.0f);
				vertMaterials[11] = v010 | v011;
				normlist[11] = Vector3DFloat(0.0,0.0,v010 - v011);
			}

			for (int i=0;triTable[iCubeIndex][i]!=-1;i+=3)
			{
				//The three vertices forming a triangle
				Vector3DFloat vertex0 = vertlist[triTable[iCubeIndex][i  ]] - offset;
				Vector3DFloat vertex1 = vertlist[triTable[iCubeIndex][i+1]] - offset;
				Vector3DFloat vertex2 = vertlist[triTable[iCubeIndex][i+2]] - offset;

				Vector3DFloat normal0 = normlist[triTable[iCubeIndex][i  ]];
				Vector3DFloat normal1 = normlist[triTable[iCubeIndex][i+1]];
				Vector3DFloat normal2 = normlist[triTable[iCubeIndex][i+2]];

				normal0.normalise();
				normal1.normalise();
				normal2.normalise();

				vertex0 += (normal0);
				vertex1 += (normal1);
				vertex2 += (normal2);

				//Cast to floats and divide by two.
				//const Vector3DFloat vertex0AsFloat = (static_cast<Vector3DFloat>(vertex0) / 2.0f) - offset;
				//const Vector3DFloat vertex1AsFloat = (static_cast<Vector3DFloat>(vertex1) / 2.0f) - offset;
				//const Vector3DFloat vertex2AsFloat = (static_cast<Vector3DFloat>(vertex2) / 2.0f) - offset;

				const uint8_t material0 = vertMaterials[triTable[iCubeIndex][i  ]];
				const uint8_t material1 = vertMaterials[triTable[iCubeIndex][i+1]];
				const uint8_t material2 = vertMaterials[triTable[iCubeIndex][i+2]];


				//If all the materials are the same, we just need one triangle for that material with all the alphas set high.
				SurfaceVertex surfaceVertex0Alpha1(vertex0,material0 + 0.1f,1.0f);
				surfaceVertex0Alpha1.setNormal(normal0);
				SurfaceVertex surfaceVertex1Alpha1(vertex1,material1 + 0.1f,1.0f);
				surfaceVertex1Alpha1.setNormal(normal1);
				SurfaceVertex surfaceVertex2Alpha1(vertex2,material2 + 0.1f,1.0f);
				surfaceVertex2Alpha1.setNormal(normal2);
				singleMaterialPatch->addTriangle(surfaceVertex0Alpha1, surfaceVertex1Alpha1, surfaceVertex2Alpha1);
			}//For each triangle
		//}//For each cell
		}
		}
		}

		//FIXME - can it happen that we have no vertices or triangles? Should exit early?


		//for(std::map<uint8_t, IndexedSurfacePatch*>::iterator iterPatch = surfacePatchMapResult.begin(); iterPatch != surfacePatchMapResult.end(); ++iterPatch)
		{

			/*std::vector<SurfaceVertex>::iterator iterSurfaceVertex = singleMaterialPatch->getVertices().begin();
			while(iterSurfaceVertex != singleMaterialPatch->getVertices().end())
			{
				Vector3DFloat tempNormal = computeNormal(volumeData, static_cast<Vector3DFloat>(iterSurfaceVertex->getPosition() + offset), SIMPLE);
				const_cast<SurfaceVertex&>(*iterSurfaceVertex).setNormal(tempNormal);
				++iterSurfaceVertex;
			}*/
		}
	}

	Vector3DFloat computeDecimatedNormal(BlockVolume<uint8_t>* volumeData, const Vector3DFloat& position, NormalGenerationMethod normalGenerationMethod)
	{
		const float posX = position.getX();
		const float posY = position.getY();
		const float posZ = position.getZ();

		const uint16_t floorX = static_cast<uint16_t>(posX);
		const uint16_t floorY = static_cast<uint16_t>(posY);
		const uint16_t floorZ = static_cast<uint16_t>(posZ);

		//Check all corners are within the volume, allowing a boundary for gradient estimation
		bool lowerCornerInside = volumeData->containsPoint(Vector3DInt32(floorX, floorY, floorZ),1);
		bool upperCornerInside = volumeData->containsPoint(Vector3DInt32(floorX+1, floorY+1, floorZ+1),1);
		if((!lowerCornerInside) || (!upperCornerInside))
		{
			normalGenerationMethod = SIMPLE;
		}

		Vector3DFloat result;

		BlockVolumeIterator<boost::uint8_t> volIter(*volumeData); //FIXME - save this somewhere - could be expensive to create?


		if(normalGenerationMethod == SOBEL)
		{
			volIter.setPosition(static_cast<uint16_t>(posX),static_cast<uint16_t>(posY),static_cast<uint16_t>(posZ));
			const Vector3DFloat gradFloor = computeSobelGradient(volIter);
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
			const Vector3DFloat gradCeil = computeSobelGradient(volIter);
			result = ((gradFloor + gradCeil) * -1.0f);
			if(result.lengthSquared() < 0.0001)
			{
				//Operation failed - fall back on simple gradient estimation
				normalGenerationMethod = SIMPLE;
			}
		}
		if(normalGenerationMethod == CENTRAL_DIFFERENCE)
		{
			volIter.setPosition(static_cast<uint16_t>(posX),static_cast<uint16_t>(posY),static_cast<uint16_t>(posZ));
			const Vector3DFloat gradFloor = computeCentralDifferenceGradient(volIter);
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
			const Vector3DFloat gradCeil = computeCentralDifferenceGradient(volIter);
			result = ((gradFloor + gradCeil) * -1.0f);
			if(result.lengthSquared() < 0.0001)
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
				result = Vector3DFloat(static_cast<float>(uFloor - uCeil),0.0,0.0);
			}
			else if((posY - floorY) > 0.25) //The result should be 0.0 or 0.5
			{
				uint8_t uCeil = volIter.peekVoxel0px1py0pz() > 0 ? 1 : 0;
				result = Vector3DFloat(0.0,static_cast<float>(uFloor - uCeil),0.0);
			}
			else if((posZ - floorZ) > 0.25) //The result should be 0.0 or 0.5
			{
				uint8_t uCeil = volIter.peekVoxel0px0py1pz() > 0 ? 1 : 0;
				result = Vector3DFloat(0.0, 0.0,static_cast<float>(uFloor - uCeil));					
			}
		}
		return result;
	}
}

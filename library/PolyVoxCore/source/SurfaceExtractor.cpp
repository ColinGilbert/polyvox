#include "SurfaceExtractor.h"

#include "IndexedSurfacePatch.h"
#include "PolyVoxImpl/MarchingCubesTables.h"
#include "SurfaceVertex.h"

namespace PolyVox
{
	SurfaceExtractor::SurfaceExtractor(Volume<uint8_t>& volData)
		:m_uLodLevel(0)
		,m_volData(volData)
		,m_iterVolume(volData)
	{
	}

	uint8_t SurfaceExtractor::getLodLevel(void)
	{
		return m_uLodLevel;
	}

	void SurfaceExtractor::setLodLevel(uint8_t uLodLevel)
	{
		m_uLodLevel = uLodLevel;

		//Step size is 2^uLodLevel
		m_uStepSize = 1 << uLodLevel;
	}

	POLYVOX_SHARED_PTR<IndexedSurfacePatch> SurfaceExtractor::extractSurfaceForRegion(Region region)
	{
		POLYVOX_SHARED_PTR<IndexedSurfacePatch> result(new IndexedSurfacePatch());

		extractSurfaceImpl(&m_volData, region, result.get());

		result->m_Region = region;

		return result;
	}

	uint32_t SurfaceExtractor::getIndex(uint32_t x, uint32_t y, uint32_t regionWidth)
	{
		return x + (y * (regionWidth+1));
	}

	////////////////////////////////////////////////////////////////////////////////
	// Level 0
	////////////////////////////////////////////////////////////////////////////////

	/*uint32_t SurfaceExtractor::computeBitmaskForSliceLevel0(VolumeSampler<uint8_t>& volIter, const Region& regSlice, const Vector3DFloat& offset, uint8_t* bitmask, uint8_t* previousBitmask)
	{
		uint32_t uNoOfNonEmptyCells = 0;

		//Iterate over each cell in the region
		for(uint16_t uYVolSpace = regSlice.getLowerCorner().getY(); uYVolSpace <= regSlice.getUpperCorner().getY(); uYVolSpace++)
		{
			for(uint16_t uXVolSpace = regSlice.getLowerCorner().getX(); uXVolSpace <= regSlice.getUpperCorner().getX(); uXVolSpace++)
			{		
				uint16_t uZVolSpace = regSlice.getLowerCorner().getZ();
				volIter.setPosition(uXVolSpace,uYVolSpace,uZVolSpace);
				//Current position
				const uint16_t uXRegSpace = volIter.getPosX() - offset.getX();
				const uint16_t uYRegSpace = volIter.getPosY() - offset.getY();

				//Determine the index into the edge table which tells us which vertices are inside of the surface
				uint8_t iCubeIndex = 0;

				if((uXVolSpace < volIter.getVolume().getWidth()-1) &&
					(uYVolSpace < volIter.getVolume().getHeight()-1) &&
					(uZVolSpace < volIter.getVolume().getDepth()-1))
				{
					bool isPrevXAvail = uXRegSpace > 0;
					bool isPrevYAvail = uYRegSpace > 0;
					bool isPrevZAvail = previousBitmask != 0;

					if(isPrevZAvail)
					{
						if(isPrevYAvail)
						{
							if(isPrevXAvail)
							{
								const uint8_t v111 = volIter.peekVoxel1px1py1pz();			

								//z
								uint8_t iPreviousCubeIndexZ = previousBitmask[getIndex(uXRegSpace,uYRegSpace, regSlice.width()+1)];
								iPreviousCubeIndexZ >>= 4;

								//y
								uint8_t iPreviousCubeIndexY = bitmask[getIndex(uXRegSpace,uYRegSpace-1, regSlice.width()+1)];
								iPreviousCubeIndexY &= 204; //204 = 128+64+8+4
								iPreviousCubeIndexY >>= 2;

								//x
								uint8_t iPreviousCubeIndexX = bitmask[getIndex(uXRegSpace-1,uYRegSpace, regSlice.width()+1)];
								iPreviousCubeIndexX &= 170; //170 = 128+32+8+2
								iPreviousCubeIndexX >>= 1;

								iCubeIndex = iPreviousCubeIndexX | iPreviousCubeIndexY | iPreviousCubeIndexZ;

								if (v111 == 0) iCubeIndex |= 128;
							}
							else //previous X not available
							{
								const uint8_t v011 = volIter.peekVoxel0px1py1pz();
								const uint8_t v111 = volIter.peekVoxel1px1py1pz();

								//z
								uint8_t iPreviousCubeIndexZ = previousBitmask[getIndex(uXRegSpace,uYRegSpace, regSlice.width()+1)];
								iPreviousCubeIndexZ >>= 4;

								//y
								uint8_t iPreviousCubeIndexY = bitmask[getIndex(uXRegSpace,uYRegSpace-1, regSlice.width()+1)];
								iPreviousCubeIndexY &= 192; //192 = 128 + 64
								iPreviousCubeIndexY >>= 2;

								iCubeIndex = iPreviousCubeIndexY | iPreviousCubeIndexZ;

								if (v011 == 0) iCubeIndex |= 64;
								if (v111 == 0) iCubeIndex |= 128;
							}
						}
						else //previous Y not available
						{
							if(isPrevXAvail)
							{
								const uint8_t v101 = volIter.peekVoxel1px0py1pz();
								const uint8_t v111 = volIter.peekVoxel1px1py1pz();			

								//z
								uint8_t iPreviousCubeIndexZ = previousBitmask[getIndex(uXRegSpace,uYRegSpace, regSlice.width()+1)];
								iPreviousCubeIndexZ >>= 4;

								//x
								uint8_t iPreviousCubeIndexX = bitmask[getIndex(uXRegSpace-1,uYRegSpace, regSlice.width()+1)];
								iPreviousCubeIndexX &= 160; //160 = 128+32
								iPreviousCubeIndexX >>= 1;

								iCubeIndex = iPreviousCubeIndexX | iPreviousCubeIndexZ;

								if (v101 == 0) iCubeIndex |= 32;
								if (v111 == 0) iCubeIndex |= 128;
							}
							else //previous X not available
							{
								const uint8_t v001 = volIter.peekVoxel0px0py1pz();
								const uint8_t v101 = volIter.peekVoxel1px0py1pz();
								const uint8_t v011 = volIter.peekVoxel0px1py1pz();
								const uint8_t v111 = volIter.peekVoxel1px1py1pz();	

								//z
								uint8_t iPreviousCubeIndexZ = previousBitmask[getIndex(uXRegSpace,uYRegSpace, regSlice.width()+1)];
								iCubeIndex = iPreviousCubeIndexZ >> 4;

								if (v001 == 0) iCubeIndex |= 16;
								if (v101 == 0) iCubeIndex |= 32;
								if (v011 == 0) iCubeIndex |= 64;
								if (v111 == 0) iCubeIndex |= 128;
							}
						}
					}
					else //previous Z not available
					{
						if(isPrevYAvail)
						{
							if(isPrevXAvail)
							{
								const uint8_t v110 = volIter.peekVoxel1px1py0pz();
								const uint8_t v111 = volIter.peekVoxel1px1py1pz();

								//y
								uint8_t iPreviousCubeIndexY = bitmask[getIndex(uXRegSpace,uYRegSpace-1, regSlice.width()+1)];
								iPreviousCubeIndexY &= 204; //204 = 128+64+8+4
								iPreviousCubeIndexY >>= 2;

								//x
								uint8_t iPreviousCubeIndexX = bitmask[getIndex(uXRegSpace-1,uYRegSpace, regSlice.width()+1)];
								iPreviousCubeIndexX &= 170; //170 = 128+32+8+2
								iPreviousCubeIndexX >>= 1;

								iCubeIndex = iPreviousCubeIndexX | iPreviousCubeIndexY;

								if (v110 == 0) iCubeIndex |= 8;
								if (v111 == 0) iCubeIndex |= 128;
							}
							else //previous X not available
							{
								const uint8_t v010 = volIter.peekVoxel0px1py0pz();
								const uint8_t v110 = volIter.peekVoxel1px1py0pz();

								const uint8_t v011 = volIter.peekVoxel0px1py1pz();
								const uint8_t v111 = volIter.peekVoxel1px1py1pz();

								//y
								uint8_t iPreviousCubeIndexY = bitmask[getIndex(uXRegSpace,uYRegSpace-1, regSlice.width()+1)];
								iPreviousCubeIndexY &= 204; //204 = 128+64+8+4
								iPreviousCubeIndexY >>= 2;

								iCubeIndex = iPreviousCubeIndexY;

								if (v010 == 0) iCubeIndex |= 4;
								if (v110 == 0) iCubeIndex |= 8;
								if (v011 == 0) iCubeIndex |= 64;
								if (v111 == 0) iCubeIndex |= 128;
							}
						}
						else //previous Y not available
						{
							if(isPrevXAvail)
							{
								const uint8_t v100 = volIter.peekVoxel1px0py0pz();
								const uint8_t v110 = volIter.peekVoxel1px1py0pz();

								const uint8_t v101 = volIter.peekVoxel1px0py1pz();
								const uint8_t v111 = volIter.peekVoxel1px1py1pz();			

								//x
								uint8_t iPreviousCubeIndexX = bitmask[getIndex(uXRegSpace-1,uYRegSpace, regSlice.width()+1)];
								iPreviousCubeIndexX &= 170; //170 = 128+32+8+2
								iPreviousCubeIndexX >>= 1;

								iCubeIndex = iPreviousCubeIndexX;

								if (v100 == 0) iCubeIndex |= 2;	
								if (v110 == 0) iCubeIndex |= 8;
								if (v101 == 0) iCubeIndex |= 32;
								if (v111 == 0) iCubeIndex |= 128;
							}
							else //previous X not available
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
								if (v010 == 0) iCubeIndex |= 4;
								if (v110 == 0) iCubeIndex |= 8;
								if (v001 == 0) iCubeIndex |= 16;
								if (v101 == 0) iCubeIndex |= 32;
								if (v011 == 0) iCubeIndex |= 64;
								if (v111 == 0) iCubeIndex |= 128;
							}
						}
					}
				}
				else //We're at the edge of the volume - use bounds checking.
				{	
					const uint8_t v000 = volIter.getVoxel();
					const uint8_t v100 = volIter.getVolume().getVoxelAtWithBoundCheck(uXVolSpace+1, uYVolSpace  , uZVolSpace  );
					const uint8_t v010 = volIter.getVolume().getVoxelAtWithBoundCheck(uXVolSpace  , uYVolSpace+1, uZVolSpace  );
					const uint8_t v110 = volIter.getVolume().getVoxelAtWithBoundCheck(uXVolSpace+1, uYVolSpace+1, uZVolSpace  );

					const uint8_t v001 = volIter.getVolume().getVoxelAtWithBoundCheck(uXVolSpace  , uYVolSpace  , uZVolSpace+1);
					const uint8_t v101 = volIter.getVolume().getVoxelAtWithBoundCheck(uXVolSpace+1, uYVolSpace  , uZVolSpace+1);
					const uint8_t v011 = volIter.getVolume().getVoxelAtWithBoundCheck(uXVolSpace  , uYVolSpace+1, uZVolSpace+1);
					const uint8_t v111 = volIter.getVolume().getVoxelAtWithBoundCheck(uXVolSpace+1, uYVolSpace+1, uZVolSpace+1);				

					if (v000 == 0) iCubeIndex |= 1;
					if (v100 == 0) iCubeIndex |= 2;
					if (v010 == 0) iCubeIndex |= 4;
					if (v110 == 0) iCubeIndex |= 8;
					if (v001 == 0) iCubeIndex |= 16;
					if (v101 == 0) iCubeIndex |= 32;
					if (v011 == 0) iCubeIndex |= 64;
					if (v111 == 0) iCubeIndex |= 128;
				}

				//Save the bitmask
				bitmask[getIndex(uXRegSpace,uYRegSpace, regSlice.width()+1)] = iCubeIndex;

				if(edgeTable[iCubeIndex] != 0)
				{
					++uNoOfNonEmptyCells;
				}

			}
		}

		return uNoOfNonEmptyCells;
	}*/

	////////////////////////////////////////////////////////////////////////////////
	// Level 1
	////////////////////////////////////////////////////////////////////////////////

	void SurfaceExtractor::extractSurfaceImpl(Volume<uint8_t>* volumeData, Region region, IndexedSurfacePatch* singleMaterialPatch)
	{	
		singleMaterialPatch->clear();

		//For edge indices
		//FIXME - do the slices need to be this big? Surely for a decimated mesh they can be smaller?
		//FIXME - Instead of region.width()+2 we used to use POLYVOX_REGION_SIDE_LENGTH+1
		//Normally POLYVOX_REGION_SIDE_LENGTH is the same as region.width() (often 32) but at the
		//edges of the volume it is 1 smaller. Need to think what values really belong here.
		m_pPreviousVertexIndicesX = new int32_t[(region.width()+8) * (region.height()+8)];
		m_pPreviousVertexIndicesY = new int32_t[(region.width()+8) * (region.height()+8)];
		m_pPreviousVertexIndicesZ = new int32_t[(region.width()+8) * (region.height()+8)];
		m_pCurrentVertexIndicesX = new int32_t[(region.width()+8) * (region.height()+8)];
		m_pCurrentVertexIndicesY = new int32_t[(region.width()+8) * (region.height()+8)];
		m_pCurrentVertexIndicesZ = new int32_t[(region.width()+8) * (region.height()+8)];

		//Cell bitmasks
		m_pPreviousBitmask = new uint8_t[(region.width()+8) * (region.height()+8)];
		m_pCurrentBitmask = new uint8_t[(region.width()+8) * (region.height()+8)];

		//When generating the mesh for a region we actually look outside it in the
		// back, bottom, right direction. Protect against access violations by cropping region here
		Region regVolume = volumeData->getEnclosingRegion();
		if(m_uLodLevel > 0)
		{
			regVolume.setUpperCorner(regVolume.getUpperCorner() - Vector3DInt32(2*m_uStepSize-1,2*m_uStepSize-1,2*m_uStepSize-1));
		}
		region.cropTo(regVolume);

		//Offset from volume corner
		const Vector3DFloat offset = static_cast<Vector3DFloat>(region.getLowerCorner());

		//Create a region corresponding to the first slice
		Region regSlice0(region);
		Vector3DInt32 v3dUpperCorner = regSlice0.getUpperCorner();
		v3dUpperCorner.setZ(regSlice0.getLowerCorner().getZ()); //Set the upper z to the lower z to make it one slice thick.
		regSlice0.setUpperCorner(v3dUpperCorner);
		Region regSlice1 = regSlice0;
		
		//Iterator to access the volume data
		VolumeSampler<uint8_t> volIter(*volumeData);	

		uint32_t uNoOfNonEmptyCellsForSlice0 = 0;
		uint32_t uNoOfNonEmptyCellsForSlice1 = 0;

		bool isFirstSliceDone = false;

		for(uint32_t uSlice = 0; ((uSlice <= region.depth()) && (uSlice + offset.getZ() <= regVolume.getUpperCorner().getZ())); uSlice += m_uStepSize)
		{
			
			uNoOfNonEmptyCellsForSlice1 = computeBitmaskForSlice(volIter, regSlice1, offset);

			if(uNoOfNonEmptyCellsForSlice1 != 0)
			{
				generateVerticesForSlice(volIter, regSlice1, offset, singleMaterialPatch);				
			}
			
			if(isFirstSliceDone)
			{
				if((uNoOfNonEmptyCellsForSlice0 != 0) || (uNoOfNonEmptyCellsForSlice1 != 0))
				{
					generateIndicesForSlice(volIter, regSlice0, singleMaterialPatch, offset);
				}
			}

			std::swap(uNoOfNonEmptyCellsForSlice0, uNoOfNonEmptyCellsForSlice1);
			std::swap(m_pPreviousBitmask, m_pCurrentBitmask);
			std::swap(m_pPreviousVertexIndicesX, m_pCurrentVertexIndicesX);
			std::swap(m_pPreviousVertexIndicesY, m_pCurrentVertexIndicesY);
			std::swap(m_pPreviousVertexIndicesZ, m_pCurrentVertexIndicesZ);

			regSlice0 = regSlice1;
			regSlice1.shift(Vector3DInt32(0,0,m_uStepSize));

			isFirstSliceDone = true;
		}

		delete[] m_pPreviousBitmask;
		delete[] m_pCurrentBitmask;
		delete[] m_pPreviousVertexIndicesX;
		delete[] m_pCurrentVertexIndicesX;
		delete[] m_pPreviousVertexIndicesY;
		delete[] m_pCurrentVertexIndicesY;
		delete[] m_pPreviousVertexIndicesZ;
		delete[] m_pCurrentVertexIndicesZ;
	}

	uint32_t SurfaceExtractor::computeBitmaskForSlice(VolumeSampler<uint8_t>& volIter, const Region& regSlice, const Vector3DFloat& offset)
	{
		uint32_t uNoOfNonEmptyCells = 0;

		//Iterate over each cell in the region
		for(uint16_t uYVolSpace = regSlice.getLowerCorner().getY(); uYVolSpace <= regSlice.getUpperCorner().getY(); uYVolSpace += m_uStepSize)
		{
			for(uint16_t uXVolSpace = regSlice.getLowerCorner().getX(); uXVolSpace <= regSlice.getUpperCorner().getX(); uXVolSpace += m_uStepSize)
			{	
				uint16_t uZVolSpace = regSlice.getLowerCorner().getZ();
				//Current position
				volIter.setPosition(uXVolSpace,uYVolSpace,uZVolSpace);

				const uint16_t uXRegSpace = uXVolSpace - offset.getX();
				const uint16_t uYRegSpace = uYVolSpace - offset.getY();
				const uint16_t uZRegSpace = uZVolSpace - offset.getZ();

				//Determine the index into the edge table which tells us which vertices are inside of the surface
				uint8_t iCubeIndex = 0;

				if((uXVolSpace < volIter.getVolume().getWidth()-m_uStepSize) &&
					(uYVolSpace < volIter.getVolume().getHeight()-m_uStepSize) &&
					(uZVolSpace < volIter.getVolume().getDepth()-m_uStepSize))
				{
				bool isPrevXAvail = uXRegSpace > 0;
				bool isPrevYAvail = uYRegSpace > 0;
				bool isPrevZAvail = uZRegSpace > 0;

				if(isPrevZAvail)
				{
					if(isPrevYAvail)
					{
						if(isPrevXAvail)
						{
							volIter.setPosition(uXVolSpace+m_uStepSize,uYVolSpace+m_uStepSize,uZVolSpace+m_uStepSize);
							const uint8_t v111 = volIter.getSubSampledVoxel(m_uLodLevel);	

							//z
							uint8_t iPreviousCubeIndexZ = m_pPreviousBitmask[getIndex(uXRegSpace,uYRegSpace, regSlice.width()+1)];
							iPreviousCubeIndexZ >>= 4;

							//y
							uint8_t iPreviousCubeIndexY = m_pCurrentBitmask[getIndex(uXRegSpace,uYRegSpace-m_uStepSize, regSlice.width()+1)];
							iPreviousCubeIndexY &= 192; //192 = 128 + 64
							iPreviousCubeIndexY >>= 2;

							//x
							uint8_t iPreviousCubeIndexX = m_pCurrentBitmask[getIndex(uXRegSpace-m_uStepSize,uYRegSpace, regSlice.width()+1)];
							iPreviousCubeIndexX &= 128;
							iPreviousCubeIndexX >>= 1;

							iCubeIndex = iPreviousCubeIndexX | iPreviousCubeIndexY | iPreviousCubeIndexZ;

							if (v111 == 0) iCubeIndex |= 128;
						}
						else //previous X not available
						{
							volIter.setPosition(uXVolSpace,uYVolSpace+m_uStepSize,uZVolSpace+m_uStepSize);
							const uint8_t v011 = volIter.getSubSampledVoxel(m_uLodLevel);
							volIter.setPosition(uXVolSpace+m_uStepSize,uYVolSpace+m_uStepSize,uZVolSpace+m_uStepSize);
							const uint8_t v111 = volIter.getSubSampledVoxel(m_uLodLevel);	

							//z
							uint8_t iPreviousCubeIndexZ = m_pPreviousBitmask[getIndex(uXRegSpace,uYRegSpace, regSlice.width()+1)];
							iPreviousCubeIndexZ >>= 4;

							//y
							uint8_t iPreviousCubeIndexY = m_pCurrentBitmask[getIndex(uXRegSpace,uYRegSpace-m_uStepSize, regSlice.width()+1)];
							iPreviousCubeIndexY &= 192; //192 = 128 + 64
							iPreviousCubeIndexY >>= 2;

							iCubeIndex = iPreviousCubeIndexY | iPreviousCubeIndexZ;

							if (v011 == 0) iCubeIndex |= 64;
							if (v111 == 0) iCubeIndex |= 128;
						}
					}
					else //previous Y not available
					{
						if(isPrevXAvail)
						{
							volIter.setPosition(uXVolSpace+m_uStepSize,uYVolSpace,uZVolSpace+m_uStepSize);
							const uint8_t v101 = volIter.getSubSampledVoxel(m_uLodLevel);
							volIter.setPosition(uXVolSpace+m_uStepSize,uYVolSpace+m_uStepSize,uZVolSpace+m_uStepSize);
							const uint8_t v111 = volIter.getSubSampledVoxel(m_uLodLevel);	

							//z
							uint8_t iPreviousCubeIndexZ = m_pPreviousBitmask[getIndex(uXRegSpace,uYRegSpace, regSlice.width()+1)];
							iPreviousCubeIndexZ >>= 4;

							//x
							uint8_t iPreviousCubeIndexX = m_pCurrentBitmask[getIndex(uXRegSpace-m_uStepSize,uYRegSpace, regSlice.width()+1)];
							iPreviousCubeIndexX &= 160; //160 = 128+32
							iPreviousCubeIndexX >>= 1;

							iCubeIndex = iPreviousCubeIndexX | iPreviousCubeIndexZ;

							if (v101 == 0) iCubeIndex |= 32;
							if (v111 == 0) iCubeIndex |= 128;
						}
						else //previous X not available
						{
							volIter.setPosition(uXVolSpace,uYVolSpace,uZVolSpace+m_uStepSize);
							const uint8_t v001 = volIter.getSubSampledVoxel(m_uLodLevel);
							volIter.setPosition(uXVolSpace+m_uStepSize,uYVolSpace,uZVolSpace+m_uStepSize);
							const uint8_t v101 = volIter.getSubSampledVoxel(m_uLodLevel);
							volIter.setPosition(uXVolSpace,uYVolSpace+m_uStepSize,uZVolSpace+m_uStepSize);
							const uint8_t v011 = volIter.getSubSampledVoxel(m_uLodLevel);
							volIter.setPosition(uXVolSpace+m_uStepSize,uYVolSpace+m_uStepSize,uZVolSpace+m_uStepSize);
							const uint8_t v111 = volIter.getSubSampledVoxel(m_uLodLevel);	

							//z
							uint8_t iPreviousCubeIndexZ = m_pPreviousBitmask[getIndex(uXRegSpace,uYRegSpace, regSlice.width()+1)];
							iCubeIndex = iPreviousCubeIndexZ >> 4;

							if (v001 == 0) iCubeIndex |= 16;
							if (v101 == 0) iCubeIndex |= 32;
							if (v011 == 0) iCubeIndex |= 64;
							if (v111 == 0) iCubeIndex |= 128;
						}
					}
				}
				else //previous Z not available
				{
					if(isPrevYAvail)
					{
						if(isPrevXAvail)
						{
							volIter.setPosition(uXVolSpace+m_uStepSize,uYVolSpace+m_uStepSize,uZVolSpace);
							const uint8_t v110 = volIter.getSubSampledVoxel(m_uLodLevel);

							volIter.setPosition(uXVolSpace+m_uStepSize,uYVolSpace+m_uStepSize,uZVolSpace+m_uStepSize);
							const uint8_t v111 = volIter.getSubSampledVoxel(m_uLodLevel);

							//y
							uint8_t iPreviousCubeIndexY = m_pCurrentBitmask[getIndex(uXRegSpace,uYRegSpace-m_uStepSize, regSlice.width()+1)];
							iPreviousCubeIndexY &= 204; //204 = 128+64+8+4
							iPreviousCubeIndexY >>= 2;

							//x
							uint8_t iPreviousCubeIndexX = m_pCurrentBitmask[getIndex(uXRegSpace-m_uStepSize,uYRegSpace, regSlice.width()+1)];
							iPreviousCubeIndexX &= 170; //170 = 128+32+8+2
							iPreviousCubeIndexX >>= 1;

							iCubeIndex = iPreviousCubeIndexX | iPreviousCubeIndexY;

							if (v110 == 0) iCubeIndex |= 8;
							if (v111 == 0) iCubeIndex |= 128;
						}
						else //previous X not available
						{
							volIter.setPosition(uXVolSpace,uYVolSpace+m_uStepSize,uZVolSpace);
							const uint8_t v010 = volIter.getSubSampledVoxel(m_uLodLevel);
							volIter.setPosition(uXVolSpace+m_uStepSize,uYVolSpace+m_uStepSize,uZVolSpace);
							const uint8_t v110 = volIter.getSubSampledVoxel(m_uLodLevel);

							volIter.setPosition(uXVolSpace,uYVolSpace+m_uStepSize,uZVolSpace+m_uStepSize);
							const uint8_t v011 = volIter.getSubSampledVoxel(m_uLodLevel);
							volIter.setPosition(uXVolSpace+m_uStepSize,uYVolSpace+m_uStepSize,uZVolSpace+m_uStepSize);
							const uint8_t v111 = volIter.getSubSampledVoxel(m_uLodLevel);

							//y
							uint8_t iPreviousCubeIndexY = m_pCurrentBitmask[getIndex(uXRegSpace,uYRegSpace-m_uStepSize, regSlice.width()+1)];
							iPreviousCubeIndexY &= 204; //204 = 128+64+8+4
							iPreviousCubeIndexY >>= 2;

							iCubeIndex = iPreviousCubeIndexY;

							if (v010 == 0) iCubeIndex |= 4;
							if (v110 == 0) iCubeIndex |= 8;
							if (v011 == 0) iCubeIndex |= 64;
							if (v111 == 0) iCubeIndex |= 128;
						}
					}
					else //previous Y not available
					{
						if(isPrevXAvail)
						{
							volIter.setPosition(uXVolSpace+m_uStepSize,uYVolSpace,uZVolSpace);
							const uint8_t v100 = volIter.getSubSampledVoxel(m_uLodLevel);
							volIter.setPosition(uXVolSpace+m_uStepSize,uYVolSpace+m_uStepSize,uZVolSpace);
							const uint8_t v110 = volIter.getSubSampledVoxel(m_uLodLevel);

							volIter.setPosition(uXVolSpace+m_uStepSize,uYVolSpace,uZVolSpace+m_uStepSize);
							const uint8_t v101 = volIter.getSubSampledVoxel(m_uLodLevel);
							volIter.setPosition(uXVolSpace+m_uStepSize,uYVolSpace+m_uStepSize,uZVolSpace+m_uStepSize);
							const uint8_t v111 = volIter.getSubSampledVoxel(m_uLodLevel);

							//x
							uint8_t iPreviousCubeIndexX = m_pCurrentBitmask[getIndex(uXRegSpace-m_uStepSize,uYRegSpace, regSlice.width()+1)];
							iPreviousCubeIndexX &= 170; //170 = 128+32+8+2
							iPreviousCubeIndexX >>= 1;

							iCubeIndex = iPreviousCubeIndexX;

							if (v100 == 0) iCubeIndex |= 2;	
							if (v110 == 0) iCubeIndex |= 8;
							if (v101 == 0) iCubeIndex |= 32;
							if (v111 == 0) iCubeIndex |= 128;
						}
						else //previous X not available
						{
							volIter.setPosition(uXVolSpace,uYVolSpace,uZVolSpace);
							const uint8_t v000 = volIter.getSubSampledVoxel(m_uLodLevel);
							volIter.setPosition(uXVolSpace+m_uStepSize,uYVolSpace,uZVolSpace);
							const uint8_t v100 = volIter.getSubSampledVoxel(m_uLodLevel);
							volIter.setPosition(uXVolSpace,uYVolSpace+m_uStepSize,uZVolSpace);
							const uint8_t v010 = volIter.getSubSampledVoxel(m_uLodLevel);
							volIter.setPosition(uXVolSpace+m_uStepSize,uYVolSpace+m_uStepSize,uZVolSpace);
							const uint8_t v110 = volIter.getSubSampledVoxel(m_uLodLevel);

							volIter.setPosition(uXVolSpace,uYVolSpace,uZVolSpace+m_uStepSize);
							const uint8_t v001 = volIter.getSubSampledVoxel(m_uLodLevel);
							volIter.setPosition(uXVolSpace+m_uStepSize,uYVolSpace,uZVolSpace+m_uStepSize);
							const uint8_t v101 = volIter.getSubSampledVoxel(m_uLodLevel);
							volIter.setPosition(uXVolSpace,uYVolSpace+m_uStepSize,uZVolSpace+m_uStepSize);
							const uint8_t v011 = volIter.getSubSampledVoxel(m_uLodLevel);
							volIter.setPosition(uXVolSpace+m_uStepSize,uYVolSpace+m_uStepSize,uZVolSpace+m_uStepSize);
							const uint8_t v111 = volIter.getSubSampledVoxel(m_uLodLevel);		

							if (v000 == 0) iCubeIndex |= 1;
							if (v100 == 0) iCubeIndex |= 2;
							if (v010 == 0) iCubeIndex |= 4;
							if (v110 == 0) iCubeIndex |= 8;
							if (v001 == 0) iCubeIndex |= 16;
							if (v101 == 0) iCubeIndex |= 32;
							if (v011 == 0) iCubeIndex |= 64;
							if (v111 == 0) iCubeIndex |= 128;
						}
					}
				}
				}
				else
				{
					if(m_uLodLevel == 0)
					{
						const uint8_t v000 = volIter.getVoxel();
						const uint8_t v100 = volIter.getVolume().getVoxelAtWithBoundCheck(uXVolSpace+1, uYVolSpace  , uZVolSpace  );
						const uint8_t v010 = volIter.getVolume().getVoxelAtWithBoundCheck(uXVolSpace  , uYVolSpace+1, uZVolSpace  );
						const uint8_t v110 = volIter.getVolume().getVoxelAtWithBoundCheck(uXVolSpace+1, uYVolSpace+1, uZVolSpace  );

						const uint8_t v001 = volIter.getVolume().getVoxelAtWithBoundCheck(uXVolSpace  , uYVolSpace  , uZVolSpace+1);
						const uint8_t v101 = volIter.getVolume().getVoxelAtWithBoundCheck(uXVolSpace+1, uYVolSpace  , uZVolSpace+1);
						const uint8_t v011 = volIter.getVolume().getVoxelAtWithBoundCheck(uXVolSpace  , uYVolSpace+1, uZVolSpace+1);
						const uint8_t v111 = volIter.getVolume().getVoxelAtWithBoundCheck(uXVolSpace+1, uYVolSpace+1, uZVolSpace+1);

						if (v000 == 0) iCubeIndex |= 1;
						if (v100 == 0) iCubeIndex |= 2;
						if (v010 == 0) iCubeIndex |= 4;
						if (v110 == 0) iCubeIndex |= 8;
						if (v001 == 0) iCubeIndex |= 16;
						if (v101 == 0) iCubeIndex |= 32;
						if (v011 == 0) iCubeIndex |= 64;
						if (v111 == 0) iCubeIndex |= 128;
					}
					else
					{
						const uint8_t v000 = volIter.getSubSampledVoxelWithBoundsCheck(m_uLodLevel);

						volIter.setPosition(uXVolSpace+1, uYVolSpace  , uZVolSpace  );
						const uint8_t v100 = volIter.getSubSampledVoxelWithBoundsCheck(m_uLodLevel);

						volIter.setPosition(uXVolSpace  , uYVolSpace+1, uZVolSpace  );
						const uint8_t v010 = volIter.getSubSampledVoxelWithBoundsCheck(m_uLodLevel);

						volIter.setPosition(uXVolSpace+1, uYVolSpace+1, uZVolSpace  );
						const uint8_t v110 = volIter.getSubSampledVoxelWithBoundsCheck(m_uLodLevel);


						volIter.setPosition(uXVolSpace  , uYVolSpace  , uZVolSpace+1);
						const uint8_t v001 = volIter.getSubSampledVoxelWithBoundsCheck(m_uLodLevel);

						volIter.setPosition(uXVolSpace+1, uYVolSpace  , uZVolSpace+1);
						const uint8_t v101 = volIter.getSubSampledVoxelWithBoundsCheck(m_uLodLevel);

						volIter.setPosition(uXVolSpace  , uYVolSpace+1, uZVolSpace+1);
						const uint8_t v011 = volIter.getSubSampledVoxelWithBoundsCheck(m_uLodLevel);

						volIter.setPosition(uXVolSpace+1, uYVolSpace+1, uZVolSpace+1);				
						const uint8_t v111 = volIter.getSubSampledVoxelWithBoundsCheck(m_uLodLevel);				

						if (v000 == 0) iCubeIndex |= 1;
						if (v100 == 0) iCubeIndex |= 2;
						if (v010 == 0) iCubeIndex |= 4;
						if (v110 == 0) iCubeIndex |= 8;
						if (v001 == 0) iCubeIndex |= 16;
						if (v101 == 0) iCubeIndex |= 32;
						if (v011 == 0) iCubeIndex |= 64;
						if (v111 == 0) iCubeIndex |= 128;
					}
				}

				//Save the bitmask
				m_pCurrentBitmask[getIndex(uXRegSpace,uYVolSpace- offset.getY(), regSlice.width()+1)] = iCubeIndex;

				if(edgeTable[iCubeIndex] != 0)
				{
					++uNoOfNonEmptyCells;
				}
				
			}//For each cell
		}

		return uNoOfNonEmptyCells;
	}

	void SurfaceExtractor::generateVerticesForSlice(VolumeSampler<uint8_t>& volIter, Region& regSlice, const Vector3DFloat& offset, IndexedSurfacePatch* singleMaterialPatch)
	{
		//Iterate over each cell in the region
		for(uint16_t uYVolSpace = regSlice.getLowerCorner().getY(); uYVolSpace <= regSlice.getUpperCorner().getY(); uYVolSpace += m_uStepSize)
		{
			for(uint16_t uXVolSpace = regSlice.getLowerCorner().getX(); uXVolSpace <= regSlice.getUpperCorner().getX(); uXVolSpace += m_uStepSize)
			{		
				uint16_t uZVolSpace = regSlice.getLowerCorner().getZ();

				//Current position
				const uint16_t uXRegSpace = uXVolSpace - offset.getX();
				const uint16_t uYRegSpace = uYVolSpace - offset.getY();
				const uint16_t uZRegSpace = uZVolSpace - offset.getZ();

				//Current position
				//const uint16_t z = regSlice.getLowerCorner().getZ();

				volIter.setPosition(uXVolSpace,uYVolSpace,uZVolSpace);
				const uint8_t v000 = volIter.getSubSampledVoxel(m_uLodLevel);

				//Determine the index into the edge table which tells us which vertices are inside of the surface
				uint8_t iCubeIndex = m_pCurrentBitmask[getIndex(uXVolSpace - offset.getX(),uYVolSpace - offset.getY(), regSlice.width()+1)];

				/* Cube is entirely in/out of the surface */
				if (edgeTable[iCubeIndex] == 0)
				{
					continue;
				}

				/* Find the vertices where the surface intersects the cube */
				if (edgeTable[iCubeIndex] & 1)
				{
					if(uXVolSpace != regSlice.getUpperCorner().getX())
					{
						volIter.setPosition(uXVolSpace + m_uStepSize,uYVolSpace,uZVolSpace);
						const uint8_t v100 = volIter.getSubSampledVoxel(m_uLodLevel);
						const Vector3DFloat v3dPosition(uXVolSpace - offset.getX() + 0.5f * m_uStepSize, uYVolSpace - offset.getY(), uZVolSpace - offset.getZ());
						const Vector3DFloat v3dNormal(v000 > v100 ? 1.0f : -1.0f,0.0,0.0);
						const uint8_t uMaterial = v000 | v100; //Because one of these is 0, the or operation takes the max.
						SurfaceVertex surfaceVertex(v3dPosition, v3dNormal, uMaterial);
						uint32_t uLastVertexIndex = singleMaterialPatch->addVertex(surfaceVertex);
						m_pCurrentVertexIndicesX[getIndex(uXVolSpace - offset.getX(),uYVolSpace - offset.getY(), regSlice.width()+1)] = uLastVertexIndex;
					}
				}
				if (edgeTable[iCubeIndex] & 8)
				{
					if(uYVolSpace != regSlice.getUpperCorner().getY())
					{
						volIter.setPosition(uXVolSpace,uYVolSpace + m_uStepSize,uZVolSpace);
						const uint8_t v010 = volIter.getSubSampledVoxel(m_uLodLevel);
						const Vector3DFloat v3dPosition(uXVolSpace - offset.getX(), uYVolSpace - offset.getY() + 0.5f * m_uStepSize, uZVolSpace - offset.getZ());
						const Vector3DFloat v3dNormal(0.0,v000 > v010 ? 1.0f : -1.0f,0.0);
						const uint8_t uMaterial = v000 | v010; //Because one of these is 0, the or operation takes the max.
						SurfaceVertex surfaceVertex(v3dPosition, v3dNormal, uMaterial);
						uint32_t uLastVertexIndex = singleMaterialPatch->addVertex(surfaceVertex);
						m_pCurrentVertexIndicesY[getIndex(uXVolSpace - offset.getX(),uYVolSpace - offset.getY(), regSlice.width()+1)] = uLastVertexIndex;
					}
				}
				if (edgeTable[iCubeIndex] & 256)
				{
					//if(z != regSlice.getUpperCorner.getZ())
					{
						volIter.setPosition(uXVolSpace,uYVolSpace,uZVolSpace + m_uStepSize);
						const uint8_t v001 = volIter.getSubSampledVoxel(m_uLodLevel);
						const Vector3DFloat v3dPosition(uXVolSpace - offset.getX(), uYVolSpace - offset.getY(), uZVolSpace - offset.getZ() + 0.5f * m_uStepSize);
						const Vector3DFloat v3dNormal(0.0,0.0,v000 > v001 ? 1.0f : -1.0f);
						const uint8_t uMaterial = v000 | v001; //Because one of these is 0, the or operation takes the max.
						const SurfaceVertex surfaceVertex(v3dPosition, v3dNormal, uMaterial);
						uint32_t uLastVertexIndex = singleMaterialPatch->addVertex(surfaceVertex);
						m_pCurrentVertexIndicesZ[getIndex(uXVolSpace - offset.getX(),uYVolSpace - offset.getY(), regSlice.width()+1)] = uLastVertexIndex;
					}
				}
			}//For each cell
		}
	}

	void SurfaceExtractor::generateIndicesForSlice(VolumeSampler<uint8_t>& volIter, const Region& regSlice, IndexedSurfacePatch* singleMaterialPatch, const Vector3DFloat& offset)
	{
		uint32_t indlist[12];

		for(uint16_t uYVolSpace = regSlice.getLowerCorner().getY(); uYVolSpace < regSlice.getUpperCorner().getY(); uYVolSpace += m_uStepSize)
		{
			for(uint16_t uXVolSpace = regSlice.getLowerCorner().getX(); uXVolSpace < regSlice.getUpperCorner().getX(); uXVolSpace += m_uStepSize)
			{		
				uint16_t uZVolSpace = regSlice.getLowerCorner().getZ();
				volIter.setPosition(uXVolSpace,uYVolSpace,uZVolSpace);	

				//Current position
				const uint16_t uXRegSpace = volIter.getPosX() - offset.getX();
				const uint16_t uYRegSpace = volIter.getPosY() - offset.getY();
				const uint16_t uZRegSpace = volIter.getPosZ() - offset.getZ();

				//Determine the index into the edge table which tells us which vertices are inside of the surface
				uint8_t iCubeIndex = m_pPreviousBitmask[getIndex(uXRegSpace,uYRegSpace, regSlice.width()+1)];

				/* Cube is entirely in/out of the surface */
				if (edgeTable[iCubeIndex] == 0)
				{
					continue;
				}

				/* Find the vertices where the surface intersects the cube */
				if (edgeTable[iCubeIndex] & 1)
				{
					indlist[0] = m_pPreviousVertexIndicesX[getIndex(uXRegSpace,uYRegSpace, regSlice.width()+1)];
					assert(indlist[0] != -1);
				}
				if (edgeTable[iCubeIndex] & 2)
				{
					indlist[1] = m_pPreviousVertexIndicesY[getIndex(uXRegSpace+m_uStepSize,uYRegSpace, regSlice.width()+1)];
					assert(indlist[1] != -1);
				}
				if (edgeTable[iCubeIndex] & 4)
				{
					indlist[2] = m_pPreviousVertexIndicesX[getIndex(uXRegSpace,uYRegSpace+m_uStepSize, regSlice.width()+1)];
					assert(indlist[2] != -1);
				}
				if (edgeTable[iCubeIndex] & 8)
				{
					indlist[3] = m_pPreviousVertexIndicesY[getIndex(uXRegSpace,uYRegSpace, regSlice.width()+1)];
					assert(indlist[3] != -1);
				}
				if (edgeTable[iCubeIndex] & 16)
				{
					indlist[4] = m_pCurrentVertexIndicesX[getIndex(uXRegSpace,uYRegSpace, regSlice.width()+1)];
					assert(indlist[4] != -1);
				}
				if (edgeTable[iCubeIndex] & 32)
				{
					indlist[5] = m_pCurrentVertexIndicesY[getIndex(uXRegSpace+m_uStepSize,uYRegSpace, regSlice.width()+1)];
					assert(indlist[5] != -1);
				}
				if (edgeTable[iCubeIndex] & 64)
				{
					indlist[6] = m_pCurrentVertexIndicesX[getIndex(uXRegSpace,uYRegSpace+m_uStepSize, regSlice.width()+1)];
					assert(indlist[6] != -1);
				}
				if (edgeTable[iCubeIndex] & 128)
				{
					indlist[7] = m_pCurrentVertexIndicesY[getIndex(uXRegSpace,uYRegSpace, regSlice.width()+1)];
					assert(indlist[7] != -1);
				}
				if (edgeTable[iCubeIndex] & 256)
				{
					indlist[8] = m_pPreviousVertexIndicesZ[getIndex(uXRegSpace,uYRegSpace, regSlice.width()+1)];
					assert(indlist[8] != -1);
				}
				if (edgeTable[iCubeIndex] & 512)
				{
					indlist[9] = m_pPreviousVertexIndicesZ[getIndex(uXRegSpace+m_uStepSize,uYRegSpace, regSlice.width()+1)];
					assert(indlist[9] != -1);
				}
				if (edgeTable[iCubeIndex] & 1024)
				{
					indlist[10] = m_pPreviousVertexIndicesZ[getIndex(uXRegSpace+m_uStepSize,uYRegSpace+m_uStepSize, regSlice.width()+1)];
					assert(indlist[10] != -1);
				}
				if (edgeTable[iCubeIndex] & 2048)
				{
					indlist[11] = m_pPreviousVertexIndicesZ[getIndex(uXRegSpace,uYRegSpace+m_uStepSize, regSlice.width()+1)];
					assert(indlist[11] != -1);
				}

				for (int i=0;triTable[iCubeIndex][i]!=-1;i+=3)
				{
					uint32_t ind0 = indlist[triTable[iCubeIndex][i  ]];
					uint32_t ind1 = indlist[triTable[iCubeIndex][i+1]];
					uint32_t ind2 = indlist[triTable[iCubeIndex][i+2]];

					singleMaterialPatch->addTriangle(ind0, ind1, ind2);
				}//For each triangle
			}//For each cell
		}
	}
}
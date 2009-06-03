#include "SurfaceExtractor.h"

#include "IndexedSurfacePatch.h"
#include "PolyVoxImpl/MarchingCubesTables.h"
#include "SurfaceVertex.h"

namespace PolyVox
{
	SurfaceExtractor::SurfaceExtractor(Volume<uint8_t>& volData)
		:m_uLodLevel(0)
		,m_volData(volData)
		,m_sampVolume(volData)
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
		m_Region = region;

		//When generating the mesh for a region we actually look outside it in the
		// back, bottom, right direction. Protect against access violations by cropping region here
		Region regVolume = m_volData.getEnclosingRegion();
		regVolume.setUpperCorner(regVolume.getUpperCorner() - Vector3DInt32(2*m_uStepSize-1,2*m_uStepSize-1,2*m_uStepSize-1));
		m_Region.cropTo(regVolume);

		m_ispCurrent = new IndexedSurfacePatch();

		m_uRegionWidth = m_Region.width();
		m_uRegionHeight = m_Region.height();

		//For edge indices
		m_pPreviousVertexIndicesX = new int32_t[(m_uRegionWidth+1) * (m_uRegionHeight+1)];
		m_pPreviousVertexIndicesY = new int32_t[(m_uRegionWidth+1) * (m_uRegionHeight+1)];
		m_pPreviousVertexIndicesZ = new int32_t[(m_uRegionWidth+1) * (m_uRegionHeight+1)];
		m_pCurrentVertexIndicesX = new int32_t[(m_uRegionWidth+1) * (m_uRegionHeight+1)];
		m_pCurrentVertexIndicesY = new int32_t[(m_uRegionWidth+1) * (m_uRegionHeight+1)];
		m_pCurrentVertexIndicesZ = new int32_t[(m_uRegionWidth+1) * (m_uRegionHeight+1)];

		//Cell bitmasks
		m_pPreviousBitmask = new uint8_t[(m_uRegionWidth+1) * (m_uRegionHeight+1)];
		m_pCurrentBitmask = new uint8_t[(m_uRegionWidth+1) * (m_uRegionHeight+1)];

		//m_v3dRegionOffset from volume corner
		m_v3dRegionOffset = static_cast<Vector3DFloat>(m_Region.getLowerCorner());

		//Create a region corresponding to the first slice
		regSlice0 = m_Region;
		Vector3DInt32 v3dUpperCorner = regSlice0.getUpperCorner();
		v3dUpperCorner.setZ(regSlice0.getLowerCorner().getZ()); //Set the upper z to the lower z to make it one slice thick.
		regSlice0.setUpperCorner(v3dUpperCorner);
		regSlice1 = regSlice0;	

		switch(m_uLodLevel)
		{
		case 0:
			extractSurfaceImpl<0>();
			break;
		case 1:
			extractSurfaceImpl<1>();
			break;
		case 2:
			extractSurfaceImpl<2>();
			break;
		}

		delete[] m_pPreviousBitmask;
		delete[] m_pCurrentBitmask;
		delete[] m_pPreviousVertexIndicesX;
		delete[] m_pCurrentVertexIndicesX;
		delete[] m_pPreviousVertexIndicesY;
		delete[] m_pCurrentVertexIndicesY;
		delete[] m_pPreviousVertexIndicesZ;
		delete[] m_pCurrentVertexIndicesZ;		

		m_ispCurrent->m_Region = m_Region;

		return POLYVOX_SHARED_PTR<IndexedSurfacePatch>(m_ispCurrent);
	}

	template<uint8_t uLodLevel>
	void SurfaceExtractor::extractSurfaceImpl(void)
	{	
		uint32_t uNoOfNonEmptyCellsForSlice0 = 0;
		uint32_t uNoOfNonEmptyCellsForSlice1 = 0;

		//Process the first slice (previous slice not available)
		computeBitmaskForSlice<false, uLodLevel>();
		uNoOfNonEmptyCellsForSlice1 = m_uNoOfOccupiedCells;

		if(uNoOfNonEmptyCellsForSlice1 != 0)
		{
			generateVerticesForSlice();				
		}

		std::swap(uNoOfNonEmptyCellsForSlice0, uNoOfNonEmptyCellsForSlice1);
		std::swap(m_pPreviousBitmask, m_pCurrentBitmask);
		std::swap(m_pPreviousVertexIndicesX, m_pCurrentVertexIndicesX);
		std::swap(m_pPreviousVertexIndicesY, m_pCurrentVertexIndicesY);
		std::swap(m_pPreviousVertexIndicesZ, m_pCurrentVertexIndicesZ);

		regSlice0 = regSlice1;
		regSlice1.shift(Vector3DInt32(0,0,m_uStepSize));

		//Process the first slice (previous slice is available)
		for(uint32_t uSlice = 1; uSlice <= m_Region.depth(); uSlice += m_uStepSize)
		{	
			computeBitmaskForSlice<true, uLodLevel>();
			uNoOfNonEmptyCellsForSlice1 = m_uNoOfOccupiedCells;

			if(uNoOfNonEmptyCellsForSlice1 != 0)
			{
				generateVerticesForSlice();				
			}

			if((uNoOfNonEmptyCellsForSlice0 != 0) || (uNoOfNonEmptyCellsForSlice1 != 0))
			{
				generateIndicesForSlice();
			}

			std::swap(uNoOfNonEmptyCellsForSlice0, uNoOfNonEmptyCellsForSlice1);
			std::swap(m_pPreviousBitmask, m_pCurrentBitmask);
			std::swap(m_pPreviousVertexIndicesX, m_pCurrentVertexIndicesX);
			std::swap(m_pPreviousVertexIndicesY, m_pCurrentVertexIndicesY);
			std::swap(m_pPreviousVertexIndicesZ, m_pCurrentVertexIndicesZ);

			regSlice0 = regSlice1;
			regSlice1.shift(Vector3DInt32(0,0,m_uStepSize));
		}
	}

	template<bool isPrevZAvail, uint8_t uLodLevel>
	uint32_t SurfaceExtractor::computeBitmaskForSlice(void)
	{
		m_uNoOfOccupiedCells = 0;

		const uint16_t uMaxXVolSpace = regSlice1.getUpperCorner().getX();
		const uint16_t uMaxYVolSpace = regSlice1.getUpperCorner().getY();

		uZVolSpace = regSlice1.getLowerCorner().getZ();
		uZRegSpace = uZVolSpace - m_v3dRegionOffset.getZ();

		//Process the lower left corner
		uYVolSpace = regSlice1.getLowerCorner().getY();
		uXVolSpace = regSlice1.getLowerCorner().getX();

		uXRegSpace = uXVolSpace - m_v3dRegionOffset.getX();
		uYRegSpace = uYVolSpace - m_v3dRegionOffset.getY();

		m_sampVolume.setPosition(uXVolSpace,uYVolSpace,uZVolSpace);
		computeBitmaskForCell<false, false, isPrevZAvail, uLodLevel>();


		//Process the edge where x is minimal.
		uXVolSpace = regSlice1.getLowerCorner().getX();
		for(uYVolSpace = regSlice1.getLowerCorner().getY() + m_uStepSize; uYVolSpace <= uMaxYVolSpace; uYVolSpace += m_uStepSize)
		{
			uXRegSpace = uXVolSpace - m_v3dRegionOffset.getX();
			uYRegSpace = uYVolSpace - m_v3dRegionOffset.getY();

			m_sampVolume.setPosition(uXVolSpace,uYVolSpace,uZVolSpace);
			computeBitmaskForCell<false, true, isPrevZAvail, uLodLevel>();
		}

		//Process the edge where y is minimal.
		uYVolSpace = regSlice1.getLowerCorner().getY();
		for(uXVolSpace = regSlice1.getLowerCorner().getX() + m_uStepSize; uXVolSpace <= uMaxXVolSpace; uXVolSpace += m_uStepSize)
		{	
			uXRegSpace = uXVolSpace - m_v3dRegionOffset.getX();
			uYRegSpace = uYVolSpace - m_v3dRegionOffset.getY();

			m_sampVolume.setPosition(uXVolSpace,uYVolSpace,uZVolSpace);
			computeBitmaskForCell<true, false, isPrevZAvail, uLodLevel>();
		}

		//Process all remaining elemnents of the slice. In this case, previous x and y values are always available
		for(uYVolSpace = regSlice1.getLowerCorner().getY() + m_uStepSize; uYVolSpace <= uMaxYVolSpace; uYVolSpace += m_uStepSize)
		{
			for(uXVolSpace = regSlice1.getLowerCorner().getX() + m_uStepSize; uXVolSpace <= uMaxXVolSpace; uXVolSpace += m_uStepSize)
			{	
				uXRegSpace = uXVolSpace - m_v3dRegionOffset.getX();
				uYRegSpace = uYVolSpace - m_v3dRegionOffset.getY();

				m_sampVolume.setPosition(uXVolSpace,uYVolSpace,uZVolSpace);
				computeBitmaskForCell<true, true, isPrevZAvail, uLodLevel>();
			}
		}

		return m_uNoOfOccupiedCells;
	}

	template<bool isPrevXAvail, bool isPrevYAvail, bool isPrevZAvail, uint8_t uLodLevel>
	void SurfaceExtractor::computeBitmaskForCell(void)
	{
		uint8_t iCubeIndex = 0;

		if(isPrevZAvail)
		{
			if(isPrevYAvail)
			{
				if(isPrevXAvail)
				{
					if(uLodLevel == 0)
					{
						v111 = m_sampVolume.peekVoxel1px1py1pz();
					}
					else
					{
						m_sampVolume.setPosition(uXVolSpace+m_uStepSize,uYVolSpace+m_uStepSize,uZVolSpace+m_uStepSize);
						v111 = m_sampVolume.getSubSampledVoxel(uLodLevel);	
					}

					//z
					uint8_t iPreviousCubeIndexZ = m_pPreviousBitmask[getIndex(uXRegSpace,uYRegSpace)];
					iPreviousCubeIndexZ >>= 4;

					//y
					uint8_t iPreviousCubeIndexY = m_pCurrentBitmask[getIndex(uXRegSpace,uYRegSpace-m_uStepSize)];
					iPreviousCubeIndexY &= 192; //192 = 128 + 64
					iPreviousCubeIndexY >>= 2;

					//x
					uint8_t iPreviousCubeIndexX = m_pCurrentBitmask[getIndex(uXRegSpace-m_uStepSize,uYRegSpace)];
					iPreviousCubeIndexX &= 128;
					iPreviousCubeIndexX >>= 1;

					iCubeIndex = iPreviousCubeIndexX | iPreviousCubeIndexY | iPreviousCubeIndexZ;

					if (v111 == 0) iCubeIndex |= 128;
				}
				else //previous X not available
				{
					if(uLodLevel == 0)
					{
						v011 = m_sampVolume.peekVoxel0px1py1pz();
						v111 = m_sampVolume.peekVoxel1px1py1pz();

					}
					else
					{
						m_sampVolume.setPosition(uXVolSpace,uYVolSpace+m_uStepSize,uZVolSpace+m_uStepSize);
						v011 = m_sampVolume.getSubSampledVoxel(uLodLevel);
						m_sampVolume.setPosition(uXVolSpace+m_uStepSize,uYVolSpace+m_uStepSize,uZVolSpace+m_uStepSize);
						v111 = m_sampVolume.getSubSampledVoxel(uLodLevel);	
					}

					//z
					uint8_t iPreviousCubeIndexZ = m_pPreviousBitmask[getIndex(uXRegSpace,uYRegSpace)];
					iPreviousCubeIndexZ >>= 4;

					//y
					uint8_t iPreviousCubeIndexY = m_pCurrentBitmask[getIndex(uXRegSpace,uYRegSpace-m_uStepSize)];
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
					if(uLodLevel == 0)
					{
						v101 = m_sampVolume.peekVoxel1px0py1pz();
						v111 = m_sampVolume.peekVoxel1px1py1pz();

					}
					else
					{
						m_sampVolume.setPosition(uXVolSpace+m_uStepSize,uYVolSpace,uZVolSpace+m_uStepSize);
						v101 = m_sampVolume.getSubSampledVoxel(uLodLevel);
						m_sampVolume.setPosition(uXVolSpace+m_uStepSize,uYVolSpace+m_uStepSize,uZVolSpace+m_uStepSize);
						v111 = m_sampVolume.getSubSampledVoxel(uLodLevel);	
					}

					//z
					uint8_t iPreviousCubeIndexZ = m_pPreviousBitmask[getIndex(uXRegSpace,uYRegSpace)];
					iPreviousCubeIndexZ >>= 4;

					//x
					uint8_t iPreviousCubeIndexX = m_pCurrentBitmask[getIndex(uXRegSpace-m_uStepSize,uYRegSpace)];
					iPreviousCubeIndexX &= 160; //160 = 128+32
					iPreviousCubeIndexX >>= 1;

					iCubeIndex = iPreviousCubeIndexX | iPreviousCubeIndexZ;

					if (v101 == 0) iCubeIndex |= 32;
					if (v111 == 0) iCubeIndex |= 128;
				}
				else //previous X not available
				{
					if(uLodLevel == 0)
					{
						v001 = m_sampVolume.peekVoxel0px0py1pz();
						v101 = m_sampVolume.peekVoxel1px0py1pz();
						v011 = m_sampVolume.peekVoxel0px1py1pz();
						v111 = m_sampVolume.peekVoxel1px1py1pz();
					}
					else
					{
						m_sampVolume.setPosition(uXVolSpace,uYVolSpace,uZVolSpace+m_uStepSize);
						v001 = m_sampVolume.getSubSampledVoxel(uLodLevel);
						m_sampVolume.setPosition(uXVolSpace+m_uStepSize,uYVolSpace,uZVolSpace+m_uStepSize);
						v101 = m_sampVolume.getSubSampledVoxel(uLodLevel);
						m_sampVolume.setPosition(uXVolSpace,uYVolSpace+m_uStepSize,uZVolSpace+m_uStepSize);
						v011 = m_sampVolume.getSubSampledVoxel(uLodLevel);
						m_sampVolume.setPosition(uXVolSpace+m_uStepSize,uYVolSpace+m_uStepSize,uZVolSpace+m_uStepSize);
						v111 = m_sampVolume.getSubSampledVoxel(uLodLevel);	
					}

					//z
					uint8_t iPreviousCubeIndexZ = m_pPreviousBitmask[getIndex(uXRegSpace,uYRegSpace)];
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
					if(uLodLevel == 0)
					{
						v110 = m_sampVolume.peekVoxel1px1py0pz();
						v111 = m_sampVolume.peekVoxel1px1py1pz();
					}
					else
					{
						m_sampVolume.setPosition(uXVolSpace+m_uStepSize,uYVolSpace+m_uStepSize,uZVolSpace);
						v110 = m_sampVolume.getSubSampledVoxel(uLodLevel);

						m_sampVolume.setPosition(uXVolSpace+m_uStepSize,uYVolSpace+m_uStepSize,uZVolSpace+m_uStepSize);
						v111 = m_sampVolume.getSubSampledVoxel(uLodLevel);
					}

					//y
					uint8_t iPreviousCubeIndexY = m_pCurrentBitmask[getIndex(uXRegSpace,uYRegSpace-m_uStepSize)];
					iPreviousCubeIndexY &= 204; //204 = 128+64+8+4
					iPreviousCubeIndexY >>= 2;

					//x
					uint8_t iPreviousCubeIndexX = m_pCurrentBitmask[getIndex(uXRegSpace-m_uStepSize,uYRegSpace)];
					iPreviousCubeIndexX &= 170; //170 = 128+32+8+2
					iPreviousCubeIndexX >>= 1;

					iCubeIndex = iPreviousCubeIndexX | iPreviousCubeIndexY;

					if (v110 == 0) iCubeIndex |= 8;
					if (v111 == 0) iCubeIndex |= 128;
				}
				else //previous X not available
				{
					if(uLodLevel == 0)
					{
						v010 = m_sampVolume.peekVoxel0px1py0pz();
						v110 = m_sampVolume.peekVoxel1px1py0pz();

						v011 = m_sampVolume.peekVoxel0px1py1pz();
						v111 = m_sampVolume.peekVoxel1px1py1pz();
					}
					else
					{
						m_sampVolume.setPosition(uXVolSpace,uYVolSpace+m_uStepSize,uZVolSpace);
						v010 = m_sampVolume.getSubSampledVoxel(uLodLevel);
						m_sampVolume.setPosition(uXVolSpace+m_uStepSize,uYVolSpace+m_uStepSize,uZVolSpace);
						v110 = m_sampVolume.getSubSampledVoxel(uLodLevel);

						m_sampVolume.setPosition(uXVolSpace,uYVolSpace+m_uStepSize,uZVolSpace+m_uStepSize);
						v011 = m_sampVolume.getSubSampledVoxel(uLodLevel);
						m_sampVolume.setPosition(uXVolSpace+m_uStepSize,uYVolSpace+m_uStepSize,uZVolSpace+m_uStepSize);
						v111 = m_sampVolume.getSubSampledVoxel(uLodLevel);
					}

					//y
					uint8_t iPreviousCubeIndexY = m_pCurrentBitmask[getIndex(uXRegSpace,uYRegSpace-m_uStepSize)];
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
					if(uLodLevel == 0)
					{
						v100 = m_sampVolume.peekVoxel1px0py0pz();
						v110 = m_sampVolume.peekVoxel1px1py0pz();

						v101 = m_sampVolume.peekVoxel1px0py1pz();
						v111 = m_sampVolume.peekVoxel1px1py1pz();
					}
					else
					{
						m_sampVolume.setPosition(uXVolSpace+m_uStepSize,uYVolSpace,uZVolSpace);
						v100 = m_sampVolume.getSubSampledVoxel(uLodLevel);
						m_sampVolume.setPosition(uXVolSpace+m_uStepSize,uYVolSpace+m_uStepSize,uZVolSpace);
						v110 = m_sampVolume.getSubSampledVoxel(uLodLevel);

						m_sampVolume.setPosition(uXVolSpace+m_uStepSize,uYVolSpace,uZVolSpace+m_uStepSize);
						v101 = m_sampVolume.getSubSampledVoxel(uLodLevel);
						m_sampVolume.setPosition(uXVolSpace+m_uStepSize,uYVolSpace+m_uStepSize,uZVolSpace+m_uStepSize);
						v111 = m_sampVolume.getSubSampledVoxel(uLodLevel);
					}

					//x
					uint8_t iPreviousCubeIndexX = m_pCurrentBitmask[getIndex(uXRegSpace-m_uStepSize,uYRegSpace)];
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
					if(uLodLevel == 0)
					{
						v000 = m_sampVolume.getVoxel();
						v100 = m_sampVolume.peekVoxel1px0py0pz();
						v010 = m_sampVolume.peekVoxel0px1py0pz();
						v110 = m_sampVolume.peekVoxel1px1py0pz();

						v001 = m_sampVolume.peekVoxel0px0py1pz();
						v101 = m_sampVolume.peekVoxel1px0py1pz();
						v011 = m_sampVolume.peekVoxel0px1py1pz();
						v111 = m_sampVolume.peekVoxel1px1py1pz();
					}
					else
					{
						m_sampVolume.setPosition(uXVolSpace,uYVolSpace,uZVolSpace);
						v000 = m_sampVolume.getSubSampledVoxel(uLodLevel);
						m_sampVolume.setPosition(uXVolSpace+m_uStepSize,uYVolSpace,uZVolSpace);
						v100 = m_sampVolume.getSubSampledVoxel(uLodLevel);
						m_sampVolume.setPosition(uXVolSpace,uYVolSpace+m_uStepSize,uZVolSpace);
						v010 = m_sampVolume.getSubSampledVoxel(uLodLevel);
						m_sampVolume.setPosition(uXVolSpace+m_uStepSize,uYVolSpace+m_uStepSize,uZVolSpace);
						v110 = m_sampVolume.getSubSampledVoxel(uLodLevel);

						m_sampVolume.setPosition(uXVolSpace,uYVolSpace,uZVolSpace+m_uStepSize);
						v001 = m_sampVolume.getSubSampledVoxel(uLodLevel);
						m_sampVolume.setPosition(uXVolSpace+m_uStepSize,uYVolSpace,uZVolSpace+m_uStepSize);
						v101 = m_sampVolume.getSubSampledVoxel(uLodLevel);
						m_sampVolume.setPosition(uXVolSpace,uYVolSpace+m_uStepSize,uZVolSpace+m_uStepSize);
						v011 = m_sampVolume.getSubSampledVoxel(uLodLevel);
						m_sampVolume.setPosition(uXVolSpace+m_uStepSize,uYVolSpace+m_uStepSize,uZVolSpace+m_uStepSize);
						v111 = m_sampVolume.getSubSampledVoxel(uLodLevel);
					}

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

		//Save the bitmask
		m_pCurrentBitmask[getIndex(uXRegSpace,uYVolSpace- m_v3dRegionOffset.getY())] = iCubeIndex;

		if(edgeTable[iCubeIndex] != 0)
		{
			++m_uNoOfOccupiedCells;
		}
	}

	void SurfaceExtractor::generateVerticesForSlice()
	{
		//Iterate over each cell in the region
		for(uint16_t uYVolSpace = regSlice1.getLowerCorner().getY(); uYVolSpace <= regSlice1.getUpperCorner().getY(); uYVolSpace += m_uStepSize)
		{
			for(uint16_t uXVolSpace = regSlice1.getLowerCorner().getX(); uXVolSpace <= regSlice1.getUpperCorner().getX(); uXVolSpace += m_uStepSize)
			{		
				uint16_t uZVolSpace = regSlice1.getLowerCorner().getZ();

				//Current position
				const uint16_t uXRegSpace = uXVolSpace - m_v3dRegionOffset.getX();
				const uint16_t uYRegSpace = uYVolSpace - m_v3dRegionOffset.getY();
				const uint16_t uZRegSpace = uZVolSpace - m_v3dRegionOffset.getZ();

				//Current position
				//const uint16_t z = regSlice.getLowerCorner().getZ();

				m_sampVolume.setPosition(uXVolSpace,uYVolSpace,uZVolSpace);
				const uint8_t v000 = m_sampVolume.getSubSampledVoxel(m_uLodLevel);

				//Determine the index into the edge table which tells us which vertices are inside of the surface
				uint8_t iCubeIndex = m_pCurrentBitmask[getIndex(uXVolSpace - m_v3dRegionOffset.getX(),uYVolSpace - m_v3dRegionOffset.getY())];

				/* Cube is entirely in/out of the surface */
				if (edgeTable[iCubeIndex] == 0)
				{
					continue;
				}

				/* Find the vertices where the surface intersects the cube */
				if (edgeTable[iCubeIndex] & 1)
				{
					//if(uXVolSpace != regSlice1.getUpperCorner().getX())
					{
						m_sampVolume.setPosition(uXVolSpace + m_uStepSize,uYVolSpace,uZVolSpace);
						const uint8_t v100 = m_sampVolume.getSubSampledVoxel(m_uLodLevel);
						const Vector3DFloat v3dPosition(uXVolSpace - m_v3dRegionOffset.getX() + 0.5f * m_uStepSize, uYVolSpace - m_v3dRegionOffset.getY(), uZVolSpace - m_v3dRegionOffset.getZ());
						const Vector3DFloat v3dNormal(v000 > v100 ? 1.0f : -1.0f,0.0,0.0);
						const uint8_t uMaterial = v000 | v100; //Because one of these is 0, the or operation takes the max.
						SurfaceVertex surfaceVertex(v3dPosition, v3dNormal, uMaterial);
						uint32_t uLastVertexIndex = m_ispCurrent->addVertex(surfaceVertex);
						m_pCurrentVertexIndicesX[getIndex(uXVolSpace - m_v3dRegionOffset.getX(),uYVolSpace - m_v3dRegionOffset.getY())] = uLastVertexIndex;
					}
				}
				if (edgeTable[iCubeIndex] & 8)
				{
					//if(uYVolSpace != regSlice1.getUpperCorner().getY())
					{
						m_sampVolume.setPosition(uXVolSpace,uYVolSpace + m_uStepSize,uZVolSpace);
						const uint8_t v010 = m_sampVolume.getSubSampledVoxel(m_uLodLevel);
						const Vector3DFloat v3dPosition(uXVolSpace - m_v3dRegionOffset.getX(), uYVolSpace - m_v3dRegionOffset.getY() + 0.5f * m_uStepSize, uZVolSpace - m_v3dRegionOffset.getZ());
						const Vector3DFloat v3dNormal(0.0,v000 > v010 ? 1.0f : -1.0f,0.0);
						const uint8_t uMaterial = v000 | v010; //Because one of these is 0, the or operation takes the max.
						SurfaceVertex surfaceVertex(v3dPosition, v3dNormal, uMaterial);
						uint32_t uLastVertexIndex = m_ispCurrent->addVertex(surfaceVertex);
						m_pCurrentVertexIndicesY[getIndex(uXVolSpace - m_v3dRegionOffset.getX(),uYVolSpace - m_v3dRegionOffset.getY())] = uLastVertexIndex;
					}
				}
				if (edgeTable[iCubeIndex] & 256)
				{
					//if(z != regSlice.getUpperCorner.getZ())
					{
						m_sampVolume.setPosition(uXVolSpace,uYVolSpace,uZVolSpace + m_uStepSize);
						const uint8_t v001 = m_sampVolume.getSubSampledVoxel(m_uLodLevel);
						const Vector3DFloat v3dPosition(uXVolSpace - m_v3dRegionOffset.getX(), uYVolSpace - m_v3dRegionOffset.getY(), uZVolSpace - m_v3dRegionOffset.getZ() + 0.5f * m_uStepSize);
						const Vector3DFloat v3dNormal(0.0,0.0,v000 > v001 ? 1.0f : -1.0f);
						const uint8_t uMaterial = v000 | v001; //Because one of these is 0, the or operation takes the max.
						const SurfaceVertex surfaceVertex(v3dPosition, v3dNormal, uMaterial);
						uint32_t uLastVertexIndex = m_ispCurrent->addVertex(surfaceVertex);
						m_pCurrentVertexIndicesZ[getIndex(uXVolSpace - m_v3dRegionOffset.getX(),uYVolSpace - m_v3dRegionOffset.getY())] = uLastVertexIndex;
					}
				}
			}//For each cell
		}
	}

	void SurfaceExtractor::generateIndicesForSlice()
	{
		uint32_t indlist[12];

		for(uint16_t uYVolSpace = regSlice0.getLowerCorner().getY(); uYVolSpace < regSlice0.getUpperCorner().getY(); uYVolSpace += m_uStepSize)
		{
			for(uint16_t uXVolSpace = regSlice0.getLowerCorner().getX(); uXVolSpace < regSlice0.getUpperCorner().getX(); uXVolSpace += m_uStepSize)
			{		
				uint16_t uZVolSpace = regSlice0.getLowerCorner().getZ();
				m_sampVolume.setPosition(uXVolSpace,uYVolSpace,uZVolSpace);	

				//Current position
				const uint16_t uXRegSpace = m_sampVolume.getPosX() - m_v3dRegionOffset.getX();
				const uint16_t uYRegSpace = m_sampVolume.getPosY() - m_v3dRegionOffset.getY();
				const uint16_t uZRegSpace = m_sampVolume.getPosZ() - m_v3dRegionOffset.getZ();

				//Determine the index into the edge table which tells us which vertices are inside of the surface
				uint8_t iCubeIndex = m_pPreviousBitmask[getIndex(uXRegSpace,uYRegSpace)];

				/* Cube is entirely in/out of the surface */
				if (edgeTable[iCubeIndex] == 0)
				{
					continue;
				}

				/* Find the vertices where the surface intersects the cube */
				if (edgeTable[iCubeIndex] & 1)
				{
					indlist[0] = m_pPreviousVertexIndicesX[getIndex(uXRegSpace,uYRegSpace)];
					assert(indlist[0] != -1);
				}
				if (edgeTable[iCubeIndex] & 2)
				{
					indlist[1] = m_pPreviousVertexIndicesY[getIndex(uXRegSpace+m_uStepSize,uYRegSpace)];
					assert(indlist[1] != -1);
				}
				if (edgeTable[iCubeIndex] & 4)
				{
					indlist[2] = m_pPreviousVertexIndicesX[getIndex(uXRegSpace,uYRegSpace+m_uStepSize)];
					assert(indlist[2] != -1);
				}
				if (edgeTable[iCubeIndex] & 8)
				{
					indlist[3] = m_pPreviousVertexIndicesY[getIndex(uXRegSpace,uYRegSpace)];
					assert(indlist[3] != -1);
				}
				if (edgeTable[iCubeIndex] & 16)
				{
					indlist[4] = m_pCurrentVertexIndicesX[getIndex(uXRegSpace,uYRegSpace)];
					assert(indlist[4] != -1);
				}
				if (edgeTable[iCubeIndex] & 32)
				{
					indlist[5] = m_pCurrentVertexIndicesY[getIndex(uXRegSpace+m_uStepSize,uYRegSpace)];
					assert(indlist[5] != -1);
				}
				if (edgeTable[iCubeIndex] & 64)
				{
					indlist[6] = m_pCurrentVertexIndicesX[getIndex(uXRegSpace,uYRegSpace+m_uStepSize)];
					assert(indlist[6] != -1);
				}
				if (edgeTable[iCubeIndex] & 128)
				{
					indlist[7] = m_pCurrentVertexIndicesY[getIndex(uXRegSpace,uYRegSpace)];
					assert(indlist[7] != -1);
				}
				if (edgeTable[iCubeIndex] & 256)
				{
					indlist[8] = m_pPreviousVertexIndicesZ[getIndex(uXRegSpace,uYRegSpace)];
					assert(indlist[8] != -1);
				}
				if (edgeTable[iCubeIndex] & 512)
				{
					indlist[9] = m_pPreviousVertexIndicesZ[getIndex(uXRegSpace+m_uStepSize,uYRegSpace)];
					assert(indlist[9] != -1);
				}
				if (edgeTable[iCubeIndex] & 1024)
				{
					indlist[10] = m_pPreviousVertexIndicesZ[getIndex(uXRegSpace+m_uStepSize,uYRegSpace+m_uStepSize)];
					assert(indlist[10] != -1);
				}
				if (edgeTable[iCubeIndex] & 2048)
				{
					indlist[11] = m_pPreviousVertexIndicesZ[getIndex(uXRegSpace,uYRegSpace+m_uStepSize)];
					assert(indlist[11] != -1);
				}

				for (int i=0;triTable[iCubeIndex][i]!=-1;i+=3)
				{
					uint32_t ind0 = indlist[triTable[iCubeIndex][i  ]];
					uint32_t ind1 = indlist[triTable[iCubeIndex][i+1]];
					uint32_t ind2 = indlist[triTable[iCubeIndex][i+2]];

					m_ispCurrent->addTriangle(ind0, ind1, ind2);
				}//For each triangle
			}//For each cell
		}
	}
}
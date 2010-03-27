/*******************************************************************************
Copyright (c) 2005-2009 David Williams

This software is provided 'as-is', without any express or implied
warranty. In no event will the authors be held liable for any damages
arising from the use of this software.

Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute it
freely, subject to the following restrictions:

    1. The origin of this software must not be misrepresented; you must not
    claim that you wrote the original software. If you use this software
    in a product, an acknowledgment in the product documentation would be
    appreciated but is not required.

    2. Altered source versions must be plainly marked as such, and must not be
    misrepresented as being the original software.

    3. This notice may not be removed or altered from any source
    distribution. 	
*******************************************************************************/

#include "SurfaceExtractor.h"

#include "Array.h"
#include "SurfaceMesh.h"
#include "PolyVoxImpl/MarchingCubesTables.h"
#include "SurfaceVertex.h"

namespace PolyVox
{
	SurfaceExtractor::SurfaceExtractor(Volume<uint8_t>& volData)
		:m_volData(volData)
		,m_sampVolume(&volData)
	{
	}

	POLYVOX_SHARED_PTR<SurfaceMesh> SurfaceExtractor::extractSurfaceForRegion(Region region)
	{		
		m_regInputUncropped = region;

		//When generating the mesh for a region we actually look outside it in the
		// back, bottom, right direction. Protect against access violations by cropping region here
		m_regVolumeCropped = m_volData.getEnclosingRegion();
		m_regInputUncropped.cropTo(m_regVolumeCropped);
		m_regVolumeCropped.setUpperCorner(m_regVolumeCropped.getUpperCorner() - Vector3DInt16(1,1,1));
	
		m_regInputCropped = region;
		m_regInputCropped.cropTo(m_regVolumeCropped);

		m_meshCurrent = new SurfaceMesh();

		m_uRegionWidth = m_regInputCropped.width();
		m_uRegionHeight = m_regInputCropped.height();

		m_uScratchPadWidth = m_uRegionWidth+1;
		m_uScratchPadHeight = m_uRegionHeight+1;

		//For edge indices
		/*m_pPreviousVertexIndicesX = new int32_t[m_uScratchPadWidth * m_uScratchPadHeight];
		m_pPreviousVertexIndicesY = new int32_t[m_uScratchPadWidth * m_uScratchPadHeight];
		m_pPreviousVertexIndicesZ = new int32_t[m_uScratchPadWidth * m_uScratchPadHeight];
		m_pCurrentVertexIndicesX = new int32_t[m_uScratchPadWidth * m_uScratchPadHeight];
		m_pCurrentVertexIndicesY = new int32_t[m_uScratchPadWidth * m_uScratchPadHeight];
		m_pCurrentVertexIndicesZ = new int32_t[m_uScratchPadWidth * m_uScratchPadHeight];*/

		//uint32_t Size1 [1]= {m_uScratchPadWidth}; // Array dimensions
		uint32_t Size2 [2]= {m_uScratchPadWidth, m_uScratchPadHeight}; // Array dimensions

		/*Array2DInt32 m_pPreviousVertexIndicesX(m_uScratchPadWidth, m_uScratchPadHeight);
		Array2DInt32 m_pPreviousVertexIndicesY(m_uScratchPadWidth, m_uScratchPadHeight);
		Array2DInt32 m_pPreviousVertexIndicesZ(m_uScratchPadWidth, m_uScratchPadHeight);
		Array2DInt32 m_pCurrentVertexIndicesX(m_uScratchPadWidth, m_uScratchPadHeight);
		Array2DInt32 m_pCurrentVertexIndicesY(m_uScratchPadWidth, m_uScratchPadHeight);
		Array2DInt32 m_pCurrentVertexIndicesZ(m_uScratchPadWidth, m_uScratchPadHeight);*/

		Array2DInt32 m_pPreviousVertexIndicesX(ArraySizes(m_uScratchPadWidth)(m_uScratchPadHeight));
		Array2DInt32 m_pPreviousVertexIndicesY(Size2);
		Array2DInt32 m_pPreviousVertexIndicesZ(Size2);
		Array2DInt32 m_pCurrentVertexIndicesX(Size2);
		Array2DInt32 m_pCurrentVertexIndicesY(Size2);
		Array2DInt32 m_pCurrentVertexIndicesZ(Size2);

		//Array1DFloat test1d(ArraySizes(10));
		//Array2DFloat test2d(ArraySizes(10)(20));



		/*m_pPreviousVertexIndicesX.resize(Size2);
		m_pPreviousVertexIndicesY.resize(Size2);
		m_pPreviousVertexIndicesZ.resize(Size2);
		m_pCurrentVertexIndicesX.resize(Size2);
		m_pCurrentVertexIndicesY.resize(Size2);
		m_pCurrentVertexIndicesZ.resize(Size2);*/


		//Cell bitmasks
		/*Array2DUint8 pPreviousBitmask(m_uScratchPadWidth, m_uScratchPadHeight);
		Array2DUint8 pCurrentBitmask(m_uScratchPadWidth, m_uScratchPadHeight);*/

		Array2DUint8 pPreviousBitmask(Size2);
		Array2DUint8 pCurrentBitmask(Size2);

		/*pPreviousBitmask.resize(Size2);
		pCurrentBitmask.resize(Size2);*/

		//Create a region corresponding to the first slice
		m_regSlicePrevious = m_regInputCropped;
		Vector3DInt16 v3dUpperCorner = m_regSlicePrevious.getUpperCorner();
		v3dUpperCorner.setZ(m_regSlicePrevious.getLowerCorner().getZ()); //Set the upper z to the lower z to make it one slice thick.
		m_regSlicePrevious.setUpperCorner(v3dUpperCorner);
		m_regSliceCurrent = m_regSlicePrevious;	

		uint32_t uNoOfNonEmptyCellsForSlice0 = 0;
		uint32_t uNoOfNonEmptyCellsForSlice1 = 0;

		//Process the first slice (previous slice not available)
		computeBitmaskForSlice<false>(pPreviousBitmask, pCurrentBitmask);
		uNoOfNonEmptyCellsForSlice1 = m_uNoOfOccupiedCells;

		if(uNoOfNonEmptyCellsForSlice1 != 0)
		{
			memset(m_pCurrentVertexIndicesX.getRawData(), 0xff, m_pCurrentVertexIndicesX.getNoOfElements() * 4);
			memset(m_pCurrentVertexIndicesY.getRawData(), 0xff, m_pCurrentVertexIndicesY.getNoOfElements() * 4);
			memset(m_pCurrentVertexIndicesZ.getRawData(), 0xff, m_pCurrentVertexIndicesZ.getNoOfElements() * 4);
			generateVerticesForSlice(pCurrentBitmask, m_pCurrentVertexIndicesX, m_pCurrentVertexIndicesY, m_pCurrentVertexIndicesZ);				
		}

		std::swap(uNoOfNonEmptyCellsForSlice0, uNoOfNonEmptyCellsForSlice1);
		std::swap(pPreviousBitmask, pCurrentBitmask);
		std::swap(m_pPreviousVertexIndicesX, m_pCurrentVertexIndicesX);
		std::swap(m_pPreviousVertexIndicesY, m_pCurrentVertexIndicesY);
		std::swap(m_pPreviousVertexIndicesZ, m_pCurrentVertexIndicesZ);

		m_regSlicePrevious = m_regSliceCurrent;
		m_regSliceCurrent.shift(Vector3DInt16(0,0,1));

		//Process the other slices (previous slice is available)
		for(int16_t uSlice = 1; uSlice <= m_regInputCropped.depth(); uSlice++)
		{	
			computeBitmaskForSlice<true>(pPreviousBitmask, pCurrentBitmask);
			uNoOfNonEmptyCellsForSlice1 = m_uNoOfOccupiedCells;

			if(uNoOfNonEmptyCellsForSlice1 != 0)
			{
				memset(m_pCurrentVertexIndicesX.getRawData(), 0xff, m_pCurrentVertexIndicesX.getNoOfElements() * 4);
				memset(m_pCurrentVertexIndicesY.getRawData(), 0xff, m_pCurrentVertexIndicesY.getNoOfElements() * 4);
				memset(m_pCurrentVertexIndicesZ.getRawData(), 0xff, m_pCurrentVertexIndicesZ.getNoOfElements() * 4);
				generateVerticesForSlice(pCurrentBitmask, m_pCurrentVertexIndicesX, m_pCurrentVertexIndicesY, m_pCurrentVertexIndicesZ);				
			}

			if((uNoOfNonEmptyCellsForSlice0 != 0) || (uNoOfNonEmptyCellsForSlice1 != 0))
			{
				generateIndicesForSlice(pPreviousBitmask, m_pPreviousVertexIndicesX, m_pPreviousVertexIndicesY, m_pPreviousVertexIndicesZ, m_pCurrentVertexIndicesX, m_pCurrentVertexIndicesY, m_pCurrentVertexIndicesZ);
			}

			std::swap(uNoOfNonEmptyCellsForSlice0, uNoOfNonEmptyCellsForSlice1);
			std::swap(pPreviousBitmask, pCurrentBitmask);
			std::swap(m_pPreviousVertexIndicesX, m_pCurrentVertexIndicesX);
			std::swap(m_pPreviousVertexIndicesY, m_pCurrentVertexIndicesY);
			std::swap(m_pPreviousVertexIndicesZ, m_pCurrentVertexIndicesZ);

			m_regSlicePrevious = m_regSliceCurrent;
			m_regSliceCurrent.shift(Vector3DInt16(0,0,1));
		}

		//A final slice just to close of the volume
		m_regSliceCurrent.shift(Vector3DInt16(0,0,-1));
		if(m_regSliceCurrent.getLowerCorner().getZ() == m_regVolumeCropped.getUpperCorner().getZ())
		{
			memset(m_pCurrentVertexIndicesX.getRawData(), 0xff, m_pCurrentVertexIndicesX.getNoOfElements() * 4);
			memset(m_pCurrentVertexIndicesY.getRawData(), 0xff, m_pCurrentVertexIndicesY.getNoOfElements() * 4);
			memset(m_pCurrentVertexIndicesZ.getRawData(), 0xff, m_pCurrentVertexIndicesZ.getNoOfElements() * 4);
			generateIndicesForSlice(pPreviousBitmask, m_pPreviousVertexIndicesX, m_pPreviousVertexIndicesY, m_pPreviousVertexIndicesZ, m_pCurrentVertexIndicesX, m_pCurrentVertexIndicesY, m_pCurrentVertexIndicesZ);
		}

		/*delete[] m_pPreviousVertexIndicesX;
		delete[] m_pCurrentVertexIndicesX;
		delete[] m_pPreviousVertexIndicesY;
		delete[] m_pCurrentVertexIndicesY;
		delete[] m_pPreviousVertexIndicesZ;
		delete[] m_pCurrentVertexIndicesZ;	*/	

		m_meshCurrent->m_Region = m_regInputUncropped;

		m_meshCurrent->m_vecLodRecords.clear();
		LodRecord lodRecord;
		lodRecord.beginIndex = 0;
		lodRecord.endIndex = m_meshCurrent->getNoOfIndices();
		m_meshCurrent->m_vecLodRecords.push_back(lodRecord);

		return POLYVOX_SHARED_PTR<SurfaceMesh>(m_meshCurrent);
	}

	template<bool isPrevZAvail>
	uint32_t SurfaceExtractor::computeBitmaskForSlice(const Array2DUint8& pPreviousBitmask, Array2DUint8& pCurrentBitmask)
	{
		m_uNoOfOccupiedCells = 0;

		const uint16_t uMaxXVolSpace = m_regSliceCurrent.getUpperCorner().getX();
		const uint16_t uMaxYVolSpace = m_regSliceCurrent.getUpperCorner().getY();

		uZVolSpace = m_regSliceCurrent.getLowerCorner().getZ();
		uZRegSpace = uZVolSpace - m_regInputCropped.getLowerCorner().getZ();

		//Process the lower left corner
		uYVolSpace = m_regSliceCurrent.getLowerCorner().getY();
		uXVolSpace = m_regSliceCurrent.getLowerCorner().getX();

		uXRegSpace = uXVolSpace - m_regInputCropped.getLowerCorner().getX();
		uYRegSpace = uYVolSpace - m_regInputCropped.getLowerCorner().getY();

		m_sampVolume.setPosition(uXVolSpace,uYVolSpace,uZVolSpace);
		computeBitmaskForCell<false, false, isPrevZAvail>(pPreviousBitmask, pCurrentBitmask);

		//Process the edge where x is minimal.
		uXVolSpace = m_regSliceCurrent.getLowerCorner().getX();
		m_sampVolume.setPosition(uXVolSpace, m_regSliceCurrent.getLowerCorner().getY(), uZVolSpace);
		for(uYVolSpace = m_regSliceCurrent.getLowerCorner().getY() + 1; uYVolSpace <= uMaxYVolSpace; uYVolSpace++)
		{
			uXRegSpace = uXVolSpace - m_regInputCropped.getLowerCorner().getX();
			uYRegSpace = uYVolSpace - m_regInputCropped.getLowerCorner().getY();

			m_sampVolume.movePositiveY();

			computeBitmaskForCell<false, true, isPrevZAvail>(pPreviousBitmask, pCurrentBitmask);
		}

		//Process the edge where y is minimal.
		uYVolSpace = m_regSliceCurrent.getLowerCorner().getY();
		m_sampVolume.setPosition(m_regSliceCurrent.getLowerCorner().getX(), uYVolSpace, uZVolSpace);
		for(uXVolSpace = m_regSliceCurrent.getLowerCorner().getX() + 1; uXVolSpace <= uMaxXVolSpace; uXVolSpace++)
		{	
			uXRegSpace = uXVolSpace - m_regInputCropped.getLowerCorner().getX();
			uYRegSpace = uYVolSpace - m_regInputCropped.getLowerCorner().getY();

			m_sampVolume.movePositiveX();

			computeBitmaskForCell<true, false, isPrevZAvail>(pPreviousBitmask, pCurrentBitmask);
		}

		//Process all remaining elemnents of the slice. In this case, previous x and y values are always available
		for(uYVolSpace = m_regSliceCurrent.getLowerCorner().getY() + 1; uYVolSpace <= uMaxYVolSpace; uYVolSpace++)
		{
			m_sampVolume.setPosition(m_regSliceCurrent.getLowerCorner().getX(), uYVolSpace, uZVolSpace);
			for(uXVolSpace = m_regSliceCurrent.getLowerCorner().getX() + 1; uXVolSpace <= uMaxXVolSpace; uXVolSpace++)
			{	
				uXRegSpace = uXVolSpace - m_regInputCropped.getLowerCorner().getX();
				uYRegSpace = uYVolSpace - m_regInputCropped.getLowerCorner().getY();

				m_sampVolume.movePositiveX();

				computeBitmaskForCell<true, true, isPrevZAvail>(pPreviousBitmask, pCurrentBitmask);
			}
		}

		return m_uNoOfOccupiedCells;
	}

	template<bool isPrevXAvail, bool isPrevYAvail, bool isPrevZAvail>
	void SurfaceExtractor::computeBitmaskForCell(const Array2DUint8& pPreviousBitmask, Array2DUint8& pCurrentBitmask)
	{
		uint8_t iCubeIndex = 0;

		uint8_t v000 = 0;
		uint8_t v100 = 0;
		uint8_t v010 = 0;
		uint8_t v110 = 0;
		uint8_t v001 = 0;
		uint8_t v101 = 0;
		uint8_t v011 = 0;
		uint8_t v111 = 0;

		if(isPrevZAvail)
		{
			if(isPrevYAvail)
			{
				if(isPrevXAvail)
				{
					v111 = m_sampVolume.peekVoxel1px1py1pz();

					//z
					uint8_t iPreviousCubeIndexZ = pPreviousBitmask[uXRegSpace][uYRegSpace];
					iPreviousCubeIndexZ >>= 4;

					//y
					uint8_t iPreviousCubeIndexY = pCurrentBitmask[uXRegSpace][uYRegSpace-1];
					iPreviousCubeIndexY &= 192; //192 = 128 + 64
					iPreviousCubeIndexY >>= 2;

					//x
					uint8_t iPreviousCubeIndexX = pCurrentBitmask[uXRegSpace-1][uYRegSpace];
					iPreviousCubeIndexX &= 128;
					iPreviousCubeIndexX >>= 1;

					iCubeIndex = iPreviousCubeIndexX | iPreviousCubeIndexY | iPreviousCubeIndexZ;

					if (v111 == 0) iCubeIndex |= 128;
				}
				else //previous X not available
				{
					v011 = m_sampVolume.peekVoxel0px1py1pz();
					v111 = m_sampVolume.peekVoxel1px1py1pz();

					//z
					uint8_t iPreviousCubeIndexZ = pPreviousBitmask[uXRegSpace][uYRegSpace];
					iPreviousCubeIndexZ >>= 4;

					//y
					uint8_t iPreviousCubeIndexY = pCurrentBitmask[uXRegSpace][uYRegSpace-1];
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
					v101 = m_sampVolume.peekVoxel1px0py1pz();
					v111 = m_sampVolume.peekVoxel1px1py1pz();

					//z
					uint8_t iPreviousCubeIndexZ = pPreviousBitmask[uXRegSpace][uYRegSpace];
					iPreviousCubeIndexZ >>= 4;

					//x
					uint8_t iPreviousCubeIndexX = pCurrentBitmask[uXRegSpace-1][uYRegSpace];
					iPreviousCubeIndexX &= 160; //160 = 128+32
					iPreviousCubeIndexX >>= 1;

					iCubeIndex = iPreviousCubeIndexX | iPreviousCubeIndexZ;

					if (v101 == 0) iCubeIndex |= 32;
					if (v111 == 0) iCubeIndex |= 128;
				}
				else //previous X not available
				{
					v001 = m_sampVolume.peekVoxel0px0py1pz();
					v101 = m_sampVolume.peekVoxel1px0py1pz();
					v011 = m_sampVolume.peekVoxel0px1py1pz();
					v111 = m_sampVolume.peekVoxel1px1py1pz();

					//z
					uint8_t iPreviousCubeIndexZ = pPreviousBitmask[uXRegSpace][uYRegSpace];
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
					v110 = m_sampVolume.peekVoxel1px1py0pz();
					v111 = m_sampVolume.peekVoxel1px1py1pz();

					//y
					uint8_t iPreviousCubeIndexY = pCurrentBitmask[uXRegSpace][uYRegSpace-1];
					iPreviousCubeIndexY &= 204; //204 = 128+64+8+4
					iPreviousCubeIndexY >>= 2;

					//x
					uint8_t iPreviousCubeIndexX = pCurrentBitmask[uXRegSpace-1][uYRegSpace];
					iPreviousCubeIndexX &= 170; //170 = 128+32+8+2
					iPreviousCubeIndexX >>= 1;

					iCubeIndex = iPreviousCubeIndexX | iPreviousCubeIndexY;

					if (v110 == 0) iCubeIndex |= 8;
					if (v111 == 0) iCubeIndex |= 128;
				}
				else //previous X not available
				{
					v010 = m_sampVolume.peekVoxel0px1py0pz();
					v110 = m_sampVolume.peekVoxel1px1py0pz();

					v011 = m_sampVolume.peekVoxel0px1py1pz();
					v111 = m_sampVolume.peekVoxel1px1py1pz();

					//y
					uint8_t iPreviousCubeIndexY = pCurrentBitmask[uXRegSpace][uYRegSpace-1];
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
					v100 = m_sampVolume.peekVoxel1px0py0pz();
					v110 = m_sampVolume.peekVoxel1px1py0pz();

					v101 = m_sampVolume.peekVoxel1px0py1pz();
					v111 = m_sampVolume.peekVoxel1px1py1pz();

					//x
					uint8_t iPreviousCubeIndexX = pCurrentBitmask[uXRegSpace-1][uYRegSpace];
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
					v000 = m_sampVolume.getVoxel();
					v100 = m_sampVolume.peekVoxel1px0py0pz();
					v010 = m_sampVolume.peekVoxel0px1py0pz();
					v110 = m_sampVolume.peekVoxel1px1py0pz();

					v001 = m_sampVolume.peekVoxel0px0py1pz();
					v101 = m_sampVolume.peekVoxel1px0py1pz();
					v011 = m_sampVolume.peekVoxel0px1py1pz();
					v111 = m_sampVolume.peekVoxel1px1py1pz();

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
		pCurrentBitmask[uXRegSpace][uYVolSpace- m_regInputCropped.getLowerCorner().getY()] = iCubeIndex;

		if(edgeTable[iCubeIndex] != 0)
		{
			++m_uNoOfOccupiedCells;
		}
	}

	void SurfaceExtractor::generateVerticesForSlice(const Array2DUint8& pCurrentBitmask,
		Array2DInt32& m_pCurrentVertexIndicesX,
		Array2DInt32& m_pCurrentVertexIndicesY,
		Array2DInt32& m_pCurrentVertexIndicesZ)
	{
		uint16_t uZVolSpace = m_regSliceCurrent.getLowerCorner().getZ();
		const uint16_t uZRegSpace = uZVolSpace - m_regInputCropped.getLowerCorner().getZ();
		//bool isZEdge = ((uZVolSpace == m_regInputCropped.getLowerCorner().getZ()) || (uZVolSpace == m_regInputCropped.getUpperCorner().getZ()));
		bool isNegZEdge = (uZVolSpace == m_regInputCropped.getLowerCorner().getZ());
		bool isPosZEdge = (uZVolSpace == m_regInputCropped.getUpperCorner().getZ());

		//Iterate over each cell in the region
		for(uint16_t uYVolSpace = m_regSliceCurrent.getLowerCorner().getY(); uYVolSpace <= m_regSliceCurrent.getUpperCorner().getY(); uYVolSpace++)
		{
			const uint16_t uYRegSpace = uYVolSpace - m_regInputCropped.getLowerCorner().getY();
			//bool isYEdge = ((uYVolSpace == m_regInputCropped.getLowerCorner().getY()) || (uYVolSpace == m_regInputCropped.getUpperCorner().getY()));
			bool isNegYEdge = (uYVolSpace == m_regInputCropped.getLowerCorner().getY());
			bool isPosYEdge = (uYVolSpace == m_regInputCropped.getUpperCorner().getY());

			for(uint16_t uXVolSpace = m_regSliceCurrent.getLowerCorner().getX(); uXVolSpace <= m_regSliceCurrent.getUpperCorner().getX(); uXVolSpace++)
			{		
				//Current position
				const uint16_t uXRegSpace = uXVolSpace - m_regInputCropped.getLowerCorner().getX();
				//bool isXEdge = ((uXVolSpace == m_regInputCropped.getLowerCorner().getX()) || (uXVolSpace == m_regInputCropped.getUpperCorner().getX()));
				bool isNegXEdge = (uXVolSpace == m_regInputCropped.getLowerCorner().getX());
				bool isPosXEdge = (uXVolSpace == m_regInputCropped.getUpperCorner().getX());

				//Determine the index into the edge table which tells us which vertices are inside of the surface
				uint8_t iCubeIndex = pCurrentBitmask[uXRegSpace][uYRegSpace];

				/* Cube is entirely in/out of the surface */
				if (edgeTable[iCubeIndex] == 0)
				{
					continue;
				}

				//Check whether the generated vertex will lie on the edge of the region


				m_sampVolume.setPosition(uXVolSpace,uYVolSpace,uZVolSpace);
				const uint8_t v000 = m_sampVolume.getVoxel();

				/* Find the vertices where the surface intersects the cube */
				if (edgeTable[iCubeIndex] & 1)
				{
					m_sampVolume.movePositiveX();
					const uint8_t v100 = m_sampVolume.getVoxel();
					const Vector3DFloat v3dPosition(static_cast<float>(uXVolSpace - m_regInputCropped.getLowerCorner().getX()) + 0.5f, static_cast<float>(uYVolSpace - m_regInputCropped.getLowerCorner().getY()), static_cast<float>(uZVolSpace - m_regInputCropped.getLowerCorner().getZ()));
					const Vector3DFloat v3dNormal(v000 > v100 ? 1.0f : -1.0f,0.0,0.0);
					const uint8_t uMaterial = v000 | v100; //Because one of these is 0, the or operation takes the max.
					SurfaceVertex surfaceVertex(v3dPosition, v3dNormal, uMaterial);
					//surfaceVertex.setOnGeometryEdge(isXEdge || isYEdge || isZEdge);
					surfaceVertex.setOnGeometryEdgeNegX(isNegXEdge);
					surfaceVertex.setOnGeometryEdgePosX(isPosXEdge);
					surfaceVertex.setOnGeometryEdgeNegY(isNegYEdge);
					surfaceVertex.setOnGeometryEdgePosY(isPosYEdge);
					surfaceVertex.setOnGeometryEdgeNegZ(isNegZEdge);
					surfaceVertex.setOnGeometryEdgePosZ(isPosZEdge);
					uint32_t uLastVertexIndex = m_meshCurrent->addVertex(surfaceVertex);
					m_pCurrentVertexIndicesX[uXVolSpace - m_regInputCropped.getLowerCorner().getX()][uYVolSpace - m_regInputCropped.getLowerCorner().getY()] = uLastVertexIndex;
				}
				if (edgeTable[iCubeIndex] & 8)
				{
					m_sampVolume.movePositiveY();
					const uint8_t v010 = m_sampVolume.getVoxel();
					const Vector3DFloat v3dPosition(static_cast<float>(uXVolSpace - m_regInputCropped.getLowerCorner().getX()), static_cast<float>(uYVolSpace - m_regInputCropped.getLowerCorner().getY()) + 0.5f, static_cast<float>(uZVolSpace - m_regInputCropped.getLowerCorner().getZ()));
					const Vector3DFloat v3dNormal(0.0,v000 > v010 ? 1.0f : -1.0f,0.0);
					const uint8_t uMaterial = v000 | v010; //Because one of these is 0, the or operation takes the max.
					SurfaceVertex surfaceVertex(v3dPosition, v3dNormal, uMaterial);
					//surfaceVertex.setOnGeometryEdge(isXEdge || isYEdge || isZEdge);
					surfaceVertex.setOnGeometryEdgeNegX(isNegXEdge);
					surfaceVertex.setOnGeometryEdgePosX(isPosXEdge);
					surfaceVertex.setOnGeometryEdgeNegY(isNegYEdge);
					surfaceVertex.setOnGeometryEdgePosY(isPosYEdge);
					surfaceVertex.setOnGeometryEdgeNegZ(isNegZEdge);
					surfaceVertex.setOnGeometryEdgePosZ(isPosZEdge);
					uint32_t uLastVertexIndex = m_meshCurrent->addVertex(surfaceVertex);
					m_pCurrentVertexIndicesY[uXVolSpace - m_regInputCropped.getLowerCorner().getX()][uYVolSpace - m_regInputCropped.getLowerCorner().getY()] = uLastVertexIndex;
				}
				if (edgeTable[iCubeIndex] & 256)
				{
					m_sampVolume.movePositiveZ();
					const uint8_t v001 = m_sampVolume.getVoxel();
					const Vector3DFloat v3dPosition(static_cast<float>(uXVolSpace - m_regInputCropped.getLowerCorner().getX()), static_cast<float>(uYVolSpace - m_regInputCropped.getLowerCorner().getY()), static_cast<float>(uZVolSpace - m_regInputCropped.getLowerCorner().getZ()) + 0.5f);
					const Vector3DFloat v3dNormal(0.0,0.0,v000 > v001 ? 1.0f : -1.0f);
					const uint8_t uMaterial = v000 | v001; //Because one of these is 0, the or operation takes the max.
					SurfaceVertex surfaceVertex(v3dPosition, v3dNormal, uMaterial);
					//surfaceVertex.setOnGeometryEdge(isXEdge || isYEdge || isZEdge);
					surfaceVertex.setOnGeometryEdgeNegX(isNegXEdge);
					surfaceVertex.setOnGeometryEdgePosX(isPosXEdge);
					surfaceVertex.setOnGeometryEdgeNegY(isNegYEdge);
					surfaceVertex.setOnGeometryEdgePosY(isPosYEdge);
					surfaceVertex.setOnGeometryEdgeNegZ(isNegZEdge);
					surfaceVertex.setOnGeometryEdgePosZ(isPosZEdge);
					uint32_t uLastVertexIndex = m_meshCurrent->addVertex(surfaceVertex);
					m_pCurrentVertexIndicesZ[uXVolSpace - m_regInputCropped.getLowerCorner().getX()][uYVolSpace - m_regInputCropped.getLowerCorner().getY()] = uLastVertexIndex;
				}
			}//For each cell
		}
	}

	void SurfaceExtractor::generateIndicesForSlice(const Array2DUint8& pPreviousBitmask,
		const Array2DInt32& m_pPreviousVertexIndicesX,
		const Array2DInt32& m_pPreviousVertexIndicesY,
		const Array2DInt32& m_pPreviousVertexIndicesZ,
		const Array2DInt32& m_pCurrentVertexIndicesX,
		const Array2DInt32& m_pCurrentVertexIndicesY,
		const Array2DInt32& m_pCurrentVertexIndicesZ)
	{
		int32_t indlist[12];
		for(int i = 0; i < 12; i++)
		{
			indlist[i] = -1;
		}

		for(uint16_t uYVolSpace = m_regSlicePrevious.getLowerCorner().getY(); uYVolSpace < m_regInputUncropped.getUpperCorner().getY(); uYVolSpace++)
		{
			for(uint16_t uXVolSpace = m_regSlicePrevious.getLowerCorner().getX(); uXVolSpace < m_regInputUncropped.getUpperCorner().getX(); uXVolSpace++)
			{		
				uint16_t uZVolSpace = m_regSlicePrevious.getLowerCorner().getZ();
				m_sampVolume.setPosition(uXVolSpace,uYVolSpace,uZVolSpace);	

				//Current position
				const uint16_t uXRegSpace = m_sampVolume.getPosX() - m_regInputCropped.getLowerCorner().getX();
				const uint16_t uYRegSpace = m_sampVolume.getPosY() - m_regInputCropped.getLowerCorner().getY();
				const uint16_t uZRegSpace = m_sampVolume.getPosZ() - m_regInputCropped.getLowerCorner().getZ();

				//Determine the index into the edge table which tells us which vertices are inside of the surface
				uint8_t iCubeIndex = pPreviousBitmask[uXRegSpace][uYRegSpace];

				/* Cube is entirely in/out of the surface */
				if (edgeTable[iCubeIndex] == 0)
				{
					continue;
				}

				/* Find the vertices where the surface intersects the cube */
				if (edgeTable[iCubeIndex] & 1)
				{
					indlist[0] = m_pPreviousVertexIndicesX[uXRegSpace][uYRegSpace];
					//assert(indlist[0] != -1);
				}
				if (edgeTable[iCubeIndex] & 2)
				{
					indlist[1] = m_pPreviousVertexIndicesY[uXRegSpace+1][uYRegSpace];
					//assert(indlist[1] != -1);
				}
				if (edgeTable[iCubeIndex] & 4)
				{
					indlist[2] = m_pPreviousVertexIndicesX[uXRegSpace][uYRegSpace+1];
					//assert(indlist[2] != -1);
				}
				if (edgeTable[iCubeIndex] & 8)
				{
					indlist[3] = m_pPreviousVertexIndicesY[uXRegSpace][uYRegSpace];
					//assert(indlist[3] != -1);
				}
				if (edgeTable[iCubeIndex] & 16)
				{
					indlist[4] = m_pCurrentVertexIndicesX[uXRegSpace][uYRegSpace];
					//assert(indlist[4] != -1);
				}
				if (edgeTable[iCubeIndex] & 32)
				{
					indlist[5] = m_pCurrentVertexIndicesY[uXRegSpace+1][uYRegSpace];
					//assert(indlist[5] != -1);
				}
				if (edgeTable[iCubeIndex] & 64)
				{
					indlist[6] = m_pCurrentVertexIndicesX[uXRegSpace][uYRegSpace+1];
					//assert(indlist[6] != -1);
				}
				if (edgeTable[iCubeIndex] & 128)
				{
					indlist[7] = m_pCurrentVertexIndicesY[uXRegSpace][uYRegSpace];
					//assert(indlist[7] != -1);
				}
				if (edgeTable[iCubeIndex] & 256)
				{
					indlist[8] = m_pPreviousVertexIndicesZ[uXRegSpace][uYRegSpace];
					//assert(indlist[8] != -1);
				}
				if (edgeTable[iCubeIndex] & 512)
				{
					indlist[9] = m_pPreviousVertexIndicesZ[uXRegSpace+1][uYRegSpace];
					//assert(indlist[9] != -1);
				}
				if (edgeTable[iCubeIndex] & 1024)
				{
					indlist[10] = m_pPreviousVertexIndicesZ[uXRegSpace+1][uYRegSpace+1];
					//assert(indlist[10] != -1);
				}
				if (edgeTable[iCubeIndex] & 2048)
				{
					indlist[11] = m_pPreviousVertexIndicesZ[uXRegSpace][uYRegSpace+1];
					//assert(indlist[11] != -1);
				}

				for (int i=0;triTable[iCubeIndex][i]!=-1;i+=3)
				{
					int32_t ind0 = indlist[triTable[iCubeIndex][i  ]];
					int32_t ind1 = indlist[triTable[iCubeIndex][i+1]];
					int32_t ind2 = indlist[triTable[iCubeIndex][i+2]];

					if((ind0 != -1) && (ind1 != -1) && (ind2 != -1))
					{
						assert(ind0 >= 0);
						assert(ind1 >= 0);
						assert(ind2 >= 0);

						assert(ind0 < 1000000);
						assert(ind1 < 1000000);
						assert(ind2 < 1000000);
						m_meshCurrent->addTriangle(ind0, ind1, ind2);
					}
				}//For each triangle
			}//For each cell
		}
	}
}
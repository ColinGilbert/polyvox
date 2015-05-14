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

#include "PolyVox/Impl/Timer.h"

namespace PolyVox
{
	template<typename VolumeType, typename MeshType, typename ControllerType>
	MarchingCubesSurfaceExtractor<VolumeType, MeshType, ControllerType>::MarchingCubesSurfaceExtractor(VolumeType* volData, Region region, MeshType* result, ControllerType controller)
		:m_volData(volData)
		,m_sampVolume(volData)
		,m_meshCurrent(result)
		,m_regSizeInVoxels(region)
		,m_controller(controller)
		,m_tThreshold(m_controller.getThreshold())
	{
		POLYVOX_THROW_IF(m_meshCurrent == nullptr, std::invalid_argument, "Provided mesh cannot be null");
		//m_regSizeInVoxels.cropTo(m_volData->getEnclosingRegion());
		m_regSizeInCells = m_regSizeInVoxels;
		m_regSizeInCells.setUpperCorner(m_regSizeInCells.getUpperCorner() - Vector3DInt32(1,1,1));
	}

	template<typename VolumeType, typename MeshType, typename ControllerType>
	void MarchingCubesSurfaceExtractor<VolumeType, MeshType, ControllerType>::execute()
	{		
		Timer timer;
		m_meshCurrent->clear();

		const uint32_t uArrayWidth = m_regSizeInVoxels.getUpperX() - m_regSizeInVoxels.getLowerX() + 1;
		const uint32_t uArrayHeight = m_regSizeInVoxels.getUpperY() - m_regSizeInVoxels.getLowerY() + 1;
		const uint32_t uArrayDepth = m_regSizeInVoxels.getUpperZ() - m_regSizeInVoxels.getLowerZ() + 1;

		//For edge indices
		Array3DInt32 pIndicesX(uArrayWidth, uArrayHeight, uArrayDepth);
		Array3DInt32 pIndicesY(uArrayWidth, uArrayHeight, uArrayDepth);
		Array3DInt32 pIndicesZ(uArrayWidth, uArrayHeight, uArrayDepth);

		memset(pIndicesX.getRawData(), 0xff, pIndicesX.getNoOfElements() * 4);
		memset(pIndicesY.getRawData(), 0xff, pIndicesY.getNoOfElements() * 4);
		memset(pIndicesZ.getRawData(), 0xff, pIndicesZ.getNoOfElements() * 4);

		Array3DUint8 pBitmask(uArrayWidth, uArrayHeight, uArrayDepth);

		//Create a region corresponding to the first slice
		m_regSlicePrevious = m_regSizeInVoxels;
		Vector3DInt32 v3dUpperCorner = m_regSlicePrevious.getUpperCorner();
		v3dUpperCorner.setZ(m_regSlicePrevious.getLowerZ()); //Set the upper z to the lower z to make it one slice thick.
		m_regSlicePrevious.setUpperCorner(v3dUpperCorner);
		m_regSliceCurrent = m_regSlicePrevious;	

		computeBitmaskForSlice<true>(pBitmask);

		generateVerticesForSlice(pBitmask, pIndicesX, pIndicesY, pIndicesZ, 0);

		m_regSlicePrevious = m_regSliceCurrent;
		m_regSliceCurrent.shift(Vector3DInt32(0,0,1));

		//Process the other slices (previous slice is available)
		for(int32_t uSlice = 1; uSlice <= m_regSizeInVoxels.getUpperZ() - m_regSizeInVoxels.getLowerZ(); uSlice++)
		{
			generateVerticesForSlice(pBitmask, pIndicesX, pIndicesY, pIndicesZ, uSlice);

			generateIndicesForSlice(pBitmask, pIndicesX, pIndicesY, pIndicesZ);

			m_regSlicePrevious = m_regSliceCurrent;
			m_regSliceCurrent.shift(Vector3DInt32(0,0,1));
		}

		m_meshCurrent->setOffset(m_regSizeInVoxels.getLowerCorner());

		POLYVOX_LOG_TRACE("Marching cubes surface extraction took ", timer.elapsedTimeInMilliSeconds(),
			"ms (Region size = ", m_regSizeInVoxels.getWidthInVoxels(), "x", m_regSizeInVoxels.getHeightInVoxels(),
			"x", m_regSizeInVoxels.getDepthInVoxels(), ")");
	}

	template<typename VolumeType, typename MeshType, typename ControllerType>
	template<bool isPrevZAvail>
	void MarchingCubesSurfaceExtractor<VolumeType, MeshType, ControllerType>::computeBitmaskForSlice(Array3DUint8& pBitmask)
	{
		const int32_t iMaxXVolSpace = m_regSizeInVoxels.getUpperX();
		const int32_t iMaxYVolSpace = m_regSizeInVoxels.getUpperY();
		const int32_t iMaxZVolSpace = m_regSizeInVoxels.getUpperZ();

		for (int32_t iZVolSpace = m_regSliceCurrent.getLowerZ(); iZVolSpace <= iMaxZVolSpace; iZVolSpace++)
		{
			uint32_t uZRegSpace = iZVolSpace - m_regSizeInVoxels.getLowerZ();

			//Process all remaining elemnents of the slice. In this case, previous x and y values are always available
			for (int32_t iYVolSpace = m_regSliceCurrent.getLowerY(); iYVolSpace <= iMaxYVolSpace; iYVolSpace++)
			{
				m_sampVolume.setPosition(m_regSliceCurrent.getLowerX(), iYVolSpace, iZVolSpace);
				for (int32_t iXVolSpace = m_regSliceCurrent.getLowerX(); iXVolSpace <= iMaxXVolSpace; iXVolSpace++)
				{
					uint32_t uXRegSpace = iXVolSpace - m_regSizeInVoxels.getLowerX();
					uint32_t uYRegSpace = iYVolSpace - m_regSizeInVoxels.getLowerY();

					m_sampVolume.movePositiveX();

					m_sampVolume.setPosition(iXVolSpace, iYVolSpace, iZVolSpace);

					uint8_t iCubeIndex = 0;

					typename VolumeType::VoxelType v000 = m_sampVolume.getVoxel();
					typename VolumeType::VoxelType v100 = m_sampVolume.peekVoxel1px0py0pz();
					typename VolumeType::VoxelType v010 = m_sampVolume.peekVoxel0px1py0pz();
					typename VolumeType::VoxelType v110 = m_sampVolume.peekVoxel1px1py0pz();

					typename VolumeType::VoxelType v001 = m_sampVolume.peekVoxel0px0py1pz();
					typename VolumeType::VoxelType v101 = m_sampVolume.peekVoxel1px0py1pz();
					typename VolumeType::VoxelType v011 = m_sampVolume.peekVoxel0px1py1pz();
					typename VolumeType::VoxelType v111 = m_sampVolume.peekVoxel1px1py1pz();

					/*typename VolumeType::VoxelType v000 = m_sampVolume.peekVoxel0px0py1nz();
					typename VolumeType::VoxelType v100 = m_sampVolume.peekVoxel1px0py1nz();
					typename VolumeType::VoxelType v010 = m_sampVolume.peekVoxel0px1py1nz();
					typename VolumeType::VoxelType v110 = m_sampVolume.peekVoxel1px1py1nz();

					typename VolumeType::VoxelType v001 = m_sampVolume.peekVoxel0px0py0pz();
					typename VolumeType::VoxelType v101 = m_sampVolume.peekVoxel1px0py0pz();
					typename VolumeType::VoxelType v011 = m_sampVolume.peekVoxel0px1py0pz();
					typename VolumeType::VoxelType v111 = m_sampVolume.peekVoxel1px1py0pz();*/

					if (m_controller.convertToDensity(v000) < m_tThreshold) iCubeIndex |= 1;
					if (m_controller.convertToDensity(v100) < m_tThreshold) iCubeIndex |= 2;
					if (m_controller.convertToDensity(v010) < m_tThreshold) iCubeIndex |= 4;
					if (m_controller.convertToDensity(v110) < m_tThreshold) iCubeIndex |= 8;
					if (m_controller.convertToDensity(v001) < m_tThreshold) iCubeIndex |= 16;
					if (m_controller.convertToDensity(v101) < m_tThreshold) iCubeIndex |= 32;
					if (m_controller.convertToDensity(v011) < m_tThreshold) iCubeIndex |= 64;
					if (m_controller.convertToDensity(v111) < m_tThreshold) iCubeIndex |= 128;

					//Save the bitmask
					pBitmask(uXRegSpace, uYRegSpace, uZRegSpace) = iCubeIndex;
				}
			}
		}
	}

	template<typename VolumeType, typename MeshType, typename ControllerType>
	void MarchingCubesSurfaceExtractor<VolumeType, MeshType, ControllerType>::generateVerticesForSlice(const Array3DUint8& pBitmask,
		Array3DInt32& pIndicesX,
		Array3DInt32& pIndicesY,
		Array3DInt32& pIndicesZ,
		uint32_t uSlice)
	{
		const uint32_t uZRegSpace = uSlice;

		const int32_t iZVolSpace = m_regSizeInVoxels.getLowerZ() + uZRegSpace;

		//Iterate over each cell in the region
		for(int32_t iYVolSpace = m_regSliceCurrent.getLowerY(); iYVolSpace <= m_regSliceCurrent.getUpperY(); iYVolSpace++)
		{
			const uint32_t uYRegSpace = iYVolSpace - m_regSizeInVoxels.getLowerY();

			for(int32_t iXVolSpace = m_regSliceCurrent.getLowerX(); iXVolSpace <= m_regSliceCurrent.getUpperX(); iXVolSpace++)
			{		
				//Current position
				const uint32_t uXRegSpace = iXVolSpace - m_regSizeInVoxels.getLowerX();

				//Determine the index into the edge table which tells us which vertices are inside of the surface
				const uint8_t iCubeIndex = pBitmask(uXRegSpace, uYRegSpace, uZRegSpace);

				/* Cube is entirely in/out of the surface */
				if (edgeTable[iCubeIndex] == 0)
				{
					continue;
				}

				//Check whether the generated vertex will lie on the edge of the region


				m_sampVolume.setPosition(iXVolSpace,iYVolSpace,iZVolSpace);
				const typename VolumeType::VoxelType v000 = m_sampVolume.getVoxel();
				const Vector3DFloat n000 = computeCentralDifferenceGradient(m_sampVolume);

				/* Find the vertices where the surface intersects the cube */
				if (edgeTable[iCubeIndex] & 1)
				{
					m_sampVolume.movePositiveX();
					const typename VolumeType::VoxelType v100 = m_sampVolume.getVoxel();
					POLYVOX_ASSERT(v000 != v100, "Attempting to insert vertex between two voxels with the same value");
					const Vector3DFloat n100 = computeCentralDifferenceGradient(m_sampVolume);

					const float fInterp = static_cast<float>(m_tThreshold - m_controller.convertToDensity(v000)) / static_cast<float>(m_controller.convertToDensity(v100) - m_controller.convertToDensity(v000));

					const Vector3DFloat v3dPosition(static_cast<float>(iXVolSpace - m_regSizeInVoxels.getLowerX()) + fInterp, static_cast<float>(iYVolSpace - m_regSizeInVoxels.getLowerY()), static_cast<float>(iZVolSpace - m_regSizeInCells.getLowerZ()));
					const Vector3DUint16 v3dScaledPosition(static_cast<uint16_t>(v3dPosition.getX() * 256.0f), static_cast<uint16_t>(v3dPosition.getY() * 256.0f), static_cast<uint16_t>(v3dPosition.getZ() * 256.0f));

					Vector3DFloat v3dNormal = (n100*fInterp) + (n000*(1-fInterp));

					// The gradient for a voxel can be zero (e.g. solid voxel surrounded by empty ones) and so
					// the interpolated normal can also be zero (e.g. a grid of alternating solid and empty voxels).
					if(v3dNormal.lengthSquared() > 0.000001f) 
					{
						v3dNormal.normalise();
					}

					// Allow the controller to decide how the material should be derived from the voxels.
					const typename VolumeType::VoxelType uMaterial = m_controller.blendMaterials(v000, v100, fInterp);

					MarchingCubesVertex<typename VolumeType::VoxelType> surfaceVertex;
					surfaceVertex.encodedPosition = v3dScaledPosition;
					surfaceVertex.encodedNormal = encodeNormal(v3dNormal);
					surfaceVertex.data = uMaterial;

					const uint32_t uLastVertexIndex = m_meshCurrent->addVertex(surfaceVertex);
					pIndicesX(iXVolSpace - m_regSizeInVoxels.getLowerX(), iYVolSpace - m_regSizeInVoxels.getLowerY(), iZVolSpace - m_regSizeInVoxels.getLowerZ()) = uLastVertexIndex;

					m_sampVolume.moveNegativeX();
				}
				if (edgeTable[iCubeIndex] & 8)
				{
					m_sampVolume.movePositiveY();
					const typename VolumeType::VoxelType v010 = m_sampVolume.getVoxel();
					POLYVOX_ASSERT(v000 != v010, "Attempting to insert vertex between two voxels with the same value");
					const Vector3DFloat n010 = computeCentralDifferenceGradient(m_sampVolume);

					const float fInterp = static_cast<float>(m_tThreshold - m_controller.convertToDensity(v000)) / static_cast<float>(m_controller.convertToDensity(v010) - m_controller.convertToDensity(v000));

					const Vector3DFloat v3dPosition(static_cast<float>(iXVolSpace - m_regSizeInVoxels.getLowerX()), static_cast<float>(iYVolSpace - m_regSizeInVoxels.getLowerY()) + fInterp, static_cast<float>(iZVolSpace - m_regSizeInVoxels.getLowerZ()));
					const Vector3DUint16 v3dScaledPosition(static_cast<uint16_t>(v3dPosition.getX() * 256.0f), static_cast<uint16_t>(v3dPosition.getY() * 256.0f), static_cast<uint16_t>(v3dPosition.getZ() * 256.0f));

					Vector3DFloat v3dNormal = (n010*fInterp) + (n000*(1-fInterp));

					// The gradient for a voxel can be zero (e.g. solid voxel surrounded by empty ones) and so
					// the interpolated normal can also be zero (e.g. a grid of alternating solid and empty voxels).
					if(v3dNormal.lengthSquared() > 0.000001f) 
					{
						v3dNormal.normalise();
					}

					// Allow the controller to decide how the material should be derived from the voxels.
					const typename VolumeType::VoxelType uMaterial = m_controller.blendMaterials(v000, v010, fInterp);

					MarchingCubesVertex<typename VolumeType::VoxelType> surfaceVertex;
					surfaceVertex.encodedPosition = v3dScaledPosition;
					surfaceVertex.encodedNormal = encodeNormal(v3dNormal);
					surfaceVertex.data = uMaterial;

					uint32_t uLastVertexIndex = m_meshCurrent->addVertex(surfaceVertex);
					pIndicesY(iXVolSpace - m_regSizeInVoxels.getLowerX(), iYVolSpace - m_regSizeInVoxels.getLowerY(), iZVolSpace - m_regSizeInVoxels.getLowerZ()) = uLastVertexIndex;

					m_sampVolume.moveNegativeY();
				}
				if (edgeTable[iCubeIndex] & 256)
				{
					m_sampVolume.movePositiveZ();
					const typename VolumeType::VoxelType v001 = m_sampVolume.getVoxel();
					POLYVOX_ASSERT(v000 != v001, "Attempting to insert vertex between two voxels with the same value");
					const Vector3DFloat n001 = computeCentralDifferenceGradient(m_sampVolume);

					const float fInterp = static_cast<float>(m_tThreshold - m_controller.convertToDensity(v000)) / static_cast<float>(m_controller.convertToDensity(v001) - m_controller.convertToDensity(v000));

					const Vector3DFloat v3dPosition(static_cast<float>(iXVolSpace - m_regSizeInVoxels.getLowerX()), static_cast<float>(iYVolSpace - m_regSizeInVoxels.getLowerY()), static_cast<float>(iZVolSpace - m_regSizeInVoxels.getLowerZ()) + fInterp);
					const Vector3DUint16 v3dScaledPosition(static_cast<uint16_t>(v3dPosition.getX() * 256.0f), static_cast<uint16_t>(v3dPosition.getY() * 256.0f), static_cast<uint16_t>(v3dPosition.getZ() * 256.0f));

					Vector3DFloat v3dNormal = (n001*fInterp) + (n000*(1-fInterp));
					// The gradient for a voxel can be zero (e.g. solid voxel surrounded by empty ones) and so
					// the interpolated normal can also be zero (e.g. a grid of alternating solid and empty voxels).
					if(v3dNormal.lengthSquared() > 0.000001f) 
					{
						v3dNormal.normalise();
					}

					// Allow the controller to decide how the material should be derived from the voxels.
					const typename VolumeType::VoxelType uMaterial = m_controller.blendMaterials(v000, v001, fInterp);

					MarchingCubesVertex<typename VolumeType::VoxelType> surfaceVertex;
					surfaceVertex.encodedPosition = v3dScaledPosition;
					surfaceVertex.encodedNormal = encodeNormal(v3dNormal);
					surfaceVertex.data = uMaterial;

					const uint32_t uLastVertexIndex = m_meshCurrent->addVertex(surfaceVertex);
					pIndicesZ(iXVolSpace - m_regSizeInVoxels.getLowerX(), iYVolSpace - m_regSizeInVoxels.getLowerY(), iZVolSpace - m_regSizeInVoxels.getLowerZ()) = uLastVertexIndex;

					m_sampVolume.moveNegativeZ();
				}
			}//For each cell
		}
	}

	template<typename VolumeType, typename MeshType, typename ControllerType>
	void MarchingCubesSurfaceExtractor<VolumeType, MeshType, ControllerType>::generateIndicesForSlice(const Array3DUint8& pBitmask,
		const Array3DInt32& pIndicesX,
		const Array3DInt32& pIndicesY,
		const Array3DInt32& pIndicesZ)
	{
		int32_t indlist[12];
		for(int i = 0; i < 12; i++)
		{
			indlist[i] = -1;
		}

		const int32_t iZVolSpace = m_regSlicePrevious.getLowerZ();
		
		for(int32_t iYVolSpace = m_regSlicePrevious.getLowerY(); iYVolSpace <= m_regSizeInCells.getUpperY(); iYVolSpace++)
		{
			for(int32_t iXVolSpace = m_regSlicePrevious.getLowerX(); iXVolSpace <= m_regSizeInCells.getUpperX(); iXVolSpace++)
			{
				m_sampVolume.setPosition(iXVolSpace,iYVolSpace,iZVolSpace);	

				//Current position
				const uint32_t uXRegSpace = m_sampVolume.getPosition().getX() - m_regSizeInVoxels.getLowerX();
				const uint32_t uYRegSpace = m_sampVolume.getPosition().getY() - m_regSizeInVoxels.getLowerY();
				const uint32_t uZRegSpace = m_sampVolume.getPosition().getZ() - m_regSizeInVoxels.getLowerZ();

				//Determine the index into the edge table which tells us which vertices are inside of the surface
				const uint8_t iCubeIndex = pBitmask(uXRegSpace, uYRegSpace, uZRegSpace);

				/* Cube is entirely in/out of the surface */
				if (edgeTable[iCubeIndex] == 0)
				{
					continue;
				}

				/* Find the vertices where the surface intersects the cube */
				if (edgeTable[iCubeIndex] & 1)
				{
					indlist[0] = pIndicesX(uXRegSpace, uYRegSpace, uZRegSpace);
				}
				if (edgeTable[iCubeIndex] & 2)
				{
					indlist[1] = pIndicesY(uXRegSpace + 1, uYRegSpace, uZRegSpace);
				}
				if (edgeTable[iCubeIndex] & 4)
				{
					indlist[2] = pIndicesX(uXRegSpace, uYRegSpace + 1, uZRegSpace);
				}
				if (edgeTable[iCubeIndex] & 8)
				{
					indlist[3] = pIndicesY(uXRegSpace, uYRegSpace, uZRegSpace);
				}
				if (edgeTable[iCubeIndex] & 16)
				{
					indlist[4] = pIndicesX(uXRegSpace, uYRegSpace, uZRegSpace + 1);
				}
				if (edgeTable[iCubeIndex] & 32)
				{
					indlist[5] = pIndicesY(uXRegSpace + 1, uYRegSpace, uZRegSpace + 1);
				}
				if (edgeTable[iCubeIndex] & 64)
				{
					indlist[6] = pIndicesX(uXRegSpace, uYRegSpace + 1, uZRegSpace + 1);
				}
				if (edgeTable[iCubeIndex] & 128)
				{
					indlist[7] = pIndicesY(uXRegSpace, uYRegSpace, uZRegSpace + 1);
				}
				if (edgeTable[iCubeIndex] & 256)
				{
					indlist[8] = pIndicesZ(uXRegSpace, uYRegSpace, uZRegSpace);
				}
				if (edgeTable[iCubeIndex] & 512)
				{
					indlist[9] = pIndicesZ(uXRegSpace + 1, uYRegSpace, uZRegSpace);
				}
				if (edgeTable[iCubeIndex] & 1024)
				{
					indlist[10] = pIndicesZ(uXRegSpace + 1, uYRegSpace + 1, uZRegSpace);
				}
				if (edgeTable[iCubeIndex] & 2048)
				{
					indlist[11] = pIndicesZ(uXRegSpace, uYRegSpace + 1, uZRegSpace);
				}

				for (int i=0;triTable[iCubeIndex][i]!=-1;i+=3)
				{
					const int32_t ind0 = indlist[triTable[iCubeIndex][i  ]];
					const int32_t ind1 = indlist[triTable[iCubeIndex][i+1]];
					const int32_t ind2 = indlist[triTable[iCubeIndex][i+2]];

					if((ind0 != -1) && (ind1 != -1) && (ind2 != -1))
					{
						m_meshCurrent->addTriangle(ind0, ind1, ind2);
					}
				}//For each triangle
			}//For each cell
		}
	}
}

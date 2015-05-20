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
	}

	template<typename VolumeType, typename MeshType, typename ControllerType>
	void MarchingCubesSurfaceExtractor<VolumeType, MeshType, ControllerType>::execute()
	{		
		Timer timer;
		m_meshCurrent->clear();

		const uint32_t uArrayWidth = m_regSizeInVoxels.getUpperX() - m_regSizeInVoxels.getLowerX() + 2;
		const uint32_t uArrayHeight = m_regSizeInVoxels.getUpperY() - m_regSizeInVoxels.getLowerY() + 2;
		const uint32_t uArrayDepth = m_regSizeInVoxels.getUpperZ() - m_regSizeInVoxels.getLowerZ() + 2;

		//For edge indices
		Array3DInt32 pIndicesX(uArrayWidth, uArrayHeight, uArrayDepth);
		Array3DInt32 pIndicesY(uArrayWidth, uArrayHeight, uArrayDepth);
		Array3DInt32 pIndicesZ(uArrayWidth, uArrayHeight, uArrayDepth);

		memset(pIndicesX.getRawData(), 0xff, pIndicesX.getNoOfElements() * 4);
		memset(pIndicesY.getRawData(), 0xff, pIndicesY.getNoOfElements() * 4);
		memset(pIndicesZ.getRawData(), 0xff, pIndicesZ.getNoOfElements() * 4);

		Array3DUint8 pBitmask(uArrayWidth, uArrayHeight, uArrayDepth);
		memset(pBitmask.getRawData(), 0x00, pBitmask.getNoOfElements());

		computeBitmaskForSlice<true>(pBitmask, pIndicesX, pIndicesY, pIndicesZ);

		m_meshCurrent->setOffset(m_regSizeInVoxels.getLowerCorner());

		POLYVOX_LOG_TRACE("Marching cubes surface extraction took ", timer.elapsedTimeInMilliSeconds(),
			"ms (Region size = ", m_regSizeInVoxels.getWidthInVoxels(), "x", m_regSizeInVoxels.getHeightInVoxels(),
			"x", m_regSizeInVoxels.getDepthInVoxels(), ")");
	}

	template<typename VolumeType, typename MeshType, typename ControllerType>
	template<bool isPrevZAvail>
	void MarchingCubesSurfaceExtractor<VolumeType, MeshType, ControllerType>::computeBitmaskForSlice(Array3DUint8& pBitmask,
		Array3DInt32& pIndicesX,
		Array3DInt32& pIndicesY,
		Array3DInt32& pIndicesZ)
	{
		for (int32_t iZVolSpace = m_regSizeInVoxels.getLowerZ(); iZVolSpace <= m_regSizeInVoxels.getUpperZ(); iZVolSpace++)
		{
			const uint32_t uZRegSpace = iZVolSpace - m_regSizeInVoxels.getLowerZ();

			for (int32_t iYVolSpace = m_regSizeInVoxels.getLowerY(); iYVolSpace <= m_regSizeInVoxels.getUpperY(); iYVolSpace++)
			{
				const uint32_t uYRegSpace = iYVolSpace - m_regSizeInVoxels.getLowerY();

				for (int32_t iXVolSpace = m_regSizeInVoxels.getLowerX(); iXVolSpace <= m_regSizeInVoxels.getUpperX(); iXVolSpace++)
				{
					const uint32_t uXRegSpace = iXVolSpace - m_regSizeInVoxels.getLowerX();

					m_sampVolume.setPosition(iXVolSpace, iYVolSpace, iZVolSpace);

					typename VolumeType::VoxelType v000;
					typename VolumeType::VoxelType v100;
					typename VolumeType::VoxelType v010;
					typename VolumeType::VoxelType v110;

					typename VolumeType::VoxelType v001;
					typename VolumeType::VoxelType v101;
					typename VolumeType::VoxelType v011;
					typename VolumeType::VoxelType v111;

					v000 = iXVolSpace > m_regSizeInVoxels.getLowerX() && iYVolSpace > m_regSizeInVoxels.getLowerY() && iZVolSpace > m_regSizeInVoxels.getLowerZ() ? m_sampVolume.peekVoxel1nx1ny1nz() : VolumeType::VoxelType();
					v100 = iYVolSpace > m_regSizeInVoxels.getLowerY() && iZVolSpace > m_regSizeInVoxels.getLowerZ() ? m_sampVolume.peekVoxel0px1ny1nz() : VolumeType::VoxelType();
					v010 = iXVolSpace > m_regSizeInVoxels.getLowerX() && iZVolSpace > m_regSizeInVoxels.getLowerZ() ? m_sampVolume.peekVoxel1nx0py1nz() : VolumeType::VoxelType();
					v110 = iZVolSpace > m_regSizeInVoxels.getLowerZ() ? m_sampVolume.peekVoxel0px0py1nz() : VolumeType::VoxelType();

					v001 = iXVolSpace > m_regSizeInVoxels.getLowerX() && iYVolSpace > m_regSizeInVoxels.getLowerY() ? m_sampVolume.peekVoxel1nx1ny0pz() : VolumeType::VoxelType();
					v101 = iYVolSpace > m_regSizeInVoxels.getLowerY() ? m_sampVolume.peekVoxel0px1ny0pz() : VolumeType::VoxelType();
					v011 = iXVolSpace > m_regSizeInVoxels.getLowerX() ? m_sampVolume.peekVoxel1nx0py0pz() : VolumeType::VoxelType();
					v111 = m_sampVolume.peekVoxel0px0py0pz();

					uint8_t iCubeIndex = 0;

					bool isPrevXAvail = iXVolSpace > m_regSizeInVoxels.getLowerX();
					bool isPrevYAvail = iYVolSpace > m_regSizeInVoxels.getLowerY();
					bool isPrevZAvail = iZVolSpace > m_regSizeInVoxels.getLowerZ();

					if (isPrevZAvail)
					{
						if (isPrevYAvail)
						{
							if (isPrevXAvail)
							{
								v111 = m_sampVolume.peekVoxel0px0py0pz();

								//z
								uint8_t iPreviousCubeIndexZ = pBitmask(uXRegSpace, uYRegSpace, uZRegSpace - 1);
								iPreviousCubeIndexZ >>= 4;

								//y
								uint8_t iPreviousCubeIndexY = pBitmask(uXRegSpace, uYRegSpace - 1, uZRegSpace);
								iPreviousCubeIndexY &= 192; //192 = 128 + 64
								iPreviousCubeIndexY >>= 2;

								//x
								uint8_t iPreviousCubeIndexX = pBitmask(uXRegSpace - 1, uYRegSpace, uZRegSpace);
								iPreviousCubeIndexX &= 128;
								iPreviousCubeIndexX >>= 1;

								iCubeIndex = iPreviousCubeIndexX | iPreviousCubeIndexY | iPreviousCubeIndexZ;

								if (m_controller.convertToDensity(v111) < m_tThreshold) iCubeIndex |= 128;
							}
							else //previous X not available
							{
								v011 = iXVolSpace > m_regSizeInVoxels.getLowerX() ? m_sampVolume.peekVoxel1nx0py0pz() : VolumeType::VoxelType();
								v111 = m_sampVolume.peekVoxel0px0py0pz();

								//z
								uint8_t iPreviousCubeIndexZ = pBitmask(uXRegSpace, uYRegSpace, uZRegSpace - 1);
								iPreviousCubeIndexZ >>= 4;

								//y
								uint8_t iPreviousCubeIndexY = pBitmask(uXRegSpace, uYRegSpace - 1, uZRegSpace);
								iPreviousCubeIndexY &= 192; //192 = 128 + 64
								iPreviousCubeIndexY >>= 2;

								iCubeIndex = iPreviousCubeIndexY | iPreviousCubeIndexZ;

								if (m_controller.convertToDensity(v011) < m_tThreshold) iCubeIndex |= 64;
								if (m_controller.convertToDensity(v111) < m_tThreshold) iCubeIndex |= 128;
							}
						}
						else //previous Y not available
						{
							if (isPrevXAvail)
							{
								v101 = iYVolSpace > m_regSizeInVoxels.getLowerY() ? m_sampVolume.peekVoxel0px1ny0pz() : VolumeType::VoxelType();
								v111 = m_sampVolume.peekVoxel0px0py0pz();

								//z
								uint8_t iPreviousCubeIndexZ = pBitmask(uXRegSpace, uYRegSpace, uZRegSpace - 1);
								iPreviousCubeIndexZ >>= 4;

								//x
								uint8_t iPreviousCubeIndexX = pBitmask(uXRegSpace - 1, uYRegSpace, uZRegSpace);
								iPreviousCubeIndexX &= 160; //160 = 128+32
								iPreviousCubeIndexX >>= 1;

								iCubeIndex = iPreviousCubeIndexX | iPreviousCubeIndexZ;

								if (m_controller.convertToDensity(v101) < m_tThreshold) iCubeIndex |= 32;
								if (m_controller.convertToDensity(v111) < m_tThreshold) iCubeIndex |= 128;
							}
							else //previous X not available
							{
								v001 = iXVolSpace > m_regSizeInVoxels.getLowerX() && iYVolSpace > m_regSizeInVoxels.getLowerY() ? m_sampVolume.peekVoxel1nx1ny0pz() : VolumeType::VoxelType();
								v101 = iYVolSpace > m_regSizeInVoxels.getLowerY() ? m_sampVolume.peekVoxel0px1ny0pz() : VolumeType::VoxelType();
								v011 = iXVolSpace > m_regSizeInVoxels.getLowerX() ? m_sampVolume.peekVoxel1nx0py0pz() : VolumeType::VoxelType();
								v111 = m_sampVolume.peekVoxel0px0py0pz();

								//z
								uint8_t iPreviousCubeIndexZ = pBitmask(uXRegSpace, uYRegSpace, uZRegSpace - 1);
								iCubeIndex = iPreviousCubeIndexZ >> 4;

								if (m_controller.convertToDensity(v001) < m_tThreshold) iCubeIndex |= 16;
								if (m_controller.convertToDensity(v101) < m_tThreshold) iCubeIndex |= 32;
								if (m_controller.convertToDensity(v011) < m_tThreshold) iCubeIndex |= 64;
								if (m_controller.convertToDensity(v111) < m_tThreshold) iCubeIndex |= 128;
							}
						}
					}
					else //previous Z not available
					{
						if (isPrevYAvail)
						{
							if (isPrevXAvail)
							{
								v110 = iZVolSpace > m_regSizeInVoxels.getLowerZ() ? m_sampVolume.peekVoxel0px0py1nz() : VolumeType::VoxelType();
								v111 = m_sampVolume.peekVoxel0px0py0pz();

								//y
								uint8_t iPreviousCubeIndexY = pBitmask(uXRegSpace, uYRegSpace - 1, uZRegSpace);
								iPreviousCubeIndexY &= 204; //204 = 128+64+8+4
								iPreviousCubeIndexY >>= 2;

								//x
								uint8_t iPreviousCubeIndexX = pBitmask(uXRegSpace - 1, uYRegSpace, uZRegSpace);
								iPreviousCubeIndexX &= 170; //170 = 128+32+8+2
								iPreviousCubeIndexX >>= 1;

								iCubeIndex = iPreviousCubeIndexX | iPreviousCubeIndexY;

								if (m_controller.convertToDensity(v110) < m_tThreshold) iCubeIndex |= 8;
								if (m_controller.convertToDensity(v111) < m_tThreshold) iCubeIndex |= 128;
							}
							else //previous X not available
							{
								v010 = iXVolSpace > m_regSizeInVoxels.getLowerX() && iZVolSpace > m_regSizeInVoxels.getLowerZ() ? m_sampVolume.peekVoxel1nx0py1nz() : VolumeType::VoxelType();
								v110 = iZVolSpace > m_regSizeInVoxels.getLowerZ() ? m_sampVolume.peekVoxel0px0py1nz() : VolumeType::VoxelType();
								
								v011 = iXVolSpace > m_regSizeInVoxels.getLowerX() ? m_sampVolume.peekVoxel1nx0py0pz() : VolumeType::VoxelType();
								v111 = m_sampVolume.peekVoxel0px0py0pz();

								//y
								uint8_t iPreviousCubeIndexY = pBitmask(uXRegSpace, uYRegSpace - 1, uZRegSpace);
								iPreviousCubeIndexY &= 204; //204 = 128+64+8+4
								iPreviousCubeIndexY >>= 2;

								iCubeIndex = iPreviousCubeIndexY;

								if (m_controller.convertToDensity(v010) < m_tThreshold) iCubeIndex |= 4;
								if (m_controller.convertToDensity(v110) < m_tThreshold) iCubeIndex |= 8;
								if (m_controller.convertToDensity(v011) < m_tThreshold) iCubeIndex |= 64;
								if (m_controller.convertToDensity(v111) < m_tThreshold) iCubeIndex |= 128;
							}
						}
						else //previous Y not available
						{
							if (isPrevXAvail)
							{								
								v100 = iYVolSpace > m_regSizeInVoxels.getLowerY() && iZVolSpace > m_regSizeInVoxels.getLowerZ() ? m_sampVolume.peekVoxel0px1ny1nz() : VolumeType::VoxelType();
								v110 = iZVolSpace > m_regSizeInVoxels.getLowerZ() ? m_sampVolume.peekVoxel0px0py1nz() : VolumeType::VoxelType();

								v101 = iYVolSpace > m_regSizeInVoxels.getLowerY() ? m_sampVolume.peekVoxel0px1ny0pz() : VolumeType::VoxelType();
								v111 = m_sampVolume.peekVoxel0px0py0pz();

								//x
								uint8_t iPreviousCubeIndexX = pBitmask(uXRegSpace - 1, uYRegSpace, uZRegSpace);
								iPreviousCubeIndexX &= 170; //170 = 128+32+8+2
								iPreviousCubeIndexX >>= 1;

								iCubeIndex = iPreviousCubeIndexX;

								if (m_controller.convertToDensity(v100) < m_tThreshold) iCubeIndex |= 2;
								if (m_controller.convertToDensity(v110) < m_tThreshold) iCubeIndex |= 8;
								if (m_controller.convertToDensity(v101) < m_tThreshold) iCubeIndex |= 32;
								if (m_controller.convertToDensity(v111) < m_tThreshold) iCubeIndex |= 128;
							}
							else //previous X not available
							{
								v000 = iXVolSpace > m_regSizeInVoxels.getLowerX() && iYVolSpace > m_regSizeInVoxels.getLowerY() && iZVolSpace > m_regSizeInVoxels.getLowerZ() ? m_sampVolume.peekVoxel1nx1ny1nz() : VolumeType::VoxelType();
								v100 = iYVolSpace > m_regSizeInVoxels.getLowerY() && iZVolSpace > m_regSizeInVoxels.getLowerZ() ? m_sampVolume.peekVoxel0px1ny1nz() : VolumeType::VoxelType();
								v010 = iXVolSpace > m_regSizeInVoxels.getLowerX() && iZVolSpace > m_regSizeInVoxels.getLowerZ() ? m_sampVolume.peekVoxel1nx0py1nz() : VolumeType::VoxelType();
								v110 = iZVolSpace > m_regSizeInVoxels.getLowerZ() ? m_sampVolume.peekVoxel0px0py1nz() : VolumeType::VoxelType();

								v001 = iXVolSpace > m_regSizeInVoxels.getLowerX() && iYVolSpace > m_regSizeInVoxels.getLowerY() ? m_sampVolume.peekVoxel1nx1ny0pz() : VolumeType::VoxelType();
								v101 = iYVolSpace > m_regSizeInVoxels.getLowerY() ? m_sampVolume.peekVoxel0px1ny0pz() : VolumeType::VoxelType();
								v011 = iXVolSpace > m_regSizeInVoxels.getLowerX() ? m_sampVolume.peekVoxel1nx0py0pz() : VolumeType::VoxelType();
								v111 = m_sampVolume.peekVoxel0px0py0pz();

								if (m_controller.convertToDensity(v000) < m_tThreshold) iCubeIndex |= 1;
								if (m_controller.convertToDensity(v100) < m_tThreshold) iCubeIndex |= 2;
								if (m_controller.convertToDensity(v010) < m_tThreshold) iCubeIndex |= 4;
								if (m_controller.convertToDensity(v110) < m_tThreshold) iCubeIndex |= 8;
								if (m_controller.convertToDensity(v001) < m_tThreshold) iCubeIndex |= 16;
								if (m_controller.convertToDensity(v101) < m_tThreshold) iCubeIndex |= 32;
								if (m_controller.convertToDensity(v011) < m_tThreshold) iCubeIndex |= 64;
								if (m_controller.convertToDensity(v111) < m_tThreshold) iCubeIndex |= 128;
							}
						}
					}

					if (iCubeIndex != 0)
					{
						//Save the bitmask
						pBitmask(uXRegSpace, uYRegSpace, uZRegSpace) = iCubeIndex;
					}
				}
			}
		}

		for (int32_t iZVolSpace = m_regSizeInVoxels.getLowerZ(); iZVolSpace <= m_regSizeInVoxels.getUpperZ(); iZVolSpace++)
		{
			const uint32_t uZRegSpace = iZVolSpace - m_regSizeInVoxels.getLowerZ();

			for (int32_t iYVolSpace = m_regSizeInVoxels.getLowerY(); iYVolSpace <= m_regSizeInVoxels.getUpperY(); iYVolSpace++)
			{
				const uint32_t uYRegSpace = iYVolSpace - m_regSizeInVoxels.getLowerY();

				for (int32_t iXVolSpace = m_regSizeInVoxels.getLowerX(); iXVolSpace <= m_regSizeInVoxels.getUpperX(); iXVolSpace++)
				{
					const uint32_t uXRegSpace = iXVolSpace - m_regSizeInVoxels.getLowerX();

					m_sampVolume.setPosition(iXVolSpace, iYVolSpace, iZVolSpace);

					uint8_t iCubeIndex = pBitmask(uXRegSpace, uYRegSpace, uZRegSpace);

					/* Cube is entirely in/out of the surface */
					if (edgeTable[iCubeIndex] == 0)
					{
						continue;
					}

					/*typename VolumeType::VoxelType v000 = m_sampVolume.peekVoxel1nx1ny1nz();
					typename VolumeType::VoxelType v100 = m_sampVolume.peekVoxel0px1ny1nz();
					typename VolumeType::VoxelType v010 = m_sampVolume.peekVoxel1nx0py1nz();
					typename VolumeType::VoxelType v001 = m_sampVolume.peekVoxel1nx1ny0pz();*/

					/*typename VolumeType::VoxelType v000 = m_sampVolume.peekVoxel0px0py0pz();
					typename VolumeType::VoxelType v100 = m_sampVolume.peekVoxel1px0py0pz();
					typename VolumeType::VoxelType v010 = m_sampVolume.peekVoxel0px1py0pz();
					typename VolumeType::VoxelType v001 = m_sampVolume.peekVoxel0px0py1pz();*/

					typename VolumeType::VoxelType v110 = m_sampVolume.peekVoxel0px0py1nz();
					typename VolumeType::VoxelType v101 = m_sampVolume.peekVoxel0px1ny0pz();
					typename VolumeType::VoxelType v011 = m_sampVolume.peekVoxel1nx0py0pz();
					typename VolumeType::VoxelType v111 = m_sampVolume.peekVoxel0px0py0pz();
					
					const Vector3DFloat n000 = computeCentralDifferenceGradient(m_sampVolume);

					/* Find the vertices where the surface intersects the cube */
					if ((edgeTable[iCubeIndex] & 64) && (uXRegSpace > 0))
					{
						m_sampVolume.moveNegativeX();
						POLYVOX_ASSERT(v011 != v111, "Attempting to insert vertex between two voxels with the same value");
						const Vector3DFloat n100 = computeCentralDifferenceGradient(m_sampVolume);

						const float fInterp = static_cast<float>(m_tThreshold - m_controller.convertToDensity(v011)) / static_cast<float>(m_controller.convertToDensity(v111) - m_controller.convertToDensity(v011));

						const Vector3DFloat v3dPosition(static_cast<float>(uXRegSpace - 1) + fInterp, static_cast<float>(uYRegSpace), static_cast<float>(uZRegSpace));
						const Vector3DUint16 v3dScaledPosition(static_cast<uint16_t>(v3dPosition.getX() * 256.0f), static_cast<uint16_t>(v3dPosition.getY() * 256.0f), static_cast<uint16_t>(v3dPosition.getZ() * 256.0f));

						Vector3DFloat v3dNormal = (n100*fInterp) + (n000*(1 - fInterp));

						// The gradient for a voxel can be zero (e.g. solid voxel surrounded by empty ones) and so
						// the interpolated normal can also be zero (e.g. a grid of alternating solid and empty voxels).
						if (v3dNormal.lengthSquared() > 0.000001f)
						{
							v3dNormal.normalise();
						}

						// Allow the controller to decide how the material should be derived from the voxels.
						const typename VolumeType::VoxelType uMaterial = m_controller.blendMaterials(v011, v111, fInterp);

						MarchingCubesVertex<typename VolumeType::VoxelType> surfaceVertex;
						surfaceVertex.encodedPosition = v3dScaledPosition;
						surfaceVertex.encodedNormal = encodeNormal(v3dNormal);
						surfaceVertex.data = uMaterial;

						const uint32_t uLastVertexIndex = m_meshCurrent->addVertex(surfaceVertex);
						pIndicesX(uXRegSpace, uYRegSpace, uZRegSpace) = uLastVertexIndex;

						m_sampVolume.movePositiveX();
					}
					if ((edgeTable[iCubeIndex] & 32) && (uYRegSpace > 0))
					{
						m_sampVolume.moveNegativeY();
						POLYVOX_ASSERT(v101 != v111, "Attempting to insert vertex between two voxels with the same value");
						const Vector3DFloat n010 = computeCentralDifferenceGradient(m_sampVolume);

						const float fInterp = static_cast<float>(m_tThreshold - m_controller.convertToDensity(v101)) / static_cast<float>(m_controller.convertToDensity(v111) - m_controller.convertToDensity(v101));

						const Vector3DFloat v3dPosition(static_cast<float>(uXRegSpace), static_cast<float>(uYRegSpace - 1) + fInterp, static_cast<float>(uZRegSpace));
						const Vector3DUint16 v3dScaledPosition(static_cast<uint16_t>(v3dPosition.getX() * 256.0f), static_cast<uint16_t>(v3dPosition.getY() * 256.0f), static_cast<uint16_t>(v3dPosition.getZ() * 256.0f));

						Vector3DFloat v3dNormal = (n010*fInterp) + (n000*(1 - fInterp));

						// The gradient for a voxel can be zero (e.g. solid voxel surrounded by empty ones) and so
						// the interpolated normal can also be zero (e.g. a grid of alternating solid and empty voxels).
						if (v3dNormal.lengthSquared() > 0.000001f)
						{
							v3dNormal.normalise();
						}

						// Allow the controller to decide how the material should be derived from the voxels.
						const typename VolumeType::VoxelType uMaterial = m_controller.blendMaterials(v101, v111, fInterp);

						MarchingCubesVertex<typename VolumeType::VoxelType> surfaceVertex;
						surfaceVertex.encodedPosition = v3dScaledPosition;
						surfaceVertex.encodedNormal = encodeNormal(v3dNormal);
						surfaceVertex.data = uMaterial;

						uint32_t uLastVertexIndex = m_meshCurrent->addVertex(surfaceVertex);
						pIndicesY(uXRegSpace, uYRegSpace, uZRegSpace) = uLastVertexIndex;

						m_sampVolume.movePositiveY();
					}
					if ((edgeTable[iCubeIndex] & 1024) && (uZRegSpace > 0))
					{
						m_sampVolume.moveNegativeZ();
						POLYVOX_ASSERT(v110 != v111, "Attempting to insert vertex between two voxels with the same value");
						const Vector3DFloat n001 = computeCentralDifferenceGradient(m_sampVolume);

						const float fInterp = static_cast<float>(m_tThreshold - m_controller.convertToDensity(v110)) / static_cast<float>(m_controller.convertToDensity(v111) - m_controller.convertToDensity(v110));

						const Vector3DFloat v3dPosition(static_cast<float>(uXRegSpace), static_cast<float>(uYRegSpace), static_cast<float>(uZRegSpace - 1) + fInterp);
						const Vector3DUint16 v3dScaledPosition(static_cast<uint16_t>(v3dPosition.getX() * 256.0f), static_cast<uint16_t>(v3dPosition.getY() * 256.0f), static_cast<uint16_t>(v3dPosition.getZ() * 256.0f));

						Vector3DFloat v3dNormal = (n001*fInterp) + (n000*(1 - fInterp));
						// The gradient for a voxel can be zero (e.g. solid voxel surrounded by empty ones) and so
						// the interpolated normal can also be zero (e.g. a grid of alternating solid and empty voxels).
						if (v3dNormal.lengthSquared() > 0.000001f)
						{
							v3dNormal.normalise();
						}

						// Allow the controller to decide how the material should be derived from the voxels.
						const typename VolumeType::VoxelType uMaterial = m_controller.blendMaterials(v110, v111, fInterp);

						MarchingCubesVertex<typename VolumeType::VoxelType> surfaceVertex;
						surfaceVertex.encodedPosition = v3dScaledPosition;
						surfaceVertex.encodedNormal = encodeNormal(v3dNormal);
						surfaceVertex.data = uMaterial;

						const uint32_t uLastVertexIndex = m_meshCurrent->addVertex(surfaceVertex);
						pIndicesZ(uXRegSpace, uYRegSpace, uZRegSpace) = uLastVertexIndex;

						m_sampVolume.movePositiveZ();
					}
				}
			}
		}

		for (int32_t iZVolSpace = m_regSizeInVoxels.getLowerZ() + 1; iZVolSpace <= m_regSizeInVoxels.getUpperZ(); iZVolSpace++)
		{
			for (int32_t iYVolSpace = m_regSizeInVoxels.getLowerY() + 1; iYVolSpace <= m_regSizeInVoxels.getUpperY(); iYVolSpace++)
			{
				for (int32_t iXVolSpace = m_regSizeInVoxels.getLowerX() + 1; iXVolSpace <= m_regSizeInVoxels.getUpperX(); iXVolSpace++)
				{
					int32_t indlist[12];

					m_sampVolume.setPosition(iXVolSpace, iYVolSpace, iZVolSpace);

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
						indlist[0] = pIndicesX(uXRegSpace, uYRegSpace - 1, uZRegSpace - 1);
					}
					if (edgeTable[iCubeIndex] & 2)
					{
						indlist[1] = pIndicesY(uXRegSpace, uYRegSpace, uZRegSpace - 1);
					}
					if (edgeTable[iCubeIndex] & 4)
					{
						indlist[2] = pIndicesX(uXRegSpace, uYRegSpace, uZRegSpace - 1);
					}
					if (edgeTable[iCubeIndex] & 8)
					{
						indlist[3] = pIndicesY(uXRegSpace - 1, uYRegSpace, uZRegSpace - 1);
					}
					if (edgeTable[iCubeIndex] & 16)
					{
						indlist[4] = pIndicesX(uXRegSpace, uYRegSpace - 1, uZRegSpace);
					}
					if (edgeTable[iCubeIndex] & 32)
					{
						indlist[5] = pIndicesY(uXRegSpace, uYRegSpace, uZRegSpace);
					}
					if (edgeTable[iCubeIndex] & 64)
					{
						indlist[6] = pIndicesX(uXRegSpace, uYRegSpace, uZRegSpace);
					}
					if (edgeTable[iCubeIndex] & 128)
					{
						indlist[7] = pIndicesY(uXRegSpace - 1, uYRegSpace, uZRegSpace);
					}
					if (edgeTable[iCubeIndex] & 256)
					{
						indlist[8] = pIndicesZ(uXRegSpace - 1, uYRegSpace - 1, uZRegSpace);
					}
					if (edgeTable[iCubeIndex] & 512)
					{
						indlist[9] = pIndicesZ(uXRegSpace, uYRegSpace - 1, uZRegSpace);
					}
					if (edgeTable[iCubeIndex] & 1024)
					{
						indlist[10] = pIndicesZ(uXRegSpace, uYRegSpace, uZRegSpace);
					}
					if (edgeTable[iCubeIndex] & 2048)
					{
						indlist[11] = pIndicesZ(uXRegSpace - 1, uYRegSpace, uZRegSpace);
					}

					for (int i = 0; triTable[iCubeIndex][i] != -1; i += 3)
					{
						const int32_t ind0 = indlist[triTable[iCubeIndex][i]];
						const int32_t ind1 = indlist[triTable[iCubeIndex][i + 1]];
						const int32_t ind2 = indlist[triTable[iCubeIndex][i + 2]];

						if ((ind0 != -1) && (ind1 != -1) && (ind2 != -1))
						{
							m_meshCurrent->addTriangle(ind0, ind1, ind2);
						}
					}//For each triangle
				}//For each cell
			}
		}
	}
}

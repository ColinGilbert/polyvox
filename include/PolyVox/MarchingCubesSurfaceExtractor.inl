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
	MarchingCubesSurfaceExtractor<VolumeType, MeshType, ControllerType>::MarchingCubesSurfaceExtractor()
	{		
	}

	template<typename VolumeType, typename MeshType, typename ControllerType>
	void MarchingCubesSurfaceExtractor<VolumeType, MeshType, ControllerType>::execute(VolumeType* volData, Region region, MeshType* result, ControllerType controller)
	{		
		POLYVOX_THROW_IF(result == nullptr, std::invalid_argument, "Provided mesh cannot be null");

		m_volData = volData;
		m_meshCurrent = result;
		m_regSizeInVoxels = region;
		m_controller = controller;
		m_tThreshold = m_controller.getThreshold();

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

		Array2DUint8 pPreviousSliceBitmask(uArrayWidth, uArrayHeight);
		Array1DUint8 pPreviousRowBitmask(uArrayWidth);
		memset(pPreviousSliceBitmask.getRawData(), 0x00, pPreviousSliceBitmask.getNoOfElements());
		memset(pPreviousRowBitmask.getRawData(), 0x00, pPreviousRowBitmask.getNoOfElements());

		uint8_t uPreviousCell = 0;

		typename VolumeType::Sampler startOfSlice(m_volData);
		startOfSlice.setPosition(m_regSizeInVoxels.getLowerX(), m_regSizeInVoxels.getLowerY(), m_regSizeInVoxels.getLowerZ());

		for (int32_t iZVolSpace = m_regSizeInVoxels.getLowerZ(); iZVolSpace <= m_regSizeInVoxels.getUpperZ(); iZVolSpace++)
		{
			const uint32_t uZRegSpace = iZVolSpace - m_regSizeInVoxels.getLowerZ();

			typename VolumeType::Sampler startOfRow = startOfSlice;

			for (int32_t iYVolSpace = m_regSizeInVoxels.getLowerY(); iYVolSpace <= m_regSizeInVoxels.getUpperY(); iYVolSpace++)
			{
				const uint32_t uYRegSpace = iYVolSpace - m_regSizeInVoxels.getLowerY();

				// Copying a sampler which is already pointing at the correct location seems (slightly) faster than
				// calling setPosition(). Therefore we make use of 'startOfRow' and 'startOfSlice' to reset the sampler.
				typename VolumeType::Sampler sampler = startOfRow;

				for (int32_t iXVolSpace = m_regSizeInVoxels.getLowerX(); iXVolSpace <= m_regSizeInVoxels.getUpperX(); iXVolSpace++)
				{
					const uint32_t uXRegSpace = iXVolSpace - m_regSizeInVoxels.getLowerX();

					uint8_t iCubeIndex = 0;

					// Four bits of our cube index are obtained by looking at the cube index for
					// the previous slice and copying four of those bits into their new positions.
					uint8_t iPreviousCubeIndexZ = pPreviousSliceBitmask(uXRegSpace, uYRegSpace);
					iPreviousCubeIndexZ >>= 4;
					iCubeIndex |= iPreviousCubeIndexZ;

					// Two bits of our cube index are obtained by looking at the cube index for
					// the previous row and copying two of those bits into their new positions.
					uint8_t iPreviousCubeIndexY = pPreviousRowBitmask(uXRegSpace);
					iPreviousCubeIndexY &= 204; //204 = 128+64+8+4
					iPreviousCubeIndexY >>= 2;
					iCubeIndex |= iPreviousCubeIndexY;

					// One bit of our cube index are obtained by looking at the cube index for
					// the previous cell and copying one of those bits into it's new position.
					uint8_t iPreviousCubeIndexX = uPreviousCell;
					iPreviousCubeIndexX &= 170; //170 = 128+32+8+2
					iPreviousCubeIndexX >>= 1;
					iCubeIndex |= iPreviousCubeIndexX;

					// The last bit of our cube index is obtained by looking
					// at the relevant voxel and comparing it to the threshold
					typename VolumeType::VoxelType v111 = sampler.getVoxel();
					if (m_controller.convertToDensity(v111) < m_tThreshold) iCubeIndex |= 128;

					// The current value becomes the previous value, ready for the next iteration.
					uPreviousCell = iCubeIndex;
					pPreviousRowBitmask(uXRegSpace) = iCubeIndex;
					pPreviousSliceBitmask(uXRegSpace, uYRegSpace) = iCubeIndex;

					/* Cube is entirely in/out of the surface */
					if (edgeTable[iCubeIndex] != 0)
					{

						// These three might not have been sampled, as v111 is the only one we sample every iteration.
						typename VolumeType::VoxelType v110 = sampler.peekVoxel0px0py1nz();
						typename VolumeType::VoxelType v101 = sampler.peekVoxel0px1ny0pz();
						typename VolumeType::VoxelType v011 = sampler.peekVoxel1nx0py0pz();

						const Vector3DFloat n000 = computeCentralDifferenceGradient(sampler);

						/* Find the vertices where the surface intersects the cube */
						if ((edgeTable[iCubeIndex] & 64) && (uXRegSpace > 0))
						{
							sampler.moveNegativeX();
							POLYVOX_ASSERT(v011 != v111, "Attempting to insert vertex between two voxels with the same value");
							const Vector3DFloat n100 = computeCentralDifferenceGradient(sampler);

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

							sampler.movePositiveX();
						}
						if ((edgeTable[iCubeIndex] & 32) && (uYRegSpace > 0))
						{
							sampler.moveNegativeY();
							POLYVOX_ASSERT(v101 != v111, "Attempting to insert vertex between two voxels with the same value");
							const Vector3DFloat n010 = computeCentralDifferenceGradient(sampler);

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

							sampler.movePositiveY();
						}
						if ((edgeTable[iCubeIndex] & 1024) && (uZRegSpace > 0))
						{
							sampler.moveNegativeZ();
							POLYVOX_ASSERT(v110 != v111, "Attempting to insert vertex between two voxels with the same value");
							const Vector3DFloat n001 = computeCentralDifferenceGradient(sampler);

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

							sampler.movePositiveZ();
						}

						// Now output the indices. For the first row, column or slice there aren't
						// any (the region size in cells is one less than the region size in voxels)
						if ((uXRegSpace != 0) && (uYRegSpace != 0) && (uZRegSpace != 0))
						{

							int32_t indlist[12];

							sampler.setPosition(iXVolSpace, iYVolSpace, iZVolSpace);

							/* Cube is entirely in/out of the surface */
							if (edgeTable[iCubeIndex] != 0)
							{

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
								} // For each triangle
							}
						}
					} // For each cell
					sampler.movePositiveX();
				} // For X
				startOfRow.movePositiveY();
			} // For Y
			startOfSlice.movePositiveZ();
		} // For Z

		m_meshCurrent->setOffset(m_regSizeInVoxels.getLowerCorner());

		POLYVOX_LOG_TRACE("Marching cubes surface extraction took ", timer.elapsedTimeInMilliSeconds(),
			"ms (Region size = ", m_regSizeInVoxels.getWidthInVoxels(), "x", m_regSizeInVoxels.getHeightInVoxels(),
			"x", m_regSizeInVoxels.getDepthInVoxels(), ")");
	}		
}

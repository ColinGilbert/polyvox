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
	template< typename Sampler, typename ControllerType>
	Vector3DFloat computeCentralDifferenceGradient(const Sampler& volIter, ControllerType& controller)
	{
		//FIXME - Should actually use DensityType here, both in principle and because the maths may be
		//faster (and to reduce casts). So it would be good to add a way to get DensityType from a voxel.
		//But watch out for when the DensityType is unsigned and the difference could be negative.
		float voxel1nx = static_cast<float>(controller.convertToDensity(volIter.peekVoxel1nx0py0pz()));
		float voxel1px = static_cast<float>(controller.convertToDensity(volIter.peekVoxel1px0py0pz()));

		float voxel1ny = static_cast<float>(controller.convertToDensity(volIter.peekVoxel0px1ny0pz()));
		float voxel1py = static_cast<float>(controller.convertToDensity(volIter.peekVoxel0px1py0pz()));

		float voxel1nz = static_cast<float>(controller.convertToDensity(volIter.peekVoxel0px0py1nz()));
		float voxel1pz = static_cast<float>(controller.convertToDensity(volIter.peekVoxel0px0py1pz()));

		return Vector3DFloat
			(
			voxel1nx - voxel1px,
			voxel1ny - voxel1py,
			voxel1nz - voxel1pz
			);
	}

	template< typename VolumeType, typename MeshType, typename ControllerType >
	void generateMeshForCell(Region& region, MeshType* result, ControllerType& controller, typename VolumeType::Sampler& sampler, Array<2, Vector3DInt32>& pIndices, Array<2, Vector3DInt32>& pPreviousIndices, uint8_t iCubeIndex, uint32_t uXRegSpace, uint32_t uYRegSpace, uint32_t uZRegSpace, typename ControllerType::DensityType tThreshold, typename VolumeType::VoxelType& v111)
	{
		auto v111Density = controller.convertToDensity(v111);
		const Vector3DFloat n000 = computeCentralDifferenceGradient(sampler, controller);

		uint16_t uEdge = edgeTable[iCubeIndex];
		/* Find the vertices where the surface intersects the cube */
		if ((uEdge & 64) && (uXRegSpace > 0))
		{
			sampler.moveNegativeX();
			typename VolumeType::VoxelType v011 = sampler.getVoxel();
			auto v011Density = controller.convertToDensity(v011);
			const float fInterp = static_cast<float>(tThreshold - v011Density) / static_cast<float>(v111Density - v011Density);

			// Compute the position
			const Vector3DFloat v3dPosition(static_cast<float>(uXRegSpace - 1) + fInterp, static_cast<float>(uYRegSpace), static_cast<float>(uZRegSpace));

			// Compute the normal
			const Vector3DFloat n100 = computeCentralDifferenceGradient(sampler, controller);
			Vector3DFloat v3dNormal = (n100*fInterp) + (n000*(1 - fInterp));

			// The gradient for a voxel can be zero (e.g. solid voxel surrounded by empty ones) and so
			// the interpolated normal can also be zero (e.g. a grid of alternating solid and empty voxels).
			if (v3dNormal.lengthSquared() > 0.000001f)
			{
				v3dNormal.normalise();
			}

			// Allow the controller to decide how the material should be derived from the voxels.
			const typename VolumeType::VoxelType uMaterial = controller.blendMaterials(v011, v111, fInterp);

			MarchingCubesVertex<typename VolumeType::VoxelType> surfaceVertex;
			const Vector3DUint16 v3dScaledPosition(static_cast<uint16_t>(v3dPosition.getX() * 256.0f), static_cast<uint16_t>(v3dPosition.getY() * 256.0f), static_cast<uint16_t>(v3dPosition.getZ() * 256.0f));
			surfaceVertex.encodedPosition = v3dScaledPosition;
			surfaceVertex.encodedNormal = encodeNormal(v3dNormal);
			surfaceVertex.data = uMaterial;

			const uint32_t uLastVertexIndex = result->addVertex(surfaceVertex);
			pIndices(uXRegSpace, uYRegSpace).setX(uLastVertexIndex);

			sampler.movePositiveX();
		}
		if ((uEdge & 32) && (uYRegSpace > 0))
		{
			sampler.moveNegativeY();
			typename VolumeType::VoxelType v101 = sampler.getVoxel();
			auto v101Density = controller.convertToDensity(v101);
			const float fInterp = static_cast<float>(tThreshold - v101Density) / static_cast<float>(v111Density - v101Density);

			// Compute the position
			const Vector3DFloat v3dPosition(static_cast<float>(uXRegSpace), static_cast<float>(uYRegSpace - 1) + fInterp, static_cast<float>(uZRegSpace));

			// Compute the normal
			const Vector3DFloat n010 = computeCentralDifferenceGradient(sampler, controller);
			Vector3DFloat v3dNormal = (n010*fInterp) + (n000*(1 - fInterp));

			// The gradient for a voxel can be zero (e.g. solid voxel surrounded by empty ones) and so
			// the interpolated normal can also be zero (e.g. a grid of alternating solid and empty voxels).
			if (v3dNormal.lengthSquared() > 0.000001f)
			{
				v3dNormal.normalise();
			}

			// Allow the controller to decide how the material should be derived from the voxels.
			const typename VolumeType::VoxelType uMaterial = controller.blendMaterials(v101, v111, fInterp);

			MarchingCubesVertex<typename VolumeType::VoxelType> surfaceVertex;
			const Vector3DUint16 v3dScaledPosition(static_cast<uint16_t>(v3dPosition.getX() * 256.0f), static_cast<uint16_t>(v3dPosition.getY() * 256.0f), static_cast<uint16_t>(v3dPosition.getZ() * 256.0f));
			surfaceVertex.encodedPosition = v3dScaledPosition;
			surfaceVertex.encodedNormal = encodeNormal(v3dNormal);
			surfaceVertex.data = uMaterial;

			uint32_t uLastVertexIndex = result->addVertex(surfaceVertex);
			pIndices(uXRegSpace, uYRegSpace).setY(uLastVertexIndex);

			sampler.movePositiveY();
		}
		if ((uEdge & 1024) && (uZRegSpace > 0))
		{
			sampler.moveNegativeZ();
			typename VolumeType::VoxelType v110 = sampler.getVoxel();
			auto v110Density = controller.convertToDensity(v110);
			const float fInterp = static_cast<float>(tThreshold - v110Density) / static_cast<float>(v111Density - v110Density);

			// Compute the position
			const Vector3DFloat v3dPosition(static_cast<float>(uXRegSpace), static_cast<float>(uYRegSpace), static_cast<float>(uZRegSpace - 1) + fInterp);

			// Compute the normal
			const Vector3DFloat n001 = computeCentralDifferenceGradient(sampler, controller);
			Vector3DFloat v3dNormal = (n001*fInterp) + (n000*(1 - fInterp));

			// The gradient for a voxel can be zero (e.g. solid voxel surrounded by empty ones) and so
			// the interpolated normal can also be zero (e.g. a grid of alternating solid and empty voxels).
			if (v3dNormal.lengthSquared() > 0.000001f)
			{
				v3dNormal.normalise();
			}

			// Allow the controller to decide how the material should be derived from the voxels.
			const typename VolumeType::VoxelType uMaterial = controller.blendMaterials(v110, v111, fInterp);

			MarchingCubesVertex<typename VolumeType::VoxelType> surfaceVertex;
			const Vector3DUint16 v3dScaledPosition(static_cast<uint16_t>(v3dPosition.getX() * 256.0f), static_cast<uint16_t>(v3dPosition.getY() * 256.0f), static_cast<uint16_t>(v3dPosition.getZ() * 256.0f));
			surfaceVertex.encodedPosition = v3dScaledPosition;
			surfaceVertex.encodedNormal = encodeNormal(v3dNormal);
			surfaceVertex.data = uMaterial;

			const uint32_t uLastVertexIndex = result->addVertex(surfaceVertex);
			pIndices(uXRegSpace, uYRegSpace).setZ(uLastVertexIndex);

			sampler.movePositiveZ();
		}

		// Now output the indices. For the first row, column or slice there aren't
		// any (the region size in cells is one less than the region size in voxels)
		if ((uXRegSpace != 0) && (uYRegSpace != 0) && (uZRegSpace != 0))
		{

			int32_t indlist[12];

			/* Find the vertices where the surface intersects the cube */
			if (uEdge & 1)
			{
				indlist[0] = pPreviousIndices(uXRegSpace, uYRegSpace - 1).getX();
			}
			if (uEdge & 2)
			{
				indlist[1] = pPreviousIndices(uXRegSpace, uYRegSpace).getY();
			}
			if (uEdge & 4)
			{
				indlist[2] = pPreviousIndices(uXRegSpace, uYRegSpace).getX();
			}
			if (uEdge & 8)
			{
				indlist[3] = pPreviousIndices(uXRegSpace - 1, uYRegSpace).getY();
			}
			if (uEdge & 16)
			{
				indlist[4] = pIndices(uXRegSpace, uYRegSpace - 1).getX();
			}
			if (uEdge & 32)
			{
				indlist[5] = pIndices(uXRegSpace, uYRegSpace).getY();
			}
			if (uEdge & 64)
			{
				indlist[6] = pIndices(uXRegSpace, uYRegSpace).getX();
			}
			if (uEdge & 128)
			{
				indlist[7] = pIndices(uXRegSpace - 1, uYRegSpace).getY();
			}
			if (uEdge & 256)
			{
				indlist[8] = pIndices(uXRegSpace - 1, uYRegSpace - 1).getZ();
			}
			if (uEdge & 512)
			{
				indlist[9] = pIndices(uXRegSpace, uYRegSpace - 1).getZ();
			}
			if (uEdge & 1024)
			{
				indlist[10] = pIndices(uXRegSpace, uYRegSpace).getZ();
			}
			if (uEdge & 2048)
			{
				indlist[11] = pIndices(uXRegSpace - 1, uYRegSpace).getZ();
			}

			for (int i = 0; triTable[iCubeIndex][i] != -1; i += 3)
			{
				const int32_t ind0 = indlist[triTable[iCubeIndex][i]];
				const int32_t ind1 = indlist[triTable[iCubeIndex][i + 1]];
				const int32_t ind2 = indlist[triTable[iCubeIndex][i + 2]];

				if ((ind0 != -1) && (ind1 != -1) && (ind2 != -1))
				{
					result->addTriangle(ind0, ind1, ind2);
				}
			} // For each triangle
		}
	}

	template< typename VolumeType, typename MeshType, typename ControllerType >
	void extractMarchingCubesMeshCustom(VolumeType* volData, Region region, MeshType* result, ControllerType controller)
	{		
		POLYVOX_THROW_IF(result == nullptr, std::invalid_argument, "Provided mesh cannot be null");

		Timer timer;
		result->clear();

		typename ControllerType::DensityType tThreshold = controller.getThreshold();

		const uint32_t uRegionWidthInVoxels = region.getWidthInVoxels();
		const uint32_t uRegionHeightInVoxels = region.getHeightInVoxels();
		const uint32_t uRegionDepthInVoxels = region.getDepthInVoxels();

		// No need to clear memory because we only read from elements we have written to.
		Array<2, Vector3DInt32> pIndices(uRegionWidthInVoxels, uRegionHeightInVoxels);
		Array<2, Vector3DInt32> pPreviousIndices(uRegionWidthInVoxels, uRegionHeightInVoxels);

		Array2DUint8 pPreviousSliceBitmask(uRegionWidthInVoxels, uRegionHeightInVoxels);
		Array1DUint8 pPreviousRowBitmask(uRegionWidthInVoxels);

		uint8_t uPreviousCell = 0;

		typename VolumeType::Sampler startOfSlice(volData);
		startOfSlice.setPosition(region.getLowerX(), region.getLowerY(), region.getLowerZ());

		for (uint32_t uZRegSpace = 0; uZRegSpace < uRegionDepthInVoxels; uZRegSpace++)
		{
			typename VolumeType::Sampler startOfRow = startOfSlice;

			for (uint32_t uYRegSpace = 0; uYRegSpace < uRegionHeightInVoxels; uYRegSpace++)
			{
				// Copying a sampler which is already pointing at the correct location seems (slightly) faster than
				// calling setPosition(). Therefore we make use of 'startOfRow' and 'startOfSlice' to reset the sampler.
				typename VolumeType::Sampler sampler = startOfRow;

				for (uint32_t uXRegSpace = 0; uXRegSpace < uRegionWidthInVoxels; uXRegSpace++)
				{
					// Note: In many cases the provided region will be (mostly) empty which means mesh vertices/indices 
					// are not generated and the only thing that is done for each cell is the computation of iCubeIndex.
					// It appears that retriving the voxel value is not so expensive and that it is the bitwise combining
					// which actually carries the cost.
					//
					// If we really need to speed this up more then it may be possible to pack 4 8-bit cell indices into
					// a single 32-bit value and then perform the bitwise logic on all four of them at the same time. 
					// However, this complicates the code and there would still be the cost of packing/unpacking so it's
					// not clear if there is really a benefit. It's something to consider in the future.

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
					if (controller.convertToDensity(v111) < tThreshold) iCubeIndex |= 128;

					// The current value becomes the previous value, ready for the next iteration.
					uPreviousCell = iCubeIndex;
					pPreviousRowBitmask(uXRegSpace) = iCubeIndex;
					pPreviousSliceBitmask(uXRegSpace, uYRegSpace) = iCubeIndex;

					/* Cube is entirely in/out of the surface */					
					if (edgeTable[iCubeIndex] != 0)
					{
						// This is a rather ugly function call and appears to have some cost compared to inlining the code.
						// As a result the case when a cell contains vertices/indices is slightly slower, but the (more common)
						// case where a cell is empty is slightly faster, probably because the main loop is a lot more compact.
						// Having a seperate function will also make it easier to profile in the future and see whether empty or
						// occupied cells are really the bottleneck. The large number of parameters is messy though, so it
						// would be nice to reduce these if we can work out how.
						generateMeshForCell<VolumeType, MeshType, ControllerType>(region, result, controller, 
							sampler, pIndices, pPreviousIndices, iCubeIndex, uXRegSpace, uYRegSpace, uZRegSpace, tThreshold, v111);
					} // For each cell
					sampler.movePositiveX();
				} // For X
				startOfRow.movePositiveY();
			} // For Y
			startOfSlice.movePositiveZ();

			pIndices.swap(pPreviousIndices);
		} // For Z

		result->setOffset(region.getLowerCorner());

		POLYVOX_LOG_TRACE("Marching cubes surface extraction took ", timer.elapsedTimeInMilliSeconds(),
			"ms (Region size = ", region.getWidthInVoxels(), "x", region.getHeightInVoxels(),
			"x", region.getDepthInVoxels(), ")");
	}		
}

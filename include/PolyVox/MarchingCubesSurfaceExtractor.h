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

#ifndef __PolyVox_SurfaceExtractor_H__
#define __PolyVox_SurfaceExtractor_H__

#include "Impl/MarchingCubesTables.h"
#include "Impl/PlatformDefinitions.h"

#include "PolyVox/Array.h"
#include "PolyVox/BaseVolume.h" //For wrap modes... should move these?
#include "PolyVox/Mesh.h"
#include "PolyVox/DefaultMarchingCubesController.h"
#include "PolyVox/Vertex.h"

namespace PolyVox
{
	template<typename _DataType>
	struct  MarchingCubesVertex
	{
		typedef _DataType DataType;

		// Each component of the position is stored using 8.8 fixed-point encoding.
		Vector3DUint16 encodedPosition;

		// The normal is encoded as a 16-bit unsigned integer using the 'oct16'
		// encoding described here: http://jcgt.org/published/0003/02/01/
		uint16_t encodedNormal;

		// User data
		DataType data;
	};

	// Convienient shorthand for declaring a mesh of marching cubes vertices
	// Currently disabled because it requires GCC 4.7
	//template <typename VertexDataType, typename IndexType = DefaultIndexType>
	//using MarchingCubesMesh = Mesh< MarchingCubesVertex<VertexDataType>, IndexType >;

	/// Decodes a position from a MarchingCubesVertex
	inline Vector3DFloat decodePosition(const Vector3DUint16& encodedPosition)
	{
		Vector3DFloat result(encodedPosition.getX(), encodedPosition.getY(), encodedPosition.getZ());
		result *= (1.0f / 256.0f); // Division is compile-time constant
		return result;
	}

	inline uint16_t encodeNormal(const Vector3DFloat& normal)
	{
		// The first part of this function is based off the code in Listing 1 of http://jcgt.org/published/0003/02/01/
		// It was rewritten in C++ and is restructued for the CPU rather than the GPU.

		// Get the input components
		float vx = normal.getX();
		float vy = normal.getY();
		float vz = normal.getZ();

		// Project the sphere onto the octahedron, and then onto the xy plane					
		float px = vx * (1.0f / (std::abs(vx) + std::abs(vy) + std::abs(vz)));
		float py = vy * (1.0f / (std::abs(vx) + std::abs(vy) + std::abs(vz)));

		// Reflect the folds of the lower hemisphere over the diagonals.
		if (vz <= 0.0f)
		{
			float refx = ((1.0f - std::abs(py)) * (px >= 0.0f ? +1.0f : -1.0f));
			float refy = ((1.0f - std::abs(px)) * (py >= 0.0f ? +1.0f : -1.0f));
			px = refx;
			py = refy;
		}

		// The next part was not given in the paper. We map our two
		// floats into two bytes and store them in a single uint16_t

		// Move from range [-1.0f, 1.0f] to [0.0f, 255.0f]
		px = (px + 1.0f) * 127.5f;
		py = (py + 1.0f) * 127.5f;

		// Convert to uints
		uint16_t resultX = static_cast<uint16_t>(px + 0.5f);
		uint16_t resultY = static_cast<uint16_t>(py + 0.5f);

		// Make sure only the lower bits are set. Probably
		// not necessary but we're just being careful really.
		resultX &= 0xFF;
		resultY &= 0xFF;

		// Contatenate the bytes and return the result.
		return (resultX << 8) | resultY;
	}

	inline Vector3DFloat decodeNormal(const uint16_t& encodedNormal)
	{
		// Extract the two bytes from the uint16_t.
		uint16_t ux = (encodedNormal >> 8) & 0xFF;
		uint16_t uy = (encodedNormal     ) & 0xFF;

		// Convert to floats in the range [-1.0f, +1.0f].
		float ex = ux / 127.5f - 1.0f;
		float ey = uy / 127.5f - 1.0f;

		// Reconstruct the origninal vector. This is a C++ implementation
		// of Listing 2 of http://jcgt.org/published/0003/02/01/
		float vx = ex;
		float vy = ey;
		float vz = 1.0f - std::abs(ex) - std::abs(ey);

		if (vz < 0.0f)
		{
			float refX = ((1.0f - std::abs(vy)) * (vx >= 0.0f ? +1.0f : -1.0f));
			float refY = ((1.0f - std::abs(vx)) * (vy >= 0.0f ? +1.0f : -1.0f));
			vx = refX;
			vy = refY;
		}

		// Normalise and return the result.
		Vector3DFloat v(vx, vy, vz);
		v.normalise();
		return v;
	}

	/// Decodes a MarchingCubesVertex by converting it into a regular Vertex which can then be directly used for rendering.
	template<typename DataType>
	Vertex<DataType> decodeVertex(const MarchingCubesVertex<DataType>& marchingCubesVertex)
	{
		Vertex<DataType> result;
		result.position = decodePosition(marchingCubesVertex.encodedPosition);
		result.normal = decodeNormal(marchingCubesVertex.encodedNormal);
		result.data = marchingCubesVertex.data; // Data is not encoded
		return result;
	}

	/// Do not use this class directly. Use the 'extractMarchingCubesSurface' function instead (see examples).
	template< typename VolumeType, typename MeshType, typename ControllerType>
	class MarchingCubesSurfaceExtractor
	{
	public:
		MarchingCubesSurfaceExtractor(VolumeType* volData, Region region, MeshType* result, ControllerType controller);

		void execute();

	private:
		//Compute the cell bitmask for a particular slice in z.
		void generateMeshForCell(uint32_t uXRegSpace, uint32_t uYRegSpace, uint32_t uZRegSpace, typename VolumeType::Sampler& sampler, typename VolumeType::VoxelType v111, uint8_t iCubeIndex, Array3DInt32& pIndicesX, Array3DInt32& pIndicesY, Array3DInt32& pIndicesZ, int32_t iXVolSpace, int32_t iYVolSpace, int32_t iZVolSpace);

		////////////////////////////////////////////////////////////////////////////////
		// NOTE: These two functions are in the .h file rather than the .inl due to an apparent bug in VC2010.
		//See http://stackoverflow.com/questions/1484885/strange-vc-compile-error-c2244 for details.
		////////////////////////////////////////////////////////////////////////////////
		Vector3DFloat computeCentralDifferenceGradient(const typename VolumeType::Sampler& volIter)
		{
			//FIXME - Should actually use DensityType here, both in principle and because the maths may be
			//faster (and to reduce casts). So it would be good to add a way to get DensityType from a voxel.
			//But watch out for when the DensityType is unsigned and the difference could be negative.
			float voxel1nx = static_cast<float>(m_controller.convertToDensity(volIter.peekVoxel1nx0py0pz()));
			float voxel1px = static_cast<float>(m_controller.convertToDensity(volIter.peekVoxel1px0py0pz()));

			float voxel1ny = static_cast<float>(m_controller.convertToDensity(volIter.peekVoxel0px1ny0pz()));
			float voxel1py = static_cast<float>(m_controller.convertToDensity(volIter.peekVoxel0px1py0pz()));

			float voxel1nz = static_cast<float>(m_controller.convertToDensity(volIter.peekVoxel0px0py1nz()));
			float voxel1pz = static_cast<float>(m_controller.convertToDensity(volIter.peekVoxel0px0py1pz()));

			return Vector3DFloat
			(
				voxel1nx - voxel1px,
				voxel1ny - voxel1py,
				voxel1nz - voxel1pz
			);
		}

		Vector3DFloat computeSobelGradient(const typename VolumeType::Sampler& volIter)
		{
			static const int weights[3][3][3] = {  {  {2,3,2}, {3,6,3}, {2,3,2}  },  {
				{3,6,3},  {6,0,6},  {3,6,3} },  { {2,3,2},  {3,6,3},  {2,3,2} } };

				//FIXME - Should actually use DensityType here, both in principle and because the maths may be
				//faster (and to reduce casts). So it would be good to add a way to get DensityType from a voxel.
				//But watch out for when the DensityType is unsigned and the difference could be negative.
				const float pVoxel1nx1ny1nz = static_cast<float>(m_controller.convertToDensity(volIter.peekVoxel1nx1ny1nz()));
				const float pVoxel1nx1ny0pz = static_cast<float>(m_controller.convertToDensity(volIter.peekVoxel1nx1ny0pz()));
				const float pVoxel1nx1ny1pz = static_cast<float>(m_controller.convertToDensity(volIter.peekVoxel1nx1ny1pz()));
				const float pVoxel1nx0py1nz = static_cast<float>(m_controller.convertToDensity(volIter.peekVoxel1nx0py1nz()));
				const float pVoxel1nx0py0pz = static_cast<float>(m_controller.convertToDensity(volIter.peekVoxel1nx0py0pz()));
				const float pVoxel1nx0py1pz = static_cast<float>(m_controller.convertToDensity(volIter.peekVoxel1nx0py1pz()));
				const float pVoxel1nx1py1nz = static_cast<float>(m_controller.convertToDensity(volIter.peekVoxel1nx1py1nz()));
				const float pVoxel1nx1py0pz = static_cast<float>(m_controller.convertToDensity(volIter.peekVoxel1nx1py0pz()));
				const float pVoxel1nx1py1pz = static_cast<float>(m_controller.convertToDensity(volIter.peekVoxel1nx1py1pz()));

				const float pVoxel0px1ny1nz = static_cast<float>(m_controller.convertToDensity(volIter.peekVoxel0px1ny1nz()));
				const float pVoxel0px1ny0pz = static_cast<float>(m_controller.convertToDensity(volIter.peekVoxel0px1ny0pz()));
				const float pVoxel0px1ny1pz = static_cast<float>(m_controller.convertToDensity(volIter.peekVoxel0px1ny1pz()));
				const float pVoxel0px0py1nz = static_cast<float>(m_controller.convertToDensity(volIter.peekVoxel0px0py1nz()));
				//const float pVoxel0px0py0pz = static_cast<float>(m_controller.convertToDensity(volIter.peekVoxel0px0py0pz()));
				const float pVoxel0px0py1pz = static_cast<float>(m_controller.convertToDensity(volIter.peekVoxel0px0py1pz()));
				const float pVoxel0px1py1nz = static_cast<float>(m_controller.convertToDensity(volIter.peekVoxel0px1py1nz()));
				const float pVoxel0px1py0pz = static_cast<float>(m_controller.convertToDensity(volIter.peekVoxel0px1py0pz()));
				const float pVoxel0px1py1pz = static_cast<float>(m_controller.convertToDensity(volIter.peekVoxel0px1py1pz()));

				const float pVoxel1px1ny1nz = static_cast<float>(m_controller.convertToDensity(volIter.peekVoxel1px1ny1nz()));
				const float pVoxel1px1ny0pz = static_cast<float>(m_controller.convertToDensity(volIter.peekVoxel1px1ny0pz()));
				const float pVoxel1px1ny1pz = static_cast<float>(m_controller.convertToDensity(volIter.peekVoxel1px1ny1pz()));
				const float pVoxel1px0py1nz = static_cast<float>(m_controller.convertToDensity(volIter.peekVoxel1px0py1nz()));
				const float pVoxel1px0py0pz = static_cast<float>(m_controller.convertToDensity(volIter.peekVoxel1px0py0pz()));
				const float pVoxel1px0py1pz = static_cast<float>(m_controller.convertToDensity(volIter.peekVoxel1px0py1pz()));
				const float pVoxel1px1py1nz = static_cast<float>(m_controller.convertToDensity(volIter.peekVoxel1px1py1nz()));
				const float pVoxel1px1py0pz = static_cast<float>(m_controller.convertToDensity(volIter.peekVoxel1px1py0pz()));
				const float pVoxel1px1py1pz = static_cast<float>(m_controller.convertToDensity(volIter.peekVoxel1px1py1pz()));

				const float xGrad(- weights[0][0][0] * pVoxel1nx1ny1nz -
					weights[1][0][0] * pVoxel1nx1ny0pz - weights[2][0][0] *
					pVoxel1nx1ny1pz - weights[0][1][0] * pVoxel1nx0py1nz -
					weights[1][1][0] * pVoxel1nx0py0pz - weights[2][1][0] *
					pVoxel1nx0py1pz - weights[0][2][0] * pVoxel1nx1py1nz -
					weights[1][2][0] * pVoxel1nx1py0pz - weights[2][2][0] *
					pVoxel1nx1py1pz + weights[0][0][2] * pVoxel1px1ny1nz +
					weights[1][0][2] * pVoxel1px1ny0pz + weights[2][0][2] *
					pVoxel1px1ny1pz + weights[0][1][2] * pVoxel1px0py1nz +
					weights[1][1][2] * pVoxel1px0py0pz + weights[2][1][2] *
					pVoxel1px0py1pz + weights[0][2][2] * pVoxel1px1py1nz +
					weights[1][2][2] * pVoxel1px1py0pz + weights[2][2][2] *
					pVoxel1px1py1pz);

				const float yGrad(- weights[0][0][0] * pVoxel1nx1ny1nz -
					weights[1][0][0] * pVoxel1nx1ny0pz - weights[2][0][0] *
					pVoxel1nx1ny1pz + weights[0][2][0] * pVoxel1nx1py1nz +
					weights[1][2][0] * pVoxel1nx1py0pz + weights[2][2][0] *
					pVoxel1nx1py1pz - weights[0][0][1] * pVoxel0px1ny1nz -
					weights[1][0][1] * pVoxel0px1ny0pz - weights[2][0][1] *
					pVoxel0px1ny1pz + weights[0][2][1] * pVoxel0px1py1nz +
					weights[1][2][1] * pVoxel0px1py0pz + weights[2][2][1] *
					pVoxel0px1py1pz - weights[0][0][2] * pVoxel1px1ny1nz -
					weights[1][0][2] * pVoxel1px1ny0pz - weights[2][0][2] *
					pVoxel1px1ny1pz + weights[0][2][2] * pVoxel1px1py1nz +
					weights[1][2][2] * pVoxel1px1py0pz + weights[2][2][2] *
					pVoxel1px1py1pz);

				const float zGrad(- weights[0][0][0] * pVoxel1nx1ny1nz +
					weights[2][0][0] * pVoxel1nx1ny1pz - weights[0][1][0] *
					pVoxel1nx0py1nz + weights[2][1][0] * pVoxel1nx0py1pz -
					weights[0][2][0] * pVoxel1nx1py1nz + weights[2][2][0] *
					pVoxel1nx1py1pz - weights[0][0][1] * pVoxel0px1ny1nz +
					weights[2][0][1] * pVoxel0px1ny1pz - weights[0][1][1] *
					pVoxel0px0py1nz + weights[2][1][1] * pVoxel0px0py1pz -
					weights[0][2][1] * pVoxel0px1py1nz + weights[2][2][1] *
					pVoxel0px1py1pz - weights[0][0][2] * pVoxel1px1ny1nz +
					weights[2][0][2] * pVoxel1px1ny1pz - weights[0][1][2] *
					pVoxel1px0py1nz + weights[2][1][2] * pVoxel1px0py1pz -
					weights[0][2][2] * pVoxel1px1py1nz + weights[2][2][2] *
					pVoxel1px1py1pz);

				//Note: The above actually give gradients going from low density to high density.
				//For our normals we want the the other way around, so we switch the components as we return them.
				return Vector3DFloat(-xGrad,-yGrad,-zGrad);
		}
		////////////////////////////////////////////////////////////////////////////////
		// End of compiler bug workaroumd.
		////////////////////////////////////////////////////////////////////////////////

		//The volume data and a sampler to access it.
		VolumeType* m_volData;

		//The surface patch we are currently filling.
		MeshType* m_meshCurrent;

		//Information about the region we are currently processing
		Region m_regSizeInVoxels;

		//Used to convert arbitrary voxel types in densities and materials.
		ControllerType m_controller;

		//Our threshold value
		typename ControllerType::DensityType m_tThreshold;
	};

	// This version of the function performs the extraction into a user-provided mesh rather than allocating a mesh automatically.
	// There are a few reasons why this might be useful to more advanced users:
	//
	//   1. It leaves the user in control of memory allocation and would allow them to implement e.g. a mesh pooling system.
	//   2. The user-provided mesh could have a different index type (e.g. 16-bit indices) to reduce memory usage.
	//   3. The user could provide a custom mesh class, e.g a thin wrapper around an OpenGL VBO to allow direct writing into this structure.
	//
	// We don't provide a default MeshType here. If the user doesn't want to provide a MeshType then it probably makes
	// more sense to use the other variant of this function where the mesh is a return value rather than a parameter.
	//
	// Note: This function is called 'extractMarchingCubesMeshCustom' rather than 'extractMarchingCubesMesh' to avoid ambiguity when only three parameters
	// are provided (would the third parameter be a controller or a mesh?). It seems this can be fixed by using enable_if/static_assert to emulate concepts,
	// but this is relatively complex and I haven't done it yet. Could always add it later as another overload.
	template< typename VolumeType, typename MeshType, typename ControllerType = DefaultMarchingCubesController<typename VolumeType::VoxelType> >
	void extractMarchingCubesMeshCustom(VolumeType* volData, Region region, MeshType* result, ControllerType controller = ControllerType())
	{
		MarchingCubesSurfaceExtractor<VolumeType, MeshType, ControllerType> extractor(volData, region, result, controller);
		extractor.execute();
	}

	template< typename VolumeType, typename ControllerType = DefaultMarchingCubesController<typename VolumeType::VoxelType> >
	Mesh<MarchingCubesVertex<typename VolumeType::VoxelType> > extractMarchingCubesMesh(VolumeType* volData, Region region, ControllerType controller = ControllerType())
	{
		Mesh<MarchingCubesVertex<typename VolumeType::VoxelType> > result;
		extractMarchingCubesMeshCustom<VolumeType, Mesh<MarchingCubesVertex<typename VolumeType::VoxelType>, DefaultIndexType > >(volData, region, &result, controller);
		return result;
	}
}

#include "PolyVox/MarchingCubesSurfaceExtractor.inl"

#endif

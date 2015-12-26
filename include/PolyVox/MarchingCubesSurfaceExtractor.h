/*******************************************************************************
* The MIT License (MIT)
*
* Copyright (c) 2015 David Williams and Matthew Williams
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in all
* copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE.
*******************************************************************************/

#ifndef __PolyVox_SurfaceExtractor_H__
#define __PolyVox_SurfaceExtractor_H__

#include "Impl/MarchingCubesTables.h"
#include "Impl/PlatformDefinitions.h"

#include "Array.h"
#include "BaseVolume.h" //For wrap modes... should move these?
#include "Mesh.h"
#include "DefaultMarchingCubesController.h"
#include "Vertex.h"

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
	void extractMarchingCubesMeshCustom(VolumeType* volData, Region region, MeshType* result, ControllerType controller = ControllerType());

	template< typename VolumeType, typename ControllerType = DefaultMarchingCubesController<typename VolumeType::VoxelType> >
	Mesh<MarchingCubesVertex<typename VolumeType::VoxelType> > extractMarchingCubesMesh(VolumeType* volData, Region region, ControllerType controller = ControllerType())
	{
		Mesh<MarchingCubesVertex<typename VolumeType::VoxelType> > result;
		extractMarchingCubesMeshCustom<VolumeType, Mesh<MarchingCubesVertex<typename VolumeType::VoxelType>, DefaultIndexType > >(volData, region, &result, controller);
		return result;
	}
}

#include "MarchingCubesSurfaceExtractor.inl"

#endif

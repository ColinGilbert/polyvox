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

#ifndef __PolyVox_CubicSurfaceExtractor_H__
#define __PolyVox_CubicSurfaceExtractor_H__

#include "Impl/PlatformDefinitions.h"

#include "Array.h"
#include "BaseVolume.h" //For wrap modes... should move these?
#include "DefaultIsQuadNeeded.h"
#include "Mesh.h"
#include "Vertex.h"

namespace PolyVox
{
	template<typename _DataType>
	struct  CubicVertex
	{
		typedef _DataType DataType;

		// Each component of the position is stored as a single unsigned byte.
		// The true position is found by offseting each component by 0.5f.
		Vector3DUint8 encodedPosition;

		// User data
		DataType data;
	};

	// Convienient shorthand for declaring a mesh of 'cubic' vertices
	// Currently disabled because it requires GCC 4.7
	//template <typename VertexDataType, typename IndexType = DefaultIndexType>
	//using CubicMesh = Mesh< CubicVertex<VertexDataType>, IndexType >;

	/// Decodes a position from a CubicVertex
	inline Vector3DFloat decodePosition(const Vector3DUint8& encodedPosition)
	{
		Vector3DFloat result(encodedPosition.getX(), encodedPosition.getY(), encodedPosition.getZ());
		result -= 0.5f; // Apply the required offset
		return result;
	}

	/// Decodes a MarchingCubesVertex by converting it into a regular Vertex which can then be directly used for rendering.
	template<typename DataType>
	Vertex<DataType> decodeVertex(const CubicVertex<DataType>& cubicVertex)
	{
		Vertex<DataType> result;
		result.position = decodePosition(cubicVertex.encodedPosition);
		result.normal.setElements(0.0f, 0.0f, 0.0f); // Currently not calculated
		result.data = cubicVertex.data; // Data is not encoded
		return result;
	}

	struct Quad
	{
		Quad(uint32_t v0, uint32_t v1, uint32_t v2, uint32_t v3)
		{
			vertices[0] = v0;
			vertices[1] = v1;
			vertices[2] = v2;
			vertices[3] = v3;
		}

		uint32_t vertices[4];
	};

	// This constant defines the maximum number of quads which can share a vertex in a cubic style mesh.
	//
	// We try to avoid duplicate vertices by checking whether a vertex has already been added at a given position.
	// However, it is possible that vertices have the same position but different materials. In this case, the
	// vertices are not true duplicates and both must be added to the mesh. As far as I can tell, it is possible to have
	// at most eight vertices with the same position but different materials. For example, this worst-case scenario
	// happens when we have a 2x2x2 group of voxels, all with different materials and some/all partially transparent.
	// The vertex position at the center of this group is then going to be used by all eight voxels all with different
	// materials.
	const uint32_t MaxVerticesPerPosition = 8;

	template<typename VolumeType>
	struct IndexAndMaterial
	{
		int32_t iIndex;
		typename VolumeType::VoxelType uMaterial;
	};

	enum FaceNames
	{
		PositiveX,
		PositiveY,
		PositiveZ,
		NegativeX,
		NegativeY,
		NegativeZ,
		NoOfFaces
	};

	template<typename MeshType>
	bool mergeQuads(Quad& q1, Quad& q2, MeshType* m_meshCurrent)
	{
		//All four vertices of a given quad have the same data,
		//so just check that the first pair of vertices match.
		if (m_meshCurrent->getVertex(q1.vertices[0]).data == m_meshCurrent->getVertex(q2.vertices[0]).data)
		{
			//Now check whether quad 2 is adjacent to quad one by comparing vertices.
			//Adjacent quads must share two vertices, and the second quad could be to the
			//top, bottom, left, of right of the first one. This gives four combinations to test.
			if ((q1.vertices[0] == q2.vertices[1]) && ((q1.vertices[3] == q2.vertices[2])))
			{
				q1.vertices[0] = q2.vertices[0];
				q1.vertices[3] = q2.vertices[3];
				return true;
			}
			else if ((q1.vertices[3] == q2.vertices[0]) && ((q1.vertices[2] == q2.vertices[1])))
			{
				q1.vertices[3] = q2.vertices[3];
				q1.vertices[2] = q2.vertices[2];
				return true;
			}
			else if ((q1.vertices[1] == q2.vertices[0]) && ((q1.vertices[2] == q2.vertices[3])))
			{
				q1.vertices[1] = q2.vertices[1];
				q1.vertices[2] = q2.vertices[2];
				return true;
			}
			else if ((q1.vertices[0] == q2.vertices[3]) && ((q1.vertices[1] == q2.vertices[2])))
			{
				q1.vertices[0] = q2.vertices[0];
				q1.vertices[1] = q2.vertices[1];
				return true;
			}
		}

		//Quads cannot be merged.
		return false;
	}

	template<typename MeshType>
	bool performQuadMerging(std::list<Quad>& quads, MeshType* m_meshCurrent)
	{
		bool bDidMerge = false;
		for (typename std::list<Quad>::iterator outerIter = quads.begin(); outerIter != quads.end(); outerIter++)
		{
			typename std::list<Quad>::iterator innerIter = outerIter;
			innerIter++;
			while (innerIter != quads.end())
			{
				Quad& q1 = *outerIter;
				Quad& q2 = *innerIter;

				bool result = mergeQuads(q1, q2, m_meshCurrent);

				if (result)
				{
					bDidMerge = true;
					innerIter = quads.erase(innerIter);
				}
				else
				{
					innerIter++;
				}
			}
		}

		return bDidMerge;
	}

	template<typename VolumeType, typename MeshType>
	int32_t addVertex(uint32_t uX, uint32_t uY, uint32_t uZ, typename VolumeType::VoxelType uMaterialIn, Array<3, IndexAndMaterial<VolumeType> >& existingVertices, MeshType* m_meshCurrent)
	{
		for (uint32_t ct = 0; ct < MaxVerticesPerPosition; ct++)
		{
			IndexAndMaterial<VolumeType>& rEntry = existingVertices(uX, uY, ct);

			if (rEntry.iIndex == -1)
			{
				//No vertices matched and we've now hit an empty space. Fill it by creating a vertex. The 0.5f offset is because vertices set between voxels in order to build cubes around them.
				CubicVertex<typename VolumeType::VoxelType> cubicVertex;
				cubicVertex.encodedPosition.setElements(static_cast<uint8_t>(uX), static_cast<uint8_t>(uY), static_cast<uint8_t>(uZ));
				cubicVertex.data = uMaterialIn;
				rEntry.iIndex = m_meshCurrent->addVertex(cubicVertex);
				rEntry.uMaterial = uMaterialIn;

				return rEntry.iIndex;
			}

			//If we have an existing vertex and the material matches then we can return it.
			if (rEntry.uMaterial == uMaterialIn)
			{
				return rEntry.iIndex;
			}
		}

		// If we exit the loop here then apparently all the slots were full but none of them matched.
		// This shouldn't ever happen, so if it does it is probably a bug in PolyVox. Please report it to us!
		POLYVOX_THROW(std::runtime_error, "All slots full but no matches during cubic surface extraction. This is probably a bug in PolyVox");
		return -1; //Should never happen.
	}

	// Generates a cubic-style mesh from the voxel data.
	template<typename VolumeType, typename MeshType, typename IsQuadNeeded = DefaultIsQuadNeeded<typename VolumeType::VoxelType> >
	void extractCubicMeshCustom(VolumeType* volData, Region region, MeshType* result, IsQuadNeeded isQuadNeeded = IsQuadNeeded(), bool bMergeQuads = true);

	// Generates a cubic-style mesh from the voxel data, placing the result into a user-provided Mesh.
	template<typename VolumeType, typename IsQuadNeeded = DefaultIsQuadNeeded<typename VolumeType::VoxelType> >
	Mesh<CubicVertex<typename VolumeType::VoxelType> > extractCubicMesh(VolumeType* volData, Region region, IsQuadNeeded isQuadNeeded = IsQuadNeeded(), bool bMergeQuads = true);
	
}

#include "CubicSurfaceExtractor.inl"

#endif

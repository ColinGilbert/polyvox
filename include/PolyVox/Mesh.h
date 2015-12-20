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

#ifndef __PolyVox_Mesh_H__
#define __PolyVox_Mesh_H__

#include "Impl/PlatformDefinitions.h"

#include "Region.h"
#include "Vertex.h" //Should probably do away with this on in the future...

#include <algorithm>
#include <cstdlib>
#include <list>
#include <memory>
#include <set>
#include <vector>

namespace PolyVox
{
	typedef uint32_t DefaultIndexType;
	template <typename _VertexType, typename _IndexType = DefaultIndexType>
	class Mesh
	{
	public:

		typedef _VertexType VertexType;
		typedef _IndexType IndexType;

		Mesh();
		~Mesh();	   

		IndexType getNoOfVertices(void) const;
		const VertexType& getVertex(IndexType index) const;
		const VertexType* getRawVertexData(void) const;
		POLYVOX_DEPRECATED const std::vector<VertexType>& getVertices(void) const;

		size_t getNoOfIndices(void) const;
		IndexType getIndex(uint32_t index) const;
		const IndexType* getRawIndexData(void) const;
		POLYVOX_DEPRECATED const std::vector<IndexType>& getIndices(void) const;

		const Vector3DInt32& getOffset(void) const;
		void setOffset(const Vector3DInt32& offset);

		IndexType addVertex(const VertexType& vertex);
		void addTriangle(IndexType index0, IndexType index1, IndexType index2);

		void clear(void);
		bool isEmpty(void) const;
		void removeUnusedVertices(void);		
	
	private:		
		std::vector<IndexType> m_vecIndices;
		std::vector<VertexType> m_vecVertices;
		Vector3DInt32 m_offset;
	};

	template <typename MeshType>
	Mesh< Vertex< typename MeshType::VertexType::DataType >, typename MeshType::IndexType > decodeMesh(const MeshType& encodedMesh)
	{
		Mesh< Vertex< typename MeshType::VertexType::DataType >, typename MeshType::IndexType > decodedMesh;

		for (typename MeshType::IndexType ct = 0; ct < encodedMesh.getNoOfVertices(); ct++)
		{
			decodedMesh.addVertex(decodeVertex(encodedMesh.getVertex(ct)));
		}

		POLYVOX_ASSERT(encodedMesh.getNoOfIndices() % 3 == 0, "The number of indices must always be a multiple of three.");
		for (uint32_t ct = 0; ct < encodedMesh.getNoOfIndices(); ct += 3)
		{
			decodedMesh.addTriangle(encodedMesh.getIndex(ct), encodedMesh.getIndex(ct + 1), encodedMesh.getIndex(ct + 2));
		}

		decodedMesh.setOffset(encodedMesh.getOffset());

		return decodedMesh;
	}
}

#include "Mesh.inl"

#endif /* __Mesh_H__ */

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

#include "Impl/TypeDef.h"

#include "PolyVoxCore/Region.h"
#include "PolyVoxCore/Vertex.h" //Should probably do away with this on in the future...

#include <algorithm>
#include <cstdlib>
#include <list>
#include <memory>
#include <set>
#include <vector>

namespace PolyVox
{
	template <typename _VertexType, typename _IndexType = uint32_t>
	class Mesh
	{
	public:

		typedef _VertexType VertexType;
		typedef _IndexType IndexType;

		Mesh();
		~Mesh();	   

		const std::vector<IndexType>& getIndices(void) const;
		uint32_t getNoOfIndices(void) const;
		IndexType getNoOfVertices(void) const;
		const std::vector<VertexType>& getVertices(void) const;
		const Vector3DInt32& getOffset(void) const;

		void setOffset(const Vector3DInt32& offset);

		void addTriangle(IndexType index0, IndexType index1, IndexType index2);
		IndexType addVertex(const VertexType& vertex);
		void clear(void);
		bool isEmpty(void) const;
		void removeUnusedVertices(void);

		Vector3DInt32 m_offset;
	
	public:		
		std::vector<IndexType> m_vecTriangleIndices;
		std::vector<VertexType> m_vecVertices;
	};

	template <typename MeshType>
	Mesh< Vertex< typename MeshType::VertexType::DataType >, typename MeshType::IndexType > decode(const MeshType& mesh)
	{
		Mesh< Vertex< typename MeshType::VertexType::DataType >, typename MeshType::IndexType > result;
		result.m_vecVertices.resize(mesh.m_vecVertices.size());

		for(typename MeshType::IndexType ct = 0; ct < mesh.m_vecVertices.size(); ct++)
		{
			result.m_vecVertices[ct] = decode(mesh.m_vecVertices[ct]);
		}

		result.m_vecTriangleIndices = mesh.m_vecTriangleIndices;

		result.m_offset = mesh.m_offset;

		return result;
	}
}

#include "PolyVoxCore/Mesh.inl"

#endif /* __Mesh_H__ */

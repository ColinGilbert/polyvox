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

namespace PolyVox
{
	template <typename VertexType, typename IndexType>
	Mesh<VertexType, IndexType>::Mesh()
	{
	}

	template <typename VertexType, typename IndexType>
	Mesh<VertexType, IndexType>::~Mesh()	  
	{
	}

	template <typename VertexType, typename IndexType>
	IndexType Mesh<VertexType, IndexType>::getNoOfVertices(void) const
	{
		return static_cast<IndexType>(m_vecVertices.size());
	}

	template <typename VertexType, typename IndexType>
	const VertexType& Mesh<VertexType, IndexType>::getVertex(IndexType index) const
	{
		return m_vecVertices[index];
	}

	template <typename VertexType, typename IndexType>
	const VertexType*  Mesh<VertexType, IndexType>::getRawVertexData(void) const
	{
		return &(m_vecVertices[0]);
	}

	template <typename VertexType, typename IndexType>
	const std::vector<VertexType>& Mesh<VertexType, IndexType>::getVertices(void) const
	{
		return m_vecVertices;
	}

	template <typename VertexType, typename IndexType>
	size_t Mesh<VertexType, IndexType>::getNoOfIndices(void) const
	{
		return m_vecIndices.size();
	}

	template <typename VertexType, typename IndexType>
	IndexType Mesh<VertexType, IndexType>::getIndex(uint32_t index) const
	{
		return m_vecIndices[index];
	}

	template <typename VertexType, typename IndexType>
	const IndexType* Mesh<VertexType, IndexType>::getRawIndexData(void) const
	{
		return &(m_vecIndices[0]);
	}

	template <typename VertexType, typename IndexType>
	const std::vector<IndexType>& Mesh<VertexType, IndexType>::getIndices(void) const
	{
		return m_vecIndices;
	}

	template <typename VertexType, typename IndexType>
	const Vector3DInt32& Mesh<VertexType, IndexType>::getOffset(void) const
	{
		return m_offset;
	}

	template <typename VertexType, typename IndexType>
	void Mesh<VertexType, IndexType>::setOffset(const Vector3DInt32& offset)
	{
		m_offset = offset;
	}

	template <typename VertexType, typename IndexType>
	void Mesh<VertexType, IndexType>::addTriangle(IndexType index0, IndexType index1, IndexType index2)
	{
		//Make sure the specified indices correspond to valid vertices.
		POLYVOX_ASSERT(index0 < m_vecVertices.size(), "Index points at an invalid vertex.");
		POLYVOX_ASSERT(index1 < m_vecVertices.size(), "Index points at an invalid vertex.");
		POLYVOX_ASSERT(index2 < m_vecVertices.size(), "Index points at an invalid vertex.");

		m_vecIndices.push_back(index0);
		m_vecIndices.push_back(index1);
		m_vecIndices.push_back(index2);
	}

	template <typename VertexType, typename IndexType>
	IndexType Mesh<VertexType, IndexType>::addVertex(const VertexType& vertex)
	{
		// We should not add more vertices than our chosen index type will let us index.
		POLYVOX_THROW_IF(m_vecVertices.size() >= std::numeric_limits<IndexType>::max(), std::out_of_range, "Mesh has more vertices that the chosen index type allows.");

		m_vecVertices.push_back(vertex);
		return m_vecVertices.size() - 1;
	}

	template <typename VertexType, typename IndexType>
	void Mesh<VertexType, IndexType>::clear(void)
	{
		m_vecVertices.clear();
		m_vecIndices.clear();
	}

	template <typename VertexType, typename IndexType>
	bool Mesh<VertexType, IndexType>::isEmpty(void) const
	{
		return (getNoOfVertices() == 0) || (getNoOfIndices() == 0);
	}

	template <typename VertexType, typename IndexType>
	void Mesh<VertexType, IndexType>::removeUnusedVertices(void)
	{
		std::vector<bool> isVertexUsed(m_vecVertices.size());
		std::fill(isVertexUsed.begin(), isVertexUsed.end(), false);

		for(uint32_t triCt = 0; triCt < m_vecIndices.size(); triCt++)
		{
			int v = m_vecIndices[triCt];
			isVertexUsed[v] = true;
		}

		int noOfUsedVertices = 0;
		std::vector<uint32_t> newPos(m_vecVertices.size());
		for(IndexType vertCt = 0; vertCt < m_vecVertices.size(); vertCt++)
		{
			if(isVertexUsed[vertCt])
			{
				m_vecVertices[noOfUsedVertices] = m_vecVertices[vertCt];
				newPos[vertCt] = noOfUsedVertices;
				noOfUsedVertices++;
			}
		}

		m_vecVertices.resize(noOfUsedVertices);

		for(uint32_t triCt = 0; triCt < m_vecIndices.size(); triCt++)
		{
			m_vecIndices[triCt] = newPos[m_vecIndices[triCt]];
		}
	}
}

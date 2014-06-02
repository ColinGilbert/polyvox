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
	template <typename VertexType>
	Mesh<VertexType>::Mesh()
	{
	}

	template <typename VertexType>
	Mesh<VertexType>::~Mesh()	  
	{
	}

	template <typename VertexType>
	const std::vector<uint32_t>& Mesh<VertexType>::getIndices(void) const
	{
		return m_vecTriangleIndices;
	}

	template <typename VertexType>
	uint32_t Mesh<VertexType>::getNoOfIndices(void) const
	{
		return m_vecTriangleIndices.size();
	}	

	template <typename VertexType>
	uint32_t Mesh<VertexType>::getNoOfNonUniformTrianges(void) const
	{
		uint32_t result = 0;
		for(uint32_t i = 0; i < m_vecTriangleIndices.size() - 2; i += 3)
		{
			if((m_vecVertices[m_vecTriangleIndices[i]].getMaterial() == m_vecVertices[m_vecTriangleIndices[i+1]].getMaterial())
			&& (m_vecVertices[m_vecTriangleIndices[i]].getMaterial() == m_vecVertices[m_vecTriangleIndices[i+2]].getMaterial()))
			{
			}
			else
			{
				result++;
			}
		}
		return result;
	}

	template <typename VertexType>
	uint32_t Mesh<VertexType>::getNoOfUniformTrianges(void) const
	{
		uint32_t result = 0;
		for(uint32_t i = 0; i < m_vecTriangleIndices.size() - 2; i += 3)
		{
			if((m_vecVertices[m_vecTriangleIndices[i]].getMaterial() == m_vecVertices[m_vecTriangleIndices[i+1]].getMaterial())
			&& (m_vecVertices[m_vecTriangleIndices[i]].getMaterial() == m_vecVertices[m_vecTriangleIndices[i+2]].getMaterial()))
			{
				result++;
			}
		}
		return result;
	}

	template <typename VertexType>
	uint32_t Mesh<VertexType>::getNoOfVertices(void) const
	{
		return m_vecVertices.size();
	}

	template <typename VertexType>
	std::vector<VertexType>& Mesh<VertexType>::getRawVertexData(void)
	{
		return m_vecVertices;
	}

	template <typename VertexType>
	const std::vector<VertexType>& Mesh<VertexType>::getVertices(void) const
	{
		return m_vecVertices;
	}		

	template <typename VertexType>
	void Mesh<VertexType>::addTriangle(uint32_t index0, uint32_t index1, uint32_t index2)
	{
		//Make sure the specified indices correspond to valid vertices.
		POLYVOX_ASSERT(index0 < m_vecVertices.size(), "Index points at an invalid vertex.");
		POLYVOX_ASSERT(index1 < m_vecVertices.size(), "Index points at an invalid vertex.");
		POLYVOX_ASSERT(index2 < m_vecVertices.size(), "Index points at an invalid vertex.");

		m_vecTriangleIndices.push_back(index0);
		m_vecTriangleIndices.push_back(index1);
		m_vecTriangleIndices.push_back(index2);
	}

	template <typename VertexType>
	void Mesh<VertexType>::addTriangleCubic(uint32_t index0, uint32_t index1, uint32_t index2)
	{
		//Make sure the specified indices correspond to valid vertices.
		POLYVOX_ASSERT(index0 < m_vecVertices.size(), "Index points at an invalid vertex.");
		POLYVOX_ASSERT(index1 < m_vecVertices.size(), "Index points at an invalid vertex.");
		POLYVOX_ASSERT(index2 < m_vecVertices.size(), "Index points at an invalid vertex.");

		m_vecTriangleIndices.push_back(index0);
		m_vecTriangleIndices.push_back(index1);
		m_vecTriangleIndices.push_back(index2);
	}

	template <typename VertexType>
	uint32_t Mesh<VertexType>::addVertex(const VertexType& vertex)
	{
		m_vecVertices.push_back(vertex);
		return m_vecVertices.size() - 1;
	}

	template <typename VertexType>
	void Mesh<VertexType>::clear(void)
	{
		m_vecVertices.clear();
		m_vecTriangleIndices.clear();
		m_vecLodRecords.clear();
	}

	template <typename VertexType>
	bool Mesh<VertexType>::isEmpty(void) const
	{
		return (getNoOfVertices() == 0) || (getNoOfIndices() == 0);
	}
	
	template <typename VertexType>
	int Mesh<VertexType>::noOfDegenerateTris(void)
	{
		int count = 0;
		for(uint32_t triCt = 0; triCt < m_vecTriangleIndices.size();)
		{
			int v0 = m_vecTriangleIndices[triCt];
			triCt++;
			int v1 = m_vecTriangleIndices[triCt];
			triCt++;
			int v2 = m_vecTriangleIndices[triCt];
			triCt++;

			if((v0 == v1) || (v1 == v2) || (v2 == v0))
			{
				count++;
			}
		}
		return count;
	}

	template <typename VertexType>
	void Mesh<VertexType>::removeDegenerateTris(void)
	{
		int noOfNonDegenerate = 0;
		int targetCt = 0;
		for(uint32_t triCt = 0; triCt < m_vecTriangleIndices.size();)
		{
			int v0 = m_vecTriangleIndices[triCt];
			triCt++;
			int v1 = m_vecTriangleIndices[triCt];
			triCt++;
			int v2 = m_vecTriangleIndices[triCt];
			triCt++;

			if((v0 != v1) && (v1 != v2) & (v2 != v0))
			{
				m_vecTriangleIndices[targetCt] = v0;
				targetCt++;
				m_vecTriangleIndices[targetCt] = v1;
				targetCt++;
				m_vecTriangleIndices[targetCt] = v2;
				targetCt++;

				noOfNonDegenerate++;
			}
		}

		m_vecTriangleIndices.resize(noOfNonDegenerate * 3);
	}

	template <typename VertexType>
	void Mesh<VertexType>::removeUnusedVertices(void)
	{
		std::vector<bool> isVertexUsed(m_vecVertices.size());
		std::fill(isVertexUsed.begin(), isVertexUsed.end(), false);

		for(uint32_t triCt = 0; triCt < m_vecTriangleIndices.size(); triCt++)
		{
			int v = m_vecTriangleIndices[triCt];
			isVertexUsed[v] = true;
		}

		int noOfUsedVertices = 0;
		std::vector<uint32_t> newPos(m_vecVertices.size());
		for(uint32_t vertCt = 0; vertCt < m_vecVertices.size(); vertCt++)
		{
			if(isVertexUsed[vertCt])
			{
				m_vecVertices[noOfUsedVertices] = m_vecVertices[vertCt];
				newPos[vertCt] = noOfUsedVertices;
				noOfUsedVertices++;
			}
		}

		m_vecVertices.resize(noOfUsedVertices);

		for(uint32_t triCt = 0; triCt < m_vecTriangleIndices.size(); triCt++)
		{
			m_vecTriangleIndices[triCt] = newPos[m_vecTriangleIndices[triCt]];
		}
	}
}

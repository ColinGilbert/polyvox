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

#include <cstdlib>
#include <list>
#include <algorithm>

using namespace std;

namespace PolyVox
{
	template <typename VertexType>
	SurfaceMesh<VertexType>::SurfaceMesh()
	{
		m_iTimeStamp = -1;
	}

	template <typename VertexType>
	SurfaceMesh<VertexType>::~SurfaceMesh()	  
	{
	}

	template <typename VertexType>
	const std::vector<uint32_t>& SurfaceMesh<VertexType>::getIndices(void) const
	{
		return m_vecTriangleIndices;
	}

	template <typename VertexType>
	uint32_t SurfaceMesh<VertexType>::getNoOfIndices(void) const
	{
		return m_vecTriangleIndices.size();
	}	

	template <typename VertexType>
	uint32_t SurfaceMesh<VertexType>::getNoOfNonUniformTrianges(void) const
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
	uint32_t SurfaceMesh<VertexType>::getNoOfUniformTrianges(void) const
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
	uint32_t SurfaceMesh<VertexType>::getNoOfVertices(void) const
	{
		return m_vecVertices.size();
	}

	template <typename VertexType>
	std::vector<VertexType>& SurfaceMesh<VertexType>::getRawVertexData(void)
	{
		return m_vecVertices;
	}

	template <typename VertexType>
	const std::vector<VertexType>& SurfaceMesh<VertexType>::getVertices(void) const
	{
		return m_vecVertices;
	}		

	template <typename VertexType>
	void SurfaceMesh<VertexType>::addTriangle(uint32_t index0, uint32_t index1, uint32_t index2)
	{
		m_vecTriangleIndices.push_back(index0);
		m_vecTriangleIndices.push_back(index1);
		m_vecTriangleIndices.push_back(index2);

		if((m_vecVertices[index0].material == m_vecVertices[index1].material) && (m_vecVertices[index0].material == m_vecVertices[index2].material))
		{
			m_mapUsedMaterials.insert(m_vecVertices[index0].material);			
		}
		else
		{
			m_vecVertices[index0].setOnMaterialEdge(true);
			m_vecVertices[index1].setOnMaterialEdge(true);
			m_vecVertices[index2].setOnMaterialEdge(true);
		}
	}

	template <typename VertexType>
	void SurfaceMesh<VertexType>::addTriangleCubic(uint32_t index0, uint32_t index1, uint32_t index2)
	{
		m_vecTriangleIndices.push_back(index0);
		m_vecTriangleIndices.push_back(index1);
		m_vecTriangleIndices.push_back(index2);
	}

	template <typename VertexType>
	uint32_t SurfaceMesh<VertexType>::addVertex(const VertexType& vertex)
	{
		m_vecVertices.push_back(vertex);
		return m_vecVertices.size() - 1;
	}

	template <typename VertexType>
	void SurfaceMesh<VertexType>::clear(void)
	{
		m_vecVertices.clear();
		m_vecTriangleIndices.clear();
		m_vecLodRecords.clear();
		m_mapUsedMaterials.clear();
	}

	template <typename VertexType>
	const bool SurfaceMesh<VertexType>::isEmpty(void) const
	{
		return (getNoOfVertices() == 0) || (getNoOfIndices() == 0);
	}

	////////////////////////////////////////////////////////////////////////////////
	/// The function works on a per triangle basis without any need for connectivity
	/// information. It determines whether a triangle is lying on a flat or curved
	/// section of the surface patch by examining the normals - therefore these
	/// normals must hve been set to something sensible before this functions is called.
	/// \param fAmount A factor controlling how much the vertices move by. Find a good
	/// value by experimentation, starting with something small such as 0.1f.
	/// \param bIncludeGeometryEdgeVertices Indicates whether vertices on the edge of an
	/// SurfaceMesh should be smoothed. This can cause dicontinuities between
	/// neighbouring patches.
	////////////////////////////////////////////////////////////////////////////////
	template <typename VertexType>
	void SurfaceMesh<VertexType>::smoothPositions(float fAmount, bool bIncludeGeometryEdgeVertices)
	{
		if(m_vecVertices.size() == 0) //FIXME - I don't think we should need this test, but I have seen crashes otherwise...
		{
			return;
		}

		//This will hold the new positions, and is initialised with the current positions.
		std::vector<Vector3DFloat> newPositions(m_vecVertices.size());
		for(uint32_t uIndex = 0; uIndex < newPositions.size(); uIndex++)
		{
			newPositions[uIndex] = m_vecVertices[uIndex].getPosition();
		}

		//Iterate over each triangle
		for(vector<uint32_t>::iterator iterIndex = m_vecTriangleIndices.begin(); iterIndex != m_vecTriangleIndices.end();)
		{
			//Get the vertex data for the triangle
			PositionMaterialNormal& v0 = m_vecVertices[*iterIndex];
			Vector3DFloat& v0New = newPositions[*iterIndex];
			iterIndex++;
			PositionMaterialNormal& v1 = m_vecVertices[*iterIndex];
			Vector3DFloat& v1New = newPositions[*iterIndex];
			iterIndex++;
			PositionMaterialNormal& v2 = m_vecVertices[*iterIndex];
			Vector3DFloat& v2New = newPositions[*iterIndex];
			iterIndex++;

			//Find the midpoint
			Vector3DFloat v3dMidpoint = (v0.position + v1.position + v2.position) / 3.0f;

			//Vectors from vertex to midpoint
			Vector3DFloat v0ToMidpoint = v3dMidpoint - v0.position;
			Vector3DFloat v1ToMidpoint = v3dMidpoint - v1.position;
			Vector3DFloat v2ToMidpoint = v3dMidpoint - v2.position;
			
			//Get the vertex normals
			Vector3DFloat n0 = v0.getNormal();
			Vector3DFloat n1 = v1.getNormal();			
			Vector3DFloat n2 = v2.getNormal();				

			//I don't think these normalisation are necessary... and could be slow.
			//Normals should be normalised anyway, and as long as all triangles are
			//about the same size the distances to midpoint should be similar too.
			//v0ToMidpoint.normalise();
			//v1ToMidpoint.normalise();
			//v2ToMidpoint.normalise();
			//n0.normalise();
			//n1.normalise();
			//n2.normalise();
			
			//If the dot product is zero the the normals are perpendicular
			//to the triangle, hence the positions do not move.
			v0New += (n0 * (n0.dot(v0ToMidpoint)) * fAmount);
			v1New += (n1 * (n1.dot(v1ToMidpoint)) * fAmount);
			v2New += (n2 * (n2.dot(v2ToMidpoint)) * fAmount);
		}

		//Update with the new positions
		for(uint32_t uIndex = 0; uIndex < newPositions.size(); uIndex++)
		{
			if((bIncludeGeometryEdgeVertices) || (m_vecVertices[uIndex].isOnGeometryEdge() == false))
			{
				m_vecVertices[uIndex].setPosition(newPositions[uIndex]);
			}
		}
	}	

	////////////////////////////////////////////////////////////////////////////////
	/// This function can help improve the visual appearance of a surface patch by
	/// smoothing normals with other nearby normals. It iterates over each triangle
	/// in the surface patch and determines the sum of its corners normals. For any
	/// given vertex, these sums are in turn summed for any triangles which use the
	/// vertex. Usually, the resulting normals should be renormalised afterwards.
	/// Note: This function can cause lighting discontinuities accross region boundaries.
	////////////////////////////////////////////////////////////////////////////////
	template <typename VertexType>
	void SurfaceMesh<VertexType>::sumNearbyNormals(bool bNormaliseResult)
	{
		if(m_vecVertices.size() == 0) //FIXME - I don't think we should need this test, but I have seen crashes otherwise...
		{
			return;
		}

		std::vector<Vector3DFloat> summedNormals(m_vecVertices.size());

		//Initialise all normals to zero. Should be ok as the vector should store all elements contiguously.
		memset(&summedNormals[0], 0, summedNormals.size() * sizeof(Vector3DFloat));

		for(vector<uint32_t>::iterator iterIndex = m_vecTriangleIndices.begin(); iterIndex != m_vecTriangleIndices.end();)
		{
			PositionMaterialNormal& v0 = m_vecVertices[*iterIndex];
			Vector3DFloat& v0New = summedNormals[*iterIndex];
			iterIndex++;
			PositionMaterialNormal& v1 = m_vecVertices[*iterIndex];
			Vector3DFloat& v1New = summedNormals[*iterIndex];
			iterIndex++;
			PositionMaterialNormal& v2 = m_vecVertices[*iterIndex];
			Vector3DFloat& v2New = summedNormals[*iterIndex];
			iterIndex++;

			Vector3DFloat sumOfNormals = v0.getNormal() + v1.getNormal() + v2.getNormal();

			v0New += sumOfNormals;
			v1New += sumOfNormals;
			v2New += sumOfNormals;
		}

		for(uint32_t uIndex = 0; uIndex < summedNormals.size(); uIndex++)
		{
			if(bNormaliseResult)
			{
				summedNormals[uIndex].normalise();
			}
			m_vecVertices[uIndex].setNormal(summedNormals[uIndex]);
		}
	}

	template <typename VertexType>
	void SurfaceMesh<VertexType>::generateAveragedFaceNormals(bool bNormalise, bool bIncludeEdgeVertices)
	{
		Vector3DFloat offset = static_cast<Vector3DFloat>(m_Region.getLowerCorner());

		//Initially zero the normals
		for(vector<PositionMaterialNormal>::iterator iterVertex = m_vecVertices.begin(); iterVertex != m_vecVertices.end(); iterVertex++)
		{
			if(m_Region.containsPoint(iterVertex->getPosition() + offset, 0.001))
			{
				iterVertex->setNormal(Vector3DFloat(0.0f,0.0f,0.0f));
			}
		}

		for(vector<uint32_t>::iterator iterIndex = m_vecTriangleIndices.begin(); iterIndex != m_vecTriangleIndices.end();)
		{
			PositionMaterialNormal& v0 = m_vecVertices[*iterIndex];
			iterIndex++;
			PositionMaterialNormal& v1 = m_vecVertices[*iterIndex];
			iterIndex++;
			PositionMaterialNormal& v2 = m_vecVertices[*iterIndex];
			iterIndex++;

			Vector3DFloat triangleNormal = (v1.getPosition()-v0.getPosition()).cross(v2.getPosition()-v0.getPosition());

			if(m_Region.containsPoint(v0.getPosition() + offset, 0.001))
			{
				v0.setNormal(v0.getNormal() + triangleNormal);
			}
			if(m_Region.containsPoint(v1.getPosition() + offset, 0.001))
			{
				v1.setNormal(v1.getNormal() + triangleNormal);
			}
			if(m_Region.containsPoint(v2.getPosition() + offset, 0.001))
			{
				v2.setNormal(v2.getNormal() + triangleNormal);
			}
		}

		if(bNormalise)
		{
			for(vector<PositionMaterialNormal>::iterator iterVertex = m_vecVertices.begin(); iterVertex != m_vecVertices.end(); iterVertex++)
			{
				Vector3DFloat normal = iterVertex->getNormal();
				normal.normalise();
				iterVertex->setNormal(normal);
			}
		}
	}

	template <typename VertexType>
	polyvox_shared_ptr< SurfaceMesh<VertexType> > SurfaceMesh<VertexType>::extractSubset(std::set<uint8_t> setMaterials)
	{
		polyvox_shared_ptr< SurfaceMesh<VertexType> > result(new SurfaceMesh<VertexType>);

		if(m_vecVertices.size() == 0) //FIXME - I don't think we should need this test, but I have seen crashes otherwise...
		{
			return result;
		}

		assert(m_vecLodRecords.size() == 1);
		if(m_vecLodRecords.size() != 1)
		{
			//If we have done progressive LOD then it's too late to split into subsets.
			return result;
		}

		std::vector<int32_t> indexMap(m_vecVertices.size());
		std::fill(indexMap.begin(), indexMap.end(), -1);

		for(uint32_t triCt = 0; triCt < m_vecTriangleIndices.size(); triCt += 3)
		{

			PositionMaterialNormal& v0 = m_vecVertices[m_vecTriangleIndices[triCt]];
			PositionMaterialNormal& v1 = m_vecVertices[m_vecTriangleIndices[triCt + 1]];
			PositionMaterialNormal& v2 = m_vecVertices[m_vecTriangleIndices[triCt + 2]];

			if(
				(setMaterials.find(v0.getMaterial()) != setMaterials.end()) || 
				(setMaterials.find(v1.getMaterial()) != setMaterials.end()) || 
				(setMaterials.find(v2.getMaterial()) != setMaterials.end()))
			{
				uint32_t i0;
				if(indexMap[m_vecTriangleIndices[triCt]] == -1)
				{
					indexMap[m_vecTriangleIndices[triCt]] = result->addVertex(v0);
				}
				i0 = indexMap[m_vecTriangleIndices[triCt]];

				uint32_t i1;
				if(indexMap[m_vecTriangleIndices[triCt+1]] == -1)
				{
					indexMap[m_vecTriangleIndices[triCt+1]] = result->addVertex(v1);
				}
				i1 = indexMap[m_vecTriangleIndices[triCt+1]];

				uint32_t i2;
				if(indexMap[m_vecTriangleIndices[triCt+2]] == -1)
				{
					indexMap[m_vecTriangleIndices[triCt+2]] = result->addVertex(v2);
				}
				i2 = indexMap[m_vecTriangleIndices[triCt+2]];

				result->addTriangle(i0,i1,i2);
			}
		}

		result->m_vecLodRecords.clear();
		LodRecord lodRecord;
		lodRecord.beginIndex = 0;
		lodRecord.endIndex = result->getNoOfIndices();
		result->m_vecLodRecords.push_back(lodRecord);

		return result;
	}

	/*int SurfaceMesh::countMaterialBoundary(void)
	{
		int count = 0;
		for(int ct = 0; ct < m_vecVertices.size(); ct++)
		{
			if(m_vecVertices[ct].m_bIsMaterialEdgeVertex)
			{
				count++;
			}
		}
		return count;
	}

	void SurfaceMesh::growMaterialBoundary(void)
	{
		std::vector<PositionMaterialNormal> vecNewVertices = m_vecVertices;

		for(vector<uint32_t>::iterator iterIndex = m_vecTriangleIndices.begin(); iterIndex != m_vecTriangleIndices.end();)
		{
			PositionMaterialNormal& v0 = m_vecVertices[*iterIndex];
			PositionMaterialNormal& v0New = vecNewVertices[*iterIndex];
			iterIndex++;
			PositionMaterialNormal& v1 = m_vecVertices[*iterIndex];
			PositionMaterialNormal& v1New = vecNewVertices[*iterIndex];
			iterIndex++;
			PositionMaterialNormal& v2 = m_vecVertices[*iterIndex];
			PositionMaterialNormal& v2New = vecNewVertices[*iterIndex];
			iterIndex++;

			if(v0.m_bIsMaterialEdgeVertex || v1.m_bIsMaterialEdgeVertex || v2.m_bIsMaterialEdgeVertex)
			{
				v0New.m_bIsMaterialEdgeVertex = true;
				v1New.m_bIsMaterialEdgeVertex = true;
				v2New.m_bIsMaterialEdgeVertex = true;
			}
		}

		m_vecVertices = vecNewVertices;
	}*/

	template <typename VertexType>
	int SurfaceMesh<VertexType>::noOfDegenerateTris(void)
	{
		int count = 0;
		for(int triCt = 0; triCt < m_vecTriangleIndices.size();)
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
	void SurfaceMesh<VertexType>::removeDegenerateTris(void)
	{
		int noOfNonDegenerate = 0;
		int targetCt = 0;
		for(int triCt = 0; triCt < m_vecTriangleIndices.size();)
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
	void SurfaceMesh<VertexType>::removeUnusedVertices(void)
	{
		vector<bool> isVertexUsed(m_vecVertices.size());
		fill(isVertexUsed.begin(), isVertexUsed.end(), false);

		for(int triCt = 0; triCt < m_vecTriangleIndices.size(); triCt++)
		{
			int v = m_vecTriangleIndices[triCt];
			isVertexUsed[v] = true;
		}

		int noOfUsedVertices = 0;
		vector<uint32_t> newPos(m_vecVertices.size());
		for(int vertCt = 0; vertCt < m_vecVertices.size(); vertCt++)
		{
			if(isVertexUsed[vertCt])
			{
				m_vecVertices[noOfUsedVertices] = m_vecVertices[vertCt];
				newPos[vertCt] = noOfUsedVertices;
				noOfUsedVertices++;
			}
		}

		m_vecVertices.resize(noOfUsedVertices);

		for(int triCt = 0; triCt < m_vecTriangleIndices.size(); triCt++)
		{
			m_vecTriangleIndices[triCt] = newPos[m_vecTriangleIndices[triCt]];
		}
	}
}

#pragma region License
/******************************************************************************
This file is part of the PolyVox library
Copyright (C) 2006  David Williams

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
******************************************************************************/
#pragma endregion

#include "IndexedSurfacePatch.h"

using namespace std;

namespace PolyVox
{
	IndexedSurfacePatch::IndexedSurfacePatch()
	{
		m_iTimeStamp = -1;
	}

	IndexedSurfacePatch::~IndexedSurfacePatch()	  
	{
	}

	const std::vector<uint32_t>& IndexedSurfacePatch::getIndices(void) const
	{
		return m_vecTriangleIndices;
	}

	uint32_t IndexedSurfacePatch::getNoOfIndices(void) const
	{
		return m_vecTriangleIndices.size();
	}	

	uint32_t IndexedSurfacePatch::getNoOfNonUniformTrianges(void) const
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

	uint32_t IndexedSurfacePatch::getNoOfUniformTrianges(void) const
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

	uint32_t IndexedSurfacePatch::getNoOfVertices(void) const
	{
		return m_vecVertices.size();
	}

	std::vector<SurfaceVertex>& IndexedSurfacePatch::getRawVertexData(void)
	{
		return m_vecVertices;
	}

	const std::vector<SurfaceVertex>& IndexedSurfacePatch::getVertices(void) const
	{
		return m_vecVertices;
	}		

	void IndexedSurfacePatch::addTriangle(uint32_t index0, uint32_t index1, uint32_t index2)
	{
		m_vecTriangleIndices.push_back(index0);
		m_vecTriangleIndices.push_back(index1);
		m_vecTriangleIndices.push_back(index2);
	}

	uint32_t IndexedSurfacePatch::addVertex(const SurfaceVertex& vertex)
	{
		m_vecVertices.push_back(vertex);
		return m_vecVertices.size() - 1;
	}

	void IndexedSurfacePatch::clear(void)
	{
		m_vecVertices.clear();
		m_vecTriangleIndices.clear();
	}

	const bool IndexedSurfacePatch::isEmpty(void) const
	{
		return (getNoOfVertices() == 0) || (getNoOfIndices() == 0);
	}

	void IndexedSurfacePatch::smooth(float fAmount, bool bIncludeEdgeVertices)
	{
		if(m_vecVertices.size() == 0) //FIXME - I don't think we should need this test, but I have seen crashes otherwise...
		{
			return;
		}

		std::vector<SurfaceVertex> vecOriginalVertices = m_vecVertices;

		Vector3DFloat offset = static_cast<Vector3DFloat>(m_Region.getLowerCorner());

		for(vector<uint32_t>::iterator iterIndex = m_vecTriangleIndices.begin(); iterIndex != m_vecTriangleIndices.end();)
		{
			SurfaceVertex& v0 = vecOriginalVertices[*iterIndex];
			SurfaceVertex& v0New = m_vecVertices[*iterIndex];
			iterIndex++;
			SurfaceVertex& v1 = vecOriginalVertices[*iterIndex];
			SurfaceVertex& v1New = m_vecVertices[*iterIndex];
			iterIndex++;
			SurfaceVertex& v2 = vecOriginalVertices[*iterIndex];
			SurfaceVertex& v2New = m_vecVertices[*iterIndex];
			iterIndex++;

			//FIXME - instead of finding these opposite points (Opp) we could just use the midpoint?
			Vector3DFloat v0Opp = (v1.position + v2.position) / 2.0f;
			Vector3DFloat v1Opp = (v0.position + v2.position) / 2.0f;
			Vector3DFloat v2Opp = (v0.position + v1.position) / 2.0f;

			Vector3DFloat v0ToOpp = v0Opp - v0.position;
			v0ToOpp.normalise();
			Vector3DFloat v1ToOpp = v1Opp - v1.position;
			v1ToOpp.normalise();
			Vector3DFloat v2ToOpp = v2Opp - v2.position;
			v2ToOpp.normalise();

			Vector3DFloat n0 = v0.getNormal();
			n0.normalise();
			Vector3DFloat n1 = v1.getNormal();
			n1.normalise();
			Vector3DFloat n2 = v2.getNormal();
			n2.normalise();			

			if(m_Region.containsPoint(v0.getPosition() + offset, 0.001))
			{
				v0New.position += (n0 * (n0.dot(v0ToOpp)) * fAmount);
			}
			if(m_Region.containsPoint(v1.getPosition() + offset, 0.001))
			{
				v1New.position += (n1 * (n1.dot(v1ToOpp)) * fAmount);
			}
			if(m_Region.containsPoint(v2.getPosition() + offset, 0.001))
			{
				v2New.position += (n2 * (n2.dot(v2ToOpp)) * fAmount);
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
	void IndexedSurfacePatch::sumNearbyNormals(bool bNormalise)
	{
		if(m_vecVertices.size() == 0) //FIXME - I don't think we should need this test, but I have seen crashes otherwise...
		{
			return;
		}

		std::vector<Vector3DFloat> summedNormals(m_vecVertices.size());

		//Initialise all normals to zero. Pretty sure this is ok,
		//as the vector should stoer all elements contiguously.
		memset(&summedNormals[0], 0, summedNormals.size() * sizeof(Vector3DFloat));

		for(vector<uint32_t>::iterator iterIndex = m_vecTriangleIndices.begin(); iterIndex != m_vecTriangleIndices.end();)
		{
			SurfaceVertex& v0 = m_vecVertices[*iterIndex];
			Vector3DFloat& v0New = summedNormals[*iterIndex];
			iterIndex++;
			SurfaceVertex& v1 = m_vecVertices[*iterIndex];
			Vector3DFloat& v1New = summedNormals[*iterIndex];
			iterIndex++;
			SurfaceVertex& v2 = m_vecVertices[*iterIndex];
			Vector3DFloat& v2New = summedNormals[*iterIndex];
			iterIndex++;

			Vector3DFloat sumOfNormals = v0.getNormal() + v1.getNormal() + v2.getNormal();

			v0New += sumOfNormals;
			v1New += sumOfNormals;
			v2New += sumOfNormals;
		}

		for(uint32_t uIndex = 0; uIndex < summedNormals.size(); uIndex++)
		{
			if(bNormalise)
			{
				summedNormals[uIndex].normalise();
			}
			m_vecVertices[uIndex].setNormal(summedNormals[uIndex]);
		}
	}

	void IndexedSurfacePatch::generateAveragedFaceNormals(bool bNormalise, bool bIncludeEdgeVertices)
	{
		Vector3DFloat offset = static_cast<Vector3DFloat>(m_Region.getLowerCorner());

		//Initially zero the normals
		for(vector<SurfaceVertex>::iterator iterVertex = m_vecVertices.begin(); iterVertex != m_vecVertices.end(); iterVertex++)
		{
			if(m_Region.containsPoint(iterVertex->getPosition() + offset, 0.001))
			{
				iterVertex->setNormal(Vector3DFloat(0.0f,0.0f,0.0f));
			}
		}

		for(vector<uint32_t>::iterator iterIndex = m_vecTriangleIndices.begin(); iterIndex != m_vecTriangleIndices.end();)
		{
			SurfaceVertex& v0 = m_vecVertices[*iterIndex];
			iterIndex++;
			SurfaceVertex& v1 = m_vecVertices[*iterIndex];
			iterIndex++;
			SurfaceVertex& v2 = m_vecVertices[*iterIndex];
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
			for(vector<SurfaceVertex>::iterator iterVertex = m_vecVertices.begin(); iterVertex != m_vecVertices.end(); iterVertex++)
			{
				Vector3DFloat normal = iterVertex->getNormal();
				normal.normalise();
				iterVertex->setNormal(normal);
			}
		}
	}

	POLYVOX_SHARED_PTR<IndexedSurfacePatch> IndexedSurfacePatch::extractSubset(std::set<uint8_t> setMaterials)
	{
		POLYVOX_SHARED_PTR<IndexedSurfacePatch> result(new IndexedSurfacePatch);

		if(m_vecVertices.size() == 0) //FIXME - I don't think we should need this test, but I have seen crashes otherwise...
		{
			return result;
		}

		for(vector<uint32_t>::iterator iterIndex = m_vecTriangleIndices.begin(); iterIndex != m_vecTriangleIndices.end();)
		{
			SurfaceVertex& v0 = m_vecVertices[*iterIndex];
			iterIndex++;
			SurfaceVertex& v1 = m_vecVertices[*iterIndex];
			iterIndex++;
			SurfaceVertex& v2 = m_vecVertices[*iterIndex];
			iterIndex++;

			if(
				(setMaterials.find(v0.getMaterial()) != setMaterials.end()) || 
				(setMaterials.find(v1.getMaterial()) != setMaterials.end()) || 
				(setMaterials.find(v2.getMaterial()) != setMaterials.end()))
			{
				uint32_t i0 = result->addVertex(v0);
				uint32_t i1 = result->addVertex(v1);
				uint32_t i2 = result->addVertex(v2);

				result->addTriangle(i0,i1,i2);
			}
		}

		return result;
	}
}

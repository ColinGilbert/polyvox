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

using namespace boost;

namespace PolyVox
{
	int32_t IndexedSurfacePatch::noOfVerticesSubmitted = 0;
	int32_t IndexedSurfacePatch::noOfVerticesAccepted = 0;
	int32_t IndexedSurfacePatch::noOfTrianglesSubmitted = 0;
	int32_t IndexedSurfacePatch::vertexIndicesX[POLYVOX_REGION_SIDE_LENGTH+1][POLYVOX_REGION_SIDE_LENGTH+1][POLYVOX_REGION_SIDE_LENGTH+1];
	int32_t IndexedSurfacePatch::vertexIndicesY[POLYVOX_REGION_SIDE_LENGTH+1][POLYVOX_REGION_SIDE_LENGTH+1][POLYVOX_REGION_SIDE_LENGTH+1];
	int32_t IndexedSurfacePatch::vertexIndicesZ[POLYVOX_REGION_SIDE_LENGTH+1][POLYVOX_REGION_SIDE_LENGTH+1][POLYVOX_REGION_SIDE_LENGTH+1];

	IndexedSurfacePatch::IndexedSurfacePatch(bool allowDuplicateVertices)
		:m_AllowDuplicateVertices(allowDuplicateVertices)
	{
		memset(vertexIndicesX,0xFF,sizeof(vertexIndicesX)); //0xFF is -1 as two's complement - this may not be portable...
		memset(vertexIndicesY,0xFF,sizeof(vertexIndicesY));
		memset(vertexIndicesZ,0xFF,sizeof(vertexIndicesZ));
	}

	IndexedSurfacePatch::~IndexedSurfacePatch()	  
	{
	}

	void IndexedSurfacePatch::addTriangle(const SurfaceVertex& v0,const SurfaceVertex& v1,const SurfaceVertex& v2)
	{
		noOfTrianglesSubmitted++;
		noOfVerticesSubmitted += 3;
		if(!m_AllowDuplicateVertices)
		{
			int32_t index = getIndexFor(v0.getPosition());
			if(index == -1)
			{
				m_vecVertices.push_back(v0);
				m_vecTriangleIndices.push_back(m_vecVertices.size()-1);
				setIndexFor(v0.getPosition(), m_vecVertices.size()-1);

				noOfVerticesAccepted++;
			}
			else
			{
				m_vecTriangleIndices.push_back(index);
			}

			index = getIndexFor(v1.getPosition());
			if(index == -1)
			{
				m_vecVertices.push_back(v1);
				m_vecTriangleIndices.push_back(m_vecVertices.size()-1);
				setIndexFor(v1.getPosition(), m_vecVertices.size()-1);

				noOfVerticesAccepted++;
			}
			else
			{
				m_vecTriangleIndices.push_back(index);
			}

			index = getIndexFor(v2.getPosition());
			if(index == -1)
			{
				m_vecVertices.push_back(v2);
				m_vecTriangleIndices.push_back(m_vecVertices.size()-1);
				setIndexFor(v2.getPosition(), m_vecVertices.size()-1);

				noOfVerticesAccepted++;
			}
			else
			{
				m_vecTriangleIndices.push_back(index);
			}
		}
		else
		{		
			m_vecVertices.push_back(v0);
			m_vecTriangleIndices.push_back(m_vecVertices.size()-1);
			m_vecVertices.push_back(v1);
			m_vecTriangleIndices.push_back(m_vecVertices.size()-1);
			m_vecVertices.push_back(v2);
			m_vecTriangleIndices.push_back(m_vecVertices.size()-1);

			noOfVerticesAccepted += 3;
		}
	}

	void IndexedSurfacePatch::fillVertexAndIndexData(std::vector<SurfaceVertex>& vecVertices, std::vector<uint32_t>& vecIndices)
	{
		vecVertices.resize(m_vecVertices.size());
		std::copy(m_vecVertices.begin(), m_vecVertices.end(), vecVertices.begin());

		vecIndices.resize(m_vecTriangleIndices.size());
		std::copy(m_vecTriangleIndices.begin(), m_vecTriangleIndices.end(), vecIndices.begin());

		/*for(std::vector<SurfaceVertexIterator>::iterator iterVertices = m_vecTriangleIndices.begin(); iterVertices != m_vecTriangleIndices.end(); ++iterVertices)
		{		
			std::vector<SurfaceVertex>::iterator iterVertex = lower_bound(vecVertices.begin(), vecVertices.end(), **iterVertices);
			vecIndices.push_back(iterVertex - vecVertices.begin());
		}*/
	}

	boost::int32_t IndexedSurfacePatch::getIndexFor(const Vector3DFloat& pos)
	{
		assert(pos.x() >= 0.0f);
		assert(pos.y() >= 0.0f);
		assert(pos.z() >= 0.0f);
		assert(pos.x() <= POLYVOX_REGION_SIDE_LENGTH);
		assert(pos.y() <= POLYVOX_REGION_SIDE_LENGTH);
		assert(pos.z() <= POLYVOX_REGION_SIDE_LENGTH);

		float xIntPart;
		float xFracPart = std::modf(pos.x(), &xIntPart);
		float yIntPart;
		float yFracPart = std::modf(pos.y(), &yIntPart);
		float zIntPart;
		float zFracPart = std::modf(pos.z(), &zIntPart);

		//Of all the fractional parts, two should be zero and one should have a value.
		if(xFracPart > 0.000001f)
		{
			return vertexIndicesX[static_cast<uint16_t>(xIntPart)][static_cast<uint16_t>(yIntPart)][static_cast<uint16_t>(zIntPart)];
		}
		if(yFracPart > 0.000001f)
		{
			return vertexIndicesY[static_cast<uint16_t>(xIntPart)][static_cast<uint16_t>(yIntPart)][static_cast<uint16_t>(zIntPart)];
		}
		if(zFracPart > 0.000001f)
		{
			return vertexIndicesZ[static_cast<uint16_t>(xIntPart)][static_cast<uint16_t>(yIntPart)][static_cast<uint16_t>(zIntPart)];
		}
		while(true);
	}

	void IndexedSurfacePatch::setIndexFor(const Vector3DFloat& pos, boost::int32_t newIndex)
	{
		assert(pos.x() >= 0.0f);
		assert(pos.y() >= 0.0f);
		assert(pos.z() >= 0.0f);
		assert(pos.x() <= POLYVOX_REGION_SIDE_LENGTH);
		assert(pos.y() <= POLYVOX_REGION_SIDE_LENGTH);
		assert(pos.z() <= POLYVOX_REGION_SIDE_LENGTH);

		assert(newIndex < 10000);

		float xIntPart;
		float xFracPart = std::modf(pos.x(), &xIntPart);
		float yIntPart;
		float yFracPart = std::modf(pos.y(), &yIntPart);
		float zIntPart;
		float zFracPart = std::modf(pos.z(), &zIntPart);

		//Of all the fractional parts, two should be zero and one should have a value.
		if(xFracPart > 0.000001f)
		{
			vertexIndicesX[static_cast<uint16_t>(xIntPart)][static_cast<uint16_t>(yIntPart)][static_cast<uint16_t>(zIntPart)] = newIndex;
		}
		if(yFracPart > 0.000001f)
		{
			vertexIndicesY[static_cast<uint16_t>(xIntPart)][static_cast<uint16_t>(yIntPart)][static_cast<uint16_t>(zIntPart)] = newIndex;
		}
		if(zFracPart > 0.000001f)
		{
			vertexIndicesZ[static_cast<uint16_t>(xIntPart)][static_cast<uint16_t>(yIntPart)][static_cast<uint16_t>(zIntPart)] = newIndex;
		}
	}

	const std::vector<SurfaceVertex>& IndexedSurfacePatch::getVertices(void) const
	{
		return m_vecVertices;
	}

	std::vector<SurfaceVertex>& IndexedSurfacePatch::getVertices(void)
	{
		return m_vecVertices;
	}

	const std::vector<boost::uint32_t>& IndexedSurfacePatch::getIndices(void) const
	{
		return m_vecTriangleIndices;
	}
}

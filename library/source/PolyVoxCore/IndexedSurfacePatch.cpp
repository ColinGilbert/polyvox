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

#include "PolyVoxCore/IndexedSurfacePatch.h"

using namespace std;

namespace PolyVox
{
	IndexedSurfacePatch::IndexedSurfacePatch()
	{
	}

	IndexedSurfacePatch::~IndexedSurfacePatch()	  
	{
	}

	void IndexedSurfacePatch::addTriangle(const SurfaceVertex& v0,const SurfaceVertex& v1,const SurfaceVertex& v2)
	{		
		m_vecVertices.push_back(v0);
		m_vecTriangleIndices.push_back(m_vecVertices.size()-1);
		m_vecVertices.push_back(v1);
		m_vecTriangleIndices.push_back(m_vecVertices.size()-1);
		m_vecVertices.push_back(v2);
		m_vecTriangleIndices.push_back(m_vecVertices.size()-1);
	}

	void IndexedSurfacePatch::fillVertexAndIndexData(std::vector<SurfaceVertex>& vecVertices, std::vector<uint32>& vecIndices)
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

	const std::vector<SurfaceVertex>& IndexedSurfacePatch::getVertices(void) const
	{
		return m_vecVertices;
	}

	std::vector<SurfaceVertex>& IndexedSurfacePatch::getVertices(void)
	{
		return m_vecVertices;
	}

	const std::vector<uint32>& IndexedSurfacePatch::getIndices(void) const
	{
		return m_vecTriangleIndices;
	}

	unsigned short IndexedSurfacePatch::getNoNonUniformTrianges(void)
	{
		unsigned short result = 0;
		for(int i = 0; i < m_vecTriangleIndices.size() - 2; i += 3)
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

	unsigned short IndexedSurfacePatch::getNoUniformTrianges(void)
	{
		unsigned short result = 0;
		for(int i = 0; i < m_vecTriangleIndices.size() - 2; i += 3)
		{
			if((m_vecVertices[m_vecTriangleIndices[i]].getMaterial() == m_vecVertices[m_vecTriangleIndices[i+1]].getMaterial())
			&& (m_vecVertices[m_vecTriangleIndices[i]].getMaterial() == m_vecVertices[m_vecTriangleIndices[i+2]].getMaterial()))
			{
				result++;
			}
		}
		return result;
	}
}

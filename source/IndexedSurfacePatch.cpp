#include "IndexedSurfacePatch.h"

namespace Ogre
{
	long int IndexedSurfacePatch::noOfVerticesSubmitted = 0;
	long int IndexedSurfacePatch::noOfVerticesAccepted = 0;
	long int IndexedSurfacePatch::noOfTrianglesSubmitted = 0;

	IndexedSurfacePatch::IndexedSurfacePatch(bool allowDuplicateVertices)
		:m_AllowDuplicateVertices(allowDuplicateVertices)
	{
		memset(vertexIndices,0xFF,sizeof(vertexIndices)); //0xFF is -1 as two's complement - this may not be portable...
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
			long int index = vertexIndices[long int(v0.getPosition().x +0.5)][long int(v0.getPosition().y +0.5)][long int(v0.getPosition().z +0.5)];
			if(index == -1)
			{
				m_vecVertices.push_back(v0);
				m_vecTriangleIndices.push_back(m_vecVertices.size()-1);
				vertexIndices[long int(v0.getPosition().x +0.5)][long int(v0.getPosition().y +0.5)][long int(v0.getPosition().z +0.5)] = m_vecVertices.size()-1;

				noOfVerticesAccepted++;
			}
			else
			{
				m_vecTriangleIndices.push_back(index);
			}

			index = vertexIndices[long int(v1.getPosition().x +0.5)][long int(v1.getPosition().y +0.5)][long int(v1.getPosition().z +0.5)];
			if(index == -1)
			{
				m_vecVertices.push_back(v1);
				m_vecTriangleIndices.push_back(m_vecVertices.size()-1);
				vertexIndices[long int(v1.getPosition().x +0.5)][long int(v1.getPosition().y +0.5)][long int(v1.getPosition().z +0.5)] = m_vecVertices.size()-1;

				noOfVerticesAccepted++;
			}
			else
			{
				m_vecTriangleIndices.push_back(index);
			}

			index = vertexIndices[long int(v2.getPosition().x +0.5)][long int(v2.getPosition().y +0.5)][long int(v2.getPosition().z +0.5)];
			if(index == -1)
			{
				m_vecVertices.push_back(v2);
				m_vecTriangleIndices.push_back(m_vecVertices.size()-1);
				vertexIndices[long int(v2.getPosition().x +0.5)][long int(v2.getPosition().y +0.5)][long int(v2.getPosition().z +0.5)] = m_vecVertices.size()-1;

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

	void IndexedSurfacePatch::fillVertexAndIndexData(std::vector<SurfaceVertex>& vecVertices, std::vector<ushort>& vecIndices)
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
}
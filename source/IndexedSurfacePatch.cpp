#include "IndexedSurfacePatch.h"

using namespace boost;

namespace PolyVox
{
	long int IndexedSurfacePatch::noOfVerticesSubmitted = 0;
	long int IndexedSurfacePatch::noOfVerticesAccepted = 0;
	long int IndexedSurfacePatch::noOfTrianglesSubmitted = 0;
	long int IndexedSurfacePatch::vertexIndices[POLYVOX_REGION_SIDE_LENGTH*2+1][POLYVOX_REGION_SIDE_LENGTH*2+1][POLYVOX_REGION_SIDE_LENGTH*2+1];
	int32_t IndexedSurfacePatch::vertexIndicesX[POLYVOX_REGION_SIDE_LENGTH][POLYVOX_REGION_SIDE_LENGTH][POLYVOX_REGION_SIDE_LENGTH];
	int32_t IndexedSurfacePatch::vertexIndicesY[POLYVOX_REGION_SIDE_LENGTH][POLYVOX_REGION_SIDE_LENGTH][POLYVOX_REGION_SIDE_LENGTH];
	int32_t IndexedSurfacePatch::vertexIndicesZ[POLYVOX_REGION_SIDE_LENGTH][POLYVOX_REGION_SIDE_LENGTH][POLYVOX_REGION_SIDE_LENGTH];

	IndexedSurfacePatch::IndexedSurfacePatch(bool allowDuplicateVertices)
		:m_AllowDuplicateVertices(allowDuplicateVertices)
	{
		memset(vertexIndices,0xFF,sizeof(vertexIndices));
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
			long int index = vertexIndices[long int(v0.getPosition().x() * 2.0 +0.5)][long int(v0.getPosition().y() * 2.0 +0.5)][long int(v0.getPosition().z() * 2.0 +0.5)];
			//int32_t index = getIndexFor(v0.getPosition());
			if(index == -1)
			{
				m_vecVertices.push_back(v0);
				m_vecTriangleIndices.push_back(m_vecVertices.size()-1);
				vertexIndices[long int(v0.getPosition().x() * 2.0 +0.5)][long int(v0.getPosition().y() * 2.0 +0.5)][long int(v0.getPosition().z() * 2.0 +0.5)] = m_vecVertices.size()-1;
				//setIndexFor(v0.getPosition(), m_vecVertices.size()-1);

				noOfVerticesAccepted++;
			}
			else
			{
				m_vecTriangleIndices.push_back(index);
			}

			index = vertexIndices[long int(v1.getPosition().x() * 2.0 +0.5)][long int(v1.getPosition().y() * 2.0 +0.5)][long int(v1.getPosition().z() * 2.0 +0.5)];
			//index = getIndexFor(v1.getPosition());
			if(index == -1)
			{
				m_vecVertices.push_back(v1);
				m_vecTriangleIndices.push_back(m_vecVertices.size()-1);
				vertexIndices[long int(v1.getPosition().x() * 2.0 +0.5)][long int(v1.getPosition().y() * 2.0 +0.5)][long int(v1.getPosition().z() * 2.0 +0.5)] = m_vecVertices.size()-1;
				//setIndexFor(v1.getPosition(), m_vecVertices.size()-1);

				noOfVerticesAccepted++;
			}
			else
			{
				m_vecTriangleIndices.push_back(index);
			}

			index = vertexIndices[long int(v2.getPosition().x() * 2.0 +0.5)][long int(v2.getPosition().y() * 2.0 +0.5)][long int(v2.getPosition().z() * 2.0 +0.5)];
			//index = getIndexFor(v2.getPosition());
			if(index == -1)
			{
				m_vecVertices.push_back(v2);
				m_vecTriangleIndices.push_back(m_vecVertices.size()-1);
				vertexIndices[long int(v2.getPosition().x() * 2.0 +0.5)][long int(v2.getPosition().y() * 2.0 +0.5)][long int(v2.getPosition().z() * 2.0 +0.5)] = m_vecVertices.size()-1;
				//setIndexFor(v2.getPosition(), m_vecVertices.size()-1);

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

	void IndexedSurfacePatch::fillVertexAndIndexData(std::vector<SurfaceVertex>& vecVertices, std::vector<uint16_t>& vecIndices)
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

	long int IndexedSurfacePatch::getSizeInBytes(void)
	{
		long int size = sizeof(IndexedSurfacePatch);
		size += m_vecVertices.capacity() * sizeof(m_vecVertices[0]);
		size += m_vecTriangleIndices.capacity() * sizeof(m_vecTriangleIndices[0]);
		return size;
	}

	boost::int32_t IndexedSurfacePatch::getIndexFor(const Vector3DFloat& pos)
	{
		float xIntPart;
		float xFracPart = modf(pos.x(), &xIntPart);
		float yIntPart;
		float yFracPart = modf(pos.y(), &yIntPart);
		float zIntPart;
		float zFracPart = modf(pos.z(), &zIntPart);

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
		float xIntPart;
		float xFracPart = modf(pos.x(), &xIntPart);
		float yIntPart;
		float yFracPart = modf(pos.y(), &yIntPart);
		float zIntPart;
		float zFracPart = modf(pos.z(), &zIntPart);

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
}

#ifndef __SurfacePatch_H__
#define __SurfacePatch_H__

#include <list>
#include <vector>

#include "SurfaceVertex.h"
#include "SurfaceTriangle.h"

#include "VolumeIterator.h"


namespace Ogre
{
	enum NormalGenerationMethod
	{
		SIMPLE,
		CENTRAL_DIFFERENCE,
		SOBEL
	};

	class SurfacePatch
	{
	public:
	   SurfacePatch();
	   ~SurfacePatch();

	   void beginDefinition(void);
	   void endDefinition(void);

	   void addTriangle(const SurfaceVertex& v0,const SurfaceVertex& v1,const SurfaceVertex& v2);	   

	   void getVertexAndIndexData(std::vector<SurfaceVertex>& vertexData, std::vector<uint>& indexData);

	   void computeNormalsFromVolume(VolumeIterator volIter);

	   UIntVector3 m_v3dOffset;

	private:
		std::set<SurfaceVertex> m_setVertices;
		std::list<SurfaceTriangle> m_listTriangles;

		//std::vector<SurfaceVertex> m_vecVertexData;
		//std::vector<uint> m_vecIndexData;

		uint m_uTrianglesAdded;
		uint m_uVerticesAdded;

		long int* vertexIndices;

		//UIntVector3 m_v3dOffset;
	};	
}

#endif /* __SurfacePatch_H__ */

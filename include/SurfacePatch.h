#ifndef __SurfacePatch_H__
#define __SurfacePatch_H__

#include <list>
#include <vector>

#include "IntegralVector3.h"

#include "VolumeIterator.h"


namespace Ogre
{
	enum NormalGenerationMethod
	{
		SIMPLE,
		CENTRAL_DIFFERENCE,
		SOBEL
	};

	class SurfaceVertex;
	typedef std::set<SurfaceVertex>::iterator SurfaceVertexIterator;
	class SurfaceTriangle;
	typedef std::set<SurfaceTriangle>::iterator SurfaceTriangleIterator;

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

	   void decimate(void);

	   //bool verticesArePlanar(SurfaceVertexIterator iterCurrentVertex);

	   UIntVector3 m_v3dOffset;

	private:
		std::set<SurfaceVertex> m_setVertices;
		std::set<SurfaceTriangle> m_setTriangles;

		//std::vector<SurfaceVertex> m_vecVertexData;
		//std::vector<uint> m_vecIndexData;

		uint m_uTrianglesAdded;
		uint m_uVerticesAdded;

		long int* vertexIndices;

		//UIntVector3 m_v3dOffset;
	};	
}

#endif /* __SurfacePatch_H__ */

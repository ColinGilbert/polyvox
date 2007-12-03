#ifndef __IndexedSurfacePatch_H__
#define __IndexedSurfacePatch_H__

#include <set>
#include <list>

#include "Constants.h"
#include "IntegralVector3.h"
#include "SurfaceTypes.h"
#include "SurfaceVertex.h"
#include "VolumeIterator.h"


namespace Ogre
{
	class IndexedSurfacePatch
	{
	public:
	   IndexedSurfacePatch();
	   ~IndexedSurfacePatch();	   

	   void addTriangle(const SurfaceVertex& v0,const SurfaceVertex& v1,const SurfaceVertex& v2);
	   void fillVertexAndIndexData(std::vector<SurfaceVertex>& vecVertices, std::vector<ushort>& vecIndices);
	
	//private:		
		std::vector<uint> m_vecTriangleIndices;
		std::vector<SurfaceVertex> m_vecVertices;

		long int vertexIndices[OGRE_REGION_SIDE_LENGTH*2+1][OGRE_REGION_SIDE_LENGTH*2+1][OGRE_REGION_SIDE_LENGTH*2+1];
	};	
}

#endif /* __IndexedSurfacePatch_H__ */

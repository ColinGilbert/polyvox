#ifndef __IndexedSurfacePatch_H__
#define __IndexedSurfacePatch_H__

#include "boost/cstdint.hpp"

//#include <set>
//#include <list>

#include <vector>

#include "Constants.h"
#include "IntegralVector3.h"
#include "SurfaceTypes.h"
#include "SurfaceVertex.h"
#include "TypeDef.h"
#include "VolumeIterator.h"


namespace PolyVox
{
	class VOXEL_SCENE_MANAGER_API IndexedSurfacePatch
	{
	public:
	   IndexedSurfacePatch(bool allowDuplicateVertices);
	   ~IndexedSurfacePatch();	   

	   void addTriangle(const SurfaceVertex& v0,const SurfaceVertex& v1,const SurfaceVertex& v2);
	   void fillVertexAndIndexData(std::vector<SurfaceVertex>& vecVertices, std::vector<boost::uint16_t>& vecIndices);
	
	//private:		
		std::vector<boost::uint16_t> m_vecTriangleIndices;
		std::vector<SurfaceVertex> m_vecVertices;

		long int vertexIndices[OGRE_REGION_SIDE_LENGTH*2+1][OGRE_REGION_SIDE_LENGTH*2+1][OGRE_REGION_SIDE_LENGTH*2+1];

		static long int noOfVerticesSubmitted;
		static long int noOfVerticesAccepted;
		static long int noOfTrianglesSubmitted;

	private:
		bool m_AllowDuplicateVertices;
	};	
}

#endif /* __IndexedSurfacePatch_H__ */

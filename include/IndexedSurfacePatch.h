#ifndef __IndexedSurfacePatch_H__
#define __IndexedSurfacePatch_H__

#include <set>
#include <list>

#include "AbstractSurfacePatch.h"
#include "IntegralVector3.h"
#include "SurfaceTypes.h"
#include "VolumeIterator.h"


namespace Ogre
{
	class IndexedSurfacePatch : public AbstractSurfacePatch
	{
	public:
	   IndexedSurfacePatch();
	   ~IndexedSurfacePatch();	   

	   void addTriangle(const SurfaceVertex& v0,const SurfaceVertex& v1,const SurfaceVertex& v2);
	   void fillVertexAndIndexData(std::vector<SurfaceVertex>& vecVertices, std::vector<ushort>& vecIndices);
	
	private:		
		std::vector<SurfaceVertexIterator> m_vecTriangleIndices;
	};	
}

#endif /* __IndexedSurfacePatch_H__ */

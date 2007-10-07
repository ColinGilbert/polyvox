#ifndef __AbstractSurfacePatch_H__
#define __AbstractSurfacePatch_H__

#include "SurfaceTypes.h"
#include "Surfacevertex.h"

namespace Ogre
{
	class AbstractSurfacePatch
	{
	public:
		AbstractSurfacePatch();
		virtual ~AbstractSurfacePatch();

		SurfaceVertexIterator getVerticesBegin(void);
		SurfaceVertexIterator getVerticesEnd(void);
		
		virtual void addTriangle(const SurfaceVertex& v0,const SurfaceVertex& v1,const SurfaceVertex& v2) = 0;
		virtual void fillVertexAndIndexData(std::vector<SurfaceVertex>& vecVertices, std::vector<ushort>& vecIndices) = 0;

	protected:
		std::set<SurfaceVertex> m_listVertices;
	};	
}

#endif /* __AbstractSurfacePatch_H__ */

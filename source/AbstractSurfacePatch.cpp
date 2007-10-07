#include "AbstractSurfacePatch.h"

namespace Ogre
{
	AbstractSurfacePatch::AbstractSurfacePatch()
	{
	}
	
	AbstractSurfacePatch::~AbstractSurfacePatch()
	{
	}

	SurfaceVertexIterator AbstractSurfacePatch::getVerticesBegin(void)
	{
		return m_listVertices.begin();
	}

	SurfaceVertexIterator AbstractSurfacePatch::getVerticesEnd(void)
	{
		return m_listVertices.end();
	}
}
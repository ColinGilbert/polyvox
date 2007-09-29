#ifndef __SurfaceTypes_H__
#define __SurfaceTypes_H__

namespace Ogre
{	
	class SurfaceVertex;
	typedef std::set<SurfaceVertex>::iterator SurfaceVertexIterator;
	typedef std::set<SurfaceVertex>::const_iterator SurfaceVertexConstIterator;
	class SurfaceTriangle;
	typedef std::set<SurfaceTriangle>::iterator SurfaceTriangleIterator;
	typedef std::set<SurfaceTriangle>::const_iterator SurfaceTriangleConstIterator;
	class SurfaceEdge;
	typedef std::set<SurfaceEdge>::iterator SurfaceEdgeIterator;
	typedef std::set<SurfaceEdge>::const_iterator SurfaceEdgeConstIterator;
}

#endif
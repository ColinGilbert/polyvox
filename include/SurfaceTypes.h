#ifndef __SurfaceTypes_H__
#define __SurfaceTypes_H__

namespace Ogre
{	
	class SurfaceVertex;
	typedef std::set<SurfaceVertex>::iterator SurfaceVertexIterator;
	class SurfaceTriangle;
	typedef std::set<SurfaceTriangle>::iterator SurfaceTriangleIterator;
	class SurfaceEdge;
	typedef std::set<SurfaceEdge>::iterator SurfaceEdgeIterator;
}

#endif
#ifndef __PolyVox_ForwardDeclarations_H__
#define __PolyVox_ForwardDeclarations_H__

#include "boost/cstdint.hpp"

namespace PolyVox
{
	template <typename VoxelType> class Block;
	class IndexedSurfacePatch;
	class IntegrealVector3;
	class PolyVoxSceneManager;
	class RegionGeometry;
	//class SurfaceEdge;
	//class SurfaceTriange;
	//class SurfaceTypes;
	class SurfaceVertex;
	template <boost::uint32_t Size, typename Type> class Vector;
	typedef Vector<2,float> Vector2DFloat;
    typedef Vector<2,double> Vector2DDouble;
	typedef Vector<2,boost::int32_t> Vector2DInt32;
	typedef Vector<2,boost::uint32_t> Vector2DUint32;
	typedef Vector<3,float> Vector3DFloat;
    typedef Vector<3,double> Vector3DDouble;
	typedef Vector<3,boost::int32_t> Vector3DInt32;
	typedef Vector<3,boost::uint32_t> Vector3DUint32;
	/*class Vector2DFloat;
	class Vector2DDouble;
	class Vector2DInt32;
	class Vector2DUint32;
	class Vector3DFloat;
	class Vector3DDouble;
	class Vector3DInt32;
	class Vector3DUint32;*/
	class Volume;
	class VolumeIterator;
}

#endif

#ifndef __PolyVox_ForwardDeclarations_H__
#define __PolyVox_ForwardDeclarations_H__

#include "boost/cstdint.hpp"

namespace PolyVox
{
	template <typename VoxelType> class Block;
	class IndexedSurfacePatch;
	class IntegrealVector3;
	template <typename VoxelType> class LinearVolume;
	class PolyVoxSceneManager;
	class RegionGeometry;
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
	template <typename VoxelType> class Volume;
	//Some handy typedefs
	typedef Volume<float> FloatVolume;
	typedef Volume<boost::uint8_t> UInt8Volume;
	typedef Volume<boost::uint16_t> UInt16Volume;	
	template <typename VoxelType> class VolumeIterator;
}

#endif

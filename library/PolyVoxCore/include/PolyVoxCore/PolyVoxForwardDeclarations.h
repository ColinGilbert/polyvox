/*******************************************************************************
Copyright (c) 2005-2009 David Williams

This software is provided 'as-is', without any express or implied
warranty. In no event will the authors be held liable for any damages
arising from the use of this software.

Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute it
freely, subject to the following restrictions:

    1. The origin of this software must not be misrepresented; you must not
    claim that you wrote the original software. If you use this software
    in a product, an acknowledgment in the product documentation would be
    appreciated but is not required.

    2. Altered source versions must be plainly marked as such, and must not be
    misrepresented as being the original software.

    3. This notice may not be removed or altered from any source
    distribution. 	
*******************************************************************************/

#ifndef __PolyVox_ForwardDeclarations_H__
#define __PolyVox_ForwardDeclarations_H__

#include "Impl/TypeDef.h"

#include <cstdint>

namespace PolyVox
{
	////////////////////////////////////////////////////////////////////////////////
	// Array
	////////////////////////////////////////////////////////////////////////////////
	/*template<uint32_t dimensions, typename ElementType> class Array;

	typedef Array<1,float> Array1DFloat;
	typedef Array<1,double> Array1DDouble;
	typedef Array<1,int8_t> Array1DInt8;
	typedef Array<1,uint8_t> Array1DUint8;
	typedef Array<1,int16_t> Array1DInt16;
	typedef Array<1,uint16_t> Array1DUint16;
	typedef Array<1,int32_t> Array1DInt32;
	typedef Array<1,uint32_t> Array1DUint32;*/

	/*typedef Array<2,float> Array2DFloat;
	typedef Array<2,double> Array2DDouble;
	typedef Array<2,int8_t> Array2DInt8;
	typedef Array<2,uint8_t> Array2DUint8;
	typedef Array<2,int16_t> Array2DInt16;
	typedef Array<2,uint16_t> Array2DUint16;
	typedef Array<2,int32_t> Array2DInt32;
	typedef Array<2,uint32_t> Array2DUint32;*/

	/*typedef Array<3,float> Array3DFloat;
	typedef Array<3,double> Array3DDouble;
	typedef Array<3,int8_t> Array3DInt8;
	typedef Array<3,uint8_t> Array3DUint8;
	typedef Array<3,int16_t> Array3DInt16;
	typedef Array<3,uint16_t> Array3DUint16;
	typedef Array<3,int32_t> Array3DInt32;
	typedef Array<3,uint32_t> Array3DUint32;*/

	////////////////////////////////////////////////////////////////////////////////
	// BlockCompressor
	////////////////////////////////////////////////////////////////////////////////
	template <typename VoxelType> class BlockCompressor;

	////////////////////////////////////////////////////////////////////////////////
	// Compressor
	////////////////////////////////////////////////////////////////////////////////
	class Compressor;

	////////////////////////////////////////////////////////////////////////////////
	// CubicSurfaceExtractor
	////////////////////////////////////////////////////////////////////////////////
	template<typename VoxelType> class DefaultIsQuadNeeded;
	template<typename VolumeType, typename MeshType, typename IsQuadNeeded = DefaultIsQuadNeeded<typename VolumeType::VoxelType> > class CubicSurfaceExtractor;

	////////////////////////////////////////////////////////////////////////////////
	// CubicVertex
	////////////////////////////////////////////////////////////////////////////////
	template<typename VoxelType> struct CubicVertex;

	////////////////////////////////////////////////////////////////////////////////
	// Density
	////////////////////////////////////////////////////////////////////////////////
	template <typename Type> class Density;

	typedef Density<int8_t> DensityI8;
	typedef Density<uint8_t> DensityU8;
	typedef Density<int16_t> DensityI16;
	typedef Density<uint16_t> DensityU16;
	typedef Density<float> DensityFloat;
	typedef Density<double> DensityDouble;

	typedef DensityU8 Density8; //Backwards compatibility
	typedef DensityU16 Density16; //Backwards compatibility

	////////////////////////////////////////////////////////////////////////////////
	// FilePager
	////////////////////////////////////////////////////////////////////////////////
	template <typename VoxelType> class FilePager;

	////////////////////////////////////////////////////////////////////////////////
	// LargeVolume
	////////////////////////////////////////////////////////////////////////////////
	template <typename VoxelType> class LargeVolume;

	////////////////////////////////////////////////////////////////////////////////
	// MarchingCubesSurfaceExtractor
	////////////////////////////////////////////////////////////////////////////////
	template<typename VolumeType, typename MeshType, typename ControllerType> class MarchingCubesSurfaceExtractor;

	////////////////////////////////////////////////////////////////////////////////
	// MarchingCubesVertex
	////////////////////////////////////////////////////////////////////////////////
	template<typename VoxelType> struct MarchingCubesVertex;

	////////////////////////////////////////////////////////////////////////////////
	// Material
	////////////////////////////////////////////////////////////////////////////////
	template <typename Type> class Material;

	typedef Material<uint8_t> MaterialU8;
	typedef Material<uint16_t> MaterialU16;
	typedef Material<uint32_t> MaterialU32;

	typedef MaterialU8 Material8;
	typedef MaterialU16 Material16;
	typedef MaterialU32 Material32;

	////////////////////////////////////////////////////////////////////////////////
	// MaterialDensityPair
	////////////////////////////////////////////////////////////////////////////////
	template <typename Type, uint8_t NoOfMaterialBits, uint8_t NoOfDensityBits> class MaterialDensityPair;

	typedef MaterialDensityPair<uint8_t, 4, 4> MaterialDensityPair44;
	typedef MaterialDensityPair<uint16_t, 8, 8> MaterialDensityPair88;

	////////////////////////////////////////////////////////////////////////////////
	// Mesh
	////////////////////////////////////////////////////////////////////////////////
	typedef uint32_t DefaultIndexType;
	template <typename VertexType, typename IndexType = DefaultIndexType> class Mesh;

	////////////////////////////////////////////////////////////////////////////////
	// Pager
	////////////////////////////////////////////////////////////////////////////////
	template <typename VoxelType> class Pager;

	////////////////////////////////////////////////////////////////////////////////
	// RawVolume
	////////////////////////////////////////////////////////////////////////////////
	template <typename VoxelType> class RawVolume;

	////////////////////////////////////////////////////////////////////////////////
	// Region
	////////////////////////////////////////////////////////////////////////////////
	class Region;

	////////////////////////////////////////////////////////////////////////////////
	// SimpleVolume
	////////////////////////////////////////////////////////////////////////////////
	template <typename VoxelType> class SimpleVolume;

	////////////////////////////////////////////////////////////////////////////////
	// Vector
	////////////////////////////////////////////////////////////////////////////////
	template <uint32_t Size, typename StorageType, typename OperationType = StorageType> class Vector;

	typedef Vector<2,float,float> Vector2DFloat;
    typedef Vector<2,double,double> Vector2DDouble;
	typedef Vector<2,int8_t,int32_t> Vector2DInt8;
	typedef Vector<2,uint8_t,int32_t> Vector2DUint8;
	typedef Vector<2,int16_t,int32_t> Vector2DInt16;
	typedef Vector<2,uint16_t,int32_t> Vector2DUint16;
	typedef Vector<2,int32_t,int32_t> Vector2DInt32;
	typedef Vector<2,uint32_t,int32_t> Vector2DUint32;

	typedef Vector<3,float,float> Vector3DFloat;
    typedef Vector<3,double,double> Vector3DDouble;
	typedef Vector<3,int8_t,int32_t> Vector3DInt8;
	typedef Vector<3,uint8_t,int32_t> Vector3DUint8;
	typedef Vector<3,int16_t,int32_t> Vector3DInt16;
	typedef Vector<3,uint16_t,int32_t> Vector3DUint16;
	typedef Vector<3,int32_t,int32_t> Vector3DInt32;
	typedef Vector<3,uint32_t,int32_t> Vector3DUint32;

	typedef Vector<4,float,float> Vector4DFloat;
    typedef Vector<4,double,double> Vector4DDouble;
	typedef Vector<4,int8_t,int32_t> Vector4DInt8;
	typedef Vector<4,uint8_t,int32_t> Vector4DUint8;
	typedef Vector<4,int16_t,int32_t> Vector4DInt16;
	typedef Vector<4,uint16_t,int32_t> Vector4DUint16;
	typedef Vector<4,int32_t,int32_t> Vector4DInt32;
	typedef Vector<4,uint32_t,int32_t> Vector4DUint32;
}

#endif

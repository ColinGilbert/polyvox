#pragma region License
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
#pragma endregion

#ifndef __PolyVox_ForwardDeclarations_H__
#define __PolyVox_ForwardDeclarations_H__

#include "PolyVoxImpl/CPlusPlusZeroXSupport.h"

namespace PolyVox
{
	template <typename VoxelType> class Block;

	//---------- Volume ----------
	template <typename VoxelType> class Volume;
	typedef Volume<float> FloatVolume;
	typedef Volume<uint8_t> UInt8Volume;
	typedef Volume<uint16_t> UInt16Volume;
	//---------------------------------

	//---------- Mesh ----------
	class Mesh;
	class MeshEdge;
	class MeshFace;
	class MeshVertex;
	//---------------------------------

	class IndexedSurfacePatch;
	class Region;
	class SurfaceVertex;
	class SurfaceExtractor;

	//---------- Vector ----------
	template <uint32_t Size, typename Type> class Vector;
	typedef Vector<3,float> Vector3DFloat;
    typedef Vector<3,double> Vector3DDouble;
	typedef Vector<3,int8_t> Vector3DInt8;
	typedef Vector<3,uint8_t> Vector3DUint8;
	typedef Vector<3,int16_t> Vector3DInt16;
	typedef Vector<3,uint16_t> Vector3DUint16;
	typedef Vector<3,int32_t> Vector3DInt32;
	typedef Vector<3,uint32_t> Vector3DUint32;
	//----------------------------

	template <typename VoxelType> class VolumeSampler;
}

#endif

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

#ifndef __PolyVox_SurfaceVertex_H__
#define __PolyVox_SurfaceVertex_H__

#include "Impl/TypeDef.h"

#include "PolyVoxCore/Vector.h"

#include <bitset>
#include <vector>

namespace PolyVox
{
	#ifdef SWIG
	struct Vertex
#else
	template<typename _DataType>
	struct POLYVOX_API Vertex
#endif
	{
		typedef _DataType DataType;

		Vector3DFloat position;
		Vector3DFloat normal;
		DataType data;
	};

#ifdef SWIG
	struct CubicVertex
#else
	template<typename _DataType>
	struct POLYVOX_API CubicVertex
#endif
	{
		typedef _DataType DataType;

		Vector3DUint8 position;
		uint8_t normal;
		DataType data;
	};

#ifdef SWIG
	struct MarchingCubesVertex
#else
	template<typename _DataType>
	struct POLYVOX_API MarchingCubesVertex
#endif
	{
		typedef _DataType DataType;

		Vector3DUint16 position;
		Vector3DFloat normal;
		DataType data;
	};

	// Hopefully the compiler will implement the 'Return value optimization' here, but
	// performance critical code will most likely decode the vertices in a shader anyway.
	template<typename DataType>
	Vertex<DataType> decode(const CubicVertex<DataType>& cubicVertex)
	{
		Vertex<DataType> result;
		Vector3DUint8 temp = cubicVertex.position; // For some reason we can't cast Vector3DUint8 to Vector3DFloat - investigate why.
		result.position = Vector3DFloat(temp.getX(), temp.getY(), temp.getZ()) - Vector3DFloat(0.5, 0.5, 0.5);
		//result.normal = cubicVertex.normal;
		result.data = cubicVertex.data;
		return result;
	}

	// Hopefully the compiler will implement the 'Return value optimization' here, but
	// performance critical code will most likely decode the vertices in a shader anyway.
	template<typename DataType>
	Vertex<DataType> decode(const MarchingCubesVertex<DataType>& cubicVertex)
	{
		Vertex<DataType> result;
		result.position = Vector3DFloat(cubicVertex.position.getX(), cubicVertex.position.getY(), cubicVertex.position.getZ());
		result.position *= (1.0 / 256.0);
		result.normal = cubicVertex.normal;
		result.data = cubicVertex.data;
		return result;
	}
}

#endif

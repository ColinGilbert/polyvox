#pragma region License
/******************************************************************************
This file is part of the PolyVox library
Copyright (C) 2006  David Williams

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
******************************************************************************/
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

	class IndexedSurfacePatch;
	class Region;
	class SurfaceVertex;

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

	template <typename VoxelType> class VolumeIterator;
}

#endif

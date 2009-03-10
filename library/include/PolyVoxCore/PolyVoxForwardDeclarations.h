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

#include "Enums.h"
#include "PolyVoxCStdInt.h"

namespace PolyVox
{
	template <typename VoxelType> class Block;

	//---------- BlockVolume ----------
	template <typename VoxelType> class BlockVolume;
	typedef BlockVolume<float> FloatBlockVolume;
	typedef BlockVolume<uint8> UInt8BlockVolume;
	typedef BlockVolume<uint16> UInt16BlockVolume;
	//---------------------------------

	class IndexedSurfacePatch;
	class IntegrealVector3;
	class Region;
	class SurfaceVertex;

	//---------- Vector ----------
	template <uint32 Size, typename Type> class Vector;
	typedef Vector<3,float> Vector3DFloat;
    typedef Vector<3,double> Vector3DDouble;
	typedef Vector<3,int8> Vector3DInt8;
	typedef Vector<3,uint8> Vector3DUint8;
	typedef Vector<3,int16> Vector3DInt16;
	typedef Vector<3,uint16> Vector3DUint16;
	typedef Vector<3,int32> Vector3DInt32;
	typedef Vector<3,uint32> Vector3DUint32;
	//----------------------------

	template <typename VoxelType> class BlockVolumeIterator;
}

#endif

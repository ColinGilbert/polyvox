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

#include "boost/cstdint.hpp"

namespace PolyVox
{
	template <typename VoxelType> class Block;
	template <typename VoxelType> class BlockVolume;
	//Some handy typedefs
	typedef BlockVolume<float> FloatBlockVolume;
	typedef BlockVolume<boost::uint8_t> UInt8BlockVolume;
	typedef BlockVolume<boost::uint16_t> UInt16BlockVolume;
	class IndexedSurfacePatch;
	class IntegrealVector3;
	template <typename VoxelType> class LinearVolume;
	class PolyVoxSceneManager;
	class RegionGeometry;
	class SurfaceVertex;
	template <boost::uint32_t Size, typename Type> class Vector;
	typedef Vector<3,float> Vector3DFloat;
    typedef Vector<3,double> Vector3DDouble;
	typedef Vector<3,boost::int8_t> Vector3DInt8;
	typedef Vector<3,boost::uint8_t> Vector3DUint8;
	typedef Vector<3,boost::int16_t> Vector3DInt16;
	typedef Vector<3,boost::uint16_t> Vector3DUint16;
	typedef Vector<3,boost::int32_t> Vector3DInt32;
	typedef Vector<3,boost::uint32_t> Vector3DUint32;		
	template <typename VoxelType> class VolumeIterator;
}

#endif

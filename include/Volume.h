/******************************************************************************
This file is part of a voxel plugin for OGRE
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
#ifndef __PolyVox_Volume_H__
#define __PolyVox_Volume_H__

#include "boost/cstdint.hpp"

#include "Constants.h"
#include "PolyVoxForwardDeclarations.h"
#include "TypeDef.h"
#include "Vector.h"

namespace PolyVox
{
	template <typename VoxelType>
	class Volume
	{
		//Make VolumeIterator a friend
		friend class VolumeIterator<VoxelType>;

		//Volume interface
	public:		
		Volume(boost::uint8_t uSideLengthPower, boost::uint8_t uBlockSideLengthPower);
		~Volume();

	private:
		Volume(const Volume& rhs);
		Volume& operator=(const Volume& rhs);

	public:
		bool containsPoint(Vector3DFloat pos, float boundary);
		bool containsPoint(Vector3DInt32 pos, boost::uint16_t boundary);

		void tidy(void);

	private:
		Block<VoxelType>* getBlock(boost::uint16_t index);
		Block<VoxelType>** mBlocks;
		boost::uint32_t m_uNoOfBlocksInVolume;

		boost::uint16_t m_uSideLengthPower;
		boost::uint16_t m_uSideLength;
	};

	//Some handy typedefs
	typedef Volume<float> FloatVolume;
	typedef Volume<boost::uint8_t> UInt8Volume;
	typedef Volume<boost::uint16_t> UInt16Volume;	
}

#include "Volume.inl"

#endif

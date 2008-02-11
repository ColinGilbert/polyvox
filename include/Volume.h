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
#ifndef __Volume_H__
#define __Volume_H__

#include "boost/cstdint.hpp"


#include "Block.h"
#include "Constants.h"
#include "TypeDef.h"
#include "Vector.h"

namespace PolyVox
{
	class POLYVOX_API Volume
	{
		//Make VolumeIterator a friend
		friend class VolumeIterator;

		//Volume interface
	public:		
		Volume();
		~Volume();

	private:
		Volume(const Volume& rhs);
		Volume& operator=(const Volume& rhs);

	public:
		Block* getBlock(boost::uint16_t index);

		bool containsPoint(Vector3DFloat pos, float boundary);
		bool containsPoint(Vector3DInt32 pos, boost::uint16_t boundary);

		bool loadFromFile(const std::string& sFilename);
		bool saveToFile(const std::string& sFilename);

		void regionGrow(boost::uint16_t xStart, boost::uint16_t yStart, boost::uint16_t zStart, boost::uint8_t value);
		void tidy(void);

	private:
		Block* mBlocks[POLYVOX_NO_OF_BLOCKS_IN_VOLUME];
	};
}

#endif

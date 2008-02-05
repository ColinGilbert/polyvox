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
#ifndef __Block_H__
#define __Block_H__

#include "OgrePrerequisites.h"

#include "Constants.h"

#include "TypeDef.h"

namespace Ogre
{

	class VOXEL_SCENE_MANAGER_API Block
	{
		//Make VolumeIterator a friend
		friend class VolumeIterator;

		//Block interface
	public:
		Block();
		Block(const Block& rhs);
		~Block();

		Block& operator=(const Block& rhs);

		bool isHomogeneous(void);

		uchar getVoxelAt(const uint xPosition, const uint yPosition, const uint zPosition) const;
		void setVoxelAt(const uint xPosition, const uint yPosition, const uint zPosition, const uchar value);

		void fillWithValue(const uchar value);

	private:
		uchar mData[OGRE_NO_OF_VOXELS_IN_BLOCK];		
	};
}

#endif

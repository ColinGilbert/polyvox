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

#ifndef __SurfaceTriangle_H__
#define __SurfaceTriangle_H__

#include "OgrePrerequisites.h"

#include "OgreVector3.h"

namespace Ogre
{
	class SurfaceTriangle
	{
	public:

		//FIXME - maybe these should be short?
		ulong v0;
		ulong v1;
		ulong v2;

		SurfaceTriangle()
			:v0(0)
			,v1(0)
			,v2(0)
		{
		}

		SurfaceTriangle(uint v0ToSet, uint v1ToSet, uint v2ToSet)
			:v0(v0ToSet)
			,v1(v1ToSet)
			,v2(v2ToSet)
		{
		}
	};
}

#endif

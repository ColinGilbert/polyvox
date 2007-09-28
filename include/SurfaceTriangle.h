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
#include "SurfaceTypes.h"

namespace Ogre
{
	class SurfaceTriangle
	{
	public:
		SurfaceTriangle();

		friend bool operator == (const SurfaceTriangle& lhs, const SurfaceTriangle& rhs);
		friend bool operator < (const SurfaceTriangle& lhs, const SurfaceTriangle& rhs);

		const SurfaceEdgeIterator& getEdge(void) const;

		void setEdge(const SurfaceEdgeIterator& edgeToSet);

		//std::string toString(void);

	private:
		SurfaceEdgeIterator edge;		
	};

	

	//bool operator < (const SurfaceTriangleIterator& lhs, const SurfaceTriangleIterator& rhs);
}

#endif

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

#ifndef __SurfaceEdge_H__
#define __SurfaceEdge_H__

#include "OgrePrerequisites.h"

namespace Ogre
{
	class SurfaceVertex;
	typedef std::list<SurfaceVertex>::iterator SurfaceVertexIterator;
	class SurfaceTriangle;
	typedef std::list<SurfaceTriangle>::iterator SurfaceTriangleIterator;
	class SurfaceEdge;
	typedef std::list<SurfaceEdge>::iterator SurfaceEdgeIterator;

	class SurfaceEdge
	{
	public:

		//int crud;

		SurfaceVertexIterator target;

		SurfaceTriangleIterator triangle;

		SurfaceEdgeIterator previousHalfEdge;

		SurfaceEdgeIterator nextHalfEdge;

		SurfaceEdgeIterator otherHalfEdge;

		//FIXME - could use boost::optional for this?

		std::string toString(void);

		bool isDegenerate(void);
	};

	bool operator == (const SurfaceEdge& lhs, const SurfaceEdge& rhs);
	//bool operator < (const SurfaceEdge& lhs, const SurfaceEdge& rhs);
}

#endif

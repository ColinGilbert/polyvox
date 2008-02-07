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

#include "SurfaceTypes.h"

namespace Ogre
{
	class SurfaceEdge
	{
	public:
		SurfaceEdge(const SurfaceVertexIterator& targetToSet,const SurfaceVertexIterator& sourceToSet);

		friend bool operator == (const SurfaceEdge& lhs, const SurfaceEdge& rhs);
		friend bool operator < (const SurfaceEdge& lhs, const SurfaceEdge& rhs);


		std::string toString(void);

		bool isDegenerate(void);

		const SurfaceVertexIterator& getTarget(void) const;
		const SurfaceVertexIterator& getSource(void) const;
		const SurfaceEdgeIterator& getOtherHalfEdge(void) const;
		const SurfaceEdgeIterator& getPreviousHalfEdge(void) const;
		const SurfaceEdgeIterator& getNextHalfEdge(void) const;
		const SurfaceTriangleIterator& getTriangle(void) const;

		void setPreviousHalfEdge(const SurfaceEdgeIterator& previousHalfEdgeToSet);
		void setNextHalfEdge(const SurfaceEdgeIterator& nextHalfEdgeToSet);
		void setTriangle(const SurfaceTriangleIterator& triangleToSet);

		void pairWithOtherHalfEdge(const SurfaceEdgeIterator& otherHalfEdgeToPair);

	private:
		SurfaceVertexIterator target;
		SurfaceVertexIterator source;

		SurfaceEdgeIterator previousHalfEdge;
		SurfaceEdgeIterator nextHalfEdge;
		SurfaceEdgeIterator otherHalfEdge;

		SurfaceTriangleIterator triangle;
	};	
}

#endif

/*******************************************************************************
Copyright (c) 2005-2009 David Williams

This software is provided 'as-is', without any express or implied
warranty. In no event will the authors be held liable for any damages
arising from the use of this software.

Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute it
freely, subject to the following restrictions:

    1. The origin of this software must not be misrepresented; you must not
    claim that you wrote the original software. If you use this software
    in a product, an acknowledgment in the product documentation would be
    appreciated but is not required.

    2. Altered source versions must be plainly marked as such, and must not be
    misrepresented as being the original software.

    3. This notice may not be removed or altered from any source
    distribution. 	
*******************************************************************************/

#ifndef __PolyVox_SurfaceEdge_H__
#define __PolyVox_SurfaceEdge_H__

#include "SurfaceTypes.h"

namespace PolyVox
{
	class SurfaceEdge
	{
	public:
		SurfaceEdge(const SurfaceVertexIterator& targetToSet,const SurfaceVertexIterator& sourceToSet);

		friend bool operator == (const SurfaceEdge& lhs, const SurfaceEdge& rhs);
		friend bool operator < (const SurfaceEdge& lhs, const SurfaceEdge& rhs);


		std::string tostring(void);

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

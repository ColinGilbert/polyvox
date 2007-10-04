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

#ifndef __SurfaceVertex_H__
#define __SurfaceVertex_H__

#include "OgrePrerequisites.h"

#include "OgreVector3.h"
#include "IntegralVector3.h"
#include "SurfaceTypes.h"

namespace Ogre
{	
	class SurfaceVertex
	{
	public:	
		SurfaceVertex();
		SurfaceVertex(UIntVector3 positionToSet);
		SurfaceVertex(UIntVector3 positionToSet, Vector3 normalToSet);	

		friend bool operator==(const SurfaceVertex& lhs, const SurfaceVertex& rhs);
		friend bool operator < (const SurfaceVertex& lhs, const SurfaceVertex& rhs);

		float getAlpha(void) const;
		const SurfaceEdgeIterator& getEdge(void) const;
		const Vector3& getNormal(void) const;
		const UIntVector3& getPosition(void) const;	

		void setAlpha(float alphaToSet);	
		void setEdge(const SurfaceEdgeIterator& edgeToSet) const;
		void setNormal(const Vector3& normalToSet) const;

		std::string toString(void) const;

	private:
		UIntVector3 position;
		mutable Vector3 normal;
		float alpha;

		mutable SurfaceEdgeIterator edge;		

		unsigned long m_uHash;
	};

	
	
	

	//bool operator < (const SurfaceVertexIterator& lhs, const SurfaceVertexIterator& rhs);
}

#endif

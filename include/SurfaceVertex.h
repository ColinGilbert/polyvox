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

namespace Ogre
{
	class SurfaceVertex
	{
	public:
		Vector3 position;
		Vector3 normal;
		float alpha;

		SurfaceVertex(Vector3 positionToSet)
			:position(positionToSet)
		{
		}

		SurfaceVertex(Vector3 positionToSet, Vector3 normalToSet)
			:position(positionToSet)
			,normal(normalToSet)
		{
		}

		bool matchesPosition(const Vector3& rhs) const
		{
			//return ((x == rhs.x) && (y == rhs.y) && (z == rhs.z));
			return ((position - rhs).length() < 0.01);
		}

		/*bool operator < (const Vertex& rhs) const
		{
			if(z < rhs.z)
			{
				return true;
			}
			else
			{
				if(y < rhs.y)
				{
					return true;
				}
				else
				{
					if(x < rhs.x)
					{
						return true;
					}
					else
					{
						return false;
					}
				}
			}
		}*/
	};
}

#endif

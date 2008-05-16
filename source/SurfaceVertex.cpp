#pragma region License
/******************************************************************************
This file is part of the PolyVox library
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
#pragma endregion

#include <sstream>

#include "Constants.h"
#include "SurfaceVertex.h"

namespace PolyVox
{
	SurfaceVertex::SurfaceVertex()
	{
	}

	SurfaceVertex::SurfaceVertex(Vector3DFloat positionToSet, float materialToSet, float alphaToSet)
		:material(materialToSet)
		,alpha(alphaToSet)
		,position(positionToSet)
	{
		
	}

	SurfaceVertex::SurfaceVertex(Vector3DFloat positionToSet, Vector3DFloat normalToSet)
		:position(positionToSet)
		,normal(normalToSet)
	{
	}

	float SurfaceVertex::getAlpha(void) const
	{
		return alpha;
	}	

	float SurfaceVertex::getMaterial(void) const
	{
		return material;
	}

	const Vector3DFloat& SurfaceVertex::getNormal(void) const
	{
		return normal;
	}

	const Vector3DFloat& SurfaceVertex::getPosition(void) const
	{
		return position;
	}

	void SurfaceVertex::setAlpha(float alphaToSet)
	{
		alpha = alphaToSet;
	}

	void SurfaceVertex::setMaterial(float materialToSet)
	{
		material = materialToSet;
	}

	void SurfaceVertex::setNormal(const Vector3DFloat& normalToSet)
	{
		normal = normalToSet;
		normal.normalise();
	}		

	std::string SurfaceVertex::tostring(void) const
	{
		std::stringstream ss;
		ss << "SurfaceVertex: Position = (" << position.x() << "," << position.y() << "," << position.z() << "), Normal = " << normal;
		return ss.str();
	}	
}

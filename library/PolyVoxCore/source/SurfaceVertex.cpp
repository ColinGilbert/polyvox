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

#include "SurfaceVertex.h"

namespace PolyVox
{
	SurfaceVertex::SurfaceVertex()
	{
	}

	SurfaceVertex::SurfaceVertex(Vector3DFloat positionToSet, float materialToSet)
		:position(positionToSet)
		,material(materialToSet)
	{
		
	}

	SurfaceVertex::SurfaceVertex(Vector3DFloat positionToSet, Vector3DFloat normalToSet, float materialToSet)
		:position(positionToSet)
		,normal(normalToSet)
		,material(materialToSet)
	{
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

	void SurfaceVertex::setMaterial(float materialToSet)
	{
		material = materialToSet;
	}

	void SurfaceVertex::setNormal(const Vector3DFloat& normalToSet)
	{
		normal = normalToSet;
	}	

	void SurfaceVertex::setPosition(const Vector3DFloat& positionToSet)
	{
		position = positionToSet;
	}
}

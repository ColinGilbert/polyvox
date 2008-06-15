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

#ifndef __PolyVox_SurfaceVertex_H__
#define __PolyVox_SurfaceVertex_H__

#include "TypeDef.h"
#include "Vector.h"

namespace PolyVox
{	
	class POLYVOX_API SurfaceVertex
	{
	public:	
		SurfaceVertex();
		SurfaceVertex(Vector3DFloat positionToSet, float materialToSet, float alphaToSet);
		SurfaceVertex(Vector3DFloat positionToSet, Vector3DFloat normalToSet, float materialToSet, float alphaToSet);	

		friend bool operator==(const SurfaceVertex& lhs, const SurfaceVertex& rhs);
		friend bool operator < (const SurfaceVertex& lhs, const SurfaceVertex& rhs);

		float getAlpha(void) const;
		float getMaterial(void) const;
		const Vector3DFloat& getNormal(void) const;
		const Vector3DFloat& getPosition(void) const;	

		void setAlpha(float alphaToSet);	
		void setMaterial(float materialToSet);
		void setNormal(const Vector3DFloat& normalToSet);
		void setPosition(const Vector3DFloat& positionToSet);

		std::string tostring(void) const;

	private:		
		Vector3DFloat position;
		Vector3DFloat normal;
		float material;
		float alpha;
			
	};

	
	

	//bool operator < (const SurfaceVertexIterator& lhs, const SurfaceVertexIterator& rhs);
}

#endif

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

#ifndef __PolyVox_SurfaceVertex_H__
#define __PolyVox_SurfaceVertex_H__

#include "Impl/TypeDef.h"

#include "PolyVoxCore/Vector.h"

#include <bitset>
#include <vector>

namespace PolyVox
{	
#ifdef SWIG
	class PositionMaterial
#else
	template<typename MaterialType>
	class POLYVOX_API PositionMaterial
#endif
	{
	public:	
		PositionMaterial()
		{
		}

		PositionMaterial(Vector3DFloat positionToSet, MaterialType materialToSet)
			:position(positionToSet)
			,material(materialToSet)
		{
		}

		MaterialType getMaterial(void) const
		{
			return material;
		}

		const Vector3DFloat& getPosition(void) const
		{
			return position;
		}

		void setMaterial(MaterialType materialToSet)
		{
			material = materialToSet;
		}

		void setPosition(const Vector3DFloat& positionToSet)
		{
			position = positionToSet;
		}
	public:		
		//Nicely fits into four floats.
		Vector3DFloat position;
		MaterialType material;
	};	

#ifdef SWIG
	class PositionMaterialNormal
#else
	template<typename MaterialType>
	class POLYVOX_API PositionMaterialNormal
#endif
	{
	public:	
		PositionMaterialNormal()
		{
		}

		PositionMaterialNormal(Vector3DFloat positionToSet, MaterialType materialToSet)
			:position(positionToSet)
			,material(materialToSet)
		{
		}

		PositionMaterialNormal(Vector3DFloat positionToSet, Vector3DFloat normalToSet, MaterialType materialToSet)
			:position(positionToSet)
			,normal(normalToSet)
			,material(materialToSet)
		{
		}

		MaterialType getMaterial(void) const
		{
			return material;
		}

		const Vector3DFloat& getNormal(void) const
		{
			return normal;
		}

		const Vector3DFloat& getPosition(void) const
		{
			return position;
		}

		void setMaterial(MaterialType materialToSet)
		{
			material = materialToSet;
		}

		void setNormal(const Vector3DFloat& normalToSet)
		{
			normal = normalToSet;
		}

		void setPosition(const Vector3DFloat& positionToSet)
		{
			position = positionToSet;
		}

	public:		
		//Nicely fits into seven floats, meaning we
		//can squeeze in one more for material blending.
		Vector3DFloat position;
		Vector3DFloat normal;
		MaterialType material; //FIXME: This shouldn't be float on CPU?
	};
}

#endif

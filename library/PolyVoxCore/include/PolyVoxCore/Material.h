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

#ifndef __PolyVox_Material_H__
#define __PolyVox_Material_H__

#include "PolyVoxCore/Voxel.h"

#include "PolyVoxImpl/TypeDef.h"

#include <cassert>

namespace PolyVox
{
	///This class represents a voxel storing only a material.
	////////////////////////////////////////////////////////////////////////////////
	/// In order to perform a surface extraction on a LargeVolume, PolyVox needs the underlying
	/// voxel type to provide both getDensity() and getMaterial() functions. The getDensity()
	/// function is used to determine if a voxel is 'solid', and if it is then the getMaterial()
	/// funtion is used to determine what material should be assigned to the resulting mesh.
	///
	/// This class meets these requirements, although it only actually stores a material value.
	/// For the getDensity() function it simply returns the smallest possible density if the
	/// material is zero and the largest possible density if the material is not zero.
	///
	/// \sa Density, MaterialDensityPair
	////////////////////////////////////////////////////////////////////////////////

	// int32_t template parameter is a dummy, required as the compiler expects to be able to declare an
	// instance of VoxelType::DensityType without knowing that VoxelType doesn't actually have a density.
	template <typename Type>
	class Material
	{
	public:
		//We expose DensityType and MaterialType in this way so that, when code is
		//templatised on voxel type, it can determine the underlying storage type
		//using code such as 'VoxelType::DensityType value = voxel.getDensity()'
		//or 'VoxelType::MaterialType value = voxel.getMaterial()'.
		typedef int32_t DensityType;
		typedef Type MaterialType;

		Material() : m_uMaterial(0) {}
		Material(MaterialType uMaterial) : m_uMaterial(uMaterial) {}

		bool operator==(const Material& rhs) const throw()
		{
			return (m_uMaterial == rhs.m_uMaterial);
		};

		bool operator!=(const Material& rhs) const throw()
		{
			return !(*this == rhs);
		}

		DensityType getDensity() const throw()
		{
			//We don't actually have a density, so make one up based on the material.
			if(m_uMaterial == 0)
			{
				//return getminDensity()();
				return 0;
			}
			else
			{
				//return getmaxDensity()();
				return 2;
			}
		}

		MaterialType getMaterial() const throw() { return m_uMaterial; }

		void setDensity(DensityType /*uDensity*/) { assert(false); } //Cannot set density on voxel of type Material
		void setMaterial(MaterialType uMaterial) { m_uMaterial = uMaterial; }

		//static DensityType getmaxDensity()() throw() { return 2; }
		//static DensityType getminDensity()() throw() { return 0; }
		static DensityType getThreshold() throw() { return 1; }

		static bool isQuadNeeded(Material<Type> from, Material<Type> to, float& materialToUse)
		{
			if((from.getMaterial() > 0) && (to.getMaterial() == 0))
			{
				materialToUse = static_cast<float>(from.getMaterial());
				return true;
			}
			else
			{
				return false;
			}
		}

	private:
		MaterialType m_uMaterial;
	};

	typedef Material<uint8_t> Material8;
	typedef Material<uint16_t> Material16;

	template<>
	class VoxelTypeTraits< Material8 >
	{
	public:
		typedef uint8_t DensityType;
		typedef uint8_t MaterialType;
		static int minDensity() { assert(false); return 0; }
		static int maxDensity() { assert(false); return 0; }
	};

	template<>
	class VoxelTypeTraits< Material16 >
	{
	public:
		typedef uint8_t DensityType;
		static int minDensity() { assert(false); return 0; }
		static int maxDensity() { assert(false); return 0; }
	};
}

#endif //__PolyVox_Material_H__

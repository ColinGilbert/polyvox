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

#ifndef __PolyVox_MaterialDensityPair_H__
#define __PolyVox_MaterialDensityPair_H__

#include "PolyVoxCore/Voxel.h"

#include "PolyVoxImpl/TypeDef.h"

namespace PolyVox
{
	///This class represents a voxel storing only a density.
	////////////////////////////////////////////////////////////////////////////////
	/// In order to perform a surface extraction on a LargeVolume, PolyVox needs the underlying
	/// voxel type to provide both getDensity() and getMaterial() functions. The getDensity()
	/// function is used to determine if a voxel is 'solid', and if it is then the getMaterial()
	/// funtion is used to determine what material should be assigned to the resulting mesh.
	///
	/// This class meets these requirements, and does so by storing and returning both a material
	/// and a density value. Via the template parameters it is possible to control how much
	/// precision is given to each. For example, if you create a class with 8 bits of storage,
	/// you might choose to allocate 6 bits for the density and 2 bits for the material.
	///
	/// \sa Density, Material
	////////////////////////////////////////////////////////////////////////////////
	template <typename Type, uint8_t NoOfMaterialBits, uint8_t NoOfDensityBits>
	class MaterialDensityPair : public Voxel<Type, Type>
	{
	public:
		//We expose DensityType and MaterialType in this way so that, when code is
		//templatised on voxel type, it can determine the underlying storage type
		//using code such as 'VoxelType::DensityType value = voxel.getDensity()'
		//or 'VoxelType::MaterialType value = voxel.getMaterial()'.
		typedef Type DensityType;
		typedef Type MaterialType;

		MaterialDensityPair() : m_uMaterial(0), m_uDensity(0) {}
		MaterialDensityPair(Type uMaterial, Type uDensity) : m_uMaterial(uMaterial), m_uDensity(uDensity) {}

		bool operator==(const MaterialDensityPair& rhs) const throw()
		{
			return (m_uMaterial == rhs.m_uMaterial) && (m_uDensity == rhs.m_uDensity);
		};

		bool operator!=(const MaterialDensityPair& rhs) const throw()
		{
			return !(*this == rhs);
		}

		DensityType getDensity() const throw() { return m_uDensity; }
		MaterialType getMaterial() const throw() { return m_uMaterial; }

		void setDensity(DensityType uDensity) { m_uDensity = uDensity; }
		void setMaterial(MaterialType uMaterial) { m_uMaterial = uMaterial; }

		//static DensityType getMaxDensity() throw() { return (0x01 << NoOfDensityBits) - 1; }
		//static DensityType getMinDensity() throw() { return 0; }
		static DensityType getThreshold() throw() {return  0x01 << (NoOfDensityBits - 1);}

	private:
		MaterialType m_uMaterial : NoOfMaterialBits;
		DensityType m_uDensity : NoOfDensityBits;
	};

	typedef MaterialDensityPair<uint8_t, 4, 4> MaterialDensityPair44;
	typedef MaterialDensityPair<uint16_t, 8, 8> MaterialDensityPair88;
	
	template<>
	class VoxelTypeTraits< MaterialDensityPair44 >
	{
	public:
		const static bool HasDensity;
		const static bool HasMaterial;
		const static MaterialDensityPair44::DensityType MinDensity;
		const static MaterialDensityPair44::DensityType MaxDensity;
	};
	
	template<>
	class VoxelTypeTraits< MaterialDensityPair88 >
	{
	public:
		const static bool HasDensity;
		const static bool HasMaterial;
		const static MaterialDensityPair88::DensityType MinDensity;
		const static MaterialDensityPair88::DensityType MaxDensity;
	};
}

#endif
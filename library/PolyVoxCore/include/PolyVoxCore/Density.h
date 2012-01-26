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

#ifndef __PolyVox_Density_H__
#define __PolyVox_Density_H__

#include "PolyVoxCore/Voxel.h"

#include "PolyVoxImpl/TypeDef.h"

#include <cassert>
#include <limits>

namespace PolyVox
{
	///This class represents a voxel storing only a density.
	////////////////////////////////////////////////////////////////////////////////
	/// In order to perform a surface extraction on a LargeVolume, PolyVox needs the underlying
	/// voxel type to provide both getDensity() and getMaterial() functions. The getDensity()
	/// function is used to determine if a voxel is 'solid', and if it is then the getMaterial()
	/// funtion is used to determine what material should be assigned to the resulting mesh.
	///
	/// This class meets these requirements, although it only actually stores a density value.
	/// For the getMaterial() function it just returns a constant value of '1'.
	///
	/// \sa Material, MaterialDensityPair
	////////////////////////////////////////////////////////////////////////////////
	template <typename Type>
	class Density : public Voxel<Type, uint8_t>
	{
	public:
		//We expose DensityType and MaterialType in this way so that, when code is
		//templatised on voxel type, it can determine the underlying storage type
		//using code such as 'VoxelType::DensityType value = voxel.getDensity()'
		//or 'VoxelType::MaterialType value = voxel.getMaterial()'.
		typedef Type DensityType;
		typedef uint8_t MaterialType; //Shouldn't define this one...

		Density() : m_uDensity(0) {}
		Density(DensityType uDensity) : m_uDensity(uDensity) {}

		bool operator==(const Density& rhs) const throw()
		{
			return (m_uDensity == rhs.m_uDensity);
		};

		bool operator!=(const Density& rhs) const throw()
		{
			return !(*this == rhs);
		}

		DensityType getDensity() const throw() { return m_uDensity; }
		//MaterialType getMaterial() const throw() { return 1; }

		void setDensity(DensityType uDensity) { m_uDensity = uDensity; }
		//void setMaterial(MaterialType /*uMaterial*/) { assert(false); } //Cannot set material on voxel of type Density

		//static DensityType getMaxDensity() throw() { return (std::numeric_limits<DensityType>::max)(); } 
		//static DensityType getMinDensity() throw() { return (std::numeric_limits<DensityType>::min)(); }
		static DensityType getThreshold() throw() { return (std::numeric_limits<DensityType>::max)() / 2; }

	private:
		DensityType m_uDensity;
	};

	// These are the predefined density types. The 8-bit types are sufficient for many purposes (including
	// most games) but 16-bit and float types do have uses particularly in medical/scientific visualisation.
	typedef Density<int8_t> DensityI8;
	typedef Density<uint8_t> DensityU8;
	typedef Density<int16_t> DensityI16;
	typedef Density<uint16_t> DensityU16;
	typedef Density<float> DensityFloat;
	typedef Density<double> DensityDouble;

	// These types are here for backwards compatibility but they are a little ambiguous as the name doesn't indicate
	// whether the values are signed. We would recommend using one of the 8 or 16 bit predefined types above instead.
	typedef DensityU8 Density8;
	typedef DensityU16 Density16;
	
	// We have to define the min and max values explicitly here rather than using std::numeric_limits because we need
	// compile time constants. The new 'constexpr' would help here but it's not supported by all compilers at the moment.	
	template<>
	class VoxelTypeTraits< DensityI8 >
	{
	public:
		const static bool HasDensity;
		const static bool HasMaterial;
		const static DensityI8::DensityType MinDensity;
		const static DensityI8::DensityType MaxDensity;
	};
	
	template<>
	class VoxelTypeTraits< DensityU8 >
	{
	public:
		const static bool HasDensity;
		const static bool HasMaterial;
		const static DensityU8::DensityType MinDensity;
		const static DensityU8::DensityType MaxDensity;
	};
	
	template<>
	class VoxelTypeTraits< DensityI16 >
	{
	public:
		const static bool HasDensity;
		const static bool HasMaterial;
		const static DensityI16::DensityType MinDensity;
		const static DensityI16::DensityType MaxDensity;
	};
	
	template<>
	class VoxelTypeTraits< DensityU16 >
	{
	public:
		const static bool HasDensity;
		const static bool HasMaterial;
		const static DensityU16::DensityType MinDensity;
		const static DensityU16::DensityType MaxDensity;
	};
	
	template<>
	class VoxelTypeTraits< DensityFloat >
	{
	public:
		const static bool HasDensity;
		const static bool HasMaterial;
		const static DensityFloat::DensityType MinDensity;
		const static DensityFloat::DensityType MaxDensity;
	};
	
	template<>
	class VoxelTypeTraits< DensityDouble >
	{
	public:
		const static bool HasDensity;
		const static bool HasMaterial;
		const static DensityDouble::DensityType MinDensity;
		const static DensityDouble::DensityType MaxDensity;
	};
}

#endif //__PolyVox_Density_H__

#pragma region License
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
#pragma endregion

#pragma region Headers
#pragma endregion

namespace PolyVox
{
	template <typename Type, uint8_t NoOfMaterialBits, uint8_t NoOfDensityBits>
	MaterialDensityPair<Type, NoOfMaterialBits, NoOfDensityBits>::MaterialDensityPair()
		:m_uMaterial(0)
		,m_uDensity(0)
	{
	}

	template <typename Type, uint8_t NoOfMaterialBits, uint8_t NoOfDensityBits>
	MaterialDensityPair<Type, NoOfMaterialBits, NoOfDensityBits>::MaterialDensityPair(Type uMaterial, Type uDensity)
		:m_uMaterial(uMaterial)
		,m_uDensity(uDensity)
	{
	}

	template <typename Type, uint8_t NoOfMaterialBits, uint8_t NoOfDensityBits>
	bool MaterialDensityPair<Type, NoOfMaterialBits, NoOfDensityBits>::operator==(const MaterialDensityPair<Type, NoOfMaterialBits, NoOfDensityBits>& rhs) const throw()
    {
		return (m_uMaterial == rhs.m_uMaterial) && (m_uDensity == rhs.m_uDensity);
    }

	template <typename Type, uint8_t NoOfMaterialBits, uint8_t NoOfDensityBits>
	bool MaterialDensityPair<Type, NoOfMaterialBits, NoOfDensityBits>::operator!=(const MaterialDensityPair<Type, NoOfMaterialBits, NoOfDensityBits>& rhs) const throw()
    {
		return !(*this == rhs);
    }

	template <typename Type, uint8_t NoOfMaterialBits, uint8_t NoOfDensityBits>
	bool MaterialDensityPair<Type, NoOfMaterialBits, NoOfDensityBits>::operator<(const MaterialDensityPair<Type, NoOfMaterialBits, NoOfDensityBits>& rhs) const throw()
    {
		if (m_uMaterial < rhs.m_uMaterial)
			return true;
		if (rhs.m_uMaterial < m_uMaterial)
			return false;
		if (m_uDensity < rhs.m_uDensity)
			return true;
		if (rhs.m_uDensity < m_uDensity)
			return false;

		return false;
		//return  m_uMaterial < rhs.m_uMaterial;
    }

	template <typename Type, uint8_t NoOfMaterialBits, uint8_t NoOfDensityBits>
	Type MaterialDensityPair<Type,NoOfMaterialBits, NoOfDensityBits>::getDensity() const throw()
	{
		return m_uDensity;
	}

	template <typename Type, uint8_t NoOfMaterialBits, uint8_t NoOfDensityBits>
	Type MaterialDensityPair<Type, NoOfMaterialBits, NoOfDensityBits>::getMaterial() const throw()
	{
		return m_uMaterial;
	}

	template <typename Type, uint8_t NoOfMaterialBits, uint8_t NoOfDensityBits>
	void MaterialDensityPair<Type, NoOfMaterialBits, NoOfDensityBits>::setDensity(Type uDensity)
	{
		m_uDensity = uDensity;
	}

	template <typename Type, uint8_t NoOfMaterialBits, uint8_t NoOfDensityBits>
	void MaterialDensityPair<Type, NoOfMaterialBits, NoOfDensityBits>::setMaterial(Type uMaterial)
	{
		m_uMaterial = uMaterial;
	}

	template <typename Type, uint8_t NoOfMaterialBits, uint8_t NoOfDensityBits>
	Type  MaterialDensityPair<Type, NoOfMaterialBits, NoOfDensityBits>::getMaxDensity() throw()
	{
		return (0x01 << NoOfDensityBits) - 1;
	}

	template <typename Type, uint8_t NoOfMaterialBits, uint8_t NoOfDensityBits>
	Type  MaterialDensityPair<Type, NoOfMaterialBits, NoOfDensityBits>::getMinDensity() throw()
	{
		return 0;
	}

	template <typename Type, uint8_t NoOfMaterialBits, uint8_t NoOfDensityBits>
	Type  MaterialDensityPair<Type, NoOfMaterialBits, NoOfDensityBits>::getThreshold() throw()
	{
		return  0x01 << (NoOfDensityBits - 1);
	}
}

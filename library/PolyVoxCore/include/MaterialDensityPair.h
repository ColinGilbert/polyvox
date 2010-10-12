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

#include "PolyVoxForwardDeclarations.h"
#include "PolyVoxImpl/TypeDef.h"

namespace PolyVox
{
	template <typename Type, uint8_t NoOfMaterialBits, uint8_t NoOfDensityBits>
	class MaterialDensityPair
	{
	public:
		MaterialDensityPair();
		MaterialDensityPair(Type uMaterial, Type uDensity);

		//Why are both of these needed?!
		bool operator==(const MaterialDensityPair& rhs) const throw();
		bool operator!=(const MaterialDensityPair& rhs) const throw();

		bool operator<(const MaterialDensityPair& rhs) const throw();

		Type getDensity() const throw();
		Type getMaterial() const throw();

		void setDensity(Type uDensity);
		void setMaterial(Type uMaterial);

		static Type getMaxDensity() throw();
		static Type getMinDensity() throw();
		static Type getThreshold() throw();

	private:
		Type m_uMaterial : NoOfMaterialBits;
		Type m_uDensity : NoOfDensityBits;
	};

	typedef MaterialDensityPair<uint8_t, 4, 4> MaterialDensityPair44;
}

#include "MaterialDensityPair.inl"

#endif
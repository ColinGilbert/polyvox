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

#ifndef __PolyVox_Utility_H__
#define __PolyVox_Utility_H__

#include "CPlusPlusZeroXSupport.h"
#include "TypeDef.h"

#include <cassert>

namespace PolyVox
{
	POLYVOX_API uint8_t logBase2(uint32_t uInput);
	POLYVOX_API bool isPowerOf2(uint32_t uInput);

	template <typename Type>
        Type trilinearlyInterpolate(
        const Type& v000,const Type& v100,const Type& v010,const Type& v110,
        const Type& v001,const Type& v101,const Type& v011,const Type& v111,
        const float x, const float y, const float z)
    {
        assert((x >= 0.0f) && (y >= 0.0f) && (z >= 0.0f) && 
            (x <= 1.0f) && (y <= 1.0f) && (z <= 1.0f));

		//Interpolate along X
		Type v000_v100 = (v100 - v000) * x + v000;
		Type v001_v101 = (v101 - v001) * x + v001;
		Type v010_v110 = (v110 - v010) * x + v010;
		Type v011_v111 = (v111 - v011) * x + v011;

		//Interpolate along Y
		Type v000_v100__v010_v110 = (v010_v110 - v000_v100) * y + v000_v100;
		Type v001_v101__v011_v111 = (v011_v111 - v001_v101) * y + v001_v101;

		//Interpolate along Z
		Type v000_v100__v010_v110____v001_v101__v011_v111 = (v001_v101__v011_v111 - v000_v100__v010_v110) * z + v000_v100__v010_v110;

		return v000_v100__v010_v110____v001_v101__v011_v111;
    }
}

#endif

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

#include "Utility.h"

#include <cassert>

namespace PolyVox
{
	//Note: this function only works for inputs which are a power of two and not zero
	//If this is not the case then the output is undefined.
	boost::uint8_t logBase2(boost::uint32_t uInput)
	{
		assert(uInput != 0);
		assert(isPowerOf2(uInput));

		boost::uint8_t result = 0;
		uInput = uInput >> 1;
		while(uInput)
		{
			uInput = uInput >> 1;
			++result;
		}
		return result;
	}

	bool isPowerOf2(boost::uint32_t uInput)
	{
		boost::uint8_t uNonZeroBits = 0;
		boost::uint32_t uMask = 0x00000001;
		for(boost::uint8_t ct = 0; ct < 32; ++ct)
		{
			if(uInput & uMask)
			{
				++uNonZeroBits;
			}
			uMask = uMask << 1;
		}
		return uNonZeroBits == 1 ? true : false;
	}
}
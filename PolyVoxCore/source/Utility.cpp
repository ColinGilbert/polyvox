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
	std::uint8_t logBase2(std::uint32_t uInput)
	{
		assert(uInput != 0);
		assert(isPowerOf2(uInput));

		std::uint32_t uResult = 0;
		while( (uInput >> uResult) != 0)
		{
			++uResult;
		}
		return static_cast<std::uint8_t>(uResult-1);
	}


	bool isPowerOf2(std::uint32_t uInput)
	{
		if(uInput == 0)
			return false;
		else
			return ((uInput & (uInput-1)) == 0);
	}
}

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
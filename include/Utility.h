#ifndef __PolyVox_Utility_H__
#define __PolyVox_Utility_H__

#include "Typedef.h"

#include "boost/cstdint.hpp"

namespace PolyVox
{
	POLYVOX_API boost::uint8_t logBase2(boost::uint32_t uInput);
	POLYVOX_API bool isPowerOf2(boost::uint32_t uInput);
}

#endif
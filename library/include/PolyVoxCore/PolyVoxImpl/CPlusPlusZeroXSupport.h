#ifndef __PolyVox_CPlusPlusZeroXSupport_H__
#define __PolyVox_CPlusPlusZeroXSupport_H__

#ifdef C_PLUS_PLUS_ZERO_X_SUPPORTED
	#include <shared_ptr> //Just a guess at what the standard name will be.
	#include <weak_ptr> //These includes may need changing

	#define POLYVOX_STD_NAMESPACE std
	#define POLYVOX_SHARED_PTR std::shared_ptr
	#define POLYVOX_WEAK_PTR std::weak_ptr
#else
	#include "boost/shared_ptr.hpp"
	#include "boost/weak_ptr.hpp"

	#define POLYVOX_STD_NAMESPACE boost
	#define POLYVOX_SHARED_PTR boost::shared_ptr
	#define POLYVOX_WEAK_PTR boost::weak_ptr
#endif

namespace PolyVox
{
	typedef char int8;
	typedef short int16;
	typedef long int32;
	typedef unsigned char uint8;
	typedef unsigned short uint16;
	typedef unsigned long uint32;
}

#endif
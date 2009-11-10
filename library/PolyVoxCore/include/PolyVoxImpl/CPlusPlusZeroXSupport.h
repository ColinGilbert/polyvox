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

#ifndef __PolyVox_CPlusPlusZeroXSupport_H__
#define __PolyVox_CPlusPlusZeroXSupport_H__

//If our version of the standard library suports shared pointers then we can use that implementation, otherwise
//we use the one from boost. Slightly ugly defines here - unfortunatly C++ does not support 'typedef templates'.
//Ironically 'typedef templates' are coming in the next version of C++, by which time we won't need them.
#ifdef C_PLUS_PLUS_ZERO_X_SUPPORTED
	#include <cstdint>
	#include <memory> //Just a guess at what the standard name will be. may need changing

	#define POLYVOX_STD_NAMESPACE std
	#define POLYVOX_SHARED_PTR std::shared_ptr
	#define POLYVOX_WEAK_PTR std::weak_ptr
#else
	#include "boost/cstdint.hpp"
	#include "boost/shared_ptr.hpp"
	#include "boost/weak_ptr.hpp"

	#define POLYVOX_STD_NAMESPACE boost
	#define POLYVOX_SHARED_PTR boost::shared_ptr
	#define POLYVOX_WEAK_PTR boost::weak_ptr
#endif

//In some ways these integer types are an implementation detail and should be in the PolyVoxImpl namespace.
//However, we will be passing them into and out of PolyVox functions which are seen by the user, and we'd
//rather not have to use the PolyVoxImpl qualifier, espessially as it will show up in Doxygen. This is just a
//temporary work around until it's properly supported by C++ anyway...
namespace PolyVox
{
	typedef POLYVOX_STD_NAMESPACE::int8_t int8_t;
	typedef POLYVOX_STD_NAMESPACE::int16_t int16_t;
	typedef POLYVOX_STD_NAMESPACE::int32_t int32_t;
	typedef POLYVOX_STD_NAMESPACE::uint8_t uint8_t;
	typedef POLYVOX_STD_NAMESPACE::uint16_t uint16_t;
	typedef POLYVOX_STD_NAMESPACE::uint32_t uint32_t;
}

#endif
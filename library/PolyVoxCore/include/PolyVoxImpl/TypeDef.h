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

#ifndef __PolyVox_TypeDef_H__
#define __PolyVox_TypeDef_H__

//Definitions needed to make library functions accessable
#ifdef _MSC_VER
	//We are using a Microsoft compiler.
	#ifdef POLYVOXCORE_EXPORT
		#define POLYVOXCORE_API __declspec(dllexport)
	#else
		#define POLYVOXCORE_API __declspec(dllimport)
	#endif
#else
	//Assume a GNU compiler.
	#define POLYVOXCORE_API __attribute__ ((visibility("default")))
#endif

//Check which compiler we are using and work around unsupported features as necessary.
#if defined(_MSC_VER) && (_MSC_VER < 1600) 
	//To support old (pre-vc2010) Microsoft compilers we use boost to replace the
	//std::shared_ptr and potentially other C++0x features. To use this capability you
	//will need to make sure you have boost installed on your system.
	#include <boost/smart_ptr.hpp>
	#define polyvox_shared_ptr boost::shared_ptr

	#include <boost/functional.hpp>
	#define polyvox_function boost::function

	#include <boost/functional/hash.hpp>
	#define polyvox_hash boost::hash


	//As long as we're requiring boost, we'll use it to compensate
	//for the missing cstdint header too.
	#include <boost/cstdint.hpp>
	using boost::int8_t;
	using boost::int16_t;
	using boost::int32_t;
	using boost::uint8_t;
	using boost::uint16_t;
	using boost::uint32_t;
#else
	//We have a decent compiler - use real C++0x features
	#include <cstdint>
	#include <functional>
	#include <memory>
	#define polyvox_shared_ptr std::shared_ptr
	#define polyvox_function std::function
	#define polyvox_hash std::hash
#endif

#endif

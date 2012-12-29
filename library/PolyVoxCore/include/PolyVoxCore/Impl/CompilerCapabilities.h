/*
 * This file describes the capabilities of the C++ compiler and is used to determine which features to enable in PolyVox.
 * It assumes that if the compiler is from VS2008 or earlier then no C++11 is present, otherwise it assumes full support
 * is present.
 *
 * Not that this file is usually overwritten by CMake which does careful tests of the true compiler capabilities. However,
 * we provide this default file so that CMake is not actually required for users of PolyVox and they can instead just drop
 * PolyVox code into their project/makefile if they prefer.
 */

#ifndef __PolyVox_CompilerCapabilities_H__
#define __PolyVox_CompilerCapabilities_H__

// If we are not using Visual Studio (or we are but it
// is a recent version) then assume support for these.
#if !defined(_MSC_VER) || (_MSC_VER >= 1600)
	#define HAS_CXX11_CONSTEXPR
	//#define HAS_CXX11_STATIC_ASSERT //This seems to cause issues on Android.
	#define HAS_CXX11_CSTDINT_H
	#define HAS_CXX11_SHARED_PTR
#endif

#endif

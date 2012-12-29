/*
 * This file provides the default compiler capabilities for for Visual Studio.
 * On other compilers CMake will detect which features are available and create
 * a file like this.
 * 
 * To Enable these features in Visual Studio, define the variables in this file.
*/

#ifndef __PolyVox_CompilerCapabilities_H__
#define __PolyVox_CompilerCapabilities_H__

//#undef HAS_CXX11_CONSTEXPR

//#define HAS_CXX11_STATIC_ASSERT

#define HAS_CXX11_CSTDINT_H

#define HAS_CXX11_SHARED_PTR

#endif

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

#if defined(_MSC_VER) && (_MSC_VER < 1800)
#error "Your version of Visual Studio is too old to build PolyVox. You need at least version Visual Stusio 2013"
#endif

//Definitions needed to make library functions accessable
// See http://gcc.gnu.org/wiki/Visibility for more info.
#if defined _WIN32 || defined __CYGWIN__
  #define POLYVOX_HELPER_IMPORT __declspec(dllimport)
  #define POLYVOX_HELPER_EXPORT __declspec(dllexport)
  #define POLYVOX_HELPER_LOCAL
  #define POLYVOX_DEPRECATED __declspec(deprecated)
#else
  #define POLYVOX_DEPRECATED __attribute__((deprecated))
  #if __GNUC__ >= 4
    #define POLYVOX_HELPER_IMPORT __attribute__ ((visibility("default")))
    #define POLYVOX_HELPER_EXPORT __attribute__ ((visibility("default")))
    #define POLYVOX_HELPER_LOCAL  __attribute__ ((visibility("hidden")))
  #else
    #define POLYVOX_HELPER_IMPORT
    #define POLYVOX_HELPER_EXPORT
    #define POLYVOX_HELPER_LOCAL
  #endif
#endif

#if defined SWIG
  //Do nothing in this case
#else
  #undef POLYVOX_DEPRECATED
  #define POLYVOX_DEPRECATED //Define it to nothing to avoid warnings
#endif

// Now we use the generic helper definitions above to define POLYVOX_API and POLYVOX_LOCAL.
// POLYVOX_API is used for the public API symbols. It either imports or exports (or does nothing for static build)
// POLYVOX_LOCAL is used for non-api symbols.

#ifdef POLYVOX_SHARED // defined if PolyVox is compiled as a shared library
  #ifdef POLYVOX_SHARED_EXPORTS // defined if we are building the PolyVox shared library (instead of using it)
    #define POLYVOX_API POLYVOX_HELPER_EXPORT
  #else
    #define POLYVOX_API POLYVOX_HELPER_IMPORT
  #endif // POLYVOX_SHARED_EXPORTS
  #define POLYVOX_LOCAL POLYVOX_HELPER_LOCAL
#else // POLYVOX_SHARED is not defined: this means PolyVox is a static library.
  #define POLYVOX_API
  #define POLYVOX_LOCAL
#endif // POLYVOX_SHARED

#endif

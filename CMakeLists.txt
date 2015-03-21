# Copyright (c) 2007-2012 Matt Williams
# Copyright (c) 2007-2012 David Williams
# 
# This software is provided 'as-is', without any express or implied
# warranty. In no event will the authors be held liable for any damages
# arising from the use of this software.
# 
# Permission is granted to anyone to use this software for any purpose,
# including commercial applications, and to alter it and redistribute it
# freely, subject to the following restrictions:
# 
# 1. The origin of this software must not be misrepresented; you must not
# claim that you wrote the original software. If you use this software
# in a product, an acknowledgment in the product documentation would be
# appreciated but is not required.
# 
# 2. Altered source versions must be plainly marked as such, and must not be
# misrepresented as being the original software.
# 
# 3. This notice may not be removed or altered from any source
# distribution.

CMAKE_MINIMUM_REQUIRED(VERSION 2.8.6)

PROJECT(PolyVox)

SET(POLYVOX_VERSION_MAJOR "0")
SET(POLYVOX_VERSION_MINOR "2")
SET(POLYVOX_VERSION_PATCH "1")
SET(POLYVOX_VERSION "${POLYVOX_VERSION_MAJOR}.${POLYVOX_VERSION_MINOR}.${POLYVOX_VERSION_PATCH}" CACHE STRING "PolyVox version")
MARK_AS_ADVANCED(FORCE POLYVOX_VERSION)

SET_PROPERTY(GLOBAL PROPERTY USE_FOLDERS ON)
include(FeatureSummary)

IF(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang") #Maybe "OR MINGW"
	ADD_DEFINITIONS(-std=c++0x) #Enable C++0x mode
ENDIF()

IF(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
	ADD_DEFINITIONS(-D_CRT_SECURE_NO_WARNINGS)
ENDIF()

ADD_SUBDIRECTORY(include)

OPTION(ENABLE_EXAMPLES "Should the examples be built" ON)
IF(ENABLE_EXAMPLES)
	ADD_SUBDIRECTORY(examples)
ELSE()
	SET(BUILD_EXAMPLES OFF)
ENDIF()

INCLUDE(Packaging.cmake)

OPTION(ENABLE_TESTS "Should the tests be built" ON)
IF(ENABLE_TESTS)
	INCLUDE(CTest)
	MARK_AS_ADVANCED(FORCE BUILD_TESTING)
	ADD_SUBDIRECTORY(tests)
ELSE()
	SET(BUILD_TESTS OFF)
ENDIF()

ADD_SUBDIRECTORY(documentation)

ADD_SUBDIRECTORY(bindings)

add_feature_info("Examples" BUILD_EXAMPLES "Examples of PolyVox usage")
add_feature_info("Tests" BUILD_TESTS "Unit tests")
add_feature_info("Bindings" BUILD_BINDINGS "SWIG bindings")
add_feature_info("API docs" BUILD_DOCS "HTML documentation of the API")
add_feature_info("Manual" BUILD_MANUAL "HTML user's manual")

feature_summary(WHAT ALL)

# Option summary
MESSAGE(STATUS "")
MESSAGE(STATUS "Summary")
MESSAGE(STATUS "-------")
MESSAGE(STATUS "Build examples:      " ${BUILD_EXAMPLES})
MESSAGE(STATUS "Build tests:         " ${BUILD_TESTS})
MESSAGE(STATUS "Build bindings:      " ${BUILD_BINDINGS})
MESSAGE(STATUS "API Docs available:  " ${BUILD_DOCS})
MESSAGE(STATUS "Build manual:        " ${BUILD_MANUAL})
MESSAGE(STATUS "")

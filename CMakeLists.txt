CMAKE_MINIMUM_REQUIRED(VERSION 2.8.3)

PROJECT(PolyVox)

SET(POLYVOX_VERSION_MAJOR "0")
SET(POLYVOX_VERSION_MINOR "1")
SET(POLYVOX_VERSION_PATCH "0")
SET(POLYVOX_VERSION "${POLYVOX_VERSION_MAJOR}.${POLYVOX_VERSION_MINOR}.${POLYVOX_VERSION_PATCH}")

include(FeatureSummary)

FIND_PACKAGE(Doxygen)
set_package_properties(Doxygen PROPERTIES URL http://www.doxygen.org DESCRIPTION "API documentation generator" TYPE OPTIONAL PURPOSE "Building the API documentation")
OPTION(ENABLE_EXAMPLES "Should the examples be built" ON)

# Qt is required for building the tests, the example and optionally for bundling the documentation
FIND_PACKAGE(Qt4 COMPONENTS QtCore QtGui QtOpenGL QtTest)
INCLUDE(${QT_USE_FILE})
set_package_properties(Qt4 PROPERTIES DESCRIPTION "C++ framework" URL http://qt-project.org)
set_package_properties(Qt4 PROPERTIES TYPE RECOMMENDED PURPOSE "Building the examples")
set_package_properties(Qt4 PROPERTIES TYPE OPTIONAL PURPOSE "Building the tests")

if(MSVC AND (MSVC_VERSION LESS 1600))
	# Require boost for older (pre-vc2010) Visual Studio compilers
	# See library/include/polyvoximpl/TypeDef.h
	find_package(Boost REQUIRED)
	include_directories(${Boost_INCLUDE_DIRS})
endif()

IF(CMAKE_COMPILER_IS_GNUCXX) #Maybe "OR MINGW"
	ADD_DEFINITIONS(-std=c++0x) #Enable C++0x mode
ENDIF()
if(CMAKE_CXX_COMPILER MATCHES "clang")
	ADD_DEFINITIONS(-std=c++0x) #Enable C++0x mode
endif()

ADD_SUBDIRECTORY(library)

OPTION(ENABLE_EXAMPLES "Should the examples be built" ON)
IF(ENABLE_EXAMPLES)
	ADD_SUBDIRECTORY(examples/Basic)
	ADD_SUBDIRECTORY(examples/Paging)
	ADD_SUBDIRECTORY(examples/OpenGL)
	ADD_SUBDIRECTORY(examples/SmoothLOD)
	if(BUILD_STATIC_LIBRARIES)
		ADD_DEPENDENCIES(BasicExample PolyVoxCoreStatic PolyVoxUtilStatic)
		ADD_DEPENDENCIES(PagingExample PolyVoxCoreStatic PolyVoxUtilStatic)
		ADD_DEPENDENCIES(OpenGLExample PolyVoxCoreStatic PolyVoxUtilStatic)
		ADD_DEPENDENCIES(SmoothLODExample PolyVoxCoreStatic PolyVoxUtilStatic)
	endif()
	if(BUILD_DYNAMIC_LIBRARIES)
		ADD_DEPENDENCIES(BasicExample PolyVoxCoreDynamic PolyVoxUtilDynamic)
		ADD_DEPENDENCIES(PagingExample PolyVoxCoreDynamic PolyVoxUtilDynamic)
		ADD_DEPENDENCIES(OpenGLExample PolyVoxCoreDynamic PolyVoxUtilDynamic)
		ADD_DEPENDENCIES(SmoothLODExample PolyVoxCoreDynamic PolyVoxUtilDynamic)
	endif()
ENDIF(ENABLE_EXAMPLES)

INCLUDE(Packaging.cmake)

OPTION(ENABLE_TESTS "Should the tests be built" ON)
IF(ENABLE_TESTS)
	INCLUDE(CTest)
	MARK_AS_ADVANCED(FORCE DART_TESTING_TIMEOUT) #This is only needed to hide the variable in the GUI (CMake bug)
	ADD_SUBDIRECTORY(tests)
ENDIF(ENABLE_TESTS)

#Check if we will building _and_ bundling the docs
IF(DOXYGEN_FOUND AND QT_QCOLLECTIONGENERATOR_EXECUTABLE)
	SET(BUILD_AND_BUNDLE_DOCS ON)
ELSE()
	SET(BUILD_AND_BUNDLE_DOCS OFF)
ENDIF()

ADD_SUBDIRECTORY(documentation)

add_feature_info("Static libraries" ENABLE_STATIC_LIBRARIES "Will static libraries be built")
add_feature_info("Dynamic libraries" ENABLE_DYNAMIC_LIBRARIES "Will dynamic libraries be built")
add_feature_info("Examples" ENABLE_EXAMPLES "Examples of PolyVox usage")
add_feature_info("Tests" ENABLE_TESTS "Unit tests")
add_feature_info("Bindings" BUILD_BINDINGS "SWIG bindings")
add_feature_info("API docs" DOXYGEN_FOUND "HTML documentation of the API")
add_feature_info("Qt Help" BUILD_AND_BUNDLE_DOCS "API docs in Qt Help format")
add_feature_info("Manual" BUILD_MANUAL "HTML user's manual")

feature_summary(WHAT ALL)

# Option summary
MESSAGE(STATUS "")
MESSAGE(STATUS "Summary")
MESSAGE(STATUS "-------")
MESSAGE(STATUS "Static libraries:    " ${ENABLE_STATIC_LIBRARIES})
MESSAGE(STATUS "Dynamic libraries:   " ${ENABLE_DYNAMIC_LIBRARIES})
MESSAGE(STATUS "Build examples:      " ${ENABLE_EXAMPLES})
MESSAGE(STATUS "Build tests:         " ${ENABLE_TESTS})
MESSAGE(STATUS "Build bindings:      " ${BUILD_BINDINGS})
MESSAGE(STATUS "API Docs available:  " ${DOXYGEN_FOUND})
MESSAGE(STATUS " - Qt Help bundling: " ${BUILD_AND_BUNDLE_DOCS})
MESSAGE(STATUS "Build manual:        " ${BUILD_MANUAL})
MESSAGE(STATUS "")

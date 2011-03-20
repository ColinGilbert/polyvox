CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

PROJECT(PolyVox)

SET(POLYVOX_VERSION_MAJOR "0")
SET(POLYVOX_VERSION_MINOR "1")
SET(POLYVOX_VERSION_PATCH "0")
SET(POLYVOX_VERSION "${POLYVOX_VERSION_MAJOR}.${POLYVOX_VERSION_MINOR}.${POLYVOX_VERSION_PATCH}")

FIND_PACKAGE(Doxygen)

# Qt is required for building the tests, the example and optionally for bundling the documentation
FIND_PACKAGE(Qt4 COMPONENTS QtCore QtGui QtOpenGL QtTest)
INCLUDE(${QT_USE_FILE})

IF(WIN32)
	#These are used on Windows to ensure that the .exe's and .dll get placed in the ame directory, so that we can run them. 
	#Note they are actually deprecated in favour of 'RUNTIME_OUTPUT_DIRECTORY' et al, but these replacements apparently don't
	#work on Windows. See http://www.vtk.org/Bug/bug_view_advanced_page.php?bug_id=8366
	SET(EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR})
	SET(LIBRARY_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR})
	#These are what _should_ be set. If we require 2.8 then test it with these.
	#SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
	#SET(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
ENDIF(WIN32)

if(MSVC AND (MSVC_VERSION LESS 1600))
	# Require boost for older (pre-vc2010) Visual Studio compilers
	# See library/include/polyvoximpl/TypeDef.h
	find_package(Boost REQUIRED)
	include_directories(${Boost_INCLUDE_DIRS})
endif()

IF(CMAKE_COMPILER_IS_GNUCXX) #Maybe "OR MINGW"
	ADD_DEFINITIONS(-std=c++0x) #Enable C++0x mode
ENDIF()

ADD_SUBDIRECTORY(library)

OPTION(ENABLE_EXAMPLES "Should the examples be built" ON)
IF(ENABLE_EXAMPLES)
	ADD_SUBDIRECTORY(examples/Basic)
	ADD_SUBDIRECTORY(examples/Paging)
	ADD_SUBDIRECTORY(examples/OpenGL)
	ADD_DEPENDENCIES(BasicExample PolyVoxCore PolyVoxUtil)
	ADD_DEPENDENCIES(PagingExample PolyVoxCore PolyVoxUtil)
	ADD_DEPENDENCIES(OpenGLExample PolyVoxCore PolyVoxUtil)
ENDIF(ENABLE_EXAMPLES)

INCLUDE(Packaging.cmake)

INCLUDE(CTest)
MARK_AS_ADVANCED(FORCE DART_TESTING_TIMEOUT) #This is only needed to hide the variable in the GUI (CMake bug)
IF(BUILD_TESTING)
	ADD_SUBDIRECTORY(tests)
ENDIF(BUILD_TESTING)

#Check if we will building _and_ bundling the docs
IF(DOXYGEN_FOUND AND QT_QCOLLECTIONGENERATOR_EXECUTABLE)
	SET(BUILD_AND_BUNDLE_DOCS YES)
ELSE()
	SET(BUILD_AND_BUNDLE_DOCS NO)
ENDIF()

ADD_SUBDIRECTORY(documentation)

# Option summary
MESSAGE(STATUS "")
MESSAGE(STATUS "Summary")
MESSAGE(STATUS "-------")
MESSAGE(STATUS "Build examples:      " ${ENABLE_EXAMPLES})
MESSAGE(STATUS "Build tests:         " ${BUILD_TESTING})
MESSAGE(STATUS "API Docs available:  " ${DOXYGEN_FOUND})
MESSAGE(STATUS " - Qt Help bundling: " ${BUILD_AND_BUNDLE_DOCS})
MESSAGE(STATUS "Build manual:        " ${BUILD_MANUAL})
MESSAGE(STATUS "")

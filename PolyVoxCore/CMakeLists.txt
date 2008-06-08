CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

PROJECT(PolyVoxCore)

SET(POLYVOX_VERSION_MAJOR "0")
SET(POLYVOX_VERSION_MINOR "1")
SET(POLYVOX_VERSION_PATCH "0")
SET(POLYVOX_VERSION "${POLYVOX_VERSION_MAJOR}.${POLYVOX_VERSION_MINOR}.${POLYVOX_VERSION_PATCH}")

#Projects source files
SET(SRC_FILES
	source/IndexedSurfacePatch.cpp
	source/MarchingCubesTables.cpp
	source/Region.cpp
	source/RegionGeometry.cpp 
	source/SurfaceExtractors.cpp 
	source/SurfaceVertex.cpp
	source/Utility.cpp
	source/VolumeChangeTracker.cpp
)

#Projects headers files
SET(INC_FILES
	include/Block.h
	include/Block.inl
	include/BlockVolume.h
	include/BlockVolume.inl
	include/BlockVolumeIterator.h
	include/BlockVolumeIterator.inl
	include/Constants.h
	include/Enums.h
	include/GradientEstimators.h	
	include/GradientEstimators.inl
	include/LinearVolume.h
	include/LinearVolume.inl
	include/IndexedSurfacePatch.h
	include/MarchingCubesTables.h
	include/PolyVoxForwardDeclarations.h
	include/Region.h
	include/RegionGeometry.h
	include/SurfaceExtractors.h
	include/SurfaceVertex.h
	include/TypeDef.h
	include/Utility.h
	include/Vector.h
	include/Vector.inl
	include/VolumeChangeTracker.h	
)

FIND_PACKAGE(Boost REQUIRED)

#under windows boost linking is automatic. Under Linux it is specified here. Might need changing for MinGW
#IF(NOT WIN32)
#	SET(BOOST_LIBRARIES boost_program_options boost_filesystem)
#ENDIF(NOT WIN32)



ADD_DEFINITIONS(-DPOLYVOX_EXPORT) #Export symbols in the .dll

#Appends "_d" to the generated library when in debug mode
SET(CMAKE_DEBUG_POSTFIX "_d")

#"Sources" and "Headers" are the group names in Visual Studio.
#They may have other uses too...
SOURCE_GROUP("Sources" FILES ${SRC_FILES})
SOURCE_GROUP("Headers" FILES ${INC_FILES})

#Tell CMake the paths
INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIRS} ${CMAKE_CURRENT_SOURCE_DIR}/include)

#Build
ADD_LIBRARY(PolyVoxCore SHARED ${SRC_FILES} ${INC_FILES})
SET_TARGET_PROPERTIES(PolyVoxCore PROPERTIES VERSION ${POLYVOX_VERSION} SOVERSION ${POLYVOX_VERSION_MAJOR})
IF(WIN32)
	SET_TARGET_PROPERTIES(PolyVoxCore PROPERTIES COMPILE_FLAGS "/wd4251") #Disable warning on STL exports
ENDIF(WIN32)

#Install
INSTALL(TARGETS PolyVoxCore
	RUNTIME DESTINATION bin
	LIBRARY DESTINATION lib
	ARCHIVE DESTINATION lib
)

INSTALL(FILES ${INC_FILES} DESTINATION include/PolyVoxCore)

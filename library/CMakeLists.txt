CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

PROJECT(PolyVoxCore)

SET(POLYVOX_VERSION_MAJOR "0")
SET(POLYVOX_VERSION_MINOR "1")
SET(POLYVOX_VERSION_PATCH "0")
SET(POLYVOX_VERSION "${POLYVOX_VERSION_MAJOR}.${POLYVOX_VERSION_MINOR}.${POLYVOX_VERSION_PATCH}")

#Projects source files
SET(CORE_SRC_FILES
	source/PolyVoxCore/GradientEstimators.cpp
	source/PolyVoxCore/IndexedSurfacePatch.cpp
	source/PolyVoxCore/MarchingCubesTables.cpp
	source/PolyVoxCore/Region.cpp
	source/PolyVoxCore/SurfaceAdjusters.cpp
	source/PolyVoxCore/SurfaceExtractors.cpp 
	source/PolyVoxCore/SurfaceVertex.cpp
	source/PolyVoxCore/Utility.cpp
	source/PolyVoxCore/VoxelFilters.cpp
)

#Projects headers files
SET(CORE_INC_FILES
	include/PolyVoxCore/Block.h
	include/PolyVoxCore/Block.inl
	include/PolyVoxCore/BlockVolume.h
	include/PolyVoxCore/BlockVolume.inl
	include/PolyVoxCore/BlockVolumeIterator.h
	include/PolyVoxCore/BlockVolumeIterator.inl
	include/PolyVoxCore/Constants.h
	include/PolyVoxCore/Enums.h
	include/PolyVoxCore/GradientEstimators.h	
	include/PolyVoxCore/GradientEstimators.inl
	include/PolyVoxCore/LinearVolume.h
	include/PolyVoxCore/LinearVolume.inl
	include/PolyVoxCore/IndexedSurfacePatch.h
	include/PolyVoxCore/MarchingCubesTables.h
	include/PolyVoxCore/PolyVoxForwardDeclarations.h
	include/PolyVoxCore/PolyVoxCStdInt.h
	include/PolyVoxCore/Region.h
	include/PolyVoxCore/SurfaceAdjusters.h
	include/PolyVoxCore/SurfaceExtractors.h
	include/PolyVoxCore/SurfaceVertex.h
	include/PolyVoxCore/TypeDef.h
	include/PolyVoxCore/Utility.h
	include/PolyVoxCore/Vector.h
	include/PolyVoxCore/Vector.inl
	include/PolyVoxCore/VoxelFilters.h
)

SET(IMPL_SRC_FILES
	source/PolyVoxCore/PolyVoxImpl/DecimatedSurfaceExtractor.cpp
	source/PolyVoxCore/PolyVoxImpl/FastSurfaceExtractor.cpp
	source/PolyVoxCore/PolyVoxImpl/ReferenceSurfaceExtractor.cpp
)

SET(IMPL_INC_FILES
	include/PolyVoxCore/PolyVoxImpl/DecimatedSurfaceExtractor.h
	include/PolyVoxCore/PolyVoxImpl/FastSurfaceExtractor.h
	include/PolyVoxCore/PolyVoxImpl/ReferenceSurfaceExtractor.h
)

#Projects source files
SET(UTIL_SRC_FILES
	source/PolyVoxUtil/VolumeChangeTracker.cpp
)

#Projects headers files
SET(UTIL_INC_FILES
	include/PolyVoxUtil/VolumeChangeTracker.h	
)

ADD_DEFINITIONS(-DPOLYVOX_EXPORT) #Export symbols in the .dll

#Appends "_d" to the generated library when in debug mode
SET(CMAKE_DEBUG_POSTFIX "_d")

#"Sources" and "Headers" are the group names in Visual Studio.
#They may have other uses too...
SOURCE_GROUP("Sources" FILES ${CORE_SRC_FILES})
SOURCE_GROUP("Headers" FILES ${CORE_INC_FILES})

SOURCE_GROUP("Sources\\PolyVoxImpl" FILES ${IMPL_SRC_FILES})
SOURCE_GROUP("Headers\\PolyVoxImpl" FILES ${IMPL_INC_FILES})

SOURCE_GROUP("Sources" FILES ${UTIL_SRC_FILES})
SOURCE_GROUP("Headers" FILES ${UTIL_INC_FILES})

#Tell CMake the paths
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/include)

#Build
ADD_LIBRARY(PolyVoxCore SHARED ${CORE_SRC_FILES} ${CORE_INC_FILES} ${IMPL_SRC_FILES} ${IMPL_INC_FILES})
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

INSTALL(FILES ${CORE_INC_FILES} DESTINATION include/PolyVoxCore)

#Build
ADD_LIBRARY(PolyVoxUtil SHARED ${UTIL_SRC_FILES} ${UTIL_INC_FILES})
TARGET_LINK_LIBRARIES(PolyVoxUtil PolyVoxCore)
SET_TARGET_PROPERTIES(PolyVoxUtil PROPERTIES VERSION ${POLYVOX_VERSION} SOVERSION ${POLYVOX_VERSION_MAJOR})
IF(WIN32)
	SET_TARGET_PROPERTIES(PolyVoxUtil PROPERTIES COMPILE_FLAGS "/wd4251") #Disable warning on STL exports
ENDIF(WIN32)

#Install
INSTALL(TARGETS PolyVoxUtil
	RUNTIME DESTINATION bin
	LIBRARY DESTINATION lib
	ARCHIVE DESTINATION lib
)

INSTALL(FILES ${UTIL_INC_FILES} DESTINATION include/PolyVoxUtil)

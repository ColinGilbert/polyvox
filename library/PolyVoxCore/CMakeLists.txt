CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

PROJECT(PolyVoxCore)

#Projects source files
SET(CORE_SRC_FILES
	source/ArraySizes.cpp
	source/GradientEstimators.cpp
	source/SurfaceMesh.cpp
	source/Log.cpp
	source/Mesh.cpp
	source/MeshEdge.cpp
	source/MeshFace.cpp
	source/MeshVertex.cpp
	source/Region.cpp
	source/SurfaceVertex.cpp
	source/VoxelFilters.cpp
)

#Projects headers files
SET(CORE_INC_FILES
	include/Array.h
	include/Array.inl
	include/ArraySizes.h
	include/Filters.h
	include/Filters.inl
	include/GradientEstimators.inl
	include/Log.h
	include/MaterialDensityPair.h
	include/MaterialDensityPair.inl
	include/Mesh.h
	include/MeshEdge.h
	include/MeshFace.h
	include/MeshVertex.h
	include/PolyVoxForwardDeclarations.h
	include/Region.h
	include/SurfaceExtractor.h
	include/SurfaceExtractor.inl
	include/SurfaceMesh.h
	include/SurfaceVertex.h
	include/Vector.h
	include/Vector.inl
	include/Volume.h
	include/Volume.inl
	include/VolumeSampler.h
	include/VolumeSampler.inl
	include/VoxelFilters.h
)

SET(IMPL_SRC_FILES
	source/PolyVoxImpl/MarchingCubesTables.cpp
	source/PolyVoxImpl/Utility.cpp
)

SET(IMPL_INC_FILES
	include/PolyVoxImpl/ArraySizesImpl.h
	include/PolyVoxImpl/ArraySizesImpl.inl
	include/PolyVoxImpl/Block.h
	include/PolyVoxImpl/Block.inl
	include/PolyVoxImpl/MarchingCubesTables.h
	include/PolyVoxImpl/SubArray.h
	include/PolyVoxImpl/SubArray.inl
	include/PolyVoxImpl/TypeDef.h
	include/PolyVoxImpl/Utility.h
)

ADD_DEFINITIONS(-DPOLYVOXCORE_EXPORT) #Export symbols in the .dll

#"Sources" and "Headers" are the group names in Visual Studio.
#They may have other uses too...
SOURCE_GROUP("Sources" FILES ${CORE_SRC_FILES})
SOURCE_GROUP("Headers" FILES ${CORE_INC_FILES})

SOURCE_GROUP("Sources\\PolyVoxImpl" FILES ${IMPL_SRC_FILES})
SOURCE_GROUP("Headers\\PolyVoxImpl" FILES ${IMPL_INC_FILES})

#Tell CMake the paths
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/include)

#Core
#Build
ADD_LIBRARY(PolyVoxCore SHARED ${CORE_SRC_FILES} ${CORE_INC_FILES} ${IMPL_SRC_FILES} ${IMPL_INC_FILES})
SET_TARGET_PROPERTIES(PolyVoxCore PROPERTIES VERSION ${POLYVOX_VERSION} SOVERSION ${POLYVOX_VERSION_MAJOR})
IF(MSVC)
        SET_TARGET_PROPERTIES(PolyVoxCore PROPERTIES COMPILE_FLAGS "/wd4251") #Disable warning on STL exports
ENDIF(MSVC)

#Install
IF(WIN32)
	INSTALL(TARGETS PolyVoxCore
		RUNTIME DESTINATION PolyVoxCore/bin
		LIBRARY DESTINATION PolyVoxCore/lib
		ARCHIVE DESTINATION PolyVoxCore/lib
		COMPONENT library
	)

	#Install the core header files, including the ones in the PolyVoxImpl subfolder.
	INSTALL(DIRECTORY include DESTINATION PolyVoxCore COMPONENT development PATTERN "*.svn*" EXCLUDE)
	
	#On windows, we also install the debug information. It's unfortunate that we have to hard-code
	#the 'Debug' part of the path, but CMake doesn't seem to provide a way around this. The best I
	#found was: http://www.cmake.org/pipermail/cmake/2007-October/016924.html (and it is a bit ugly).
	INSTALL(FILES ${CMAKE_CURRENT_BINARY_DIR}/../../Debug/PolyVoxCore.pdb DESTINATION PolyVoxCore/lib CONFIGURATIONS Debug)
ELSE(WIN32)
	INSTALL(TARGETS PolyVoxCore
		RUNTIME DESTINATION bin
		LIBRARY DESTINATION lib
		ARCHIVE DESTINATION lib
		COMPONENT library
	)

	#Install the core header files, including the ones in the PolyVoxImpl subfolder.
	INSTALL(DIRECTORY include/ DESTINATION include/PolyVoxCore COMPONENT development PATTERN "*.svn*" EXCLUDE)
ENDIF(WIN32)

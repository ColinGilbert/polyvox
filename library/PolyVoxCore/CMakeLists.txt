CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

PROJECT(PolyVoxCore)

#Projects source files
SET(CORE_SRC_FILES
	source/GradientEstimators.cpp
	source/IndexedSurfacePatch.cpp
	source/Log.cpp
	source/Mesh.cpp
	source/MeshEdge.cpp
	source/MeshFace.cpp
	source/MeshVertex.cpp
	source/progmesh.cpp
	source/Region.cpp
	source/SurfaceExtractor.cpp 
	source/SurfaceVertex.cpp
	source/vector.cpp
	source/VoxelFilters.cpp
)

#Projects headers files
SET(CORE_INC_FILES
	include/GradientEstimators.inl
	include/IndexedSurfacePatch.h
	include/list.h
	include/Log.h
	include/Mesh.h
	include/MeshEdge.h
	include/MeshFace.h
	include/MeshVertex.h
	include/PolyVoxForwardDeclarations.h
	include/progmesh.h
	include/Region.h
	include/SurfaceExtractor.h
	include/SurfaceVertex.h
	include/Vector.h
	include/vector_melax.h
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
	include/PolyVoxImpl/Block.h
	include/PolyVoxImpl/Block.inl
	include/PolyVoxImpl/CPlusPlusZeroXSupport.h
	include/PolyVoxImpl/MarchingCubesTables.h
	include/PolyVoxImpl/TypeDef.h
	include/PolyVoxImpl/Utility.h
)

ADD_DEFINITIONS(-DPOLYVOXCORE_EXPORT) #Export symbols in the .dll

#Appends "_d" to the generated library when in debug mode
SET(CMAKE_DEBUG_POSTFIX "_d")

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
IF(WIN32)
	SET_TARGET_PROPERTIES(PolyVoxCore PROPERTIES COMPILE_FLAGS "/wd4251") #Disable warning on STL exports
ENDIF(WIN32)

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
	INSTALL(FILES ${CMAKE_CURRENT_BINARY_DIR}/Debug/PolyVoxCore_d.pdb DESTINATION PolyVoxCore/lib CONFIGURATIONS Debug)
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

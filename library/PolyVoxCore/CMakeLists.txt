CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

PROJECT(PolyVoxCore)

#Projects source files
SET(CORE_SRC_FILES
	source/ArraySizes.cpp
	source/AStarPathfinder.cpp
	source/GradientEstimators.cpp
	source/Log.cpp
	source/MeshDecimator.cpp
	source/Region.cpp
	source/VertexTypes.cpp
	source/VoxelFilters.cpp
)

#Projects headers files
SET(CORE_INC_FILES
	include/AmbientOcclusionCalculator.h
	include/AmbientOcclusionCalculator.inl
	include/Array.h
	include/Array.inl
	include/ArraySizes.h
	include/AStarPathfinder.h
	include/AStarPathfinder.inl
	include/ConstVolumeProxy.h
	include/CubicSurfaceExtractor.h
	include/CubicSurfaceExtractor.inl
	include/CubicSurfaceExtractorWithNormals.h
	include/CubicSurfaceExtractorWithNormals.inl
	include/Density.h
	include/Filters.h
	include/Filters.inl
	include/GradientEstimators.h
	include/GradientEstimators.inl
	include/ImprovedCubicSurfaceExtractor.h
	include/ImprovedCubicSurfaceExtractor.inl
	include/LargeVolume.h
	include/LargeVolume.inl
	include/LargeVolumeSampler.inl
	include/Log.h
	include/Material.h
	include/MaterialDensityPair.h
	include/MeshDecimator.h
	include/MeshDecimator.inl
	include/PolyVoxForwardDeclarations.h
	include/Raycast.h
	include/Raycast.inl
	include/RaycastWithCallback.h
	include/RaycastWithCallback.inl
	include/Region.h
	include/SimpleVolume.h
	include/SimpleVolume.inl
	include/SimpleVolumeSampler.inl
	include/SurfaceExtractor.h
	include/SurfaceExtractor.inl
	include/SurfaceMesh.h
	include/SurfaceMesh.inl
	include/Vector.h
	include/Vector.inl
	include/VertexTypes.h
	include/VoxelFilters.h
)

SET(IMPL_SRC_FILES
	source/PolyVoxImpl/MarchingCubesTables.cpp
	source/PolyVoxImpl/RandomUnitVectors.cpp
	source/PolyVoxImpl/RandomVectors.cpp
	source/PolyVoxImpl/Utility.cpp
)

SET(IMPL_INC_FILES
	include/PolyVoxImpl/ArraySizesImpl.h
	include/PolyVoxImpl/ArraySizesImpl.inl
	include/PolyVoxImpl/AStarPathfinderImpl.h
	include/PolyVoxImpl/Block.h
	include/PolyVoxImpl/Block.inl
	include/PolyVoxImpl/MarchingCubesTables.h
	include/PolyVoxImpl/RandomUnitVectors.h
	include/PolyVoxImpl/RandomVectors.h
	include/PolyVoxImpl/SubArray.h
	include/PolyVoxImpl/SubArray.inl
	include/PolyVoxImpl/TypeDef.h
	include/PolyVoxImpl/Utility.h
)

#NOTE: The following line should be uncommented when building shared libs.

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
IF(BUILD_STATIC_LIBRARIES)
	ADD_LIBRARY(PolyVoxCoreStatic STATIC ${CORE_SRC_FILES} ${CORE_INC_FILES} ${IMPL_SRC_FILES} ${IMPL_INC_FILES})
	SET_TARGET_PROPERTIES(PolyVoxCoreStatic PROPERTIES OUTPUT_NAME "PolyVoxCore")
	SET_TARGET_PROPERTIES(PolyVoxCoreStatic PROPERTIES VERSION ${POLYVOX_VERSION} SOVERSION ${POLYVOX_VERSION_MAJOR})
	IF(MSVC)
			SET_TARGET_PROPERTIES(PolyVoxCoreStatic PROPERTIES COMPILE_FLAGS "/wd4251") #Disable warning on STL exports
	ENDIF(MSVC)
	SET(PolyVoxCore_LIBRARY "PolyVoxCoreStatic")
ENDIF()
IF(BUILD_DYNAMIC_LIBRARIES)
	ADD_LIBRARY(PolyVoxCoreDynamic SHARED ${CORE_SRC_FILES} ${CORE_INC_FILES} ${IMPL_SRC_FILES} ${IMPL_INC_FILES})
	SET_TARGET_PROPERTIES(PolyVoxCoreDynamic PROPERTIES OUTPUT_NAME "PolyVoxCore")
	SET_TARGET_PROPERTIES(PolyVoxCoreDynamic PROPERTIES COMPILE_FLAGS "-DPOLYVOX_SHARED_EXPORTS")
	SET_TARGET_PROPERTIES(PolyVoxCoreDynamic PROPERTIES VERSION ${POLYVOX_VERSION} SOVERSION ${POLYVOX_VERSION_MAJOR})
	IF(MSVC)
			SET_TARGET_PROPERTIES(PolyVoxCoreDynamic PROPERTIES COMPILE_FLAGS "/wd4251") #Disable warning on STL exports
	ENDIF(MSVC)
	SET(PolyVoxCore_LIBRARY "PolyVoxCoreDynamic")
ENDIF()

#Install
IF(WIN32)
	IF(BUILD_STATIC_LIBRARIES)
		INSTALL(TARGETS PolyVoxCoreStatic
			RUNTIME DESTINATION PolyVoxCore/bin COMPONENT library
			LIBRARY DESTINATION PolyVoxCore/lib COMPONENT library
			ARCHIVE DESTINATION PolyVoxCore/lib COMPONENT library
		)
	ENDIF()
	IF(BUILD_DYNAMIC_LIBRARIES)
		INSTALL(TARGETS PolyVoxCoreDynamic
			RUNTIME DESTINATION PolyVoxCore/bin COMPONENT library
			LIBRARY DESTINATION PolyVoxCore/lib COMPONENT library
			ARCHIVE DESTINATION PolyVoxCore/lib COMPONENT library
		)
	ENDIF()

	#Install the core header files, including the ones in the PolyVoxImpl subfolder.
	INSTALL(DIRECTORY include DESTINATION PolyVoxCore COMPONENT development PATTERN "*.svn*" EXCLUDE)
	
	#On windows, we also install the debug information. It's unfortunate that we have to hard-code
	#the 'Debug' part of the path, but CMake doesn't seem to provide a way around this. The best I
	#found was: http://www.cmake.org/pipermail/cmake/2007-October/016924.html (and it is a bit ugly).
	INSTALL(FILES ${CMAKE_CURRENT_BINARY_DIR}/Debug/PolyVoxCore.pdb DESTINATION PolyVoxCore/lib CONFIGURATIONS Debug)
	INSTALL(FILES ${CMAKE_CURRENT_BINARY_DIR}/RelWithDebInfo/PolyVoxCore.pdb DESTINATION PolyVoxCore/lib CONFIGURATIONS RelWithDebInfo)
ELSE(WIN32)
	IF(BUILD_STATIC_LIBRARIES)
		INSTALL(TARGETS PolyVoxCoreStatic
			RUNTIME DESTINATION bin COMPONENT library
			LIBRARY DESTINATION lib COMPONENT library
			ARCHIVE DESTINATION lib COMPONENT library
		)
	ENDIF()
	IF(BUILD_DYNAMIC_LIBRARIES)
		INSTALL(TARGETS PolyVoxCoreDynamic
			RUNTIME DESTINATION bin COMPONENT library
			LIBRARY DESTINATION lib COMPONENT library
			ARCHIVE DESTINATION lib COMPONENT library
		)
	ENDIF()

	#Install the core header files, including the ones in the PolyVoxImpl subfolder.
	INSTALL(DIRECTORY include/ DESTINATION include/PolyVoxCore COMPONENT development PATTERN "*.svn*" EXCLUDE)
ENDIF(WIN32)

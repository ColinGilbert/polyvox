PROJECT(PolyVoxSceneManager)

#Projects source files
SET(SRC_FILES
	source/Block.cpp
	source/IndexedSurfacePatch.cpp
	source/MarchingCubesTables.cpp
	source/PolyVoxSceneManager.cpp
        source/SurfaceEdge.cpp        
        source/SurfacePatchRenderable.cpp
	source/SurfaceTriangle.cpp
	source/SurfaceVertex.cpp
	source/Volume.cpp
	source/VolumeIterator.cpp
	source/VolumeManager.cpp
	source/VolumeSerializer.cpp
)

#Projects headers files
SET(INC_FILES
	include/Block.h
	include/Constants.h
	include/IndexedSurfacePatch.h
	include/IntegralVector3.h
	include/MarchingCubesTables.h
        include/PolyVoxSceneManager.h
	include/RegionGeometry.h
        include/SurfaceEdge.h
	include/SurfaceTypes.h
        include/SurfacePatchRenderable.h
        include/SurfaceTriangle.h
        include/SurfaceVertex.h
	include/TypeDef.h
	include/Volume.h
	include/VolumeIterator.h
	include/VolumeManager.h
	include/VolumeSerializer.h	
)

ADD_DEFINITIONS(-DVOXEL_SCENE_MANAGER_EXPORT) #Export symbols in the .dll

#Appends "_d" to the generated library when in debug mode
SET(CMAKE_DEBUG_POSTFIX "_d")

#"Sources" and "Headers" are the group names in Visual Studio.
#They may have other uses too...
SOURCE_GROUP("Sources" FILES ${SRC_FILES})
SOURCE_GROUP("Headers" FILES ${INC_FILES})

#Tell CMake the paths
LINK_DIRECTORIES(${OGRE_LIB_DIR})
INCLUDE_DIRECTORIES(${OGRE_INCLUDE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/include)

#Build
ADD_LIBRARY(PolyVoxSceneManager SHARED ${SRC_FILES} ${INC_FILES})
TARGET_LINK_LIBRARIES(PolyVoxSceneManager ${OGRE_LIBRARIES})
SET_TARGET_PROPERTIES(PolyVoxSceneManager PROPERTIES VERSION ${THERMITE_VERSION} SOVERSION ${THERMITE_VERSION_MAJOR})

#Install
INSTALL(TARGETS PolyVoxSceneManager
	RUNTIME DESTINATION bin
	LIBRARY DESTINATION lib
	ARCHIVE DESTINATION lib
)

INSTALL(DIRECTORY media/ DESTINATION ${CMAKE_INSTALL_PREFIX}/bin/media PATTERN "*.svn*" EXCLUDE)
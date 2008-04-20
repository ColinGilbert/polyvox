PROJECT(PolyVoxSceneManager)

#Projects source files
SET(SRC_FILES
	source/IndexedSurfacePatch.cpp
	source/MarchingCubesTables.cpp
	source/PolyVoxSceneManager.cpp
        source/RegionGeometry.cpp  
	source/SurfaceVertex.cpp
)

#Projects headers files
SET(INC_FILES
	include/Block.h
	include/Block.inl
	include/Constants.h	
	include/IndexedSurfacePatch.h
	include/MarchingCubesTables.h
	include/PolyVoxForwardDeclarations.h
        include/PolyVoxSceneManager.h
	include/RegionGeometry.h
        include/SurfaceVertex.h
	include/TypeDef.h
	include/Vector.h
	include/Vector.inl
	include/Volume.h
	include/Volume.inl
	include/VolumeIterator.h
	include/VolumeIterator.inl	
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
ADD_LIBRARY(PolyVoxSceneManager SHARED ${SRC_FILES} ${INC_FILES})
TARGET_LINK_LIBRARIES(PolyVoxSceneManager)
SET_TARGET_PROPERTIES(PolyVoxSceneManager PROPERTIES VERSION ${THERMITE_VERSION} SOVERSION ${THERMITE_VERSION_MAJOR})

#Install
INSTALL(TARGETS PolyVoxSceneManager
	RUNTIME DESTINATION bin
	LIBRARY DESTINATION lib
	ARCHIVE DESTINATION lib
)
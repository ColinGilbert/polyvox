CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

PROJECT(PolyVoxUtil)

SET(POLYVOX_VERSION_MAJOR "0")
SET(POLYVOX_VERSION_MINOR "1")
SET(POLYVOX_VERSION_PATCH "0")
SET(POLYVOX_VERSION "${POLYVOX_VERSION_MAJOR}.${POLYVOX_VERSION_MINOR}.${POLYVOX_VERSION_PATCH}")

#Projects source files
SET(SRC_FILES
	source/VolumeChangeTracker.cpp
)

#Projects headers files
SET(INC_FILES
	include/VolumeChangeTracker.h	
)

ADD_DEFINITIONS(-DPOLYVOX_EXPORT) #Export symbols in the .dll

#Appends "_d" to the generated library when in debug mode
SET(CMAKE_DEBUG_POSTFIX "_d")

#"Sources" and "Headers" are the group names in Visual Studio.
#They may have other uses too...
SOURCE_GROUP("Sources" FILES ${SRC_FILES})
SOURCE_GROUP("Headers" FILES ${INC_FILES})

#Tell CMake the paths
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/include)

#Build
ADD_LIBRARY(PolyVoxUtil SHARED ${SRC_FILES} ${INC_FILES})
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

INSTALL(FILES ${INC_FILES} DESTINATION include/PolyVoxUtil)

CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

PROJECT(PolyVoxUtil)

#Projects source files
SET(UTIL_SRC_FILES
	source/Serialization.cpp
	source/VolumeChangeTracker.cpp
)

#Projects headers files
SET(UTIL_INC_FILES
	include/Export.h
	include/Serialization.h
	include/VolumeChangeTracker.h	
)

ADD_DEFINITIONS(-DPOLYVOXUTIL_EXPORT) #Export symbols in the .dll

#"Sources" and "Headers" are the group names in Visual Studio.
#They may have other uses too...
SOURCE_GROUP("Sources" FILES ${UTIL_SRC_FILES})
SOURCE_GROUP("Headers" FILES ${UTIL_INC_FILES})

#Tell CMake the paths
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/include ${PolyVoxCore_SOURCE_DIR}/include)
#There has to be a better way!
LINK_DIRECTORIES(${PolyVoxCore_BINARY_DIR}/debug ${PolyVoxCore_BINARY_DIR}/release)

#Util
#Build
ADD_LIBRARY(PolyVoxUtil SHARED ${UTIL_SRC_FILES} ${UTIL_INC_FILES})
TARGET_LINK_LIBRARIES(PolyVoxUtil PolyVoxCore)
SET_TARGET_PROPERTIES(PolyVoxUtil PROPERTIES VERSION ${POLYVOX_VERSION} SOVERSION ${POLYVOX_VERSION_MAJOR})
IF(MSVC)
	SET_TARGET_PROPERTIES(PolyVoxUtil PROPERTIES COMPILE_FLAGS "/wd4251") #Disable warning on STL exports
ENDIF(MSVC)

#Install
IF(WIN32)
	INSTALL(TARGETS PolyVoxUtil
		RUNTIME DESTINATION PolyVoxUtil/bin
		LIBRARY DESTINATION PolyVoxUtil/lib
		ARCHIVE DESTINATION PolyVoxUtil/lib
		COMPONENT library
	)

	#Install the util header files.
	INSTALL(DIRECTORY include DESTINATION PolyVoxUtil COMPONENT development PATTERN "*.svn*" EXCLUDE)
	
	#On windows, we also install the debug information. It's unfortunate that we have to hard-code
	#the 'Debug' part of the path, but CMake doesn't seem to provide a way around this. The best I
	#found was: http://www.cmake.org/pipermail/cmake/2007-October/016924.html (and it is a bit ugly).
	INSTALL(FILES ${CMAKE_CURRENT_BINARY_DIR}/../../Debug/PolyVoxUtil.pdb DESTINATION PolyVoxUtil/lib CONFIGURATIONS Debug)
ELSE(WIN32)
	INSTALL(TARGETS PolyVoxUtil
		RUNTIME DESTINATION bin
		LIBRARY DESTINATION lib
		ARCHIVE DESTINATION lib
		COMPONENT library
	)

	#Install the util header files.
	INSTALL(DIRECTORY include/ DESTINATION include/PolyVoxUtil COMPONENT development PATTERN "*.svn*" EXCLUDE)
ENDIF(WIN32)

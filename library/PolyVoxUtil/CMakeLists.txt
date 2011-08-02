CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

PROJECT(PolyVoxUtil)

#Projects source files
SET(UTIL_SRC_FILES
	source/Dummy.cpp
)

#Projects headers files
SET(UTIL_INC_FILES
	include/PolyVoxUtil/Serialization.h
	include/PolyVoxUtil/Serialization.inl
	include/PolyVoxUtil/VolumeChangeTracker.h	
	include/PolyVoxUtil/VolumeChangeTracker.inl
)

ADD_DEFINITIONS(-DPOLYVOX_SHARED_EXPORTS) #Export symbols in the .dll

#"Sources" and "Headers" are the group names in Visual Studio.
#They may have other uses too...
SOURCE_GROUP("Sources" FILES ${UTIL_SRC_FILES})
SOURCE_GROUP("Headers" FILES ${UTIL_INC_FILES})

#Tell CMake the paths
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/include ${PolyVoxCore_SOURCE_DIR}/include)
#There has to be a better way!
LINK_DIRECTORIES(${PolyVoxCore_BINARY_DIR}/debug ${PolyVoxCore_BINARY_DIR}/release ${PolyVoxCore_BINARY_DIR})

#Util
#Build
IF(BUILD_STATIC_LIBRARIES)
	ADD_LIBRARY(PolyVoxUtilStatic STATIC ${UTIL_SRC_FILES} ${UTIL_INC_FILES})
	TARGET_LINK_LIBRARIES(PolyVoxUtilStatic PolyVoxCore)
	SET_TARGET_PROPERTIES(PolyVoxUtilStatic PROPERTIES OUTPUT_NAME "PolyVoxUtil")
	SET_TARGET_PROPERTIES(PolyVoxUtilStatic PROPERTIES VERSION ${POLYVOX_VERSION} SOVERSION ${POLYVOX_VERSION_MAJOR})
	IF(MSVC)
			SET_TARGET_PROPERTIES(PolyVoxUtilStatic PROPERTIES COMPILE_FLAGS "/W4 /wd4251 /wd4127") #Disable warning on STL exports
	ENDIF(MSVC)
	ADD_DEPENDENCIES(PolyVoxUtilStatic PolyVoxCoreStatic)
ENDIF()
IF(BUILD_DYNAMIC_LIBRARIES)
	ADD_LIBRARY(PolyVoxUtilDynamic SHARED ${UTIL_SRC_FILES} ${UTIL_INC_FILES})
	TARGET_LINK_LIBRARIES(PolyVoxUtilDynamic PolyVoxCore)
	SET_TARGET_PROPERTIES(PolyVoxUtilDynamic PROPERTIES OUTPUT_NAME "PolyVoxUtil")
	SET_TARGET_PROPERTIES(PolyVoxUtilDynamic PROPERTIES COMPILE_FLAGS "-DPOLYVOX_SHARED_EXPORTS")
	SET_TARGET_PROPERTIES(PolyVoxUtilDynamic PROPERTIES VERSION ${POLYVOX_VERSION} SOVERSION ${POLYVOX_VERSION_MAJOR})
	IF(MSVC)
			SET_TARGET_PROPERTIES(PolyVoxUtilDynamic PROPERTIES COMPILE_FLAGS "/W4 /wd4251 /wd4127") #Disable warning on STL exports
	ENDIF(MSVC)
	ADD_DEPENDENCIES(PolyVoxUtilDynamic PolyVoxCoreDynamic)
ENDIF()

#Install
IF(WIN32)
	IF(BUILD_STATIC_LIBRARIES)
		INSTALL(TARGETS PolyVoxUtilStatic
			RUNTIME DESTINATION PolyVoxUtil/bin COMPONENT library
			LIBRARY DESTINATION PolyVoxUtil/lib COMPONENT library
			ARCHIVE DESTINATION PolyVoxUtil/lib COMPONENT library
		)
	ENDIF()
	IF(BUILD_DYNAMIC_LIBRARIES)
		INSTALL(TARGETS PolyVoxUtilDynamic
			RUNTIME DESTINATION PolyVoxUtil/bin COMPONENT library
			LIBRARY DESTINATION PolyVoxUtil/lib COMPONENT library
			ARCHIVE DESTINATION PolyVoxUtil/lib COMPONENT library
		)
	ENDIF()

	#Install the util header files.
	INSTALL(DIRECTORY include DESTINATION PolyVoxUtil COMPONENT development PATTERN "*.svn*" EXCLUDE)
	
	#On windows, we also install the debug information. It's unfortunate that we have to hard-code
	#the 'Debug' part of the path, but CMake doesn't seem to provide a way around this. The best I
	#found was: http://www.cmake.org/pipermail/cmake/2007-October/016924.html (and it is a bit ugly).
	INSTALL(FILES ${CMAKE_CURRENT_BINARY_DIR}/Debug/PolyVoxUtil.pdb DESTINATION PolyVoxUtil/lib CONFIGURATIONS Debug)
	INSTALL(FILES ${CMAKE_CURRENT_BINARY_DIR}/RelWithDebInfo/PolyVoxUtil.pdb DESTINATION PolyVoxUtil/lib CONFIGURATIONS RelWithDebInfo)
ELSE(WIN32)
	IF(BUILD_STATIC_LIBRARIES)
		INSTALL(TARGETS PolyVoxUtilStatic
			RUNTIME DESTINATION bin COMPONENT library
			LIBRARY DESTINATION lib COMPONENT library
			ARCHIVE DESTINATION lib COMPONENT library
		)
	ENDIF()
	IF(BUILD_DYNAMIC_LIBRARIES)
		INSTALL(TARGETS PolyVoxUtilDynamic
			RUNTIME DESTINATION bin COMPONENT library
			LIBRARY DESTINATION lib COMPONENT library
			ARCHIVE DESTINATION lib COMPONENT library
		)
	ENDIF()

	#Install the util header files.
	INSTALL(DIRECTORY include/ DESTINATION include/PolyVoxUtil COMPONENT development PATTERN "*.svn*" EXCLUDE)
ENDIF(WIN32)

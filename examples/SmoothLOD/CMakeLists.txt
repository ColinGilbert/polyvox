CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

PROJECT(SmoothLODExample)

#Projects source files
SET(SRC_FILES
	glew/glew.cpp
	
	main.cpp
	OpenGLWidget.cpp
	)

#Projects headers files	
SET(INC_FILES	
	glew/glew.h
	glew/glxew.h
	glew/wglew.h
	
	OpenGLWidget.h
	)

ADD_DEFINITIONS(-DGLEW_STATIC)

#"Sources" and "Headers" are the group names in Visual Studio.
#They may have other uses too...
SOURCE_GROUP("Sources" FILES ${SRC_FILES})
SOURCE_GROUP("Headers" FILES ${INC_FILES})

FIND_PACKAGE(OpenGL REQUIRED)

#Tell CMake the paths for OpenGL and for PolyVox (which is just relative to our current location)
INCLUDE_DIRECTORIES(${OPENGL_INCLUDE_DIR} ${PolyVoxCore_SOURCE_DIR}/include)
LINK_DIRECTORIES(${PolyVoxCore_BINARY_DIR})

#Build
ADD_EXECUTABLE(SmoothLODExample ${SRC_FILES})
IF(MSVC)
	SET_TARGET_PROPERTIES(SmoothLODExample PROPERTIES COMPILE_FLAGS "/W4 /wd4127") #All warnings
ENDIF(MSVC)
TARGET_LINK_LIBRARIES(SmoothLODExample ${QT_LIBRARIES} ${OPENGL_gl_LIBRARY} ${OPENGL_glu_LIBRARY} PolyVoxCore)

#Install - Only install the example in Windows
IF(WIN32)
	INSTALL(TARGETS SmoothLODExample
		RUNTIME DESTINATION Examples/OpenGL/bin
		LIBRARY DESTINATION Examples/OpenGL/lib
		ARCHIVE DESTINATION Examples/OpenGL/lib
		COMPONENT example
	)

	#.dlls should be installed in shared builds.
	#INSTALL(FILES ${CMAKE_CURRENT_BINARY_DIR}/../../release/PolyVoxCore.dll DESTINATION Examples/OpenGL/bin CONFIGURATIONS Release)
	#INSTALL(FILES ${CMAKE_CURRENT_BINARY_DIR}/../../release/PolyVoxUtil.dll DESTINATION Examples/OpenGL/bin CONFIGURATIONS Release)
	
	#INSTALL(FILES ${CMAKE_CURRENT_BINARY_DIR}/../../debug/PolyVoxCore.dll DESTINATION Examples/OpenGL/bin CONFIGURATIONS Debug)
	#INSTALL(FILES ${CMAKE_CURRENT_BINARY_DIR}/../../debug/PolyVoxUtil.dll DESTINATION Examples/OpenGL/bin CONFIGURATIONS Debug)
ENDIF(WIN32)

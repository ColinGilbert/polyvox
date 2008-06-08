CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

PROJECT(OpenGLExample)

#Projects source files
SET(SRC_FILES main.cpp)

#Projects headers files
#SET(INC_FILES)

#Appends "_d" to the generated library when in debug mode
SET(CMAKE_DEBUG_POSTFIX "_d")

#"Sources" and "Headers" are the group names in Visual Studio.
#They may have other uses too...
SOURCE_GROUP("Sources" FILES ${SRC_FILES})
#SOURCE_GROUP("Headers" FILES ${INC_FILES})

FIND_PACKAGE(OpenGL REQUIRED)

IF (WIN32)
	#NOTE:  In Windows I haven't had much luck getting the FindGLUT script to work correctly. 
	#The easiest solution has been  to install GLUT alongside OpenGL, which is already in the system path.
	#This means glut.h and glut32.lib go in the 'include' and 'lib' folders within  Microsoft Visual Studio 8\VC\PlatformSDK\ 
	#Also, glut32.dll goes in C:\Windows\System. Using C:\Windows\System32 doesn't seem to work
	
	#Tell CMake the paths
	INCLUDE_DIRECTORIES(${OPENGL_INCLUDE_DIR})

	#Build
	ADD_EXECUTABLE(OpenGLExample ${SRC_FILES})
	TARGET_LINK_LIBRARIES(OpenGLExample glut32.lib ${OPENGL_gl_LIBRARY})
ENDIF (WIN32)
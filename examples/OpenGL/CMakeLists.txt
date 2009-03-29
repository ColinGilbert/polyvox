CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

PROJECT(OpenGLExample)

#Projects source files
SET(SRC_FILES
	glew/glew.c
	main.cpp
	OpenGLImmediateModeSupport.cpp
	OpenGLSupport.cpp
	OpenGLVertexBufferObjectSupport.cpp
	OpenGLWidget.cpp
	Shapes.cpp
	)

#Projects headers files	
SET(INC_FILES
	glew/glew.h
	glew/glxew.h
	glew/wglew.h
	OpenGLImmediateModeSupport.h
	OpenGLSupport.h
	OpenGLVertexBufferObjectSupport.h
	OpenGLWidget.h
	Shapes.h
	)

ADD_DEFINITIONS(-DGLEW_STATIC)

#Appends "_d" to the generated library when in debug mode
SET(CMAKE_DEBUG_POSTFIX "_d")

#"Sources" and "Headers" are the group names in Visual Studio.
#They may have other uses too...
SOURCE_GROUP("Sources" FILES ${SRC_FILES})
#SOURCE_GROUP("Headers" FILES ${INC_FILES})

FIND_PACKAGE(Qt4)
SET(QT_USE_QTGUI 1)
SET(QT_USE_QTOPENGL 1)
INCLUDE(${QT_USE_FILE})

#IF (WIN32)
	FIND_PACKAGE(OpenGL REQUIRED)

	#NOTE:  In Windows I haven't had much luck getting the FindGLUT script to work correctly. 
	#The easiest solution has been  to install GLUT alongside OpenGL, which is already in the system path.
	#This means glut.h and glut32.lib go in the 'include' and 'lib' folders within  Microsoft Visual Studio 8\VC\PlatformSDK\ 
	#Also, glut32.dll goes in C:\Windows\System. Using C:\Windows\System32 doesn't seem to work
	
	#Tell CMake the paths for OpenGL and for PolyVox (which is just relative to our current location)
	INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIRS} ${OPENGL_INCLUDE_DIR} "../../library/include")
	LINK_DIRECTORIES("../../library")

	#Build
	ADD_EXECUTABLE(OpenGLExample ${SRC_FILES})
	TARGET_LINK_LIBRARIES(OpenGLExample ${QT_LIBRARIES} ${OPENGL_gl_LIBRARY} debug PolyVoxCore_d.lib optimized PolyVoxCore.lib)

	#Install
	INSTALL(TARGETS OpenGLExample
		RUNTIME DESTINATION bin
		LIBRARY DESTINATION lib
		ARCHIVE DESTINATION lib
		COMPONENT example
	)
	
	ADD_DEPENDENCIES(OpenGLExample PolyVoxCore) #This shouldn't be needed
#ENDIF (WIN32)
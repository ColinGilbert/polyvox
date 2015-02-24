# Copyright (c) 2010-2012 David Williams
# 
# This software is provided 'as-is', without any express or implied
# warranty. In no event will the authors be held liable for any damages
# arising from the use of this software.
# 
# Permission is granted to anyone to use this software for any purpose,
# including commercial applications, and to alter it and redistribute it
# freely, subject to the following restrictions:
# 
# 1. The origin of this software must not be misrepresented; you must not
# claim that you wrote the original software. If you use this software
# in a product, an acknowledgment in the product documentation would be
# appreciated but is not required.
# 
# 2. Altered source versions must be plainly marked as such, and must not be
# misrepresented as being the original software.
# 
# 3. This notice may not be removed or altered from any source
# distribution.

PROJECT(SmoothLODExample)

#Projects source files
SET(SRC_FILES
	main.cpp
	../common/PolyVoxExample.cpp
)

#Projects headers files	
SET(INC_FILES	
	../common/OpenGLWidget.h
	../common/OpenGLWidget.inl
	../common/PolyVoxExample.h
)

#"Sources" and "Headers" are the group names in Visual Studio.
#They may have other uses too...
SOURCE_GROUP("Sources" FILES ${SRC_FILES})
SOURCE_GROUP("Headers" FILES ${INC_FILES})

#Tell CMake the paths for OpenGL and for PolyVox (which is just relative to our current location)
INCLUDE_DIRECTORIES(${OPENGL_INCLUDE_DIR} ${PolyVoxHeaders_SOURCE_DIR} ../common)

#This will include the shader files inside the compiled binary
QT5_ADD_RESOURCES(COMMON_RESOURCES_RCC ../common/example.qrc)

# Put the resources in a seperate folder in Visual Studio
SOURCE_GROUP("Resource Files" FILES ../common/example.qrc ${COMMON_RESOURCES_RCC})

#Build
ADD_EXECUTABLE(SmoothLODExample ${SRC_FILES} ${COMMON_RESOURCES_RCC})
IF(MSVC)
	SET_TARGET_PROPERTIES(SmoothLODExample PROPERTIES COMPILE_FLAGS "/W4 /wd4127") #All warnings
ENDIF(MSVC)
TARGET_LINK_LIBRARIES(SmoothLODExample Qt5::OpenGL)
SET_PROPERTY(TARGET SmoothLODExample PROPERTY FOLDER "Examples")

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

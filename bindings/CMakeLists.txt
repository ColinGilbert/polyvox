# Copyright (c) 2009-2013 Matt Williams
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

option(ENABLE_BINDINGS "Build bindings" OFF) #Off by default
if(ENABLE_BINDINGS)
	find_package(SWIG)
	mark_as_advanced(SWIG_DIR SWIG_VERSION)
	find_package(PythonLibs 3)
	if(CMAKE_VERSION VERSION_LESS "2.8.6")
		set_package_info(SWIG "Bindings generator" http://www.swig.org)
		set_package_info(PythonLibs "Programming language" http://www.python.org)
	else()
		set_package_properties(SWIG PROPERTIES DESCRIPTION "Bindings generator" URL http://www.swig.org)
		set_package_properties(PythonLibs PROPERTIES DESCRIPTION "Programming language" URL http://www.python.org)
	endif()
	if(SWIG_FOUND)
		set(BUILD_BINDINGS ON CACHE BOOL "Will the bindings be built" FORCE)
		include(${SWIG_USE_FILE})
		
		set(CMAKE_SWIG_FLAGS "")
		set_source_files_properties(PolyVoxCore.i PROPERTIES CPLUSPLUS ON)
		
		include_directories(${PolyVoxCore_BINARY_DIR}/include ${PolyVoxCore_SOURCE_DIR}/include ${PolyVoxCore_SOURCE_DIR}/include/PolyVoxCore)
		if(PYTHONLIBS_FOUND)
			include_directories(${PYTHON_INCLUDE_PATH})
			link_directories(${PolyVoxCore_BINARY_DIR})
			
			#set_source_files_properties(PolyVoxCore.i PROPERTIES SWIG_FLAGS "-builtin")
			set(SWIG_MODULE_PolyVoxCorePython_EXTRA_FLAGS "-py3")
			swig_add_module(PolyVoxCorePython python PolyVoxCore.i)
			swig_link_libraries(PolyVoxCorePython ${PYTHON_LIBRARIES} PolyVoxCore)
			set_target_properties(${SWIG_MODULE_PolyVoxCorePython_REAL_NAME} PROPERTIES OUTPUT_NAME _PolyVoxCore)
			#set_target_properties(${SWIG_MODULE_PolyVoxCore_REAL_NAME} PROPERTIES SUFFIX ".pyd")
			SET_PROPERTY(TARGET ${SWIG_MODULE_PolyVoxCorePython_REAL_NAME} PROPERTY FOLDER "Bindings")
		endif()
		
		set(SWIG_MODULE_PolyVoxCoreCSharp_EXTRA_FLAGS "-dllimport;PolyVoxCoreCSharp") #This _should_ be inside UseSWIG.cmake - http://www.cmake.org/Bug/view.php?id=13814
		swig_add_module(PolyVoxCoreCSharp csharp PolyVoxCore.i)
		swig_link_libraries(PolyVoxCoreCSharp PolyVoxCore)
		SET_PROPERTY(TARGET ${SWIG_MODULE_PolyVoxCoreCSharp_REAL_NAME} PROPERTY FOLDER "Bindings")
	else()
		set(BUILD_BINDINGS OFF CACHE BOOL "Will the bindings be built" FORCE)
	endif()
else()
	set(BUILD_BINDINGS OFF CACHE BOOL "Will the bindings be built" FORCE)
endif()
mark_as_advanced(FORCE BUILD_BINDINGS)

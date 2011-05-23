find_package(SWIG)
if(SWIG_FOUND)
	include(${SWIG_USE_FILE})
	
	find_package(PythonLibs)
	include_directories(${PYTHON_INCLUDE_PATH})
	include_directories(${PolyVoxCore_SOURCE_DIR}/include ${PolyVoxCore_SOURCE_DIR}/include/PolyVoxCore)
	link_directories(${PolyVoxCore_BINARY_DIR})

	set(CMAKE_SWIG_FLAGS "")
	set_source_files_properties(PolyVoxCore.i PROPERTIES CPLUSPLUS ON)
	
	swig_add_module(PolyVoxCore python PolyVoxCore.i)
	swig_link_libraries(PolyVoxCore ${PYTHON_LIBRARIES} PolyVoxCore)
	#set_target_properties(${SWIG_MODULE_PolyVoxCore_REAL_NAME} PROPERTIES SUFFIX ".pyd")
endif()
find_package(SWIG)
if(SWIG_FOUND)
	include(${SWIG_USE_FILE})
	
	find_package(PythonLibs)
	include_directories(${PYTHON_INCLUDE_PATH})

	set(CMAKE_SWIG_FLAGS "")
	set_source_files_properties(bindings/PolyVoxCore.i PROPERTIES CPLUSPLUS ON)
	
	swig_add_module(PolyVoxCore python bindings/PolyVoxCore.i)
	swig_link_libraries(PolyVoxCore ${PYTHON_LIBRARIES} PolyVoxCore)
	set_target_properties(${SWIG_MODULE_PolyVoxCore_REAL_NAME} PROPERTIES SUFFIX ".pyd")
endif()
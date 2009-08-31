CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

PROJECT(PolyVox)

add_subdirectory(PolyVoxCore)
add_subdirectory(PolyVoxUtil)

#Set up PolyVoxConfig.cmake
if(WIN32)
	set(CONFIG_FILE_DIR "CMake")
	set(PolyVoxCore_LIBRARY_INSTALL_DIRS "PolyVoxCore/lib")
	set(PolyVoxUtil_LIBRARY_INSTALL_DIRS "PolyVoxUtil/lib")
	set(PolyVoxCore_INCLUDE_INSTALL_DIRS "PolyVoxCore/include")
	set(PolyVoxUtil_INCLUDE_INSTALL_DIRS "PolyVoxUtil/include")
else(WIN32)
	set(CONFIG_FILE_DIR "share/PolyVox/cmake")
	set(PolyVoxCore_LIBRARY_INSTALL_DIRS "lib")
	set(PolyVoxUtil_LIBRARY_INSTALL_DIRS "lib")
	set(PolyVoxCore_INCLUDE_INSTALL_DIRS "include/PolyVoxCore")
	set(PolyVoxUtil_INCLUDE_INSTALL_DIRS "include/PolyVoxUtil")
endif(WIN32)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/PolyVoxConfig.cmake.in ${CMAKE_CURRENT_BINARY_DIR}/PolyVoxConfig.cmake @ONLY)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/PolyVoxConfig.cmake DESTINATION ${CONFIG_FILE_DIR} COMPONENT development)

find_package(Doxygen)
if(DOXYGEN_FOUND)
	#configure_file(${CMAKE_CURRENT_SOURCE_DIR}/polyvox.css ${CMAKE_CURRENT_BINARY_DIR}/polyvox.css)
	configure_file(${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)
	add_custom_target(doc COMMAND ${DOXYGEN_EXECUTABLE} WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR} COMMENT "Building documentation" VERBATIM)
endif()

#add_subdirectory(bindings)

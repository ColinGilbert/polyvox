CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

PROJECT(PolyVox)

#By default only build static libraries on Windows but both on Linux
#If we ever require CMake 2.8 then use http://www.kitware.com/blog/home/post/82
option(BUILD_STATIC_LIBRARIES "Build static libraries" ON)
if(WIN32)
	option(BUILD_DYNAMIC_LIBRARIES "Build dynamic libraries" OFF)
else()
	option(BUILD_DYNAMIC_LIBRARIES "Build dynamic libraries" ON)
endif()

if(WIN32)
	#If both are enabled then diable the dyanamic build
	if(BUILD_STATIC_LIBRARIES AND BUILD_DYNAMIC_LIBRARIES)
		message(STATUS "Building both static and dynamic libraries is not supported on Windows. Disabling dynamic libraries.")
		set(BUILD_DYNAMIC_LIBRARIES OFF CACHE BOOL "Build dynamic libraries" FORCE)
	endif()
	#If both are diabled then re-enable the static build
	if(NOT BUILD_STATIC_LIBRARIES AND NOT BUILD_DYNAMIC_LIBRARIES)
		message(STATUS "Both dynamic and static libraries were disabled - re-enabling static build.")
		set(BUILD_STATIC_LIBRARIES ON CACHE BOOL "Build static libraries" FORCE)
	endif()
else()
	#It's nonsense to disble both so on Linux, re-enable both.
	if(NOT BUILD_STATIC_LIBRARIES AND NOT BUILD_DYNAMIC_LIBRARIES)
		message(STATUS "Both dynamic and static libraries were disabled - re-enabling both.")
		set(BUILD_STATIC_LIBRARIES ON CACHE BOOL "Build static libraries" FORCE)
		set(BUILD_DYNAMIC_LIBRARIES ON CACHE BOOL "Build dynamic libraries" FORCE)
	endif()
endif()

add_subdirectory(PolyVoxCore)
add_subdirectory(PolyVoxUtil)
#add_subdirectory(bindings)

#Set up install paths e.g. for PolyVoxConfig.cmake
if(WIN32)
	set(CONFIG_FILE_DIR "CMake")
	set(PolyVoxCore_LIBRARY_INSTALL_DIRS "PolyVoxCore/lib")
	set(PolyVoxUtil_LIBRARY_INSTALL_DIRS "PolyVoxUtil/lib")
	set(PolyVoxCore_INCLUDE_INSTALL_DIRS "PolyVoxCore/include")
	set(PolyVoxUtil_INCLUDE_INSTALL_DIRS "PolyVoxUtil/include")
	set(PolyVox_DOC_INSTALL_DIR "PolyVox/doc")
else(WIN32)
	set(CONFIG_FILE_DIR "share/PolyVox/cmake")
	set(PolyVoxCore_LIBRARY_INSTALL_DIRS "lib")
	set(PolyVoxUtil_LIBRARY_INSTALL_DIRS "lib")
	set(PolyVoxCore_INCLUDE_INSTALL_DIRS "include/PolyVoxCore")
	set(PolyVoxUtil_INCLUDE_INSTALL_DIRS "include/PolyVoxUtil")
	set(PolyVox_DOC_INSTALL_DIR "share/doc/packages/polyvox")
endif(WIN32)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/PolyVoxConfig.cmake.in ${CMAKE_CURRENT_BINARY_DIR}/PolyVoxConfig.cmake @ONLY)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/PolyVoxConfig.cmake DESTINATION ${CONFIG_FILE_DIR} COMPONENT development)

if(DOXYGEN_FOUND)
	#configure_file(${CMAKE_CURRENT_SOURCE_DIR}/polyvox.css ${CMAKE_CURRENT_BINARY_DIR}/polyvox.css)
	configure_file(${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)
	#This is just the default doc target which only runs Doxygen
	add_custom_target(doc
		COMMAND ${DOXYGEN_EXECUTABLE}
		WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
		COMMENT "Building documentation"
		SOURCES Doxyfile.in polyvox.qhcp.in Mainpage.dox
		VERBATIM
	)
	set_target_properties(doc PROPERTIES PROJECT_LABEL "Documentation") #Set label seen in IDE
	
	#If we found qcollectiongenerator then do more processing
	if(QT_QCOLLECTIONGENERATOR_EXECUTABLE)
		configure_file(${CMAKE_CURRENT_SOURCE_DIR}/polyvox.qhcp.in ${CMAKE_CURRENT_BINARY_DIR}/doc/qthelp/polyvox.qhcp) #The QtHelp config file
		
		#We attach this command to the doc target so it will be run automatically
		add_custom_command(TARGET doc POST_BUILD
			COMMAND ${QT_QCOLLECTIONGENERATOR_EXECUTABLE} polyvox.qhcp -o polyvox.qhc
			WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/doc/qthelp
			COMMENT "Compiling API documentation to Qt Help format"
		)
		
		install(FILES
			${CMAKE_CURRENT_BINARY_DIR}/doc/qthelp/polyvox.qhc
			${CMAKE_CURRENT_BINARY_DIR}/doc/qthelp/polyvox.qch
			DESTINATION ${PolyVox_DOC_INSTALL_DIR}/qthelp
			COMPONENT development
			OPTIONAL
		)
	endif()
endif()

#add_subdirectory(bindings)

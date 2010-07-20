find_program(SPHINXBUILD_EXECUTABLE sphinx-build DOC "The location of the sphinx-build executable")

if(SPHINXBUILD_EXECUTABLE AND QT_QCOLLECTIONGENERATOR_EXECUTABLE)
	message(STATUS "Found `sphinx-build` at ${SPHINXBUILD_EXECUTABLE}")
	set(BUILD_MANUAL YES PARENT_SCOPE)
	
	configure_file(${CMAKE_CURRENT_SOURCE_DIR}/conf.in.py ${CMAKE_CURRENT_BINARY_DIR}/conf.py @ONLY)
	#Generates the HTML docs and the Qt help file which can be opened with "assistant -collectionFile thermite.qhc"
	#add_custom_target(manual ${SPHINXBUILD_EXECUTABLE} -b qthelp ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR} COMMAND ${QT_QCOLLECTIONGENERATOR_EXECUTABLE} polyvox.qhcp -o polyvox.qhc)
	add_custom_target(manual 
		${SPHINXBUILD_EXECUTABLE} -b html
		-c ${CMAKE_CURRENT_BINARY_DIR} #Load the conf.py from the binary dir
		${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR}
		COMMENT "Building PolyVox manual"
	)
	add_dependencies(manual doc)
else()
	message(STATUS "`sphinx-build` was not found. Try setting SPHINXBUILD_EXECUTABLE to its location.")
	set(BUILD_MANUAL NO PARENT_SCOPE)
endif()

# Creates a test from the inputs
#
# Also sets LATEST_TEST to point to the output executable of the test for easy
# ADD_TEST()ing
MACRO(CREATE_TEST headerfile sourcefile executablename)
	UNSET(test_moc_SRCS) #clear out the MOCs from previous tests
	QT4_WRAP_CPP(test_moc_SRCS ${headerfile})
	ADD_EXECUTABLE(${executablename} ${sourcefile} ${test_moc_SRCS})
	TARGET_LINK_LIBRARIES(${executablename} PolyVoxCore PolyVoxUtil ${QT_QTTEST_LIBRARY} ${QT_QTCORE_LIBRARY})
	#HACK. This is needed since everything is built in the base dir in Windows. As of 2.8 we should change this.
	IF(WIN32)
		SET(LATEST_TEST ${EXECUTABLE_OUTPUT_PATH}/${executablename})
	ELSE(WIN32)
		SET(LATEST_TEST ${CMAKE_CURRENT_BINARY_DIR}/${executablename})
	ENDIF(WIN32)
ENDMACRO(CREATE_TEST)

IF(NOT QT_QTTEST_FOUND)
	MESSAGE(STATUS "QtTest not found. Either install it or disable tests by setting BUILD_TESTING to OFF")
ENDIF()

INCLUDE_DIRECTORIES(${PolyVox_SOURCE_DIR}/PolyVoxCore/include ${CMAKE_CURRENT_BINARY_DIR})
REMOVE_DEFINITIONS(-DQT_GUI_LIB) #Make sure the tests don't link to the QtGui

# Array tests
CREATE_TEST(TestArray.h TestArray.cpp TestArray)
ADD_TEST(ArrayReadWriteTest ${LATEST_TEST} testReadWrite)

#Vector tests
CREATE_TEST(testvector.h testvector.cpp testvector)
ADD_TEST(VectorLengthTest ${LATEST_TEST} testLength)
ADD_TEST(VectorDotProductTest ${LATEST_TEST} testDotProduct)

# Volume tests
CREATE_TEST(testvolume.h testvolume.cpp testvolume)
ADD_TEST(VolumeSizeTest ${LATEST_TEST} testSize)

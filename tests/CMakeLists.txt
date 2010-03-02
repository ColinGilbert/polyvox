# Creates a test from the inputs
#
# Also sets LATEST_TEST to point to the output executable of the test for easy
# ADD_TEST()ing
MACRO(CREATE_TEST headerfile sourcefile executablename)
	UNSET(test_moc_SRCS) #clear out the MOCs from previous tests
	QT4_WRAP_CPP(test_moc_SRCS ${headerfile})
	ADD_EXECUTABLE(${executablename} ${sourcefile} ${test_moc_SRCS})
	TARGET_LINK_LIBRARIES(${executablename} PolyVoxCore PolyVoxUtil ${QT_LIBRARIES})
	SET(LATEST_TEST ${CMAKE_CURRENT_BINARY_DIR}/${executablename})
ENDMACRO(CREATE_TEST)

# Find the QTestLib stuff
FIND_PACKAGE(Qt4 REQUIRED)
SET(QT_USE_QTTEST TRUE)
SET(QT_DONT_USE_QTGUI TRUE)
INCLUDE(${QT_USE_FILE})

INCLUDE_DIRECTORIES(${PolyVox_SOURCE_DIR}/PolyVoxCore/include ${CMAKE_CURRENT_BINARY_DIR})

# Volume tests
CREATE_TEST(testvolume.h testvolume.cpp testvolume)
ADD_TEST(VolumeSizeTest ${LATEST_TEST} testSize)

#Vector tests
CREATE_TEST(testvector.h testvector.cpp testvector)
ADD_TEST(VectorLengthTest ${LATEST_TEST} testLength)
ADD_TEST(VectorDotProductTest ${LATEST_TEST} testDotProduct)

FIND_PACKAGE(Qt4)
SET(QT_USE_QTTEST TRUE)
SET(QT_DONT_USE_QTGUI TRUE)
INCLUDE(${QT_USE_FILE})

INCLUDE_DIRECTORIES(${PolyVox_SOURCE_DIR}/PolyVoxCore/include ${CMAKE_CURRENT_BINARY_DIR})

# Volume test executable

SET(volume_SRCS testvolume.cpp)
SET(volume_HDRS testvolume.h)
QT4_WRAP_CPP(volume_moc_SRCS ${volume_HDRS})
ADD_EXECUTABLE(testvolume ${volume_SRCS} ${volume_moc_SRCS})
TARGET_LINK_LIBRARIES(testvolume PolyVoxCore ${QT_LIBRARIES})

# Set up volume tests

ADD_TEST(VolumeSizeTest ${EXECUTABLE_OUTPUT_PATH}/testvolume testSize)



SET(CTEST_START_WITH_EMPTY_BINARY_DIRECTORY TRUE)

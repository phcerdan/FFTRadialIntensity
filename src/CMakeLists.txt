cmake_minimum_required(VERSION 2.6)
set(CMAKE_ECLIPSE_VERSION=4.4)
cmake_policy(SET CMP0012 NEW)
if (UNIX)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}  -Wall -std=c++11")
endif()

# cmake ../src  -G"Eclipse CDT4 - Unix Makefiles" -DCMAKE_BUILD_TYPE=Release -DCMAKE_ECLIPSE_GENERATE_SOURCE_PROJECT=TRUE
project(tem-saxs_p)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
find_package(OpenCV)
include_directories( ${OpenCV_INCLUDE_DIRS})
message(status "** IncludeDirs: ${OpenCV_INCLUDE_DIRS}")
# message(status "** Libraries: ${OpenCV_LIBS}")
add_executable(SAXSsim SAXSsim.cpp)
include_directories(${IPP_LIB_FOLDER})
target_link_libraries(SAXSsim ${OpenCV_LIBS} )

# TEST
### Google TEST AND MOCK
# There must be an environment variable (in .bashrc or profile.d)
# export GMOCK_HOME=/path/to/gmock-1.7.0
set(ENABLE_GOOGLE_TEST "ON" CACHE BOOL "Requires ENV variable GMOCK_HOME pointing to gmock parent folder")
set(GMOCK_HOME "$ENV{GMOCK_HOME}" CACHE PATH "Set this to gmock parent folder")
IF(ENABLE_GOOGLE_TEST)
    find_path(GTEST_INCLUDE_DIR
             gtest/gtest.h
             HINTS ${GMOCK_HOME}/gtest/include
             PATH_SUFFIXES gtest
             DOC "gtest include dir")
    find_path(GMOCK_INCLUDE_DIR
             gmock/gmock.h
             HINTS ${GMOCK_HOME}/include
             PATH_SUFFIXES gmock
             DOC "gmock include dir")
    include_directories(${GMOCK_INCLUDE_DIR} ${GTEST_INCLUDE_DIR})

    find_library(GTEST_LIBRARY
                NAMES gtest
                      libgtest
                      libgtest.a
                HINTS ${GMOCK_HOME}/mybuild/gtest
                      ${GMOCK_HOME}/gtest
                      ${GMOCK_HOME}/gtest/mybuild)
    find_library(GTEST_LIBRARY_MAIN
                NAMES gtest_main
                      libgtest_main
                      libgtest.a
                HINTS ${GMOCK_HOME}/mybuild/gtest
                      ${GMOCK_HOME}/gtest
                      ${GMOCK_HOME}/gtest/mybuild)

    find_library(GMOCK_LIBRARY
                NAMES gmock
                      libgmock
                      libgmock.a
                HINTS ${GMOCK_HOME}/mybuild
                      ${GMOCK_HOME})
    find_library(GMOCK_LIBRARY_MAIN
                NAMES gmock_main
                      libgmock_main
                      libgmock.a
                HINTS ${GMOCK_HOME}/mybuild
                      ${GMOCK_HOME})
ENDIF()

IF(ENABLE_GOOGLE_TEST)
    add_executable(test_SAXSsim test_SAXSsim.cpp)
    # target_link_libraries(gmock-test pthread gmock_main gtest_main)
    target_link_libraries(test_SAXSsim ${GMOCK_LIBRARY_MAIN} ${GTEST_LIBRARY_MAIN})
    target_link_libraries(test_SAXSsim ${OpenCV_LIBS} )
ENDIF()

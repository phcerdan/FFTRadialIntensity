cmake_minimum_required(VERSION 2.6)
set(CMAKE_ECLIPSE_VERSION=4.4)
# cmake ../src  -G"Eclipse CDT4 - Unix Makefiles" -DCMAKE_BUILD_TYPE=Release -DCMAKE_ECLIPSE_GENERATE_SOURCE_PROJECT=TRUE
cmake_policy(SET CMP0012 NEW) # TO avoid ipcv lib error
if (UNIX)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}  -Wall -std=c++11")
endif()

project(tem-saxs_p)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR})
SET(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib)
SET(ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib)

find_package(OpenCV)
include_directories( ${OpenCV_INCLUDE_DIRS})
message(status "** IncludeDirs: ${OpenCV_INCLUDE_DIRS}")
# message(status "** Libraries: ${OpenCV_LIBS}")
find_package(Boost COMPONENTS
        system filesystem
        program_options
        REQUIRED)
include_directories(SYSTEM ${Boost_INCLUDE_DIRS})
add_subdirectory(src)

# TEST
### Google TEST AND MOCK
# There must be an environment variable (in .bashrc or profile.d)
# export GMOCK_HOME=/path/to/gmock-1.7.0
set(ENABLE_GOOGLE_TEST "ON" CACHE BOOL "Requires ENV variable GMOCK_HOME pointing to gmock parent folder")

IF(ENABLE_GOOGLE_TEST)
    add_subdirectory(test)
ENDIF()

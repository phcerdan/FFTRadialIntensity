cmake_minimum_required(VERSION 2.9)
set(CMAKE_ECLIPSE_VERSION=4.4)
# cmake ../src  -G"Eclipse CDT4 - Unix Makefiles" -DCMAKE_BUILD_TYPE=Release -DCMAKE_ECLIPSE_GENERATE_SOURCE_PROJECT=TRUE
cmake_policy(SET CMP0012 NEW) # TO avoid ipcv lib error
if (UNIX)
    if ("${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang")
        list(APPEND CMAKE_CXX_FLAGS "-std=c++11 -stdlib=libc++")
        list(APPEND CMAKE_LINKER_FLAGS "-stdlib=libc++")
    else()
        list(APPEND CMAKE_CXX_FLAGS "-fmessage-length=0  -Wall -std=c++11")
    endif()
endif()

project(tem-saxs_p)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR})
SET(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib)
SET(ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib)

set(CMAKE_POSITION_INDEPENDENT_CODE ON)
find_package(ITK REQUIRED)
include_directories(${ITK_INCLUDE_DIRS})
include(${ITK_USE_FILE})
message(status "** ITK Libraries:  ${ITK_LIBRARIES}")
message(status "** ITK Includes:  ${ITK_INCLUDE_DIRS}")

find_package(Boost COMPONENTS
        system filesystem
        program_options
        REQUIRED )
include_directories(SYSTEM ${Boost_INCLUDE_DIRS})

set(ENABLE_PARALLEL "ON" CACHE BOOL "Enable to use openMP")
set(ENABLE_QT "ON" CACHE BOOL "Enable to use QT interface")
set(ENABLE_R "ON" CACHE BOOL "Enable to use R in QT interface")
#Catch framework header in ./include/catch.hpp
include_directories(SYSTEM ${CMAKE_CURRENT_SOURCE_DIR}/include)
IF(ENABLE_PARALLEL)
    find_package(OpenMP REQUIRED)
    if(${OPENMP_FOUND})
        SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
        SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    else()
        message("OpenMP library not found")
    endif()

    add_definitions(-DENABLE_PARALLEL)
ENDIF(ENABLE_PARALLEL)

IF(ENABLE_R)
    add_definitions(-DENABLE_R)
ENDIF()

IF(ENABLE_QT)
    find_package(Qt5Widgets)
    find_package(Qt5PrintSupport)
    find_package(Qt5Svg)
    find_package(Qt5DBus)
    include_directories(SYSTEM
        ${Qt5Widgets_INCLUDE_DIRS}
        ${Qt5PrintSupport_INCLUDE_DIRS}
        ${Qt5Svg_INCLUDE_DIRS}
        ${Qt5DBus_INCLUDE_DIRS}
        ${VTK_INCLUDE_DIRS})
    add_definitions(-DENABLE_QT)
    message(status "Qt5Widgets: ${Qt5Widgets_INCLUDE_DIRS}")
    add_subdirectory(QT)
ENDIF(ENABLE_QT)


add_subdirectory(2D)

set(ENABLE_TEST "ON" CACHE BOOL "Requires ENV variable GMOCK_HOME pointing to gmock parent folder")

file(COPY "./fixtures" DESTINATION ${EXECUTABLE_OUTPUT_PATH} )

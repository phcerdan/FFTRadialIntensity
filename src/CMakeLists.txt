cmake_minimum_required(VERSION 2.9)
set(CMAKE_ECLIPSE_VERSION=4.4)
# cmake ../src  -G"Eclipse CDT4 - Unix Makefiles" -DCMAKE_BUILD_TYPE=Release -DCMAKE_ECLIPSE_GENERATE_SOURCE_PROJECT=TRUE
cmake_policy(SET CMP0012 NEW) # TO avoid ipcv lib error
if (UNIX)
    # if ("${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang")
        # list(APPEND CMAKE_CXX_FLAGS "-std=c++11 -stdlib=libc++")
        # list(APPEND CMAKE_LINKER_FLAGS "-stdlib=libc++")
    # else()
    list(APPEND CMAKE_CXX_FLAGS "-fmessage-length=0  -Wall")
    # endif()
endif()

project(tem-saxs_p)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR})
set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib)
set(ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib)

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
include_directories(SYSTEM ${CMAKE_CURRENT_SOURCE_DIR}/external)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
if(ENABLE_PARALLEL)
    find_package(OpenMP REQUIRED)
    if(${OPENMP_FOUND})
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    else()
        message("OpenMP library not found")
    endif()

    add_definitions(-DENABLE_PARALLEL)
endif(ENABLE_PARALLEL)

if(ENABLE_R)
    add_definitions(-DENABLE_R)
endif()

if(ENABLE_QT)
    find_package(Qt5Widgets)
    find_package(Qt5PrintSupport)
    find_package(Qt5Svg)
    find_package(Qt5DBus)
    find_package(Qt5WebEngineWidgets)
    find_package(Qt5WebChannel)
    include_directories(SYSTEM
        ${Qt5Widgets_INCLUDE_DIRS}
        ${Qt5PrintSupport_INCLUDE_DIRS}
        ${Qt5Svg_INCLUDE_DIRS}
        ${Qt5DBus_INCLUDE_DIRS}
        ${Qt5WebEngineWidgets_INCLUDE_DIRS}
        ${Qt5WebChannel}
        ${VTK_INCLUDE_DIRS})
    add_definitions(-DENABLE_QT)
    message(status "Qt5Widgets: ${Qt5Widgets_INCLUDE_DIRS}")
endif(ENABLE_QT)

set(ENABLE_TEST "ON" CACHE BOOL "Perform Tests")

if(ENABLE_TEST)
    enable_testing()
endif()

## Common functions. Cannot create a library in subfolder and then share with other subdirectories.
add_library(SAXSsim ${CMAKE_CURRENT_SOURCE_DIR}/src/SAXSsim.cpp)
target_link_libraries(SAXSsim ${Boost_LIBRARIES})
target_link_libraries(SAXSsim ${ITK_LIBRARIES} )
install(TARGETS SAXSsim
    BUNDLE  DESTINATION .
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    )

add_subdirectory(apps)

file(COPY "./fixtures" DESTINATION ${EXECUTABLE_OUTPUT_PATH} )

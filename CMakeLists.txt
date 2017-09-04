cmake_minimum_required(VERSION 2.9)
set(CMAKE_ECLIPSE_VERSION=4.4)
# cmake ../src  -G"Eclipse CDT4 - Unix Makefiles" -DCMAKE_BUILD_TYPE=Release -DCMAKE_ECLIPSE_GENERATE_SOURCE_PROJECT=TRUE
cmake_policy(SET CMP0012 NEW) # TO avoid ipcv lib error
set(CMAKE_CXX_STANDARD 14)
if (UNIX)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fmessage-length=0  -Wall -Wextra")
endif()
message(status "** CMAKE_CXX_FLAGS:  ${CMAKE_CXX_FLAGS}")

project(tem-saxs_p)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR})
set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib)
set(ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib)

set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(EXT_PROJECTS_DIR ${CMAKE_SOURCE_DIR}/external)


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
    include_directories(SYSTEM
        ${Qt5Widgets_INCLUDE_DIRS}
        ${Qt5PrintSupport_INCLUDE_DIRS}
        ${Qt5Svg_INCLUDE_DIRS}
        ${Qt5DBus_INCLUDE_DIRS}
        ${VTK_INCLUDE_DIRS})
    add_definitions(-DENABLE_QT)
    message(status "Qt5Widgets: ${Qt5Widgets_INCLUDE_DIRS}")
endif(ENABLE_QT)

set(ENABLE_TEST "ON" CACHE BOOL "Perform Tests")

if(ENABLE_TEST)
    # Includes Catch in the project:
    add_subdirectory(${EXT_PROJECTS_DIR}/catch)
    include_directories(SYSTEM ${CATCH_INCLUDE_DIR} ${COMMON_INCLUDES})
    enable_testing(true)  # Enables unit-testing.
endif()

# For proper linking in install tree.
SET(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
# the RPATH to be used when installing, but only if it's not a system directory
LIST(FIND CMAKE_PLATFORM_IMPLICIT_LINK_DIRECTORIES "${CMAKE_INSTALL_PREFIX}/lib" isSystemDir)
if("${isSystemDir}" STREQUAL "-1")
    set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")
endif()
## Common functions. Cannot create a library in subfolder and then share with other subdirectories.
add_library(SAXSsim-dev ${CMAKE_CURRENT_SOURCE_DIR}/src/SAXSsim.cpp)
target_link_libraries(SAXSsim-dev ${Boost_LIBRARIES})
target_link_libraries(SAXSsim-dev ${ITK_LIBRARIES} )
install(TARGETS SAXSsim-dev DESTINATION lib OPTIONAL)

add_library(radial_intensity-dev ${CMAKE_CURRENT_SOURCE_DIR}/src/radial_intensity.cpp)
install(TARGETS radial_intensity-dev DESTINATION lib OPTIONAL)

install(DIRECTORY extra/deploy/ DESTINATION share/icons
    FILES_MATCHING
    PATTERN "*.svg"
    PERMISSIONS OWNER_READ OWNER_WRITE GROUP_READ WORLD_READ)
install(DIRECTORY extra/deploy/ DESTINATION share/applications
    FILES_MATCHING
    PATTERN "*.desktop"
    PERMISSIONS OWNER_READ OWNER_WRITE GROUP_READ WORLD_READ)

add_subdirectory(apps)

file(COPY "./fixtures" DESTINATION ${EXECUTABLE_OUTPUT_PATH} )


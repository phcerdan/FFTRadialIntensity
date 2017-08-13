SET( ITK_LIBRARIES ${ITK_LIBRARIES} PARENT_SCOPE )
SET( Qt5Widgets_LIBRARIES ${Qt5Widgets_LIBRARIES} PARENT_SCOPE )
SET( Qt5PrintSupport_LIBRARIES ${Qt5PrintSupport_LIBRARIES} PARENT_SCOPE )
SET( Qt5Svg_LIBRARIES ${Qt5Svg_LIBRARIES} PARENT_SCOPE )
SET( Qt5DBus_LIBRARIES ${Qt5DBus_LIBRARIES} PARENT_SCOPE )

find_package(VTK COMPONENTS vtkGUISupportQt)
message(status "** Libraries VTK: ${VTK_LIBRARIES}")

# Find includes in corresponding build directories. For Eclipse CDT parsing (or eclim)
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
# include_directories(SYSTEM ${QT5Widges_INCLUDE_DIRS} ${QT5PrintSupport_INCLUDE_DIRS}
#     ${VTK_INCLUDE_DIRS})

# Instruct CMake to run moc, ui and rc automatically when needed.

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)
# QT5_WRAP_UI(QT_UI_HEADERS mainwindow.ui)
# QT5_ADD_RESOURCES(QT_RESOURCES resources.qrc)
add_executable(FFTFromImage mainwindow.cpp newdialog.cpp rdialog.cpp WorkerSim.cpp mainQT.cpp resources.qrc)
# include_directories(${IPP_LIB_FOLDER})
target_link_libraries(FFTFromImage
    ${Qt5Widgets_LIBRARIES}
    ${Qt5PrintSupport_LIBRARIES}
    ${Qt5Svg_LIBRARIES}
    ${Qt5DBus_LIBRARIES}
    )
target_link_libraries(FFTFromImage SAXSsim-dev)
target_link_libraries(FFTFromImage ${VTK_LIBRARIES})
file(COPY ${CMAKE_SOURCE_DIR}/scripts DESTINATION ${EXECUTABLE_OUTPUT_PATH} )

#IF WIN
# include(InstallRequiredSystemLibraries)
#END IF
# if(POLICY CMP0026)
#     cmake_policy(SET CMP0026 OLD)
# endif()
# install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} DESTINATION include
#         FILES_MATCHING  PATTERN "*.h"
#         PATTERN "test/"      EXCLUDE
#         PATTERN "resources/" EXCLUDE)
install(TARGETS FFTFromImage
    BUNDLE  DESTINATION .
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    )
# get_target_property(MY_BINARY_LOCATION FFTFromImage LOCATION)
# include(GetPrerequisites)
# get_prerequisites("${MY_BINARY_LOCATION}" DEPENDENCIES 1 1 "" "")
# foreach(DEPENDENCY_FILE ${DEPENDENCIES})
#     gp_resolve_item("\${CMAKE_INSTALL_PREFIX}/FFTFromImage" "${DEPENDENCY_FILE}" "" "" resolved_file)
#     message("resolved_file='${resolved_file}'")
#     install(PROGRAMS ${resolved_file} DESTINATION dependencies)
# endforeach()

# if(APPLE)
#     set(APPS "\${CMAKE_INSTALL_PREFIX}/bin/FFTFromImage")
#     set(YOUR_PLUGINS "") #empty for now
#     set(YOUR_LIBRARY_DIRS "")
#     install(CODE "
#     include(BundleUtilities)
#     fixup_bundle(\"${APPS}\" \"\${YOUR_PLUGINS}\" \"${YOUR_LIBRARY_DIRS}\")
#     ")
#     SET(CPACK_GENERATOR "DropNgo")
# else()
#     SET(CPACK_GENERATOR "ZIP")
# endif()

# include(CPack)

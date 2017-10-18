SET( ITK_LIBRARIES ${ITK_LIBRARIES} PARENT_SCOPE )
SET( Qt5Widgets_LIBRARIES ${Qt5Widgets_LIBRARIES} PARENT_SCOPE )
SET( Qt5PrintSupport_LIBRARIES ${Qt5PrintSupport_LIBRARIES} PARENT_SCOPE )
SET( Qt5Svg_LIBRARIES ${Qt5Svg_LIBRARIES} PARENT_SCOPE )
SET( Qt5DBus_LIBRARIES ${Qt5DBus_LIBRARIES} PARENT_SCOPE )

find_package(VTK COMPONENTS
            vtkGUISupportQt
            vtkChartsCore
            vtkCommonCore
            vtkCommonDataModel
            vtkInteractionImage
            vtkRenderingOpenGL2
            vtkRenderingContextOpenGL2
            vtkViewsContext2D
            NO_MODULE
            )
set_property(DIRECTORY APPEND PROPERTY COMPILE_DEFINITIONS ${VTK_DEFINITIONS})
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
add_executable(FFTRadialIntensity mainwindow.cpp newdialog.cpp radialtabwidget.cpp resources.qrc mainQT.cpp)
# include_directories(${IPP_LIB_FOLDER})
target_link_libraries(FFTRadialIntensity radial_intensity-dev)
target_link_libraries(FFTRadialIntensity
    ${Qt5Widgets_LIBRARIES}
    ${Qt5PrintSupport_LIBRARIES}
    ${Qt5Svg_LIBRARIES}
    ${Qt5DBus_LIBRARIES}
    )
# target_link_libraries(FFTRadialIntensity
target_link_libraries(FFTRadialIntensity ${ITK_LIBRARIES})
target_link_libraries(FFTRadialIntensity ${VTK_LIBRARIES})
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
install(TARGETS FFTRadialIntensity
    BUNDLE  DESTINATION .
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    )
# get_target_property(MY_BINARY_LOCATION FFTRadialIntensity LOCATION)
# include(GetPrerequisites)
# get_prerequisites("${MY_BINARY_LOCATION}" DEPENDENCIES 1 1 "" "")
# foreach(DEPENDENCY_FILE ${DEPENDENCIES})
#     gp_resolve_item("\${CMAKE_INSTALL_PREFIX}/FFTRadialIntensity" "${DEPENDENCY_FILE}" "" "" resolved_file)
#     message("resolved_file='${resolved_file}'")
#     install(PROGRAMS ${resolved_file} DESTINATION dependencies)
# endforeach()

# if(APPLE)
#     set(APPS "\${CMAKE_INSTALL_PREFIX}/bin/FFTRadialIntensity")
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

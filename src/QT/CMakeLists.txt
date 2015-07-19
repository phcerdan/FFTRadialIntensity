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
add_executable(FFTFromImage.exe mainwindow.cpp newdialog.cpp WorkerSim.cpp mainQT.cpp resources.qrc)
# include_directories(${IPP_LIB_FOLDER})
target_link_libraries(FFTFromImage.exe
    ${Qt5Widgets_LIBRARIES}
    ${Qt5PrintSupport_LIBRARIES}
    ${Qt5Svg_LIBRARIES}
    ${Qt5DBus_LIBRARIES}
    )
target_link_libraries(FFTFromImage.exe SAXSsim)
target_link_libraries(FFTFromImage.exe ${VTK_LIBRARIES})
# SET(ENABLE_GOOGLE_TEST ${ENABLE_GOOGLE_TEST} PARENT_SCOPE)
# IF(ENABLE_GOOGLE_TEST)
#     add_subdirectory(test)
#     add_subdirectory(singleImagesFromMontage)
# ENDIF()

#IF WIN
# include(InstallRequiredSystemLibraries)
#END IF
INSTALL(TARGETS FFTFromImage.exe
    BUNDLE  DESTINATION lib_external  COMPONENT Runtime
    RUNTIME DESTINATION bin COMPONENT Runtime
    LIBRARY DESTINATION lib COMPONENT Runtime
    ARCHIVE DESTINATION lib COMPONENT Runtime
    )
SET(APPS "\${CMAKE_INSTALL_PREFIX}/bin/FFTFromImage.exe")
set(YOUR_PLUGINS "") #empty for now
set(YOUR_LIBRARY_DIRS "")
install(CODE "
    include(BundleUtilities)
    fixup_bundle(\"${APPS}\" \"\${YOUR_PLUGINS}\" \"${YOUR_LIBRARY_DIRS}\")
    ")

SET(CPACK_GENERATOR "ZIP")
SET(CPACK_DEBIAN_PACKAGE_MAINTAINER "Pablo Hernandez") #required
include(CPack)

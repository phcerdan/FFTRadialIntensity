SET( ITK_LIBRARIES ${ITK_LIBRARIES} PARENT_SCOPE )
SET( Qt5Widgets_LIBRARIES ${Qt5Widgets_LIBRARIES} PARENT_SCOPE )
SET( Qt5PrintSupport_LIBRARIES ${Qt5PrintSupport_LIBRARIES} PARENT_SCOPE )

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
add_executable(FFTFromImage.exe mainwindow.cpp Q_DebugStream.h newdialog.cpp WorkerSim.cpp mainQT.cpp resources.qrc)
# include_directories(${IPP_LIB_FOLDER})
target_link_libraries(FFTFromImage.exe  ${Qt5Widgets_LIBRARIES} ${Qt5PrintSupport_LIBRARIES})
target_link_libraries(FFTFromImage.exe SAXSsim)
target_link_libraries(FFTFromImage.exe ${VTK_LIBRARIES})
message(status "** Libraries QT: ${Qt5Widgets_LIBRARIES} ${Qt5PrintSupport_LIBRARIES}")

# SET(ENABLE_GOOGLE_TEST ${ENABLE_GOOGLE_TEST} PARENT_SCOPE)
# IF(ENABLE_GOOGLE_TEST)
#     add_subdirectory(test)
#     add_subdirectory(singleImagesFromMontage)
# ENDIF()

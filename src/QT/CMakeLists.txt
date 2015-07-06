# SET( OpenCV_LIBS ${OpenCV_LIBS} PARENT_SCOPE )
# SET( ITK_LIBRARIES ${ITK_LIBRARIES} PARENT_SCOPE )
# SET( Boost_LIBRARIES ${Boost_LIBRARIES} PARENT_SCOPE )

find_package(Qt5Widgets)
find_package(Qt5PrintSupport)

# Find includes in corresponding build directories. For Eclipse CDT parsing (or eclim)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(SYSTEM ${QT5Widges_INCLUDE_DIRS} ${QT5PrintSupport_INCLUDE_DIRS})

# Instruct CMake to run moc automatically when needed.
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
# QT5_WRAP_UI(QT_UI_HEADERS mainwindow.ui)
QT5_ADD_RESOURCES(QT_RESOURCES resources.qrc)
add_executable(FFTFromImage.exe mainwindow.cpp mainQT.cpp)
# include_directories(${IPP_LIB_FOLDER})
target_link_libraries(FFTFromImage.exe  Qt5::Widgets Qt5::PrintSupport)
target_link_libraries(FFTFromImage.exe SAXSsim)
message(status "** Libraries QT: ${Qt5Widgets_LIBRARIES} ${Qt5PrintSupport_LIBRARIES}")

# SET(ENABLE_GOOGLE_TEST ${ENABLE_GOOGLE_TEST} PARENT_SCOPE)
# IF(ENABLE_GOOGLE_TEST)
#     add_subdirectory(test)
#     add_subdirectory(singleImagesFromMontage)
# ENDIF()

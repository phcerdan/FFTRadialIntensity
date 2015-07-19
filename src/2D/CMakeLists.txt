SET( ITK_LIBRARIES ${ITK_LIBRARIES} PARENT_SCOPE )
SET( Boost_LIBRARIES ${Boost_LIBRARIES} PARENT_SCOPE )
SET( ENABLE_QT ${ENABLE_QT} PARENT_SCOPE)

    set(CMAKE_AUTOMOC ON)
    set(CMAKE_AUTOUIC ON)
    set(CMAKE_AUTORCC ON)
add_library(SAXSsim STATIC SAXSsim.cpp ../QT/QtMessenger.h)
target_link_libraries(SAXSsim ${Boost_LIBRARIES})
target_link_libraries(SAXSsim ${ITK_LIBRARIES} )

IF(ENABLE_QT)
    SET( Qt5Widgets_LIBRARIES ${Qt5Widgets_LIBRARIES} PARENT_SCOPE )
    SET( Qt5PrintSupport_LIBRARIES ${Qt5PrintSupport_LIBRARIES} PARENT_SCOPE )
    SET( Qt5Svg_LIBRARIES ${Qt5Svg_LIBRARIES} PARENT_SCOPE )
    SET( Qt5DBus_LIBRARIES ${Qt5DBus_LIBRARIES} PARENT_SCOPE )
    target_link_libraries(SAXSsim
        ${Qt5Widgets_LIBRARIES}
        ${Qt5PrintSupport_LIBRARIES}
        ${Qt5Svg_LIBRARIES}
        ${Qt5DBus_LIBRARIES}
        )
ENDIF()

add_executable(SAXSsim.exe main.cpp)
# include_directories(${IPP_LIB_FOLDER})
target_link_libraries(SAXSsim.exe SAXSsim)


SET(ENABLE_GOOGLE_TEST ${ENABLE_GOOGLE_TEST} PARENT_SCOPE)
IF(ENABLE_GOOGLE_TEST)
    add_subdirectory(test)
    add_subdirectory(singleImagesFromMontage)
ENDIF()

INSTALL(TARGETS SAXSsim.exe SAXSsim
    BUNDLE  DESTINATION lib_external COMPONENT Runtime
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

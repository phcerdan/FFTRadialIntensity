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
    target_link_libraries(SAXSsim ${Qt5Widgets_LIBRARIES} ${Qt5PrintSupport_LIBRARIES} )
ENDIF()

add_executable(SAXSsim.exe main.cpp)
# include_directories(${IPP_LIB_FOLDER})
target_link_libraries(SAXSsim.exe SAXSsim)

install(TARGETS SAXSsim.exe SAXSsim
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    )

install(TARGETS ${QT5Widget_LIBRARIES} ${QT5PrintSupport_LIBRARIES}
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    )
SET(ENABLE_GOOGLE_TEST ${ENABLE_GOOGLE_TEST} PARENT_SCOPE)
IF(ENABLE_GOOGLE_TEST)
    add_subdirectory(test)
    add_subdirectory(singleImagesFromMontage)
ENDIF()

INCLUDE(InstallRequiredSystemLibraries)

SET(CPACK_GENERATOR "TGZ")
SET(CPACK_PACKAGE_DESCRIPTION_SUMMARY "My funky project")
SET(CPACK_PACKAGE_VENDOR "Me, myself, and I")
SET(CPACK_PACKAGE_DESCRIPTION_FILE "${CMAKE_CURRENT_SOURCE_DIR}/../../Readme.md")
SET(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/../../Licence.txt")
SET(CPACK_PACKAGE_VERSION_MAJOR "1")
SET(CPACK_PACKAGE_VERSION_MINOR "3")
SET(CPACK_PACKAGE_VERSION_PATCH "2")
SET(CPACK_PACKAGE_INSTALL_DIRECTORY "CMake ${CMake_VERSION_MAJOR}.${CMake_VERSION_MINOR}")
IF(WIN32 AND NOT UNIX)
  # There is a bug in NSI that does not handle full unix paths properly. Make
  # sure there is at least one set of four (4) backlasshes.
  # SET(CPACK_PACKAGE_ICON "${CMake_SOURCE_DIR}/Utilities/Release\\\\InstallIcon.bmp")
  SET(CPACK_NSIS_INSTALLED_ICON_NAME "bin\\\\SAXSsim.exe")
  SET(CPACK_NSIS_DISPLAY_NAME "${CPACK_PACKAGE_INSTALL_DIRECTORY} FFT from Image")
  SET(CPACK_NSIS_HELP_LINK "http:\\\\\\\\www.my-project-home-page.org")
  SET(CPACK_NSIS_URL_INFO_ABOUT "http:\\\\\\\\www.my-personal-home-page.com")
  SET(CPACK_NSIS_CONTACT "me@my-personal-home-page.com")
  SET(CPACK_NSIS_MODIFY_PATH ON)
ELSE(WIN32 AND NOT UNIX)
  SET(CPACK_STRIP_FILES "bin/SAXSsim.exe")
  SET(CPACK_SOURCE_STRIP_FILES "")
ENDIF(WIN32 AND NOT UNIX)
SET(CPACK_PACKAGE_EXECUTABLES "SAXSsim.exe")
INCLUDE(CPack)

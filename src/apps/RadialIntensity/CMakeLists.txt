add_executable(SAXSsim.exe radial_intensity.cpp)
# include_directories(${IPP_LIB_FOLDER})
target_link_libraries(SAXSsim.exe SAXSsim)

IF(ENABLE_TEST)
    add_subdirectory(test)
ENDIF()

install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} DESTINATION include
    FILES_MATCHING PATTERN "SAXSsim.h"
    PATTERN "test/"       EXCLUDE
    REGEX "singleImages*/" EXCLUDE)
install(DIRECTORY ../scripts/ DESTINATION .
        USE_SOURCE_PERMISSIONS
        PATTERN "*plotI-q_SVG*" EXCLUDE)
install(DIRECTORY ../fixtures DESTINATION .)

install(PROGRAMS ../scripts/plotI-q_SVG.R DESTINATION bin)

install(TARGETS SAXSsim.exe
    BUNDLE  DESTINATION .
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    )
if(APPLE)
    set(APPS "\${CMAKE_INSTALL_PREFIX}/bin/SAXSsim.exe")
    set(YOUR_PLUGINS "") #empty for now
    set(YOUR_LIBRARY_DIRS "")
    install(CODE "
        include(BundleUtilities)
        fixup_bundle(\"${APPS}\" \"\${YOUR_PLUGINS}\" \"${YOUR_LIBRARY_DIRS}\")
        ")
endif()

add_executable(SAXSsim.exe SAXSsim_class.cpp)
# include_directories(${IPP_LIB_FOLDER})
target_link_libraries(SAXSsim.exe SAXSsim)

add_executable(SAXSsim_functional.exe SAXSsim_functional.cpp)
# include_directories(${IPP_LIB_FOLDER})
target_link_libraries(SAXSsim_functional.exe SAXSsim)

IF(ENABLE_TEST)
    add_subdirectory(test)
ENDIF()

install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} DESTINATION include
    FILES_MATCHING PATTERN "SAXSsim.h"
    PATTERN "test/"       EXCLUDE
    REGEX "singleImages*/" EXCLUDE)
install(DIRECTORY ${CMAKE_SOURCE_DIR}/scripts/ DESTINATION bin
        USE_SOURCE_PERMISSIONS
        PATTERN "*plotI-q_SVG*" EXCLUDE)
install(DIRECTORY ${CMAKE_SOURCE_DIR}/fixtures DESTINATION bin)

install(PROGRAMS ${CMAKE_SOURCE_DIR}/scripts/plotI-q.R DESTINATION bin)

install(TARGETS SAXSsim.exe
    DESTINATION bin OPTIONAL)
if(APPLE)
    set(APPS "\${CMAKE_INSTALL_PREFIX}/bin/SAXSsim.exe")
    set(YOUR_PLUGINS "") #empty for now
    set(YOUR_LIBRARY_DIRS "")
    install(CODE "
        include(BundleUtilities)
        fixup_bundle(\"${APPS}\" \"\${YOUR_PLUGINS}\" \"${YOUR_LIBRARY_DIRS}\")
        ")
endif()

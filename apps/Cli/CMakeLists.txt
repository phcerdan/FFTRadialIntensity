add_executable(SAXSsim_functional SAXSsim_functional.cpp)
target_link_libraries(SAXSsim_functional radial_intensity-dev)
target_link_libraries(SAXSsim_functional ${ITK_LIBRARIES})
target_link_libraries(SAXSsim_functional ${Boost_LIBRARIES})

IF(ENABLE_TEST)
    add_subdirectory(test)
ENDIF()

# install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} DESTINATION include
#     FILES_MATCHING PATTERN "SAXSsim.h"
#     PATTERN "test/"       EXCLUDE
#     REGEX "singleImages*/" EXCLUDE)
# install(DIRECTORY ${CMAKE_SOURCE_DIR}/scripts/ DESTINATION bin
#         USE_SOURCE_PERMISSIONS
#         PATTERN "*plotI-q_SVG*" EXCLUDE)
# install(DIRECTORY ${CMAKE_SOURCE_DIR}/fixtures DESTINATION bin)
#
# install(PROGRAMS ${CMAKE_SOURCE_DIR}/scripts/plotI-q.R DESTINATION bin)
#
# install(TARGETS SAXSsim
#     DESTINATION bin OPTIONAL)
# if(APPLE)
#     set(APPS "\${CMAKE_INSTALL_PREFIX}/bin/SAXSsim")
#     set(YOUR_PLUGINS "") #empty for now
#     set(YOUR_LIBRARY_DIRS "")
#     install(CODE "
#         include(BundleUtilities)
#         fixup_bundle(\"${APPS}\" \"\${YOUR_PLUGINS}\" \"${YOUR_LIBRARY_DIRS}\")
#         ")
# endif()

include_directories("..")
SET(ENABLE_QT ${ENABLE_QT} PARENT_SCOPE)

add_executable(test_SAXSsim_functional test_main.cpp test_SAXSsim_functional.cpp)
target_link_libraries(test_SAXSsim_functional radial_intensity-dev)
target_link_libraries(test_SAXSsim_functional ${ITK_LIBRARIES})
target_link_libraries(test_SAXSsim_functional ${Boost_LIBRARIES})
add_dependencies(test_SAXSsim_functional catch)

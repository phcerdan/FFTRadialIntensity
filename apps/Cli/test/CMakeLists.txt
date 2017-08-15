include_directories("..")
SET(ENABLE_QT ${ENABLE_QT} PARENT_SCOPE)

add_executable(test_SAXSsim_class test_main.cpp test_SAXSsim_class.cpp)
set_source_files_properties (test_SAXSsim_class PROPERTIES COMPILE_FLAGS "-Wno-sign-compare")
target_link_libraries(test_SAXSsim_class SAXSsim-dev)

add_dependencies(test_SAXSsim_class catch)

add_executable(test_SAXSsim_functional test_main.cpp test_SAXSsim_functional.cpp)
target_link_libraries(test_SAXSsim_functional SAXSsim-dev)

add_dependencies(test_SAXSsim_functional catch)

include_directories("..")
SET(ENABLE_QT ${ENABLE_QT} PARENT_SCOPE)

add_executable(test_SAXSsim.exe test_main.cpp test_SAXSsim.cpp)
set_source_files_properties (test_SAXSsim.exe PROPERTIES COMPILE_FLAGS "-Wno-sign-compare")
target_link_libraries(test_SAXSsim.exe SAXSsim)

add_executable(test_saxs_sim_functional.exe test_main.cpp test_saxs_sim_functional.cpp)
target_link_libraries(test_saxs_sim_functional.exe SAXSsim)

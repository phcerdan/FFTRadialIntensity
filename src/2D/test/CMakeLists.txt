include_directories(SYSTEM ${GMOCK_INCLUDE_DIR} ${GTEST_INCLUDE_DIR})
include_directories("..")
file(COPY "../../fixtures" DESTINATION ${EXECUTABLE_OUTPUT_PATH} )
SET(ENABLE_QT ${ENABLE_QT} PARENT_SCOPE)

add_executable(test_SAXSsim.exe test_SAXSsim.cpp)
set_source_files_properties (test_SAXSsim.exe PROPERTIES COMPILE_FLAGS "-Wno-sign-compare")
target_link_libraries(test_SAXSsim.exe SAXSsim)
target_link_libraries(test_SAXSsim.exe ${GMOCK_LIBRARY_MAIN} ${GTEST_LIBRARY_MAIN} ${CMAKE_THREAD_LIBS_INIT} )

add_executable(test_SAXSsimCatch.exe test_main.cpp test_SAXSsimCatch.cpp)
set_source_files_properties (test_SAXSsimCatch.exe PROPERTIES COMPILE_FLAGS "-Wno-sign-compare")
target_link_libraries(test_SAXSsimCatch.exe SAXSsim)

SET( OpenCV_LIBS ${OpenCV_LIBS} PARENT_SCOPE )
SET( ITK_LIBRARIES ${ITK_LIBRARIES} PARENT_SCOPE )
SET( Boost_LIBRARIES ${Boost_LIBRARIES} PARENT_SCOPE )

add_library(SAXSsim STATIC SAXSsim.cpp)
target_link_libraries(SAXSsim ${Boost_LIBRARIES} ${OpenCV_LIBS})
target_link_libraries(SAXSsim ${ITK_LIBRARIES} )

add_executable(SAXSsim.exe main.cpp)
# include_directories(${IPP_LIB_FOLDER})
target_link_libraries(SAXSsim.exe  )
target_link_libraries(SAXSsim.exe SAXSsim)
message(status "** Libraries: ${OpenCV_LIBS}")

SET(ENABLE_GOOGLE_TEST ${ENABLE_GOOGLE_TEST} PARENT_SCOPE)
IF(ENABLE_GOOGLE_TEST)
    add_subdirectory(test)
ENDIF()

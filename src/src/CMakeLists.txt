SET( OpenCV_LIBS ${OpenCV_LIBS} PARENT_SCOPE )
SET( Boost_LIBRARIES ${Boost_LIBRARIES} PARENT_SCOPE )
# SET( histo-header_LIBRARIES ${histo-header_LIBRARY} PARENT_SCOPE)

add_library(SAXSsim SAXSsim.cpp )
add_executable(SAXSsim.exe main.cpp)
include_directories(${IPP_LIB_FOLDER})
target_link_libraries(SAXSsim.exe ${OpenCV_LIBS} ${Boost_LIBRARIES} )
target_link_libraries(SAXSsim.exe SAXSsim)
message(status "** Libraries: ${OpenCV_LIBS}")
message(status "** Libraries: ${histo-header_LIBRARY}")


add_library(SAXSsim SAXSsim.cpp GLOBAL)
target_link_libraries(SAXSsim ${Boost_LIBRARIES})
target_link_libraries(SAXSsim ${ITK_LIBRARIES} )


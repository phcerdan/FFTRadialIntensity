add_library(SAXSsim-dev SAXSsim.cpp GLOBAL)
target_link_libraries(SAXSsim-dev ${Boost_LIBRARIES})
target_link_libraries(SAXSsim-dev ${ITK_LIBRARIES} )


SET( ITK_LIBRARIES ${ITK_LIBRARIES} PARENT_SCOPE )
SET( Boost_LIBRARIES ${Boost_LIBRARIES} PARENT_SCOPE )
SET( ENABLE_QT ${ENABLE_QT} PARENT_SCOPE)

add_library(SAXSsim STATIC SAXSsim.cpp)
target_link_libraries(SAXSsim ${Boost_LIBRARIES})
target_link_libraries(SAXSsim ${ITK_LIBRARIES} )
target_link_libraries(SAXSsim ${Qt5Widgets_LIBRARIES} ${Qt5PrintSupport_LIBRARIES} )

IF(ENABLE_QT)
    SET( Qt5Widgets_LIBRARIES ${Qt5Widgets_LIBRARIES} PARENT_SCOPE )
    SET( Qt5PrintSupport_LIBRARIES ${Qt5PrintSupport_LIBRARIES} PARENT_SCOPE )
    target_link_libraries(SAXSsim ${Qt5Widgets_LIBRARIES} ${Qt5PrintSupport_LIBRARIES} )
ENDIF()

add_executable(SAXSsim.exe main.cpp)
# include_directories(${IPP_LIB_FOLDER})
target_link_libraries(SAXSsim.exe SAXSsim)
message(status "** Libraries: ${OpenCV_LIBS}")


SET(ENABLE_GOOGLE_TEST ${ENABLE_GOOGLE_TEST} PARENT_SCOPE)
IF(ENABLE_GOOGLE_TEST)
    add_subdirectory(test)
    add_subdirectory(singleImagesFromMontage)
ENDIF()

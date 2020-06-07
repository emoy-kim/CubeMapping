target_link_libraries(CubeMapping glad glfw3dll)

if(${CMAKE_BUILD_TYPE} MATCHES Debug)
   target_link_libraries(CubeMapping FreeImaged opencv_cored opencv_imgprocd opencv_imgcodecsd opencv_videoiod)
else()
   target_link_libraries(CubeMapping FreeImage opencv_core opencv_imgproc opencv_imgcodecs opencv_videoio)
endif()
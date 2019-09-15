cmake_minimum_required(VERSION 2.8)
project(streaming)

SET(CMAKE_CXX_FLAGS "-std=c++0x")
set (PLATFORM_LIBS pthread ${CMAKE_DL_LIBS})
find_package(OpenCV REQUIRED)
find_package(yaml-cpp REQUIRED)

include_directories(include) 

#add_executable( server "src/server.cpp" "src/socket.cpp" "src/camera_node.cpp" )
#target_link_libraries( server ${OpenCV_LIBS} ${PLATFORM_LIBS} ${YAML_CPP_LIBRARIES})

add_executable( client "src/client.cpp" "src/socket.cpp" "src/camera_node.cpp" "src/client_node.cpp" "src/video.cpp")
target_link_libraries( client ${OpenCV_LIBS} ${PLATFORM_LIBS} ${YAML_CPP_LIBRARIES})



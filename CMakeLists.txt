cmake_minimum_required(VERSION 3.1)
project( badApple )
find_package( OpenCV REQUIRED )
find_package(X11 REQUIRED)
include_directories(${X11_INCLUDE_DIR})
include_directories( ${OpenCV_INCLUDE_DIRS} )
add_executable( badApple badApple.cpp )
target_link_libraries( badApple ${OpenCV_LIBS} )


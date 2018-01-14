cmake_minimum_required(VERSION 2.8)
project( AD )
SET(CMAKE_CXX_FLAGS "-std=c++0x")
find_package( OpenCV REQUIRED )
add_executable( AD 4th.cpp sort.h )
target_link_libraries( AD ${OpenCV_LIBS} )



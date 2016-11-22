cmake_minimum_required(VERSION 2.8)
project(Depth2XYX )
find_package( OpenCV REQUIRED )


add_executable( Depth2XYX  Depth2XYX.cpp )
target_link_libraries( Depth2XYX  ${OpenCV_LIBS} )

cmake_minimum_required(VERSION 2.8)
project( PID )

set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11" )
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fopenmp")

aux_source_directory(./src/ SRC_LIST)
add_executable(radar ${SRC_LIST})

include_directories( include/canbus)
include_directories( include/)
include_directories( src/ )

find_package( OpenCV REQUIRED )
include_directories( ${OpenCV_INCLUDE_DIRS} )

target_link_libraries( radar ${OpenCV_LIBS})



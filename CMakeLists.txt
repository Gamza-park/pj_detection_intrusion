cmake_minimum_required(VERSION 2.8)
project(OpenCV)
 
find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS} user/local/include/opencv4 /user/local/Cellar/opencv)

add_subdirectory(src)

cmake_minimum_required(VERSION 3.2)
project(patchmatch2)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
set(SOURCE_FILES src/main.cpp)
set(OpenCV_DIR "/opt/opencv/share/OpenCV")

find_package(OpenCV REQUIRED core highgui imgproc)
include_directories(${OpenCV_INCLUDE_DIRS})

add_executable(patchmatch2 ${SOURCE_FILES} src/patchmatch.h src/patchmatch.cpp src/util.h src/util.cpp src/ssd.h src/ssd.cpp src/pyramid.h src/pyramid.cpp src/main.h)

target_link_libraries(patchmatch2 ${OpenCV_LIBS})
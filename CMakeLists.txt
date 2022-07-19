cmake_minimum_required(VERSION 3.16)

project(GstreamCustom) #Project name

set(CMAKE_CXX_STANDARD 11) #setting C++ 14 standard
find_package(PkgConfig) #finding pkg-config is a helper tool

#using pkg-config to getting Gstreamer
pkg_check_modules(GSTREAMER REQUIRED gstreamer-1.0)

#including GStreamer header files directory
include_directories(
        ${GLIB_INCLUDE_DIRS}
        ${GSTREAMER_INCLUDE_DIRS}
)

#linking GStreamer library directory
link_directories(
        ${GLIB_LIBRARY_DIRS}
        ${GSTREAMER_LIBRARY_DIRS}
)

#building target executable
add_executable(exp1 gstream1.cpp)

add_executable(exp2 gstream2.cpp)

add_executable(zed zedstream.cpp)

#linking Gstreamer library with target executable
target_link_libraries(exp1 ${GSTREAMER_LIBRARIES})

target_link_libraries(exp2 ${GSTREAMER_LIBRARIES})

target_link_libraries(zed ${GSTREAMER_LIBRARIES})
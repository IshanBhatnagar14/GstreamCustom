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
add_executable(${PROJECT_NAME} gstream1.cpp)

add_executable(${PROJECT_NAME}_2 gstream2.cpp)

#linking Gstreamer library with target executable
target_link_libraries(${PROJECT_NAME} ${GSTREAMER_LIBRARIES})

target_link_libraries(${PROJECT_NAME}_2 ${GSTREAMER_LIBRARIES})
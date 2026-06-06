# Sets the minimum CMake version required for this project.
cmake_minimum_required(VERSION 3.22.1)

# Declares the project name.
project("practica2_visioncomputador")

# Manual OpenCV configuration to avoid broken path calculations in OpenCVConfig.cmake
# CMAKE_SOURCE_DIR is app/src/main/cpp
set(OPENCV_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/../../../../opencv/native/jni/include)
include_directories(${OPENCV_INCLUDE_DIR})

# Creates and names a library, sets it as either SHARED,
# and provides the relative paths to its source code.
add_library(${CMAKE_PROJECT_NAME} SHARED
        native-lib.cpp)

# Specifies libraries CMake should link to your target library.
# We link to the opencv_java4 shared library provided in the opencv module.
target_link_libraries(${CMAKE_PROJECT_NAME}
        ${CMAKE_SOURCE_DIR}/../../../../opencv/native/libs/${ANDROID_ABI}/libopencv_java4.so
        android
        log)
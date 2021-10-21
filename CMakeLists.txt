cmake_minimum_required(VERSION 3.1)
project(openvision)

set(CMAKE_BUILD_TYPE Release)
set(PROJECT_SOURCE_DIR, src)
set(PROJECT_BINARY_DIR, build)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR})
set(CMAKE_BUILD_FILES_DIRECTORY ${PROJECT_BINARY_DIR})
set(CMAKE_BUILD_DIRECTORY ${PROJECT_BINARY_DIR})
set(CMAKE_BINARY_DIR  ${PROJECT_BINARY_DIR})
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/../bin)
set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/../lib)
set(INCLUDE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/../include)
set(CMAKE_CACHEFILE_DIR ${PROJECT_BINARY_DIR})

Option(MIRROR_OPENMP "openmp support" ON)
Option(MIRROR_VULKAN "vulkan compute used" ON)
Option(MIRROR_BUILD_CLASSIFIER "build classifier test" ON)

add_subdirectory(ncnn)

add_subdirectory(src)
# add_subdirectory(examples)

file(GLOB_RECURSE SRC_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cxx
)

message(${SRC_FILES})

# list(APPEND SRCS ${LAYER_ARCH_SRC})
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -O2 -fPIC -std=c++11 -fopenmp")
add_library(openvision STATIC ${SRC_FILES})
target_link_libraries(openvision PUBLIC ncnn)

if(OV_OPENMP)
    find_package(OpenMP)
    if(NOT TARGET OpenMP::OpenMP_CXX AND (OpenMP_CXX_FOUND OR OPENMP_FOUND))
        target_compile_options(openvision PRIVATE ${OpenMP_CXX_FLAGS})
    endif()
endif()

if(OV_OPENMP AND OpenMP_CXX_FOUND)
    message("Building with OpenMP")
    target_link_libraries(openvision PUBLIC OpenMP::OpenMP_CXX)
endif()

if(NCNN_VULKAN)
    find_package(Vulkan REQUIRED)
    target_link_libraries(openvision PUBLIC Vulkan::Vulkan)
    set(OV_VULKAN ON)
else()
    set(OV_VULKAN OFF)
endif()

configure_file (
  "${CMAKE_CURRENT_SOURCE_DIR}/common/config.h.in"
  "${CMAKE_CURRENT_SOURCE_DIR}/common/config.h"
)

target_include_directories(openvision
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../include>
        $<INSTALL_INTERFACE:include/openvision>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/common>

        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/face>
        #$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/face/align>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/face/common>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/face/detecter>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/face/detecter/centerface>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/face/detecter/mtcnn>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/face/detecter/anticonv>

        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/face/landmarker>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/face/landmarker/zqlandmarker>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/face/landmarker/insightface>

        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/face/recognizer>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/face/recognizer/mobilefacenet>

        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/face/tracker>

        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/face/hopenet>

        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/hand>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/hand/common>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/hand/detecter>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/hand/pose>

        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/pose>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/pose/common>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/pose/detecter>
)

#install(TARGETS openvision EXPORT openvision ARCHIVE DESTINATION ${LIBRARY_OUTPUT_PATH})
file(COPY
    ${CMAKE_CURRENT_SOURCE_DIR}/common/common.h
    DESTINATION ${INCLUDE_OUTPUT_PATH}/openvision/common
)

file(COPY
    ${CMAKE_CURRENT_SOURCE_DIR}/face/common.h
    ${CMAKE_CURRENT_SOURCE_DIR}/face/detecter.h
    ${CMAKE_CURRENT_SOURCE_DIR}/face/landmarker.h
    ${CMAKE_CURRENT_SOURCE_DIR}/face/recognizer.h
    ${CMAKE_CURRENT_SOURCE_DIR}/face/tracker.h
    ${CMAKE_CURRENT_SOURCE_DIR}/face/hopenet.h
    DESTINATION ${INCLUDE_OUTPUT_PATH}/openvision/face
)

file(COPY
    ${CMAKE_CURRENT_SOURCE_DIR}/hand/common.h
    ${CMAKE_CURRENT_SOURCE_DIR}/hand/detecter.h
    ${CMAKE_CURRENT_SOURCE_DIR}/hand/pose.h
    DESTINATION ${INCLUDE_OUTPUT_PATH}/openvision/hand
)

file(COPY
    ${CMAKE_CURRENT_SOURCE_DIR}/pose/common.h
    ${CMAKE_CURRENT_SOURCE_DIR}/pose/detecter.h
    DESTINATION ${INCLUDE_OUTPUT_PATH}/openvision/pose
)


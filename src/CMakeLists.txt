file(GLOB_RECURSE SRC_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
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

if (OV_VULKAN)
# Begin compile realsgran shader
find_program(GLSLANGVALIDATOR_EXECUTABLE NAMES glslangValidator PATHS $ENV{VULKAN_SDK}/bin NO_CMAKE_FIND_ROOT_PATH)
message(STATUS "Found glslangValidator: ${GLSLANGVALIDATOR_EXECUTABLE}")

macro(compile_shader SHADER_SRC)
    set(SHADER_SRC_FULLPATH ${CMAKE_CURRENT_SOURCE_DIR}/realesrgan/${SHADER_SRC})
    set(SHADER_OUT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/realesrgan/shader)

    get_filename_component(SHADER_SRC_NAME_WE ${SHADER_SRC} NAME_WE)
    set(SHADER_SPV_HEX_FILE ${SHADER_OUT_PATH}/${SHADER_SRC_NAME_WE}.spv.hex.h)
    add_custom_command(
        OUTPUT ${SHADER_SPV_HEX_FILE}
        COMMAND ${GLSLANGVALIDATOR_EXECUTABLE}
        ARGS -V -s -x -o ${SHADER_SPV_HEX_FILE} ${SHADER_SRC_FULLPATH}
        DEPENDS ${SHADER_SRC_FULLPATH}
        COMMENT "Building SPIR-V module ${SHADER_SRC_NAME_WE}.spv"
        VERBATIM
    )
    set_source_files_properties(${SHADER_SPV_HEX_FILE} PROPERTIES GENERATED TRUE)
    list(APPEND SHADER_SPV_HEX_FILES ${SHADER_SPV_HEX_FILE})

    # fp16 storage
    set(SHADER_fp16s_SRC_NAME_WE "${SHADER_SRC_NAME_WE}_fp16s")

    set(SHADER_fp16s_SPV_HEX_FILE ${SHADER_OUT_PATH}/${SHADER_fp16s_SRC_NAME_WE}.spv.hex.h)
    add_custom_command(
        OUTPUT ${SHADER_fp16s_SPV_HEX_FILE}
        COMMAND ${GLSLANGVALIDATOR_EXECUTABLE}
        ARGS -DNCNN_fp16_storage=1 -V -s -x -o ${SHADER_fp16s_SPV_HEX_FILE} ${SHADER_SRC_FULLPATH}
        DEPENDS ${SHADER_SRC_FULLPATH}
        COMMENT "Building SPIR-V module ${SHADER_fp16s_SRC_NAME_WE}.spv"
        VERBATIM
    )
    set_source_files_properties(${SHADER_fp16s_SPV_HEX_FILE} PROPERTIES GENERATED TRUE)
    list(APPEND SHADER_SPV_HEX_FILES ${SHADER_fp16s_SPV_HEX_FILE})

    # int8 storage
    set(SHADER_int8s_SRC_NAME_WE "${SHADER_SRC_NAME_WE}_int8s")

    set(SHADER_int8s_SPV_HEX_FILE ${SHADER_OUT_PATH}/${SHADER_int8s_SRC_NAME_WE}.spv.hex.h)
    add_custom_command(
        OUTPUT ${SHADER_int8s_SPV_HEX_FILE}
        COMMAND ${GLSLANGVALIDATOR_EXECUTABLE}
        ARGS -DNCNN_fp16_storage=1 -DNCNN_int8_storage=1 -V -s -x -o ${SHADER_int8s_SPV_HEX_FILE} ${SHADER_SRC_FULLPATH}
        DEPENDS ${SHADER_SRC_FULLPATH}
        COMMENT "Building SPIR-V module ${SHADER_int8s_SRC_NAME_WE}.spv"
        VERBATIM
    )
    set_source_files_properties(${SHADER_int8s_SPV_HEX_FILE} PROPERTIES GENERATED TRUE)
    list(APPEND SHADER_SPV_HEX_FILES ${SHADER_int8s_SPV_HEX_FILE})
endmacro()

# look for vulkan compute shader and compile
set(SHADER_SPV_HEX_FILES)

compile_shader(realesrgan_preproc.comp)
compile_shader(realesrgan_postproc.comp)
compile_shader(realesrgan_preproc_tta.comp)
compile_shader(realesrgan_postproc_tta.comp)

add_custom_target(generate-spirv DEPENDS ${SHADER_SPV_HEX_FILES})

add_dependencies(openvision generate-spirv)
# End compile realesrgan shader
endif()


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

        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/face/pose>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/face/pose/common>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/face/pose/detecter>
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
    ${CMAKE_CURRENT_SOURCE_DIR}/pose/common.h
    ${CMAKE_CURRENT_SOURCE_DIR}/pose/detecter.h
    DESTINATION ${INCLUDE_OUTPUT_PATH}/openvision/pose
)

file(COPY
    ${CMAKE_CURRENT_SOURCE_DIR}/realesrgan/realesrgan.h
    DESTINATION ${INCLUDE_OUTPUT_PATH}/openvision/realsr
)

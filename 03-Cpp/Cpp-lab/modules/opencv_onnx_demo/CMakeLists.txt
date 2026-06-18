cmake_minimum_required(VERSION 3.15)
project(opencv_onnx_demo)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ========== OpenCV ==========
find_package(OpenCV REQUIRED)

# ========== ONNX Runtime ==========
if(NOT DEFINED ENV{ONNXRUNTIME_DIR})
    message(FATAL_ERROR "Please set ONNXRUNTIME_DIR environment variable")
endif()

set(ORT_ROOT $ENV{ONNXRUNTIME_DIR})

set(ORT_INCLUDE_DIR ${ORT_ROOT}/include)
set(ORT_LIB_DIR ${ORT_ROOT}/lib)

find_path(ORT_INCLUDE_DIR_CHECK
    NAMES onnxruntime_cxx_api.h
    PATHS ${ORT_INCLUDE_DIR}
    NO_DEFAULT_PATH
)

find_library(ORT_LIB
    NAMES onnxruntime
    PATHS ${ORT_LIB_DIR}
    NO_DEFAULT_PATH
)

if(NOT ORT_INCLUDE_DIR_CHECK)
    message(FATAL_ERROR "ONNX Runtime include not found: ${ORT_INCLUDE_DIR}")
endif()

if(NOT ORT_LIB)
    message(FATAL_ERROR "ONNX Runtime library not found: ${ORT_LIB_DIR}")
endif()

# ========== Source ==========
file(GLOB_RECURSE SRC
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/app/*.cpp
)

add_executable(app ${SRC})

# ========== Include ==========
target_include_directories(app PRIVATE
    ${OpenCV_INCLUDE_DIRS}
    ${ORT_INCLUDE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# ========== Link ==========
target_link_libraries(app PRIVATE
    ${OpenCV_LIBS}
    ${ORT_LIB}
)

# ========== compile_commands ==========
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
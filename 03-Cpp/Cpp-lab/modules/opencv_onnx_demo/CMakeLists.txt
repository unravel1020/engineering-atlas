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

# ========== Common includes ==========
set(COMMON_INCLUDES
    ${OpenCV_INCLUDE_DIRS}
    ${ORT_INCLUDE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# ========== Core library (excludes app main) ==========
file(GLOB_RECURSE CORE_SRC
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
)

add_library(opencv_onnx_demo_core STATIC ${CORE_SRC})
target_include_directories(opencv_onnx_demo_core PRIVATE ${COMMON_INCLUDES})
target_link_libraries(opencv_onnx_demo_core PRIVATE
    ${OpenCV_LIBS}
    ${ORT_LIB}
)

# ========== Application ==========
add_executable(app ${CMAKE_CURRENT_SOURCE_DIR}/app/main.cpp)
target_include_directories(app PRIVATE ${COMMON_INCLUDES})
target_link_libraries(app PRIVATE opencv_onnx_demo_core)

# ========== Tests ==========
file(GLOB_RECURSE TEST_SRC
    ${CMAKE_CURRENT_SOURCE_DIR}/tests/*.cpp
)

if(TEST_SRC)
    add_executable(unit_tests ${TEST_SRC})
    target_include_directories(unit_tests PRIVATE ${COMMON_INCLUDES})
    target_link_libraries(unit_tests PRIVATE opencv_onnx_demo_core)

    enable_testing()
    add_test(NAME opencv_onnx_demo_unit COMMAND unit_tests)
endif()

# ========== compile_commands ==========
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

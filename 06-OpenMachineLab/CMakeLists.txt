cmake_minimum_required(VERSION 3.16)
project(OpenMachineLab LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/build)

# OpenCV is used for image preprocessing and visualization.
find_package(OpenCV REQUIRED)

# ONNX Runtime is bundled locally, matching opencv_onnx_demo setup.
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

set(COMMON_INCLUDES
    ${OpenCV_INCLUDE_DIRS}
    ${ORT_INCLUDE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

file(GLOB_RECURSE OML_SRC
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
)
# Demo and tool entry points have their own executables; do not link them into
# the core static library where they would conflict with test mains.
list(FILTER OML_SRC EXCLUDE REGEX "/src/(demo|tools)/")

add_library(oml_core STATIC ${OML_SRC})
target_include_directories(oml_core PRIVATE ${COMMON_INCLUDES})
target_link_libraries(oml_core PRIVATE
    ${OpenCV_LIBS}
    ${ORT_LIB}
)

# Demo executable
add_executable(oml_demo ${CMAKE_CURRENT_SOURCE_DIR}/src/demo/oml_demo.cpp)
target_include_directories(oml_demo PRIVATE ${COMMON_INCLUDES})
# Link the whole static library so backend static registrars are not dropped.
target_link_libraries(oml_demo PRIVATE
    -Wl,--whole-archive oml_core -Wl,--no-whole-archive
    ${OpenCV_LIBS}
    ${ORT_LIB}
)

# Tests
file(GLOB_RECURSE TEST_SRC
    ${CMAKE_CURRENT_SOURCE_DIR}/tests/*.cpp
)

if(TEST_SRC)
    add_executable(oml_tests ${TEST_SRC})
    target_include_directories(oml_tests PRIVATE ${COMMON_INCLUDES})
    # Link the whole static library so backend static registrars are included.
    target_link_libraries(oml_tests PRIVATE
        -Wl,--whole-archive oml_core -Wl,--no-whole-archive
        ${OpenCV_LIBS}
        ${ORT_LIB}
    )

    enable_testing()
    add_test(NAME oml_unit COMMAND oml_tests)
endif()

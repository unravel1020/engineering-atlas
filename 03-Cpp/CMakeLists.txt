cmake_minimum_required(VERSION 3.16)

project(CppLab LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# 可选：统一输出目录（避免 build 混乱）
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# 包含头文件路径
include_directories(${CMAKE_SOURCE_DIR})

# timer 静态库
add_library(timer STATIC
    timer/Timer.cpp
)

target_include_directories(timer PUBLIC
    ${CMAKE_SOURCE_DIR}/timer
)

# main 可执行文件
add_executable(main
    app/main.cpp
)

target_link_libraries(main PRIVATE timer)
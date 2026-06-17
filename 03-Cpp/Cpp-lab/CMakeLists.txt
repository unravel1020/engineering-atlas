cmake_minimum_required(VERSION 3.16)
project(CppLab LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# build输出
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/build)

# ===== modules统一管理 =====
add_subdirectory(modules/timer)
add_subdirectory(modules/event)
add_subdirectory(modules/logger)

# ===== app入口 =====
add_executable(main app/main.cpp)

target_link_libraries(main
    PRIVATE
        timer
        event
        logger
)
cmake_minimum_required(VERSION 3.22)

project(CppLab)

set(CMAKE_CXX_STANDARD 20)

add_library(timer
    timer/Timer.cpp
)

target_include_directories(timer
    PUBLIC
    timer
)

add_executable(main
    app/main.cpp
)

target_link_libraries(main
    PRIVATE
    timer
)
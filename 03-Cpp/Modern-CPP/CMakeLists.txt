cmake_minimum_required(VERSION 3.16)
project(ModernCPP LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Centralize binaries so learners can find and run each example easily.
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/build)

# Each .cpp file is a self-contained minimal example; build them as separate
# executables so they can be read, run and debugged independently.
file(GLOB EXAMPLE_SOURCES ${CMAKE_SOURCE_DIR}/src/*.cpp)

foreach(source_file ${EXAMPLE_SOURCES})
    get_filename_component(example_name ${source_file} NAME_WE)
    add_executable(${example_name} ${source_file})
endforeach()

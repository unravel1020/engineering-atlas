cmake_minimum_required(VERSION 3.16)
project(CppLab LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Centralize build artifacts so out-of-source rules are easy to enforce
# and generated binaries are simple to locate and clean.
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/build)

# Build each learning module as a static library so the app and future tests
# can link only what they need without recompiling module sources.
add_subdirectory(modules/timer)
add_subdirectory(modules/event)
add_subdirectory(modules/logger)

# Keep the executable target separate from the module libraries so the same
# modules can be reused by tests or other entry points without pulling in main.
add_executable(main app/main.cpp)

target_link_libraries(main
    PRIVATE
        timer
        event
        logger
)
add_library(logger STATIC
    Logger.cpp
)

target_include_directories(logger
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
)
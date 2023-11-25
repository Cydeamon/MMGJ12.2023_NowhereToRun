cmake_minimum_required(VERSION 3.26)
set(CMAKE_C_STANDARD 23)

project(
    NowhereToRun
    LANGUAGES C
    VERSION 0.1.0
)

set(PROJECT_LABEL "Nowhere To Run")

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../)

configure_file(
    "${PROJECT_SOURCE_DIR}/Sources/CMakeConfig.h.in"
    "${PROJECT_SOURCE_DIR}/Sources/CMakeConfig.h"
)

add_executable(${PROJECT_NAME} Sources/main.c Sources/CMakeConfig.h)
set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME "${PROJECT_NAME}_${PROJECT_VERSION}_${CMAKE_BUILD_TYPE}")
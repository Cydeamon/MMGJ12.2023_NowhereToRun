cmake_minimum_required(VERSION 3.26)
set(CMAKE_CXX_STANDARD 23)
project(NowhereToRun VERSION 0.1.0)
set(PROJECT_LABEL "Nowhere To Run")

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../)

configure_file("${PROJECT_SOURCE_DIR}/Sources/CMakeConfig.h.in" "${PROJECT_SOURCE_DIR}/Sources/CMakeConfig.h")

add_executable(${PROJECT_NAME}
    Sources/Main.cpp
    Sources/CMakeConfig.h
    Sources/Engine/Render.cpp
    Sources/Engine/Render.h
    Sources/Engine/Engine.cpp
    Sources/Engine/Engine.h
)

set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME "${PROJECT_NAME}_${PROJECT_VERSION}_${CMAKE_BUILD_TYPE}")

# GLFW
add_subdirectory(Sources/Vendor/glfw)
target_link_libraries(${PROJECT_NAME} glfw)

# Deploy command
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:${PROJECT_NAME}> ${CMAKE_BINARY_DIR}/../Deploy
)


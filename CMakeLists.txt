cmake_minimum_required(VERSION 3.26)
set(CMAKE_CXX_STANDARD 23)
project(NowhereToRun VERSION 0.1.0)
set(PROJECT_LABEL "Nowhere To Run")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../)

configure_file("${PROJECT_SOURCE_DIR}/Sources/CMakeConfig.h.in" "${PROJECT_SOURCE_DIR}/Sources/CMakeConfig.h")

add_executable(
    ${PROJECT_NAME}
    Sources/Main.cpp

    # Vendor
    Sources/Vendor/glad/include/glad/glad.h
    Sources/Vendor/glad/include/KHR/khrplatform.h
    Sources/Vendor/glad/src/glad.c

    # Project sources
    Sources/CMakeConfig.h
    Sources/Engine/Render.cpp
    Sources/Engine/Render.h
    Sources/Engine/Engine.cpp
    Sources/Engine/Engine.h
    Sources/Engine/Math/Vector2.cpp
    Sources/Engine/Math/Math.h
    Sources/Engine/Math/Vector2.cpp
    Sources/Engine/Math/Vector2.h
    Sources/Engine/Math/Vector2.cpp
    Sources/Engine/Math/Vector2.h
    Sources/Engine/Color.cpp
    Sources/Engine/Color.h
    Sources/Engine/Input/Input.cpp
    Sources/Engine/Input/Input.h
    Sources/Engine/Input/InputKeys.h
    Sources/Engine/Input/BtnState.h
)

set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME "${PROJECT_NAME}_${PROJECT_VERSION}_${CMAKE_BUILD_TYPE}")

# Include directories
target_include_directories(${PROJECT_NAME} PRIVATE Sources/Vendor/glad/include)

# GLFW
add_subdirectory(Sources/Vendor/glfw)
target_link_libraries(${PROJECT_NAME} glfw)
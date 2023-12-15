cmake_minimum_required(VERSION 3.26)
set(CMAKE_CXX_STANDARD 23)
project(NowhereToRun VERSION 0.2.0)
set(PROJECT_LABEL "Nowhere To Run")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../)

add_executable(
    ${PROJECT_NAME}
    Sources/Main.cpp

    # Vendor
    Sources/Vendor/glad/include/glad/glad.h
    Sources/Vendor/glad/include/KHR/khrplatform.h
    Sources/Vendor/glad/src/glad.c
    Sources/Vendor/stb/stb_image.h

    # Project sources
    Sources/CMakeConfig.h
    Sources/Engine/GraphicLibraries/OpenGL.cpp
    Sources/Engine/GraphicLibraries/OpenGL.h
    Sources/Engine/Engine.cpp
    Sources/Engine/Engine.h
    Sources/Engine/Math/Vector2.cpp
    Sources/Engine/Color.cpp
    Sources/Engine/Color.h
    Sources/Engine/Input/Input.cpp
    Sources/Engine/Input/Input.h
    Sources/Engine/Input/InputKeys.h
    Sources/Engine/Input/BtnState.h
    Sources/Engine/GraphicLibraries/Shaders/ShaderProgram.cpp
    Sources/Engine/GraphicLibraries/Shaders/ShaderProgram.h
    Sources/Engine/GraphicLibraries/Shaders/ShaderVec4.h
    Sources/Engine/GraphicLibraries/Shaders/ShaderVec4.cpp
    Sources/Engine/GraphicLibraries/Texture.cpp
    Sources/Engine/GraphicLibraries/Texture.h
    Sources/Engine/Objects/GameObject.cpp
    Sources/Engine/Objects/GameObject.h
    Sources/Engine/Sprite.cpp
    Sources/Engine/Sprite.h
    Sources/Game/Game.cpp
    Sources/Game/Game.h
    Sources/Game/Menu/Menu.cpp
    Sources/Game/Menu/Menu.h
    Sources/Game/Menu/MenuOption.cpp
    Sources/Game/Menu/MenuOption.h
)

set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME "${PROJECT_NAME}_${PROJECT_VERSION}_${CMAKE_BUILD_TYPE}")

file(REMOVE "${PROJECT_SOURCE_DIR}/Sources/CMakeConfig.h")
configure_file("${PROJECT_SOURCE_DIR}/Sources/CMakeConfig.h.in" "${PROJECT_SOURCE_DIR}/Sources/CMakeConfig.h")

# Include directories
target_include_directories(${PROJECT_NAME} PRIVATE Sources/Vendor/glad/include)
target_include_directories(${PROJECT_NAME} PRIVATE Sources/Vendor/glm)

# GLFW
add_subdirectory(Sources/Vendor/glfw)
add_subdirectory(Sources/Vendor/glm)
target_link_libraries(${PROJECT_NAME} glfw)
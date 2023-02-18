cmake_minimum_required(VERSION 3.23)
project(BP_2022_Zappe)

find_package(OpenGL REQUIRED)

set(CMAKE_CXX_STANDARD 20)

add_definitions(-DGLEW_STATIC)

# Including GLFW
add_subdirectory(lib/glfw)
include_directories(${GLFW_INCLUDE_DIRS})

# Including GLEW
add_subdirectory(lib/glew)
include_directories(${GLEW_INCLUDE_DIRS})

# Including GLM
add_subdirectory(lib/glm)
include_directories(${GLM_INCLUDE_DIRS})

add_executable(
    BP_2022_Zappe
    src/main.cpp
    src/maze/graph.cpp
    src/maze/graph.h
    src/graphics/drawing.cpp
    src/graphics/drawing.h
    src/maze/node.cpp
    src/maze/node.h
    src/maze/generator.cpp
    src/maze/generator.h
    src/graphics/shader.cpp
    src/graphics/shader.h
    src/const.h
    src/player.cpp
    src/player.h
        src/maze/cellular_automata.cpp src/maze/cellular_automata.h)

target_link_libraries(BP_2022_Zappe glfw libglew_static glm ${GLFW_LIBRARIES} ${GLEW_LIBRARIES} ${GLM_LIBRARIES} ${OPENGL_LIBRARY})

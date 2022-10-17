cmake_minimum_required(VERSION 3.23)
project(BP_2022_Zappe)

set(CMAKE_CXX_STANDARD 20)

# Including GLFW
add_subdirectory(glfw)
include_directories(glfw/include)

add_executable(BP_2022_Zappe src/main.cpp src/maze/graph.cpp src/maze/graph.h src/graphics/drawing.cpp src/graphics/drawing.h)

target_link_libraries(BP_2022_Zappe glfw ${GLFW_LIBRARIES})

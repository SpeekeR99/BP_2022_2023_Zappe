cmake_minimum_required(VERSION 3.23)
project(BP_2022_Zappe)

set(CMAKE_CXX_STANDARD 20)

# Including GLFW
add_subdirectory(glfw)
include_directories(glfw/include)

add_executable(BP_2022_Zappe main.cpp)

target_link_libraries(BP_2022_Zappe glfw ${GLFW_LIBRARIES})

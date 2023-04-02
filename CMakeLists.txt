cmake_minimum_required(VERSION 3.22)
project(BP_2022_Zappe)

set(CMAKE_CXX_STANDARD 20)

find_package(OpenGL REQUIRED)

add_definitions(-DGLEW_STATIC)

# Including GLFW
add_subdirectory(lib/glfw)
include_directories(${GLFW_INCLUDE_DIRS})

# Including GLEW
add_subdirectory(lib/glew)
include_directories(${GLEW_INCLUDE_DIRS})

# Including ImGui
include_directories(lib/imgui)
include_directories(lib/imgui/backends)
include_directories(lib/imgui/misc/cpp)

set(
    imgui_files
    lib/imgui/imconfig.h
    lib/imgui/imgui.cpp
    lib/imgui/imgui.h
    lib/imgui/imgui_draw.cpp
    lib/imgui/imgui_internal.h
    lib/imgui/imgui_tables.cpp
    lib/imgui/imgui_widgets.cpp
    lib/imgui/imstb_rectpack.h
    lib/imgui/imstb_textedit.h
    lib/imgui/imstb_truetype.h
    lib/imgui/misc/cpp/imgui_stdlib.cpp
    lib/imgui/misc/cpp/imgui_stdlib.h
)

set(
    imgui_impl_files
    lib/imgui/backends/imgui_impl_glfw.cpp
    lib/imgui/backends/imgui_impl_glfw.h
    lib/imgui/backends/imgui_impl_opengl3.cpp
    lib/imgui/backends/imgui_impl_opengl3.h
    lib/imgui/backends/imgui_impl_opengl3_loader.h
)

# Including stb
include_directories(lib/stb)

set(
    stb
    lib/stb/stb_image_write.h
)

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
    src/graphics/shaders/shader.cpp
    src/graphics/shaders/shader.h
    src/const.h
    src/player.cpp
    src/player.h
    src/maze/cellular_automata.cpp
    src/maze/cellular_automata.h
    src/maze/solver.cpp
    src/maze/solver.h
    src/graphics/shaders/VAO.cpp
    src/graphics/shaders/VAO.h
    src/graphics/shaders/VBO.cpp
    src/graphics/shaders/VBO.h
    src/graphics/shaders/EBO.cpp
    src/graphics/shaders/EBO.h
    src/maze/disjoint_union_sets.cpp
    src/maze/disjoint_union_sets.h
    ${imgui_files}
    ${imgui_impl_files}
    ${stb}
)

target_link_libraries(BP_2022_Zappe glfw libglew_static ${GLFW_LIBRARIES} ${GLEW_LIBRARIES} ${OPENGL_LIBRARY})

#pragma once

#include <array>
#include <memory>
#include "GL/glew.h"
#include "GLFW/glfw3.h"
#include "../maze/graph.h"
#include "shaders/VAO.h"
#include "shaders/VBO.h"
#include "shaders/EBO.h"
#include "imgui.h"

class Drawing {
public:
    static void transform_x_y_to_opengl(int x, int y, float& x_opengl, float& y_opengl);
    static void transform_rgb_to_opengl(std::array<int, 3> color, float& red, float& green, float& blue);
    static void draw_circle(int x, int y, float radius, ImVec4 color);
    static void buffer_graph(std::shared_ptr<VAO> &vao, std::shared_ptr<VBO> &vbo, std::shared_ptr<EBO> &ebo, const std::shared_ptr<Graph>& graph, ImVec4 color);
    static void buffer_lines(std::shared_ptr<VAO> &vao, std::shared_ptr<VBO> &vbo, std::shared_ptr<EBO> &ebo, const std::vector<std::pair<int, int>> &lines, ImVec4 color);
};
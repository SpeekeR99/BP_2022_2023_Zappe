#pragma once

#include <array>
#include <memory>
#include "GL/glew.h"
#include "GLFW/glfw3.h"
#include "../maze/graph.h"

class Drawing {
public:
    static void transform_x_y_to_opengl(int x, int y, float& x_opengl, float& y_opengl);
    static void transform_rgb_to_opengl(std::array<int, 3> color, float& red, float& green, float& blue);
    static void buffer_graph(const std::shared_ptr<Graph>& graph, int& size, int& buffer);
};
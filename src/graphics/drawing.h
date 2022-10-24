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
    static void draw_line(int x0, int y0, int x1, int y1, std::array<int, 3> color = {0, 0, 0}, float line_width = 2.0f);
    static void draw_rectangle(int x, int y, int width, int height, std::array<int, 3> color = {0, 0, 0}, float line_width = 2.0f);
    static void draw_graph(const std::shared_ptr<Graph>& graph, std::array<int, 3> color = {0, 0, 0});
};
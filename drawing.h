#pragma once

#include <glfw/glfw3.h>
#include <array>

class Drawing {
public:
    static void draw_line(int x0, int y0, int x1, int y1, std::array<int, 3> color = {0, 0, 0}, float line_width = 2.0f);
    static void draw_square(int x, int y, int width, int height, std::array<int, 3> color = {0, 0, 0}, float line_width = 2.0f);
};
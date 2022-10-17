#include "drawing.h"

void Drawing::draw_line(int x0, int y0, int x1, int y1, std::array<int, 3> color, float line_width) {
    float x_start = static_cast<float>(x0) / static_cast<float>(static_cast<double>(640) / 2) - 1.0f;
    float y_start = -1 * (static_cast<float>(y0) / static_cast<float>(static_cast<double>(480) / 2) - 1.0f);
    float x_end = static_cast<float>(x1) / static_cast<float>(static_cast<double>(640) / 2) - 1.0f;
    float y_end = -1 * (static_cast<float>(y1) / static_cast<float>(static_cast<double>(480) / 2) - 1.0f);
    float red = static_cast<float>(color[0])/255;
    float green = static_cast<float>(color[1])/255;
    float blue = static_cast<float>(color[2])/255;

    glLineWidth(line_width);
    glBegin(GL_LINES);
    glColor3f(red, green, blue);
    glVertex2f(x_start, y_start);
    glVertex2f(x_end, y_end);
    glEnd();
}

void Drawing::draw_square(int x, int y, int width, int height, std::array<int, 3> color, float line_width) {
    draw_line(x, y, x + width, y, color, line_width);
    draw_line(x + width, y, x + width, y + height, color, line_width);
    draw_line(x + width, y + height, x, y + height, color, line_width);
    draw_line(x, y + height, x, y, color, line_width);
}
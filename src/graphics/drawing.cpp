#include "drawing.h"

int window_width = 640;
int window_height = 480;

void Drawing::draw_line(int x0, int y0, int x1, int y1, std::array<int, 3> color, float line_width) {
    float x_start = static_cast<float>(x0) / static_cast<float>(static_cast<double>(window_width) / 2) - 1.0f;
    float y_start = -1 * (static_cast<float>(y0) / static_cast<float>(static_cast<double>(window_height) / 2) - 1.0f);
    float x_end = static_cast<float>(x1) / static_cast<float>(static_cast<double>(window_width) / 2) - 1.0f;
    float y_end = -1 * (static_cast<float>(y1) / static_cast<float>(static_cast<double>(window_height) / 2) - 1.0f);
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

void Drawing::draw_rectangle(int x, int y, int width, int height, std::array<int, 3> color, float line_width) {
    draw_line(x, y, x + width, y, color, line_width);
    draw_line(x + width, y, x + width, y + height, color, line_width);
    draw_line(x + width, y + height, x, y + height, color, line_width);
    draw_line(x, y + height, x, y, color, line_width);
}

void Drawing::draw_grid_graph(const std::shared_ptr<Graph>& grid_graph) {
    for (int i = 0; i < grid_graph->get_v(); i++) {
        auto node = grid_graph->get_nodes()[i];

        Drawing::draw_rectangle(node->get_x() - 5, node->get_y() - 5, 10, 10, {255, 255, 255}, 2.0f);

        for (int &j: grid_graph->get_adj()[i]) {
            auto adjacent_node = grid_graph->get_nodes()[j];

            Drawing::draw_line(node->get_x(), node->get_y(), adjacent_node->get_x(), adjacent_node->get_y(), {255, 255, 255}, 20.0f);
        }
    }
}

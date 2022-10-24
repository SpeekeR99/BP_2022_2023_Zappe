#include "drawing.h"

int window_width = 1080;
int window_height = 620;

void Drawing::transform_x_y_to_opengl(int x, int y, float &x_opengl, float &y_opengl) {
    x_opengl = static_cast<float>(x) / static_cast<float>(static_cast<double>(window_width) / 2) - 1.0f;
    y_opengl =  -1 * (static_cast<float>(y) / static_cast<float>(static_cast<double>(window_height) / 2) - 1.0f);
}

void Drawing::transform_rgb_to_opengl(std::array<int, 3> color, float &red, float &green, float &blue) {
    red = static_cast<float>(color[0]) / 255.0f;
    green = static_cast<float>(color[1]) / 255.0f;
    blue = static_cast<float>(color[2]) / 255.0f;
}

void Drawing::buffer_graph(const std::shared_ptr<Graph>& graph, int &size, int &buffer) {
    std::vector<float> points;

    for (int i = 0; i < graph->get_v(); i++) {
        auto node = graph->get_nodes()[i];

        for (int &j: graph->get_adj()[i]) {
            auto adjacent_node = graph->get_nodes()[j];

            float x_start, y_start, x_end, y_end;
            transform_x_y_to_opengl(node->get_x(), node->get_y(), x_start, y_start);
            transform_x_y_to_opengl(adjacent_node->get_x(), adjacent_node->get_y(), x_end, y_end);

            points.push_back(x_start);
            points.push_back(y_start);
            points.push_back(x_end);
            points.push_back(y_end);
        }
    }

    unsigned int buffer_edges;
    glGenBuffers(1, &buffer_edges);
    glBindBuffer(GL_ARRAY_BUFFER, buffer_edges);
    glBufferData(GL_ARRAY_BUFFER, points.size() * sizeof(float), &points[0], GL_STATIC_DRAW);

    size = points.size();
    buffer = buffer_edges;
}

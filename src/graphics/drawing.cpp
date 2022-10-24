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

void Drawing::draw_line(int x0, int y0, int x1, int y1, std::array<int, 3> color, float line_width) {
    float x_start, y_start, x_end, y_end;
    transform_x_y_to_opengl(x0, y0, x_start, y_start);
    transform_x_y_to_opengl(x1, y1, x_end, y_end);
    float red, green, blue;
    transform_rgb_to_opengl(color, red, green, blue);

    float points[4] = {
            x_start, y_start,
            x_end, y_end
    };

    unsigned int buffer;
    glGenBuffers(1, &buffer);
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glBufferData(GL_ARRAY_BUFFER, 4 * sizeof(float), points, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, nullptr);
    glEnableVertexAttribArray(0);
    glLineWidth(line_width);
    glColor3f(red, green, blue);
    glDrawArrays(GL_LINES, 0, 2);
}

void Drawing::draw_graph(const std::shared_ptr<Graph>& graph, std::array<int, 3> color) {
    std::vector<float> edges;

    for (int i = 0; i < graph->get_v(); i++) {
        auto node = graph->get_nodes()[i];

        for (int &j: graph->get_adj()[i]) {
            auto adjacent_node = graph->get_nodes()[j];

            float x_start, y_start, x_end, y_end;
            transform_x_y_to_opengl(node->get_x(), node->get_y(), x_start, y_start);
            transform_x_y_to_opengl(adjacent_node->get_x(), adjacent_node->get_y(), x_end, y_end);

            edges.push_back(x_start);
            edges.push_back(y_start);
            edges.push_back(x_end);
            edges.push_back(y_end);
        }
    }

    float red, green, blue;
    transform_rgb_to_opengl(color, red, green, blue);

    unsigned int buffer_edges;
    glGenBuffers(1, &buffer_edges);
    glBindBuffer(GL_ARRAY_BUFFER, buffer_edges);
    glBufferData(GL_ARRAY_BUFFER, edges.size() * sizeof(float), &edges[0], GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, nullptr);
    glEnableVertexAttribArray(0);
    glLineWidth(4.0f);
    glColor3f(red, green, blue);
    glDrawArrays(GL_LINES, 0, edges.size() / 2);
}

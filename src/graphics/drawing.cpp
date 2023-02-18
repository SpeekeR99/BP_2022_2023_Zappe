#include "drawing.h"

void Drawing::transform_x_y_to_opengl(int x, int y, float &x_opengl, float &y_opengl) {
    x_opengl = static_cast<float>(x) / static_cast<float>(static_cast<double>(WINDOW_WIDTH) / 2) - 1.0f;
    y_opengl = -1 * (static_cast<float>(y) / static_cast<float>(static_cast<double>(WINDOW_HEIGHT) / 2) - 1.0f);
}

void Drawing::transform_rgb_to_opengl(std::array<int, 3> color, float &red, float &green, float &blue) {
    red = static_cast<float>(color[0]) / 255.0f;
    green = static_cast<float>(color[1]) / 255.0f;
    blue = static_cast<float>(color[2]) / 255.0f;
}

void Drawing::draw_line(int x1, int y1, int x2, int y2) {
    float x1_opengl, y1_opengl, x2_opengl, y2_opengl;
    transform_x_y_to_opengl(x1, y1, x1_opengl, y1_opengl);
    transform_x_y_to_opengl(x2, y2, x2_opengl, y2_opengl);

    glBegin(GL_LINES);
    glVertex2f(x1_opengl, y1_opengl);
    glVertex2f(x2_opengl, y2_opengl);
    glEnd();
}

void Drawing::draw_circle(int x, int y, float radius) {
    float x_opengl, y_opengl;
    transform_x_y_to_opengl(x, y, x_opengl, y_opengl);
    float radius_x = radius / 2.0f, radius_y = radius / 2.0f;
    if (WINDOW_WIDTH > WINDOW_HEIGHT) {
        radius_x = radius / (static_cast<float>(WINDOW_WIDTH) / static_cast<float>(WINDOW_HEIGHT));
        radius_y = radius * (static_cast<float>(WINDOW_HEIGHT) / static_cast<float>(WINDOW_WIDTH)) *
                   (static_cast<float>(WINDOW_WIDTH) / static_cast<float>(WINDOW_HEIGHT));
    }
    else if (WINDOW_WIDTH < WINDOW_HEIGHT) {
        radius_x = radius * (static_cast<float>(WINDOW_WIDTH) / static_cast<float>(WINDOW_HEIGHT)) *
                   (static_cast<float>(WINDOW_HEIGHT) / static_cast<float>(WINDOW_WIDTH));
        radius_y = radius / (static_cast<float>(WINDOW_HEIGHT) / static_cast<float>(WINDOW_WIDTH));
    }
    int num_segments = 40;

    glBegin(GL_TRIANGLE_FAN);

    glVertex2f(x_opengl, y_opengl); // Center
    for (int i = 0; i <= num_segments; i++)
        glVertex2f(
                static_cast<float>(radius_x * cos(2 * PI * i / num_segments) + x_opengl),
                static_cast<float>(radius_y * sin(2 * PI * i / num_segments) + y_opengl)
        );

    glEnd();
}

void Drawing::buffer_graph(const std::shared_ptr<Graph> &graph, int &size, int &buffer) {
    std::vector<float> points;

    for (int i = 0; i < graph->get_v(); i++) {
        auto node = graph->get_nodes()[i];
        if (!node->is_alive())
            continue;

        for (int &j: graph->get_adj()[i]) {
            auto adjacent_node = graph->get_nodes()[j];
            if (!adjacent_node->is_alive())
                continue;

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
    glBufferData(GL_ARRAY_BUFFER, static_cast<int>(points.size() * sizeof(float)), &points[0], GL_STATIC_DRAW);

    size = static_cast<int>(points.size());
    buffer = buffer_edges;
}

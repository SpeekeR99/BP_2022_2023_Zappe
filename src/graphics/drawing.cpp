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

void Drawing::draw_circle(int x, int y, float radius, ImVec4 color) {
    float x_opengl, y_opengl;
    transform_x_y_to_opengl(x + WINDOW_X_OFFSET, y, x_opengl, y_opengl);
    float radius_x = radius / (static_cast<float>(WINDOW_WIDTH) / static_cast<float>(WINDOW_HEIGHT));
    float radius_y = radius * (static_cast<float>(WINDOW_HEIGHT) / static_cast<float>(WINDOW_WIDTH)) *
                     (static_cast<float>(WINDOW_WIDTH) / static_cast<float>(WINDOW_HEIGHT));
    // Number of segments to draw the circle
    const int num_segments = 36;

    auto *vertices = new GLfloat[(num_segments + 1) * 2 * 5]; // 5 floats per vertex
    vertices[0] = x_opengl;
    vertices[1] = y_opengl;
    vertices[2] = color.x;
    vertices[3] = color.y;
    vertices[4] = color.z;
    for (int i = 5; i < (num_segments + 1) * 2 * 5; i += 5) {
        auto angle = static_cast<float>(2.0f * PI * static_cast<float>(i / 5. - 1) / static_cast<float>(num_segments));
        vertices[i] = static_cast<float>(x_opengl + radius_x * static_cast<float>(cos(angle)));
        vertices[i + 1] = static_cast<float>(y_opengl + radius_y * static_cast<float>(sin(angle)));
        vertices[i + 2] = color.x;
        vertices[i + 3] = color.y;
        vertices[i + 4] = color.z;
    }

    auto *indices = new GLuint[3 * num_segments];
    for (int i = 0; i < num_segments; i++) {
        indices[i * 3] = 0; // Center of the circle
        indices[i * 3 + 1] = i + 1; // First vertex of the triangle
        indices[i * 3 + 2] = i + 2; // Second vertex of the triangle
    }

    std::shared_ptr<VAO> vao = std::make_shared<VAO>();
    vao->bind();
    std::shared_ptr<VBO> vbo = std::make_shared<VBO>(vertices, (num_segments + 1) * 2 * 5 * sizeof(GLfloat));
    std::shared_ptr<EBO> ebo = std::make_shared<EBO>(indices, 3 * num_segments * sizeof(GLuint));
    vao->link_attribute(vbo, 0, 2, GL_FLOAT, 5 * sizeof(GLfloat), (void *) 0);
    vao->link_attribute(vbo, 1, 3, GL_FLOAT, 5 * sizeof(GLfloat), (void *) (2 * sizeof(GLfloat)));
    vao->unbind();
    vbo->unbind();
    ebo->unbind();

    // Draw the circle
    vao->bind();
    glDrawElements(GL_TRIANGLES, ebo->num_elements, GL_UNSIGNED_INT, 0);
    vao->unbind();

    // Cleanup
    vao->del();
    vbo->del();
    ebo->del();
    delete[] vertices;
    delete[] indices;
}

void Drawing::buffer_graph(std::shared_ptr<VAO> &vao, std::shared_ptr<VBO> &vbo, std::shared_ptr<EBO> &ebo,
                           const std::shared_ptr<Graph> &graph, ImVec4 color) {
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
            transform_x_y_to_opengl(node->get_x() + WINDOW_X_OFFSET, node->get_y(), x_start, y_start);
            transform_x_y_to_opengl(adjacent_node->get_x() + WINDOW_X_OFFSET, adjacent_node->get_y(), x_end, y_end);

            points.push_back(x_start);
            points.push_back(y_start);
            points.push_back(x_end);
            points.push_back(y_end);
        }
    }

    auto *vertices = new GLfloat[points.size() * 5 / 2]; // 5 floats per vertex
    for (int i = 0, index = 0; i < points.size(); i += 2, index += 5) {
        vertices[index] = points[i];
        vertices[index + 1] = points[i + 1];
        vertices[index + 2] = color.x;
        vertices[index + 3] = color.y;
        vertices[index + 4] = color.z;
    }

    auto *indices = new GLuint[points.size() / 2]; // 2 vertices per line
    for (int i = 0; i < points.size() / 2; i++)
        indices[i] = i; // Index of the vertex

    vao = std::make_shared<VAO>();
    vao->bind();
    vbo = std::make_shared<VBO>(vertices, points.size() * 5 / 2 * sizeof(GLfloat));
    ebo = std::make_shared<EBO>(indices, points.size() / 2 * sizeof(GLuint));
    vao->link_attribute(vbo, 0, 2, GL_FLOAT, 5 * sizeof(GLfloat), (void *) 0);
    vao->link_attribute(vbo, 1, 3, GL_FLOAT, 5 * sizeof(GLfloat), (void *) (2 * sizeof(GLfloat)));
    vao->unbind();
    vbo->unbind();
    ebo->unbind();

    // Cleanup
    delete[] vertices;
    delete[] indices;
}

void Drawing::buffer_lines(std::shared_ptr<VAO> &vao, std::shared_ptr<VBO> &vbo, std::shared_ptr<EBO> &ebo,
                           const std::vector<std::pair<int, int>> &lines, ImVec4 color) {
    if (lines.empty())
        return;

    auto *vertices = new GLfloat[lines.size() * 5]; // 5 floats per vertex
    for (int i = 0, index = 0; i < lines.size(); i++, index += 5) {
        float x, y;
        transform_x_y_to_opengl(lines[i].first + WINDOW_X_OFFSET, lines[i].second, x, y);
        vertices[index] = x;
        vertices[index + 1] = y;
        vertices[index + 2] = color.x;
        vertices[index + 3] = color.y;
        vertices[index + 4] = color.z;
    }

    auto *indices = new GLuint[(lines.size() - 1) * 2];
    indices[0] = 0;
    indices[1] = 1;
    for (int i = 1, index = 2; i < lines.size() - 1; i++, index += 2) {
        indices[index] = i;
        indices[index + 1] = i + 1;
    }

    vao = std::make_shared<VAO>();
    vao->bind();
    vbo = std::make_shared<VBO>(vertices, lines.size() * 5 * sizeof(GLfloat));
    ebo = std::make_shared<EBO>(indices, (lines.size() - 1) * 2 * sizeof(GLuint));
    vao->link_attribute(vbo, 0, 2, GL_FLOAT, 5 * sizeof(GLfloat), (void *) 0);
    vao->link_attribute(vbo, 1, 3, GL_FLOAT, 5 * sizeof(GLfloat), (void *) (2 * sizeof(GLfloat)));
    vao->unbind();
    vbo->unbind();
    ebo->unbind();

    // Cleanup
    delete[] vertices;
    delete[] indices;
}

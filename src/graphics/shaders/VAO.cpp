#include "VAO.h"

VAO::VAO() {
    glGenVertexArrays(1, &id);
}

void VAO::link_attribute(std::shared_ptr<VBO> &vbo, GLuint layout, GLuint num_components, GLenum type, GLsizei stride, void *offset) {
    vbo->bind();
    glEnableVertexAttribArray(layout);
    glVertexAttribPointer(layout, num_components, type, GL_FALSE, stride, offset);
    vbo->unbind();
}

void VAO::bind() const {
    glBindVertexArray(id);
}

void VAO::unbind() const {
    glBindVertexArray(0);
}

void VAO::del() const {
    glDeleteVertexArrays(1, &id);
}

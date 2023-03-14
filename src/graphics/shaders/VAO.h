#pragma once

#include "GL/glew.h"
#include "GLFW/glfw3.h"
#include <memory>
#include "VBO.h"

class VAO {
public:
    GLuint id{};
    VAO();

    void link_attribute(std::shared_ptr<VBO> &vbo, GLuint layout, GLuint num_components, GLenum type, GLsizei stride, void *offset);
    void bind() const;
    void unbind() const;
    void del() const;
};

#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>

class VBO {
public:
    GLuint id{};
    VBO(GLfloat *vertices, GLsizeiptr size);

    void bind() const;
    void unbind() const;
    void del() const;

};

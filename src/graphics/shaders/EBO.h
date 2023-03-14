#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>

class EBO {
public:
    GLuint id{};
    GLuint num_elements{};
    EBO(GLuint *indices, GLsizeiptr size);

    void bind() const;
    void unbind() const;
    void del() const;

};

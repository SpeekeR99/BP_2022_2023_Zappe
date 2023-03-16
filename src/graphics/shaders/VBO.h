#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>

/**
 * Vertex Buffer Object
 */
class VBO {
public:
    /** ID of the VBO */
    GLuint id{};
    /**
     * Constructor
     * @param vertices Array of vertices
     * @param size Size of the array
     */
    VBO(GLfloat *vertices, GLsizeiptr size);

    /**
     * Bind the VBO
     */
    void bind() const;
    /**
     * Unbind the VBO
     */
    void unbind() const;

    /**
     * Delete the VBO
     */
    void del() const;
};

#pragma once

#include <memory>
#include "GL/glew.h"
#include "GLFW/glfw3.h"
#include "VBO.h"

/**
 * Vertex Array Object
 */
class VAO {
public:
    /** ID of the VAO */
    GLuint id{};
    /**
     * Constructor
     */
    VAO();

    /**
     * Links a VBO to a layout
     * @param vbo Vertex Buffer Object
     * @param layout Layout
     * @param num_components Number of components
     * @param type Type of the components
     * @param stride Stride
     * @param offset Offset
     */
    void link_attribute(std::shared_ptr<VBO> &vbo, GLuint layout, GLuint num_components, GLenum type, GLsizei stride, void *offset);
    /**
     * Binds the VAO
     */
    void bind() const;
    /**
     * Unbinds the VAO
     */
    void unbind() const;

    /**
     * Deletes the VAO
     */
    void del() const;
};

#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>

/**
 * Element Buffer Object
 */
class EBO {
public:
    /** ID of the EBO */
    GLuint id{};
    /** Number of elements in the EBO */
    GLsizei num_elements{};

    /**
     * Constructor
     * @param indices Array of indices
     * @param size Size of the array
     */
    EBO(GLuint *indices, GLsizeiptr size);

    /**
     * Binds the EBO
     */
    void bind() const;

    /**
     * Unbinds the EBO
     */
    void unbind() const;

    /**
     * Deletes the EBO
     */
    void del() const;
};

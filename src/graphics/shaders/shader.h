#pragma once

#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <malloc.h>
#include "GL/glew.h"
#include "GLFW/glfw3.h"
#include "../../const.h"

/**
 * Shader source
 * Vertex shader source and fragment shader source
 */
struct ShaderProgramSource {
    /** Vertex shader source */
    std::string vertex_source;
    /** Fragment shader source */
    std::string fragment_source;
};

/**
 * Shader class
 */
class Shader {
public:
    /**
     * Parse shader string to get vertex shader source and fragment shader source
     * @param shader_string String of shader
     * @return ShaderProgramSource struct of vertex shader source and fragment shader source
     */
    static struct ShaderProgramSource parse_shader(const std::string &shader_string);
    /**
     * Compile shader
     * @param source Source of shader
     * @param type Type of shader (GL_VERTEX_SHADER or GL_FRAGMENT_SHADER)
     * @return ID of shader
     */
    static unsigned int compile_shader(const std::string& source, unsigned int type);

    /**
     * Create shader
     * @param vertex_shader Vertex shader source
     * @param fragment_shader Fragment shader source
     * @return ID of shader
     */
    static unsigned int create_shader(const std::string& vertex_shader, const std::string& fragment_shader);
};
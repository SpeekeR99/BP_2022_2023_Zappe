#pragma once

#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <malloc.h>
#include "GL/glew.h"
#include "GLFW/glfw3.h"
#include "../../const.h"

struct ShaderProgramSource {
    std::string vertex_source;
    std::string fragment_source;
};

class Shader {
public:
    static struct ShaderProgramSource parse_shader(const std::string &shader_string);
    static unsigned int compile_shader(const std::string& source, unsigned int type);
    static unsigned int create_shader(const std::string& vertex_shader, const std::string& fragment_shader);
};
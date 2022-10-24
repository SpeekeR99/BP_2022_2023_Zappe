#include "shader.h"

struct ShaderProgramSource Shader::parse_shader(const std::string &filepath) {
    std::ifstream stream(filepath);

    enum class shader_type {
        NONE = -1, VERTEX = 0, FRAGMENT = 1
    };

    std::string line;
    std::stringstream ss[2];
    shader_type type = shader_type::NONE;
    while (getline(stream, line)) {
        if (line.find("#shader") != std::string::npos) {
            if (line.find("vertex") != std::string::npos) {
                type = shader_type::VERTEX;
            }
            else if (line.find("fragment") != std::string::npos) {
                type = shader_type::FRAGMENT;
            }
        }
        else if (type != shader_type::NONE) {
            ss[static_cast<int>(type)] << line << std::endl;
        }
    }

    return {ss[0].str(), ss[1].str()};
}

unsigned int Shader::compile_shader(const std::string &source, unsigned int type) {
    auto id = glCreateShader(type);
    const char* src = source.c_str();
    glShaderSource(id, 1, &src, nullptr);
    glCompileShader(id);

    int result;
    glGetShaderiv(id, GL_COMPILE_STATUS, &result);
    if (result == GL_FALSE) {
        int length;
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
        char* message = (char*)alloca(length * sizeof(char));
        glGetShaderInfoLog(id, length, &length, message);
        std::cout << "Failed to compile " << (type == GL_VERTEX_SHADER ? "vertex" : "fragment") << " shader!" << std::endl;
        std::cout << message << std::endl;
        glDeleteShader(id);
        return 0;
    }

    return id;
}

unsigned int Shader::create_shader(const std::string &vertex_shader, const std::string &fragment_shader) {
    auto program = glCreateProgram();
    auto vs = compile_shader(vertex_shader, GL_VERTEX_SHADER);
    auto fs = compile_shader(fragment_shader, GL_FRAGMENT_SHADER);

    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);
    glValidateProgram(program);

    glDeleteShader(vs);
    glDeleteShader(fs);

    return program;
}

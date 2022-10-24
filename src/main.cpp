#include <malloc.h>
#include "GL/glew.h"
#include "GLFW/glfw3.h"
#include "glm/glm.hpp"
#include "graphics/drawing.h"
#include "maze/graph.h"
#include "maze/generator.h"

int width = 1080;
int height = 620;

static unsigned int compile_shader(const std::string& source, unsigned int type) {
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

static unsigned int create_shader(const std::string& vertex_shader, const std::string& fragment_shader) {
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

int main() {
    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(width, height, "Hello World", nullptr, nullptr);
    if (!window) {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    GLenum err = glewInit();
    if (err != GLEW_OK) {
        /* Problem: glewInit failed, something is seriously wrong. */
        std::cerr << "Error: " << glewGetErrorString(err) << std::endl;
    }

    /* Print out some info about the graphics drivers */
    std::cout << "OpenGL version: " << glGetString(GL_VERSION) << std::endl;
    std::cout << "GLEW version: " << glewGetString(GLEW_VERSION) << std::endl;

    /* White background */
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

    /* Generate maze */
    auto paths = Generator::generate_maze_dfs(20, 11);
    auto walls = Graph::transform_paths_to_walls(paths);

    /* Buffer maze */
    int size_paths, buffer_paths, size_walls, buffer_walls;
    Drawing::buffer_graph(paths, size_paths, buffer_paths);
    Drawing::buffer_graph(walls, size_walls, buffer_walls);

    /* Create shader */
    std::string vertex_shader =
        "#version 330 core\n"
        "\n"
        "layout(location = 0) in vec4 position;\n"
        "\n"
        "void main() {\n"
        " gl_Position = position;\n"
        "}\n";
    std::string fragment_shader =
        "#version 330 core\n"
        "\n"
        "layout(location = 0) out vec4 color;\n"
        "\n"
        "void main() {\n"
        " color = vec4(0.0, 0.0, 0.0, 1.0);\n"
        "}\n";
    auto shader = create_shader(vertex_shader, fragment_shader);
    std::string fragment_shader_green =
        "#version 330 core\n"
        "\n"
        "layout(location = 0) out vec4 color;\n"
        "\n"
        "void main() {\n"
        " color = vec4(0.0, 1.0, 0.0, 1.0);\n"
        "}\n";
    auto shader_green = create_shader(vertex_shader, fragment_shader_green);

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT);

        /* Draw maze */
//        glLineWidth(4.0f);
        glUseProgram(shader_green);
        glBindBuffer(GL_ARRAY_BUFFER, buffer_paths);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), 0);
        glDrawArrays(GL_LINES, 0, size_paths);

//        glLineWidth(1.0f);
        glUseProgram(shader);
        glBindBuffer(GL_ARRAY_BUFFER, buffer_walls);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), 0);
        glDrawArrays(GL_LINES, 0, size_walls);

        /* Swap front and back buffers */
        glfwSwapBuffers(window);
        glfwSwapInterval(1);

        /* Poll for and process events */
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}

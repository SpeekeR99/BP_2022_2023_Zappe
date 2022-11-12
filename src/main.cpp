#include "GL/glew.h"
#include "GLFW/glfw3.h"
#include "glm/glm.hpp"
#include "const.h"
#include "graphics/shader.h"
#include "graphics/drawing.h"
#include "maze/graph.h"
#include "maze/generator.h"

int main() {
    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Maze", nullptr, nullptr);
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

    /* Black background */
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    /* Generate maze */
    auto graph = Graph::create_orthogonal_grid_graph(WINDOW_WIDTH / GRID_SIZE - 1, WINDOW_HEIGHT / GRID_SIZE - 1);
    auto maze = Generator::generate_maze_dfs(graph);

    /* Buffer maze */
    int size_paths, buffer_paths, size_walls, buffer_walls;
    Drawing::buffer_graph(maze, size_paths, buffer_paths);

    /* Create shader_basic */
    auto source_basic = Shader::parse_shader("src/graphics/shaders/basic.shader");
    auto shader_basic = Shader::create_shader(source_basic.vertex_source, source_basic.fragment_source);

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT);

        /* Draw maze */
        glLineWidth(LINE_WIDTH);
        glUseProgram(shader_basic);
        glBindBuffer(GL_ARRAY_BUFFER, buffer_paths);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), 0);
        glDrawArrays(GL_LINES, 0, size_paths);

        /* Swap front and back buffers */
        glfwSwapBuffers(window);
        glfwSwapInterval(1);

        /* Poll for and process events */
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}

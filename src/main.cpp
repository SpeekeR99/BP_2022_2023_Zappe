#include "GL/glew.h"
#include "GLFW/glfw3.h"
#include "glm/glm.hpp"
#include "graphics/drawing.h"
#include "maze/graph.h"
#include "maze/generator.h"

int width = 1080;
int height = 620;

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
//    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

//    auto maze = Generator::generate_maze_dfs(20, 11);

    /* Modern OpenGL */
    float positions[6] = {
            -0.5f, -0.5f,
            0.0f, 0.5f,
            0.5f, -0.5f
    };

    unsigned int buffer;
    glGenBuffers(1, &buffer);
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glBufferData(GL_ARRAY_BUFFER, 6 * sizeof(float), positions, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, nullptr);
    glEnableVertexAttribArray(0);

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT);

//        Drawing::draw_graph(maze, {0, 155, 0});
//        Drawing::draw_graph(Graph::transform_paths_to_walls(maze), {0, 0, 0});

        /* Legacy OpenGL drawing */
//        glBegin(GL_TRIANGLES);
//        glVertex2f(-0.5f, -0.5f);
//        glVertex2f(0.0f, 0.5f);
//        glVertex2f(0.5f, -0.5f);
//        glEnd();

        /* Modern OpenGL drawing */
        glDrawArrays(GL_TRIANGLES, 0, 3);

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}

#include <glfw/glfw3.h>
#include "drawing.h"
#include "graph.h"

int width = 640;
int height = 480;

int main() {
    auto grid_graph = Graph::create_grid_graph(5);
    grid_graph->print_adj();

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

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT);

//        draw_line(0, 0, 100, 100, {255, 0, 0}, 5.0f);
//        draw_line(100, 100, 200, 200, {0, 255, 0}, 3.0f);
//        draw_line(200, 200, 300, 300, {0, 0, 255});
        grid_graph->draw_grid_graph();

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}

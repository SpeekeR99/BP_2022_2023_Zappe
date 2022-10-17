#include "GLFW/glfw3.h"
#include "graphics/drawing.h"
#include "maze/graph.h"

int width = 640;
int height = 480;

int main() {
    auto ortho_grid_graph = Graph::create_orthogonal_grid_graph(3, 8);
    auto hex_grid_graph = Graph::create_hexagonal_grid_graph(10, 6);
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

    /* White background */
//    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT);

//        draw_line(0, 0, 100, 100, {255, 0, 0}, 5.0f);
//        draw_line(100, 100, 200, 200, {0, 255, 0}, 3.0f);
//        draw_line(200, 200, 300, 300, {0, 0, 255});
//        Drawing::draw_grid_graph(ortho_grid_graph);
        Drawing::draw_grid_graph(hex_grid_graph);

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}

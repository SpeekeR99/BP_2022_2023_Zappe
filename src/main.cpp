#include "GL/glew.h"
#include "GLFW/glfw3.h"
#include "glm/glm.hpp"
#include "graphics/drawing.h"
#include "maze/graph.h"
#include "maze/generator.h"

int width = 1080;
int height = 620;

int main() {
    auto maze = Generator::generate_maze_dfs(20, 11);

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

    if (glewInit() != GLEW_OK) {
        std::cout << "Error!" << std::endl;
    }
    std::cout << glGetString(GL_VERSION) << std::endl;

    glm::vec4 pos = glm::vec4( glm::vec3(0.0f), 1.0f);
    glm::mat4 model = glm::mat4(1.0f);
    model[3] = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
    glm::vec4 trans = model * pos;

    /* White background */
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT);

        Drawing::draw_graph(maze, {0, 155, 0});
        Drawing::draw_graph(Graph::transform_paths_to_walls(maze), {0, 0, 0});

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}

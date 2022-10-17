#include <glfw/glfw3.h>
#include <array>

int width = 640;
int height = 480;

void draw_line(int x0, int y0, int x1, int y1, std::array<int, 3> color = {255, 255, 255}) {
    float x_start = static_cast<float>(x0) / static_cast<float>(static_cast<double>(width) / 2) - 1.0f;
    float y_start = -1 * (static_cast<float>(y0) / static_cast<float>(static_cast<double>(height) / 2) - 1.0f);
    float x_end = static_cast<float>(x1) / static_cast<float>(static_cast<double>(width) / 2) - 1.0f;
    float y_end = -1 * (static_cast<float>(y1) / static_cast<float>(static_cast<double>(height) / 2) - 1.0f);
    float red = static_cast<float>(color[0])/255;
    float green = static_cast<float>(color[1])/255;
    float blue = static_cast<float>(color[2])/255;
    glBegin(GL_LINES);
    glColor3f(red, green, blue);
    glVertex2f(x_start, y_start);
    glVertex2f(x_end, y_end);
    glEnd();
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

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT);

        draw_line(0, 0, 100, 100, {255, 0, 0});
        draw_line(100, 100, 200, 200, {0, 255, 0});
        draw_line(200, 200, 300, 300, {0, 0, 255});

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}

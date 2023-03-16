#pragma once

#include <array>
#include <memory>
#include "GL/glew.h"
#include "GLFW/glfw3.h"
#include "../maze/graph.h"
#include "shaders/VAO.h"
#include "shaders/VBO.h"
#include "shaders/EBO.h"
#include "imgui.h"

/**
 * Drawing class
 */
class Drawing {
public:
    /**
     * Transforms x and y coordinates from window coordinates to OpenGL coordinates
     * @param x Original x coordinate
     * @param y Original y coordinate
     * @param x_opengl X coordinate in OpenGL coordinates
     * @param y_opengl Y coordinate in OpenGL coordinates
     */
    static void transform_x_y_to_opengl(int x, int y, float& x_opengl, float& y_opengl);
    /**
     * Transforms RGB color to OpenGL color
     * @param color Array of RGB color
     * @param red Red color normalized to 0.0 - 1.0
     * @param green Green color normalized to 0.0 - 1.0
     * @param blue Blue color normalized to 0.0 - 1.0
     */
    static void transform_rgb_to_opengl(std::array<int, 3> color, float& red, float& green, float& blue);
    /**
     * Draws a circle
     * @param x X coordinate of the center of the circle
     * @param y Y coordinate of the center of the circle
     * @param radius Radius of the circle
     * @param color Color of the circle
     */
    static void draw_circle(int x, int y, float radius, ImVec4 color);
    /**
     * Buffer graph paths to VAO, VBO and EBO
     * @param vao Vertex array object
     * @param vbo Vertex buffer object
     * @param ebo Element buffer object
     * @param graph Maze graph to buffer
     * @param color Color of the graph
     */
    static void buffer_graph(std::shared_ptr<VAO> &vao, std::shared_ptr<VBO> &vbo, std::shared_ptr<EBO> &ebo, const std::shared_ptr<Graph>& graph, ImVec4 color);

    /**
     * Buffer lines to VAO, VBO and EBO
     * @param vao Vertex array object
     * @param vbo Vertex buffer object
     * @param ebo Element buffer object
     * @param lines Lines to buffer
     * @param color Color of the lines
     */
    static void buffer_lines(std::shared_ptr<VAO> &vao, std::shared_ptr<VBO> &vbo, std::shared_ptr<EBO> &ebo, const std::vector<std::pair<int, int>> &lines, ImVec4 color);
};
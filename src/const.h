#pragma once

constexpr double PI = 3.14159265359;
const std::string SHADER_STRING = "#shader vertex\n"
                                  "#version 330 core\n"
                                  "\n"
                                  "layout(location = 0) in vec2 aPosition;\n"
                                  "layout(location = 1) in vec3 aColor;\n"
                                  "\n"
                                  "out vec3 color;\n"
                                  "\n"
                                  "void main() {\n"
                                  "    gl_Position = vec4(aPosition.x, aPosition.y, 0.0f, 1.0f);\n"
                                  "    color = aColor;\n"
                                  "}\n"
                                  "\n"
                                  "#shader fragment\n"
                                  "#version 330 core\n"
                                  "\n"
                                  "in vec3 color;\n"
                                  "\n"
                                  "out vec4 fragColor;\n"
                                  "\n"
                                  "void main() {\n"
                                  "    fragColor = vec4(color, 0.5f);\n"
                                  "}";

extern int WINDOW_WIDTH;
extern int WINDOW_HEIGHT;
extern int WINDOW_X_OFFSET;
extern int GRID_SIZE;
extern float WHITE_LINE_WIDTH;
extern float WHITE_NODE_RADIUS;
extern float PLAYER_RADIUS;

enum class MazeType {
    STATIC = 0,
    DYNAMIC = 1
};

enum class GraphType {
    ORTHOGONAL = 0,
    HEXAGONAL = 1,
    LAPLACIAN = 2
};

enum class NeighborhoodGraphType {
    SAME_AS_GRAPH_TYPE = 0,
    ORTHOGONAL = 1,
    HEXAGONAL = 2,
    LAPLACIAN = 3
};

enum class GeneratorType {
    DEPTH_FIRST_SEARCH = 0,
    KRUSKAL = 1
};

enum class SolverType {
    BREATH_FIRST_SEARCH = 0,
    DIJKSTRA = 1,
    A_STAR = 2
};

enum class HeuristicType {
    CONSTANT_ZERO = -1,
    MANHATTAN_DISTANCE = 0,
    EUCLIDEAN_DISTANCE = 1,
    COSINE_DISTANCE = 2
};
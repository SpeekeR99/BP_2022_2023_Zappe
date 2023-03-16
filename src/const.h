#pragma once

/** PI */
constexpr double PI = 3.14159265359;
/** Shader used to draw the maze - copied from src/graphics/shaders/default.shader to make this app launchable from anywhere */
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

/** Window width */
extern int WINDOW_WIDTH;
/** Window height */
extern int WINDOW_HEIGHT;
/** Window x offset */
extern int WINDOW_X_OFFSET;
/** Grid size */
extern int GRID_SIZE;
/** Width of the white line */
extern float WHITE_LINE_WIDTH;
/** Radius of the white node */
extern float WHITE_NODE_RADIUS;
/** Radius of the player */
extern float PLAYER_RADIUS;

/**
 * Maze type
 */
enum class MazeType {
    STATIC = 0,
    DYNAMIC = 1
};

/**
 * Graph type
 */
enum class GraphType {
    ORTHOGONAL = 0,
    HEXAGONAL = 1,
    LAPLACIAN = 2
};

/**
 * Neighborhood graph type
 */
enum class NeighborhoodGraphType {
    SAME_AS_GRAPH_TYPE = 0,
    ORTHOGONAL = 1,
    HEXAGONAL = 2,
    LAPLACIAN = 3
};

/**
 * Generator type
 */
enum class GeneratorType {
    DEPTH_FIRST_SEARCH = 0,
    KRUSKAL = 1
};

/**
 * Solver type
 */
enum class SolverType {
    BREATH_FIRST_SEARCH = 0,
    DIJKSTRA = 1,
    A_STAR = 2
};

/**
 * Heuristic type
 */
enum class HeuristicType {
    CONSTANT_ZERO = -1,
    MANHATTAN_DISTANCE = 0,
    EUCLIDEAN_DISTANCE = 1,
    COSINE_DISTANCE = 2
};
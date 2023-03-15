#pragma once

#include <algorithm>

constexpr double PI = 3.14159265359;
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
};

enum class SolverType {
    BREATH_FIRST_SEARCH = 0,
    DIJKSTRA = 1,
    A_STAR = 2,
};

enum class HeuristicType {
    CONSTANT_ZERO = -1,
    MANHATTAN_DISTANCE = 0,
    EUCLIDEAN_DISTANCE = 1,
    COSINE_DISTANCE = 2,
};
#pragma once

#include <memory>
#include <vector>
#include <random>
#include <chrono>
#include <algorithm>
#include "graph.h"
#include "disjoint_union_sets.h"

class Generator {
public:
    static std::shared_ptr<Graph> create_orthogonal_grid_graph(int width, int height, bool non_grid = false);
    static std::shared_ptr<Graph> create_hexagonal_grid_graph(int width, int height, bool non_grid = false);
    static std::shared_ptr<Graph> create_orthogonal_grid_graph_laplacian(int width, int height, bool non_grid = false);

    static std::shared_ptr<Graph> generate_maze_dfs(std::shared_ptr<Graph> &maze);
    static std::shared_ptr<Graph> generate_maze_kruskal(std::shared_ptr<Graph> &maze, float horizontal_bias = 0.5f, float vertical_bias = 0.5f, float cycle_bias = 0.0f);
};

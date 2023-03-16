#pragma once

#include <memory>
#include <vector>
#include <random>
#include <chrono>
#include <algorithm>
#include "graph.h"
#include "disjoint_union_sets.h"

/**
 * Generator class
 */
class Generator {
public:
    /**
     * Create a graph with a grid orthogonal structure
     * @param width Width of the grid
     * @param height Height of the grid
     * @param non_grid Non-grid flag (default: false)
     * @return Orthogonal grid graph
     */
    static std::shared_ptr<Graph> create_orthogonal_grid_graph(int width, int height, bool non_grid = false);
    /**
     * Create a graph with a grid hexagonal structure
     * @param width Width of the grid
     * @param height Height of the grid
     * @param non_grid Non-grid flag (default: false)
     * @return Hexagonal grid graph
     */
    static std::shared_ptr<Graph> create_hexagonal_grid_graph(int width, int height, bool non_grid = false);
    /**
     * Create a graph with a grid orthogonal structure with diagonals
     * @param width Width of the grid
     * @param height Height of the grid
     * @param non_grid Non-grid flag (default: false)
     * @return Orthogonal grid graph with diagonals
     */
    static std::shared_ptr<Graph> create_orthogonal_grid_graph_laplacian(int width, int height, bool non_grid = false);

    /**
     * Generate a maze using the depth-first search algorithm
     * @param maze Base graph
     * @return Maze graph
     */
    static std::shared_ptr<Graph> generate_maze_dfs(std::shared_ptr<Graph> &maze);

    /**
     * Generate a maze using the Modified Kruskal's algorithm
     * @param maze Base graph
     * @param horizontal_bias Horizontal bias
     * @param vertical_bias Vertical bias
     * @param cycle_bias Cycle bias
     * @return Maze graph
     */
    static std::shared_ptr<Graph> generate_maze_kruskal(std::shared_ptr<Graph> &maze, float horizontal_bias = 0.5f, float vertical_bias = 0.5f, float cycle_bias = 0.0f);
};

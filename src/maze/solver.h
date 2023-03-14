#pragma once

#include <array>
#include <vector>
#include "graph.h"

class Solver {
public:
    static bool is_maze_solvable_bfs(std::shared_ptr<Graph> &maze, const std::pair<int, int> &start, const std::pair<int, int> &end);
    static std::vector<std::pair<int, int>> solve_maze_bfs(std::shared_ptr<Graph> &maze, const std::pair<int, int> &start, const std::pair<int, int> &end);
    static bool is_maze_solvable_dijkstra(std::shared_ptr<Graph> &maze, const std::pair<int, int> &start, const std::pair<int, int> &end);
    static std::vector<std::pair<int, int>> solve_maze_dijkstra(std::shared_ptr<Graph> &maze, const std::pair<int, int> &start, const std::pair<int, int> &end);
    static bool is_maze_solvable_a_star(std::shared_ptr<Graph> &maze, const std::pair<int, int> &start, const std::pair<int, int> &end);
    static std::vector<std::pair<int, int>> solve_maze_a_star(std::shared_ptr<Graph> &maze, const std::pair<int, int> &start, const std::pair<int, int> &end);
};



#include "solver.h"

#include <ranges>

bool Solver::is_maze_solvable_bfs(std::shared_ptr<Graph> &maze, const std::pair<int, int> &start, const std::pair<int, int> &end) {
    int current;
    std::vector<int> queue;
    std::vector<bool> visited(maze->get_v(), false);

    for (auto &node : maze->get_nodes()) {
        if (node->get_x() == start.first && node->get_y() == start.second) {
            current = node->get_v();
            break;
        }
    }

    queue.push_back(current);
    visited[current] = true;

    while (!queue.empty()) {
        current = queue.front();
        queue.erase(queue.begin());

        if (maze->get_nodes()[current]->get_x() == end.first && maze->get_nodes()[current]->get_y() == end.second)
            return true;

        std::vector<int> neighbors;
        for (auto &node: maze->get_nodes()) {
            if (maze->is_adjacent(current, node->get_v()) && !visited[node->get_v()] && maze->get_nodes()[node->get_v()]->is_alive()) {
                neighbors.push_back(node->get_v());
            }
        }

        for (auto &neighbor : neighbors) {
            queue.push_back(neighbor);
            visited[neighbor] = true;
        }
    }

    return false;
}

std::vector<std::pair<int, int>> Solver::solve_maze_bfs(std::shared_ptr<Graph> &maze, const std::pair<int, int> &start, const std::pair<int, int> &end) {
    if (!is_maze_solvable_bfs(maze, start, end))
        return {};

    std::vector<int> path;
    std::vector<int> shortest_path;
    int current = 0;
    std::vector<int> queue;
    std::vector<bool> visited(maze->get_v(), false);

    for (auto &node : maze->get_nodes()) {
        if (node->get_x() == start.first && node->get_y() == start.second) {
            current = node->get_v();
        }
        if (node->get_x() == end.first && node->get_y() == end.second) {
            shortest_path.push_back(node->get_v());
        }
    }

    queue.push_back(current);
    visited[current] = true;

    while (!queue.empty()) {
        current = queue.front();
        queue.erase(queue.begin());
        path.push_back(current);

        if (maze->get_nodes()[current]->get_x() == end.first && maze->get_nodes()[current]->get_y() == end.second)
            break;

        std::vector<int> neighbors;
        for (auto &node: maze->get_nodes()) {
            if (maze->is_adjacent(current, node->get_v()) && !visited[node->get_v()] && maze->get_nodes()[node->get_v()]->is_alive()) {
                neighbors.push_back(node->get_v());
            }
        }

        for (auto &neighbor : neighbors) {
            queue.push_back(neighbor);
            visited[neighbor] = true;
        }
    }

    for (auto &node : path | std::ranges::views::reverse) {
        if (maze->is_adjacent(node, shortest_path.back())) {
            shortest_path.push_back(node);
        }
    }

    std::vector<std::pair<int, int>> result;
    for (auto &node : shortest_path)
        result.emplace_back(maze->get_nodes()[node]->get_x(), maze->get_nodes()[node]->get_y());
    std::reverse(result.begin(), result.end());

    return result;
}
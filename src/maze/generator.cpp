#include "generator.h"

std::shared_ptr<Graph> Generator::generate_maze_dfs(int width, int height) {
    srand(time(nullptr));

    auto to_be_removed_paths = Graph::create_orthogonal_grid_graph(width, height);

    std::vector<int> stack;
    std::vector<bool> visited(to_be_removed_paths->get_v(), false);
    int current = 0;
    stack.push_back(current);
    visited[current] = true;

    while (!stack.empty()) {
        std::vector<int> neighbors;
        for (int i : to_be_removed_paths->get_adj()[current]) {
            if (!visited[i]) {
                neighbors.push_back(i);
            }
        }
        if (neighbors.empty()) {
            stack.pop_back();
            if (!stack.empty()) {
                current = stack.back();
            }
        } else {
            int random_neighbor = neighbors[rand() % neighbors.size()];
            to_be_removed_paths->remove_edge(current, random_neighbor);
            stack.push_back(random_neighbor);
            current = random_neighbor;
            visited[current] = true;
        }
    }

    auto maze = Graph::create_orthogonal_grid_graph(width, height);
    for (int i = 0; i < maze->get_v(); i++) {
        for (int j : maze->get_adj()[i]) {
            if (to_be_removed_paths->is_adjacent(i, j)) {
                maze->remove_edge(i, j);
            }
        }
    }

    return maze;
}

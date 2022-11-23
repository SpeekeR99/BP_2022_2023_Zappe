#include "generator.h"

std::shared_ptr<Graph> Generator::generate_maze_dfs(std::shared_ptr<Graph> &maze) {
    std::default_random_engine gen(std::chrono::system_clock::now().time_since_epoch().count());
    std::uniform_int_distribution<> dis(0, RAND_MAX);

    auto result = maze->create_copy();
    for (int i = 0; i < result->get_v(); i++)
        for (int j = 0; j < result->get_adj()[i].size(); j++)
            result->remove_edge(i, result->get_adj()[i][j]);

    std::vector<int> stack;
    std::vector<bool> visited(maze->get_v(), false);
    int current = 0;
    stack.push_back(current);
    visited[current] = true;

    while (!stack.empty()) {
        std::vector<int> neighbors;
        for (int i : maze->get_adj()[current])
            if (!visited[i])
                neighbors.push_back(i);

        if (neighbors.empty()) {
            stack.pop_back();
            if (!stack.empty())
                current = stack.back();

        } else {
            int random_neighbor = neighbors[dis(gen) % neighbors.size()];
            result->add_edge(maze->get_nodes()[current], maze->get_nodes()[random_neighbor]);
            stack.push_back(random_neighbor);
            current = random_neighbor;
            visited[current] = true;
        }
    }

    return result;
}

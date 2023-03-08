#include "generator.h"

std::shared_ptr<Graph> Generator::create_orthogonal_grid_graph(int width, int height, bool non_grid) {
    auto grid_graph = std::make_shared<Graph>(width, height);
    std::default_random_engine gen(std::chrono::system_clock::now().time_since_epoch().count());
    std::normal_distribution<> dis(0, 1);

    for (int i = 0; i < width; i++) {
        for (int j = 0; j < height; j++) {
            int v = i * height + j;
            int x = i * GRID_SIZE + GRID_SIZE;
            int y = j * GRID_SIZE + GRID_SIZE;
            if (non_grid) {
                x += static_cast<int>(dis(gen) * GRID_SIZE / 8);
                y += static_cast<int>(dis(gen) * GRID_SIZE / 8);
            }
            grid_graph->set_node(v, x, y);

            if (i > 0)
                grid_graph->add_edge(v, (i - 1) * height + j);
            if (j > 0)
                grid_graph->add_edge(v, i * height + j - 1);
        }
    }

    return grid_graph;
}

std::shared_ptr<Graph> Generator::create_hexagonal_grid_graph(int width, int height, bool non_grid) {
    auto grid_graph = std::make_shared<Graph>(width, height);
    std::default_random_engine gen(std::chrono::system_clock::now().time_since_epoch().count());
    std::normal_distribution<> dis(0, 1);

    for (int i = 0; i < width; i++) {
        for (int j = 0; j < height; j++) {
            int v = i * height + j;
            int x = i * GRID_SIZE + GRID_SIZE;
            int y = j * GRID_SIZE + GRID_SIZE;
            if (i % 2 == 1)
                y += GRID_SIZE / 2;
            if (non_grid) {
                x += static_cast<int>(dis(gen) * GRID_SIZE / 8);
                y += static_cast<int>(dis(gen) * GRID_SIZE / 8);
            }
            grid_graph->set_node(v, x, y);

            if (i > 0)
                grid_graph->add_edge(v, (i - 1) * height + j);
            if (j > 0)
                grid_graph->add_edge(v, i * height + j - 1);
            if (i > 0 && j > 0 && i % 2 == 0)
                grid_graph->add_edge(v, (i - 1) * height + j - 1);
            if (i > 0 && j < height - 1 && i % 2 == 1)
                grid_graph->add_edge(v, (i - 1) * height + j + 1);
        }
    }

    return grid_graph;
}

std::shared_ptr<Graph> Generator::create_orthogonal_grid_graph_laplacian(int width, int height, bool non_grid) {
    auto grid_graph = std::make_shared<Graph>(width, height);
    std::default_random_engine gen(std::chrono::system_clock::now().time_since_epoch().count());
    std::normal_distribution<> dis(0, 1);

    for (int i = 0; i < width; i++) {
        for (int j = 0; j < height; j++) {
            int v = i * height + j;
            int x = i * GRID_SIZE + GRID_SIZE;
            int y = j * GRID_SIZE + GRID_SIZE;
            if (non_grid) {
                x += static_cast<int>(dis(gen) * GRID_SIZE / 8);
                y += static_cast<int>(dis(gen) * GRID_SIZE / 8);
            }
            grid_graph->set_node(v, x, y);

            if (i > 0)
                grid_graph->add_edge(v, (i - 1) * height + j);
            if (j > 0)
                grid_graph->add_edge(v, i * height + j - 1);
            if (i > 0 && j > 0)
                grid_graph->add_edge(v, (i - 1) * height + j - 1);
            if (i > 0 && j < height - 1)
                grid_graph->add_edge(v, (i - 1) * height + j + 1);
        }
    }

    return grid_graph;
}

std::shared_ptr<Graph> Generator::generate_maze_dfs(std::shared_ptr<Graph> &maze) {
    std::default_random_engine gen(std::chrono::system_clock::now().time_since_epoch().count());
    std::uniform_int_distribution<> dis(0, RAND_MAX);

    auto to_be_removed_paths = maze->create_copy();

    std::vector<int> stack;
    std::vector<bool> visited(maze->get_v(), false);
    int current = 0;
    stack.push_back(current);
    visited[current] = true;

    while (!stack.empty()) {
        std::vector<int> neighbors;
        for (int i : to_be_removed_paths->get_adj()[current])
            if (!visited[i])
                neighbors.push_back(i);

        if (neighbors.empty()) {
            stack.pop_back();
            if (!stack.empty())
                current = stack.back();

        } else {
            int random_neighbor = neighbors[dis(gen) % neighbors.size()];
            to_be_removed_paths->remove_edge(current, random_neighbor);
            stack.push_back(random_neighbor);
            current = random_neighbor;
            visited[current] = true;
        }
    }

    for (int i = 0; i < maze->get_v(); i++) {
        for (int j = 0; j < maze->get_v(); j++) {
            if (to_be_removed_paths->is_adjacent(i, j)) {
                maze->remove_edge(i, j);
            }
        }
    }

    return maze;
}

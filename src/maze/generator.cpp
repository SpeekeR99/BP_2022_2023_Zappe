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

bool is_horizontal(int x1, int y1, int x2, int y2) {
    return abs(x1 - x2) > abs(y1 - y2);
}

std::shared_ptr<Graph>
Generator::generate_maze_kruskal(std::shared_ptr<Graph> &maze, float horizontal_bias, float vertical_bias,
                                 float cycle_bias) {
    int to_add = (int) (cycle_bias * (float) (maze->get_v() - maze->get_width() - maze->get_height() + 1));
    int total_walls = maze->get_v() - maze->get_width() - maze->get_height() - to_add + 1;
    int horizontal_walls = (int) (vertical_bias * (float) total_walls);
    int vertical_walls = total_walls - horizontal_walls;
    int horizontal_edges = maze->get_v() - maze->get_width() - horizontal_walls;
    int vertical_edges = maze->get_v() - maze->get_height() - vertical_walls;
    int counter_horizontal = 0;
    int counter_vertical = 0;
    std::shared_ptr<Graph> available_edges = maze->create_copy();
    std::shared_ptr<Graph> maze_copy = maze->create_copy();
    Disjoint_union_sets disjoint_union_sets(maze->get_v());

    std::default_random_engine gen(std::chrono::system_clock::now().time_since_epoch().count());
    std::uniform_int_distribution dis(0, RAND_MAX);

    for (int i = 0; i < maze->get_v(); i++) {
        std::shuffle(maze_copy->get_adj()[i].begin(), maze_copy->get_adj()[i].end(), gen);
        for (int j : maze->get_adj()[i]) {
            maze->remove_edge(i, j);
        }
    }

    std::vector<int> random_indices = std::vector<int>(maze->get_v());
    std::iota(random_indices.begin(), random_indices.end(), 0);
    std::shuffle(random_indices.begin(), random_indices.end(), gen);

    for (int i = 0; i < maze->get_height() - 1; i++) {
        bool break_flag = false;
        for (int j : random_indices) {
            for (int k : random_indices) {
                if (j % maze->get_height() == i && k % maze->get_height() == i + 1 && available_edges->is_adjacent(j, k)) {
                    maze->add_edge(j, k);
                    available_edges->remove_edge(j, k);
                    disjoint_union_sets.merge(j, k);
                    counter_vertical++;
                    break_flag = true;
                    break;
                }
            }
            if (break_flag)
                break;
        }
    }

    for (int i = 0; i < maze->get_width() - 1; i++) {
        bool break_flag = false;
        for (int j : random_indices) {
            for (int k : random_indices) {
                if (j / maze->get_height() == i && k / maze->get_height() == i + 1 && available_edges->is_adjacent(j, k)) {
                    maze->add_edge(j, k);
                    available_edges->remove_edge(j, k);
                    disjoint_union_sets.merge(j, k);
                    counter_horizontal++;
                    break_flag = true;
                    break;
                }
            }
            if (break_flag)
                break;
        }
    }

    for (int i : random_indices) {
        for (int j : maze_copy->get_adj()[i]) {
            if (disjoint_union_sets.find(i) != disjoint_union_sets.find(j)) {
                if (is_horizontal(maze->get_nodes()[i]->get_x(), maze->get_nodes()[i]->get_y(),
                                  maze->get_nodes()[j]->get_x(), maze->get_nodes()[j]->get_y())) {
                    if (counter_horizontal < horizontal_edges) {
                        maze->add_edge(i, j);
                        available_edges->remove_edge(i, j);
                        disjoint_union_sets.merge(i, j);
                        counter_horizontal++;
                    }
                } else {
                    if (counter_vertical < vertical_edges) {
                        maze->add_edge(i, j);
                        available_edges->remove_edge(i, j);
                        disjoint_union_sets.merge(i, j);
                        counter_vertical++;
                    }
                }
            }
        }
    }

    while (counter_horizontal + counter_vertical < horizontal_edges + vertical_edges) {
        for (int i : random_indices) {
            for (int j : random_indices) {
                if (available_edges->is_adjacent(i, j)) {
                    if (is_horizontal(maze->get_nodes()[i]->get_x(), maze->get_nodes()[i]->get_y(),
                                      maze->get_nodes()[j]->get_x(), maze->get_nodes()[j]->get_y())) {
                        if (counter_horizontal < horizontal_edges) {
                            maze->add_edge(i, j);
                            available_edges->remove_edge(i, j);
                            disjoint_union_sets.merge(i, j);
                            counter_horizontal++;
                        }
                    } else {
                        if (counter_vertical < vertical_edges) {
                            maze->add_edge(i, j);
                            available_edges->remove_edge(i, j);
                            disjoint_union_sets.merge(i, j);
                            counter_vertical++;
                        }
                    }
                }
            }
        }
    }

    return maze;
}

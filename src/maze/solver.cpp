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

bool Solver::is_maze_solvable_dijkstra(std::shared_ptr<Graph> &maze, const std::pair<int, int> &start,
                                       const std::pair<int, int> &end) {
    std::vector<int> g_score(maze->get_v(), INT_MAX);
    std::vector<int> f_score(maze->get_v(), INT_MAX);
    std::vector<int> previous(maze->get_v(), -1);
    std::vector<int> list(maze->get_v());

    auto start_node = maze->get_nearest_node_to(start.first, start.second);
    auto end_node = maze->get_nearest_node_to(end.first, end.second);

    g_score[start_node] = 0;
    f_score[start_node] = 0;
    list.push_back(start_node);

    int current = -1;
    while (!list.empty()) {
        int min = INT_MAX;
        for (auto &node : list) {
            if (f_score[node] < min) {
                min = f_score[node];
                current = node;
            }
        }
        list.erase(std::remove(list.begin(), list.end(), current), list.end());

        if (current == end_node)
            return true;

        for (auto &node: maze->get_nodes()) {
            if (maze->is_adjacent(current, node->get_v()) && maze->get_nodes()[node->get_v()]->is_alive()) {
                auto tentative_g_score = g_score[current] + 1;
                if (tentative_g_score < g_score[node->get_v()]) {
                    previous[node->get_v()] = current;
                    g_score[node->get_v()] = tentative_g_score;
                    f_score[node->get_v()] = g_score[node->get_v()] + 0;
                    if (std::find(list.begin(), list.end(), node->get_v()) == list.end())
                        list.push_back(node->get_v());
                }
            }
        }
    }

    return false;
}

std::vector<std::pair<int, int>>
Solver::solve_maze_dijkstra(std::shared_ptr<Graph> &maze, const std::pair<int, int> &start,
                            const std::pair<int, int> &end) {
    if (!is_maze_solvable_dijkstra(maze, start, end))
        return {};

    std::vector<int> g_score(maze->get_v(), INT_MAX);
    std::vector<int> f_score(maze->get_v(), INT_MAX);
    std::vector<int> previous(maze->get_v(), -1);
    std::vector<int> list(maze->get_v());

    auto start_node = maze->get_nearest_node_to(start.first, start.second);
    auto end_node = maze->get_nearest_node_to(end.first, end.second);

    g_score[start_node] = 0;
    f_score[start_node] = 0; // h() = 0 for dijkstra
    list.push_back(start_node);

    int current = -1;
    while (!list.empty()) {
        int min = INT_MAX;
        for (auto &node : list) {
            if (f_score[node] < min) {
                min = f_score[node];
                current = node;
            }
        }
        list.erase(std::remove(list.begin(), list.end(), current), list.end());

        if (current == end_node)
            break;

        for (auto &node: maze->get_nodes()) {
            if (maze->is_adjacent(current, node->get_v()) && maze->get_nodes()[node->get_v()]->is_alive()) {
                auto tentative_g_score = g_score[current] + 1;
                if (tentative_g_score < g_score[node->get_v()]) {
                    previous[node->get_v()] = current;
                    g_score[node->get_v()] = tentative_g_score;
                    f_score[node->get_v()] = g_score[node->get_v()] + 0; // h() = 0 for dijkstra
                    if (std::find(list.begin(), list.end(), node->get_v()) == list.end())
                        list.push_back(node->get_v());
                }
            }
        }
    }

    std::vector<int> path;
    while (current != start_node) {
        path.push_back(current);
        current = previous[current];
    }
    path.push_back(start_node);

    std::vector<std::pair<int, int>> result;
    for (auto &index : path)
        result.emplace_back(maze->get_nodes()[index]->get_x(), maze->get_nodes()[index]->get_y());


    return result;
}

int diagonal_distance(int x1, int y1, int x2, int y2) {
    int dx = std::abs(x1 - x2);
    int dy = std::abs(y1 - y2);
    return (dx + dy) + (std::sqrt(2) - 2) * std::min(dx, dy);
}

int manhattan_distance(int x1, int y1, int x2, int y2) {
    return std::abs(x1 - x2) + std::abs(y1 - y2);
}

int euclidean_distance(int x1, int y1, int x2, int y2) {
    return std::sqrt(std::pow(x1 - x2, 2) + std::pow(y1 - y2, 2));
}

int euclidean_distance_squared(int x1, int y1, int x2, int y2) {
    return std::pow(x1 - x2, 2) + std::pow(y1 - y2, 2);
}

bool Solver::is_maze_solvable_a_star(std::shared_ptr<Graph> &maze, const std::pair<int, int> &start,
                                     const std::pair<int, int> &end, int heuristic) {
    std::vector<int> g_score(maze->get_v(), INT_MAX);
    std::vector<int> f_score(maze->get_v(), INT_MAX);
    std::vector<int> previous(maze->get_v(), -1);
    std::vector<int> list(maze->get_v());

    auto start_node = maze->get_nearest_node_to(start.first, start.second);
    auto end_node = maze->get_nearest_node_to(end.first, end.second);

    g_score[start_node] = 0;
    if (heuristic == 0)
        f_score[start_node] = manhattan_distance(start.first, start.second, end.first, end.second);
    else if (heuristic == 1)
        f_score[start_node] = euclidean_distance(start.first, start.second, end.first, end.second);
    else if (heuristic == 2)
        f_score[start_node] = euclidean_distance_squared(start.first, start.second, end.first, end.second);
    else if (heuristic == 3)
        f_score[start_node] = diagonal_distance(start.first, start.second, end.first, end.second);
    list.push_back(start_node);

    int current = -1;
    while (!list.empty()) {
        int min = INT_MAX;
        for (auto &node : list) {
            if (f_score[node] < min) {
                min = f_score[node];
                current = node;
            }
        }
        list.erase(std::remove(list.begin(), list.end(), current), list.end());

        if (current == end_node)
            return true;

        for (auto &node: maze->get_nodes()) {
            if (maze->is_adjacent(current, node->get_v()) && maze->get_nodes()[node->get_v()]->is_alive()) {
                auto tentative_g_score = g_score[current] + 1;
                if (tentative_g_score < g_score[node->get_v()]) {
                    previous[node->get_v()] = current;
                    g_score[node->get_v()] = tentative_g_score;
                    f_score[node->get_v()] = g_score[node->get_v()];
                    if (heuristic == 0)
                        f_score[node->get_v()] += manhattan_distance(node->get_x(), node->get_y(), end.first, end.second);
                    else if (heuristic == 1)
                        f_score[node->get_v()] += euclidean_distance(node->get_x(), node->get_y(), end.first, end.second);
                    else if (heuristic == 2)
                        f_score[node->get_v()] += euclidean_distance_squared(node->get_x(), node->get_y(), end.first, end.second);
                    else if (heuristic == 3)
                        f_score[node->get_v()] += diagonal_distance(node->get_x(), node->get_y(), end.first, end.second);
                    if (std::find(list.begin(), list.end(), node->get_v()) == list.end())
                        list.push_back(node->get_v());
                }
            }
        }
    }

    return false;
}

std::vector<std::pair<int, int>>
Solver::solve_maze_a_star(std::shared_ptr<Graph> &maze, const std::pair<int, int> &start,
                          const std::pair<int, int> &end, int heuristic) {
    if (!is_maze_solvable_a_star(maze, start, end))
        return {};

    std::vector<int> g_score(maze->get_v(), INT_MAX);
    std::vector<int> f_score(maze->get_v(), INT_MAX);
    std::vector<int> previous(maze->get_v(), -1);
    std::vector<int> list(maze->get_v());

    auto start_node = maze->get_nearest_node_to(start.first, start.second);
    auto end_node = maze->get_nearest_node_to(end.first, end.second);

    g_score[start_node] = 0;
    if (heuristic == 0)
        f_score[start_node] = manhattan_distance(start.first, start.second, end.first, end.second);
    else if (heuristic == 1)
        f_score[start_node] = euclidean_distance(start.first, start.second, end.first, end.second);
    else if (heuristic == 2)
        f_score[start_node] = euclidean_distance_squared(start.first, start.second, end.first, end.second);
    else if (heuristic == 3)
        f_score[start_node] = diagonal_distance(start.first, start.second, end.first, end.second);
    list.push_back(start_node);

    int current = -1;
    while (!list.empty()) {
        int min = INT_MAX;
        for (auto &node : list) {
            if (f_score[node] < min) {
                min = f_score[node];
                current = node;
            }
        }
        list.erase(std::remove(list.begin(), list.end(), current), list.end());

        if (current == end_node)
            break;

        for (auto &node: maze->get_nodes()) {
            if (maze->is_adjacent(current, node->get_v()) && maze->get_nodes()[node->get_v()]->is_alive()) {
                auto tentative_g_score = g_score[current] + 1;
                if (tentative_g_score < g_score[node->get_v()]) {
                    previous[node->get_v()] = current;
                    g_score[node->get_v()] = tentative_g_score;
                    f_score[node->get_v()] = g_score[node->get_v()];
                    if (heuristic == 0)
                        f_score[node->get_v()] += manhattan_distance(node->get_x(), node->get_y(), end.first, end.second);
                    else if (heuristic == 1)
                        f_score[node->get_v()] += euclidean_distance(node->get_x(), node->get_y(), end.first, end.second);
                    else if (heuristic == 2)
                        f_score[node->get_v()] += euclidean_distance_squared(node->get_x(), node->get_y(), end.first, end.second);
                    else if (heuristic == 3)
                        f_score[node->get_v()] += diagonal_distance(node->get_x(), node->get_y(), end.first, end.second);
                    if (std::find(list.begin(), list.end(), node->get_v()) == list.end())
                        list.push_back(node->get_v());
                }
            }
        }
    }

    std::vector<int> path;
    while (current != start_node) {
        path.push_back(current);
        current = previous[current];
    }
    path.push_back(start_node);

    std::vector<std::pair<int, int>> result;
    for (auto &index : path)
        result.emplace_back(maze->get_nodes()[index]->get_x(), maze->get_nodes()[index]->get_y());


    return result;
}

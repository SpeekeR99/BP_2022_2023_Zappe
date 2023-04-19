#include "solver.h"

bool Solver::is_maze_solvable_bfs(std::shared_ptr<Graph> &maze, const std::pair<int, int> &start,
                                  const std::pair<int, int> &end) {
    int current;
    std::vector<int> queue;
    std::vector<bool> visited(maze->get_v(), false);

    // Set start node as current
    current = maze->get_nearest_node_to(start.first, start.second);

    queue.push_back(current);
    visited[current] = true;

    // While there are nodes to be visited
    while (!queue.empty()) {
        current = queue.front();
        queue.erase(queue.begin());

        // If current node is the end node, return true
        if (maze->get_nodes()[current]->get_x() == end.first && maze->get_nodes()[current]->get_y() == end.second)
            return true;

        // Add all adjacent nodes to the queue
        std::vector<int> neighbors;
        for (auto &node: maze->get_nodes()) {
            if (maze->is_adjacent(current, node->get_v()) && !visited[node->get_v()] &&
                maze->get_nodes()[node->get_v()]->is_alive()) {
                neighbors.push_back(node->get_v());
            }
        }

        for (auto &neighbor: neighbors) {
            queue.push_back(neighbor);
            visited[neighbor] = true;
        }
    }

    // If no path was found, return false
    return false;
}

std::vector<std::pair<int, int>>
Solver::solve_maze_bfs(std::shared_ptr<Graph> &maze, const std::pair<int, int> &start, const std::pair<int, int> &end) {
    // If maze is not solvable, return empty vector
    if (!is_maze_solvable_bfs(maze, start, end))
        return {};

    std::vector<int> previous(maze->get_v(), -1);
    std::vector<int> shortest_path;
    int current = maze->get_nearest_node_to(start.first, start.second);
    std::vector<int> queue;
    std::vector<bool> visited(maze->get_v(), false);

    queue.push_back(current);
    visited[current] = true;

    // While there are nodes to be visited
    while (!queue.empty()) {
        current = queue.front();
        queue.erase(queue.begin());

        // If current node is the end node, break
        if (maze->get_nodes()[current]->get_x() == end.first && maze->get_nodes()[current]->get_y() == end.second)
            break;

        // Add all adjacent nodes to the queue
        std::vector<int> neighbors;
        for (auto &node: maze->get_nodes()) {
            if (maze->is_adjacent(current, node->get_v()) && !visited[node->get_v()] &&
                maze->get_nodes()[node->get_v()]->is_alive()) {
                neighbors.push_back(node->get_v());
            }
        }

        for (auto &neighbor: neighbors) {
            queue.push_back(neighbor);
            previous[neighbor] = current;
            visited[neighbor] = true;
        }
    }

    // Find the shortest path
    current = maze->get_nearest_node_to(end.first, end.second);
    while (current != -1) {
        shortest_path.push_back(current);
        current = previous[current];
    }

    // Convert the shortest path to vector of pairs
    std::vector<std::pair<int, int>> result;
    for (auto &node: shortest_path)
        result.emplace_back(maze->get_nodes()[node]->get_x(), maze->get_nodes()[node]->get_y());
    std::reverse(result.begin(), result.end());

    return result;
}

bool Solver::is_maze_solvable_dijkstra(std::shared_ptr<Graph> &maze, const std::pair<int, int> &start,
                                       const std::pair<int, int> &end) {
    return is_maze_solvable_a_star(maze, start, end,
                                   HeuristicType::CONSTANT_ZERO); // dijkstra == a* with heuristic of constant 0
}

std::vector<std::pair<int, int>>
Solver::solve_maze_dijkstra(std::shared_ptr<Graph> &maze, const std::pair<int, int> &start,
                            const std::pair<int, int> &end) {
    // If maze is not solvable, return empty vector
    if (!is_maze_solvable_dijkstra(maze, start, end))
        return {};

    return solve_maze_a_star(maze, start, end,
                             HeuristicType::CONSTANT_ZERO); // dijkstra == a* with heuristic of constant 0
}

/**
 * Calculates the manhattan distance between two points
 * @param x1 X coordinate of the first point
 * @param y1 Y coordinate of the first point
 * @param x2 X coordinate of the second point
 * @param y2 Y coordinate of the second point
 * @return The manhattan distance between the two points
 */
int manhattan_distance(int x1, int y1, int x2, int y2) {
    return std::abs(x1 - x2) + std::abs(y1 - y2);
}

/**
 * Calculates the euclidean distance between two points
 * @param x1 X coordinate of the first point
 * @param y1 Y coordinate of the first point
 * @param x2 X coordinate of the second point
 * @param y2 Y coordinate of the second point
 * @return The euclidean distance between the two points
 */
int euclidean_distance(int x1, int y1, int x2, int y2) {
    return static_cast<int>(std::sqrt(std::pow(x1 - x2, 2) + std::pow(y1 - y2, 2)));
}

/**
 * Calculates the cosine distance between two points
 * @param x1 X coordinate of the first point
 * @param y1 Y coordinate of the first point
 * @param x2 X coordinate of the second point
 * @param y2 Y coordinate of the second point
 * @return The cosine distance between the two points
 */
int cosine_distance(int x1, int y1, int x2, int y2) {
    return static_cast<int>((x1 * y1 + x2 * y2) / (std::sqrt(std::pow(x1, 2) + std::pow(y1, 2)) *
                                                   std::sqrt(std::pow(x2, 2) + std::pow(y2, 2))));
}

bool Solver::is_maze_solvable_a_star(std::shared_ptr<Graph> &maze, const std::pair<int, int> &start,
                                     const std::pair<int, int> &end, HeuristicType heuristic) {
    return is_maze_solvable_bfs(maze, start, end);
}

std::vector<std::pair<int, int>>
Solver::solve_maze_a_star(std::shared_ptr<Graph> &maze, const std::pair<int, int> &start,
                          const std::pair<int, int> &end, HeuristicType heuristic) {
    // If the maze is not solvable, return an empty vector
    if (!is_maze_solvable_a_star(maze, start, end))
        return {};

    std::vector<int> g_score(maze->get_v(), INT_MAX);
    std::vector<int> f_score(maze->get_v(), INT_MAX);
    std::vector<int> previous(maze->get_v(), -1);
    std::vector<int> list(maze->get_v());

    auto start_node = maze->get_nearest_node_to(start.first, start.second);
    auto end_node = maze->get_nearest_node_to(end.first, end.second);

    g_score[start_node] = 0;
    f_score[start_node] = 0;
    int (*h_funcs[])(int, int, int, int) = {&manhattan_distance, &euclidean_distance, &cosine_distance};

    if (heuristic != HeuristicType::CONSTANT_ZERO)
        f_score[start_node] += h_funcs[static_cast<int>(heuristic)](start.first, start.second, end.first, end.second);
    list.push_back(start_node);

    int current = -1;
    while (!list.empty()) {
        // Find the node with the lowest f_score
        int min = INT_MAX;
        for (auto &node: list) {
            if (f_score[node] < min) {
                min = f_score[node];
                current = node;
            }
        }
        list.erase(std::remove(list.begin(), list.end(), current), list.end());

        // If the current node is the end node, break
        if (current == end_node)
            break;

        // If the current node is not the end node, continue
        for (auto &node: maze->get_nodes()) {
            if (maze->is_adjacent(current, node->get_v()) && maze->get_nodes()[node->get_v()]->is_alive()) {
                // If the current path is better than the previous path, update the path
                auto tentative_g_score = g_score[current] + 1;
                if (tentative_g_score < g_score[node->get_v()]) {
                    previous[node->get_v()] = current;
                    g_score[node->get_v()] = tentative_g_score;
                    f_score[node->get_v()] = g_score[node->get_v()];
                    if (heuristic != HeuristicType::CONSTANT_ZERO)
                        f_score[node->get_v()] += h_funcs[static_cast<int>(heuristic)](node->get_x(), node->get_y(),
                                                                                       end.first, end.second);
                    if (std::find(list.begin(), list.end(), node->get_v()) == list.end())
                        list.push_back(node->get_v());
                }
            }
        }
    }

    // Create the path
    std::vector<int> path;
    while (current != start_node) {
        path.push_back(current);
        current = previous[current];
    }
    path.push_back(start_node);

    // Convert the path to a vector of pairs
    std::vector<std::pair<int, int>> result;
    for (auto &index: path)
        result.emplace_back(maze->get_nodes()[index]->get_x(), maze->get_nodes()[index]->get_y());

    return result;
}

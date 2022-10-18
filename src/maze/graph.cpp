#include "graph.h"

constexpr int offset = 50;
constexpr int grid_size = 50;

Graph::Graph(int width, int height) : width{width}, height{height}, v{width * height} {
    adj = new std::vector<int>[v];
    nodes.resize(v);
}

Graph::~Graph() {
    delete[] adj;
}

int Graph::get_width() const {
    return width;
}

int Graph::get_height() const {
    return height;
}

int Graph::get_v() const {
    return v;
}

std::vector<int> *Graph::get_adj() const {
    return adj;
}

std::vector<std::shared_ptr<Node>> Graph::get_nodes() const {
    return nodes;
}

void Graph::add_edge(const std::shared_ptr<Node>& src, const std::shared_ptr<Node>& dest) {
    adj[src->get_v()].push_back(dest->get_v());
    adj[dest->get_v()].push_back(src->get_v());
}

void Graph::remove_edge(int src, int dest) const {
    for (int i = 0; i < adj[src].size(); i++) {
        if (adj[src][i] == dest) {
            adj[src].erase(adj[src].begin() + i);
            break;
        }
    }
    for (int i = 0; i < adj[dest].size(); i++) {
        if (adj[dest][i] == src) {
            adj[dest].erase(adj[dest].begin() + i);
            break;
        }
    }
}

bool Graph::is_adjacent(int src, int dest) const {
    for (int i : adj[src]) {
        if (i == dest) {
            return true;
        }
    }
    return false;
}

void Graph::print_adj() const {
    for (int i = 0; i < v; i++) {
        std::cout << i << " -> ";
        for (int &j: adj[i]) {
            std::cout << j << " ";
        }
        std::cout << std::endl;
    }
}

std::shared_ptr<Graph> Graph::create_orthogonal_grid_graph(int width, int height) {
    auto grid_graph = std::make_shared<Graph>(width, height);
    for (int i = 0; i < width; i++) {
        for (int j = 0; j < height; j++) {
            int v = i * height + j;
            auto node = std::make_shared<Node>(v, i * grid_size + offset, j * grid_size + offset);
            grid_graph->nodes[v] = node;
            if (i > 0) {
                grid_graph->add_edge(node, grid_graph->nodes[(i - 1) * height + j]);
            }
            if (j > 0) {
                grid_graph->add_edge(node, grid_graph->nodes[i * height + j - 1]);
            }
        }
    }
    return grid_graph;
}

std::shared_ptr<Graph> Graph::create_hexagonal_grid_graph(int width, int height) {
    auto grid_graph = std::make_shared<Graph>(width, height);
    for (int i = 0; i < width; i++) {
        for (int j = 0; j < height; j++) {
            int v = i * height + j;
            int x = i * grid_size + offset;
            int y = j * grid_size + offset;
            if (i % 2 == 1) {
                y += grid_size / 2;
            }
            auto node = std::make_shared<Node>(v, x, y);
            grid_graph->nodes[v] = node;
            if (i > 0) {
                grid_graph->add_edge(node, grid_graph->nodes[(i - 1) * height + j]);
                if (i > 1 && j > 0) {
                    grid_graph->add_edge(node, grid_graph->nodes[(i - 2) * height + j - 1]);
                }
                if (i > 1 && j < height - 1) {
                    grid_graph->add_edge(node, grid_graph->nodes[(i - 2) * height + j + 1]);
                }
            }
            if (j > 0) {
                grid_graph->add_edge(node, grid_graph->nodes[i * height + j - 1]);
            }
        }
    }
    return grid_graph;
}

std::shared_ptr<Graph> Graph::transform_paths_to_walls(const std::shared_ptr<Graph> &maze_paths) {
    auto maze_walls = std::make_shared<Graph>(maze_paths->get_width() + 1, maze_paths->get_height() + 1);

    for (int i = 0; i < maze_walls->get_width(); i++) {
        for (int j = 0; j < maze_walls->get_height(); j++) {
            int v = i * maze_walls->get_height() + j;
            int x = i * grid_size + offset / 2;
            int y = j * grid_size + offset / 2;
            auto node = std::make_shared<Node>(v, x, y);
            maze_walls->nodes[v] = node;
            if (i > 0) {
                maze_walls->add_edge(node, maze_walls->nodes[(i - 1) * maze_walls->get_height() + j]);
            }
            if (j > 0) {
                maze_walls->add_edge(node, maze_walls->nodes[i * maze_walls->get_height() + j - 1]);
            }
        }
    }

    for (int i = 0; i < maze_paths->get_width(); i++) {
        for (int j = 0; j < maze_paths->get_height(); j++) {
            int v = i * maze_paths->get_height() + j;
            if (maze_paths->is_adjacent(v, v + 1)) {
                maze_walls->remove_edge(i * maze_walls->get_height() + j + 1, (i + 1) * maze_walls->get_height() + j + 1);
            }
            if (maze_paths->is_adjacent(v, v + maze_paths->get_height())) {
                maze_walls->remove_edge((i + 1) * maze_walls->get_height() + j, (i + 1) * maze_walls->get_height() + j + 1);
            }
        }
    }

    return maze_walls;
}

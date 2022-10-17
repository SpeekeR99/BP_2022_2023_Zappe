#include "graph.h"

constexpr int offset = 50;
constexpr int grid_size = 50;

Graph::Graph(int v) : v{v} {
    adj = new std::vector<int>[v];
    nodes.resize(v);
}

Graph::~Graph() {
    delete[] adj;
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
    auto grid_graph = std::make_shared<Graph>(width * height);
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
    auto grid_graph = std::make_shared<Graph>(width * height);
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
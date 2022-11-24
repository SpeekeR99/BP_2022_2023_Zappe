#include "graph.h"

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
    adj[src].erase(std::remove(adj[src].begin(), adj[src].end(), dest), adj[src].end());
    adj[dest].erase(std::remove(adj[dest].begin(), adj[dest].end(), src), adj[dest].end());
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

std::shared_ptr<Graph> Graph::create_copy() const {
    auto copy = std::make_unique<Graph>(width, height);
    for (int i = 0; i < v; i++) {
        copy->adj[i] = adj[i];
    }
    copy->nodes = nodes;
    return copy;
}

std::shared_ptr<Graph> Graph::create_orthogonal_grid_graph(int width, int height) {
    auto grid_graph = std::make_shared<Graph>(width, height);
    for (int i = 0; i < width; i++) {
        for (int j = 0; j < height; j++) {
            int v = i * height + j;
            int x = i * GRID_SIZE + GRID_SIZE;
            int y = j * GRID_SIZE + GRID_SIZE;
            auto node = std::make_shared<Node>(v, x, y);
            grid_graph->nodes[v] = node;

            if (i > 0)
                grid_graph->add_edge(node, grid_graph->nodes[(i - 1) * height + j]);
            if (j > 0)
                grid_graph->add_edge(node, grid_graph->nodes[i * height + j - 1]);
        }
    }
    return grid_graph;
}

std::shared_ptr<Graph> Graph::create_hexagonal_grid_graph(int width, int height) {
    auto grid_graph = std::make_shared<Graph>(width, height);
    for (int i = 0; i < width; i++) {
        for (int j = 0; j < height; j++) {
            int v = i * height + j;
            int x = i * GRID_SIZE + GRID_SIZE;
            int y = j * GRID_SIZE + GRID_SIZE;
            if (i % 2 == 1)
                y += GRID_SIZE / 2;
            auto node = std::make_shared<Node>(v, x, y);
            grid_graph->nodes[v] = node;

            if (i > 0)
                grid_graph->add_edge(node, grid_graph->nodes[(i - 1) * height + j]);
            if (j > 0)
                grid_graph->add_edge(node, grid_graph->nodes[i * height + j - 1]);
            if (i > 0 && j > 0 && i % 2 == 0)
                grid_graph->add_edge(node, grid_graph->nodes[(i - 1) * height + j - 1]);
            if (i > 0 && j < height - 1 && i % 2 == 1)
                grid_graph->add_edge(node, grid_graph->nodes[(i - 1) * height + j + 1]);
        }
    }
    return grid_graph;
}

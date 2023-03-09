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

int Graph::get_nearest_node_to(const int x, const int y) const {
    auto nearest = -1;
    auto nearest_dx = GRID_SIZE * 2;
    auto nearest_dy = GRID_SIZE * 2;

    for (auto &node : get_nodes()) {
        auto dx = std::abs(node->get_x() - x);
        auto dy = std::abs(node->get_y() - y);
        if (dx * dx + dy * dy < nearest_dx * nearest_dx + nearest_dy * nearest_dy) {
            nearest = node->get_v();
            nearest_dx = dx;
            nearest_dy = dy;
        }
    }

    return nearest;
}

void Graph::set_node(int index, int x, int y) {
    nodes[index] = std::make_shared<Node>(index, x, y);
}

void Graph::add_edge(int src, int dest) {
    if (std::find(adj[src].begin(), adj[src].end(), dest) == adj[src].end())
        adj[src].push_back(dest);
    if (std::find(adj[dest].begin(), adj[dest].end(), src) == adj[dest].end())
        adj[dest].push_back(src);
}

void Graph::remove_edge(int src, int dest) const {
    adj[src].erase(std::remove(adj[src].begin(), adj[src].end(), dest), adj[src].end());
    adj[dest].erase(std::remove(adj[dest].begin(), adj[dest].end(), src), adj[dest].end());
}

bool Graph::is_adjacent(int src, int dest) const {
    return std::find(adj[src].begin(), adj[src].end(), dest) != adj[src].end();
}

void Graph::print_adj() const {
    for (int i = 0; i < v; i++) {
        std::cout << i << " -> ";
        for (int &j: adj[i])
            std::cout << j << " ";
        std::cout << std::endl;
    }
}

std::shared_ptr<Graph> Graph::create_copy() const {
    auto copy = std::make_unique<Graph>(width, height);
    for (int i = 0; i < v; i++)
        copy->adj[i] = adj[i];
    for (auto& node: nodes)
        copy->set_node(node->get_v(), node->get_x(), node->get_y());
    return copy;
}

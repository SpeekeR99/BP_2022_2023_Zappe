#include "graph.h"

Graph::Graph(int width, int height) : width{width}, height{height}, v{width * height} {
    adj.resize(v);
    nodes.resize(v);
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

std::vector<std::vector<int>> &Graph::get_adj() {
    return adj;
}

std::vector<std::shared_ptr<Node>> Graph::get_nodes() const {
    return nodes;
}

int Graph::get_nearest_node_to(const int x, const int y) const {
    auto nearest = -1;
    auto nearest_dx = WINDOW_WIDTH;
    auto nearest_dy = WINDOW_WIDTH;

    // Find the nearest node
    for (auto &node: get_nodes()) {
        auto dx = std::abs(node->get_x() - x);
        auto dy = std::abs(node->get_y() - y);
        // If the node is closer than the previous nearest node
        if (dx * dx + dy * dy < nearest_dx * nearest_dx + nearest_dy * nearest_dy) {
            nearest = node->get_v();
            nearest_dx = dx;
            nearest_dy = dy;
        }
    }

    return nearest;
}

int Graph::get_nearest_alive_node_to(int x, int y) const {
    std::vector<int> visited(v, 0);

    while (true) { // true because there are always at least two alive nodes (start and end)
        auto nearest = -1;
        auto nearest_dx = WINDOW_WIDTH;
        auto nearest_dy = WINDOW_WIDTH;

        // Find the nearest node
        for (auto &node: get_nodes()) {
            auto dx = std::abs(node->get_x() - x);
            auto dy = std::abs(node->get_y() - y);
            // If the node is closer than the previous nearest node
            if (dx * dx + dy * dy < nearest_dx * nearest_dx + nearest_dy * nearest_dy && !visited[node->get_v()]) {
                nearest = node->get_v();
                nearest_dx = dx;
                nearest_dy = dy;
            }
        }

        // If the nearest node is alive, return it
        if (get_nodes()[nearest]->is_alive())
            return nearest;

        visited[nearest] = 1;
    }
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

void Graph::remove_edge(int src, int dest) {
    adj[src].erase(std::remove(adj[src].begin(), adj[src].end(), dest), adj[src].end());
    adj[dest].erase(std::remove(adj[dest].begin(), adj[dest].end(), src), adj[dest].end());
}

bool Graph::is_adjacent(int src, int dest) const {
    return std::find(adj[src].begin(), adj[src].end(), dest) != adj[src].end();
}

std::shared_ptr<Graph> Graph::create_copy() const {
    auto copy = std::make_shared<Graph>(width, height);
    for (auto &node: nodes) {
        copy->set_node(node->get_v(), node->get_x(), node->get_y());
        if (!node->is_alive())
            copy->get_nodes()[node->get_v()]->set_alive(false);
    }
    for (int i = 0; i < v; i++)
        for (auto &j: adj[i])
            copy->add_edge(i, j);
    return copy;
}

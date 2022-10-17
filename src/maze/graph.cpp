#include "graph.h"

Graph::Graph(int v) : v{v} {
    adj = new std::vector<int>[v];
    nodes.resize(v);
}

Graph::~Graph() {
    delete[] adj;
}

void Graph::add_edge(const std::shared_ptr<Node>& src, const std::shared_ptr<Node>& dest) {
    if (!nodes[src->get_v()])
        nodes.insert(nodes.begin() + src->get_v(), src);
    if (!nodes[dest->get_v()])
        nodes.insert(nodes.begin() + dest->get_v(), dest);
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

void Graph::draw_grid_graph() const {
    for (int i = 0; i < v; i++) {
        auto node = nodes[i];

        Drawing::draw_rectangle(node->get_x() - 5, node->get_y() - 5, 10, 10, {255, 255, 255}, 2.0f);

        for (int &j: adj[i]) {
            auto adjacent_node = nodes[j];

            Drawing::draw_line(node->get_x(), node->get_y(), adjacent_node->get_x(), adjacent_node->get_y(), {255, 255, 255}, 2.0f);
        }
    }
}

std::unique_ptr<Graph> Graph::create_grid_graph(int size) {
    int grid_len = 50;
    int offset = 50;
    auto grid_graph = std::make_unique<Graph>(size * size);
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            if (i < size - 1) {
                int v1 = i * size + j;
                int v2 = i * size + j + size;
                grid_graph->add_edge(std::make_shared<Node>(v1, v1 % size * grid_len + offset, v1 / size * grid_len + offset), std::make_shared<Node>(v2, v2 % size * grid_len + offset, v2 / size * grid_len + offset));
            }
            if (j < size - 1) {
                int v1 = i * size + j;
                int v2 = i * size + j + 1;
                grid_graph->add_edge(std::make_shared<Node>(v1, v1 % size * grid_len + offset, v1 / size * grid_len + offset), std::make_shared<Node>(v2, v2 % size * grid_len + offset, v2 / size * grid_len + offset));
            }
        }
    }
    return grid_graph;
}


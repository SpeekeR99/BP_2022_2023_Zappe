#include "graph.h"

Graph::Graph(int v) {
    this->v = v;
    adj = new std::vector<int>[v];
}

Graph::~Graph() {
    delete[] adj;
}

void Graph::add_edge(int src, int dest) const {
    adj[src].push_back(dest);
    adj[dest].push_back(src);
}

void Graph::print_adj() const {
    for (int i = 0; i < v; i++) {
        std::cout << i << " -> ";
        for (int & j : adj[i]) {
            std::cout << j << " ";
        }
        std::cout << std::endl;
    }
}

void Graph::draw_grid_graph() const {
    int size = sqrt(v);
    int grid_len = 50;
    int offset = 50;

    for (int i = 0; i < v; i++) {
        int x = i % size * grid_len + offset;
        int y = i / size * grid_len + offset;

        Drawing::draw_square(x - 5, y - 5, 10, 10, {0, 255, 0}, 7.0f);

        for (int & j : adj[i]) {
            int x2 = j % size * grid_len + offset;
            int y2 = j / size * grid_len + offset;

            Drawing::draw_line(x, y, x2, y2, {255, 0, 0}, 5.0f);
        }
    }
}

std::unique_ptr<Graph> Graph::create_grid_graph(int size) {
    auto grid_graph = std::make_unique<Graph>(size * size);
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            if (i < size - 1) {
                grid_graph->add_edge(i * size + j, i * size + j + size);
            }
            if (j < size - 1) {
                grid_graph->add_edge(i * size + j, i * size + j + 1);
            }
        }
    }
    return grid_graph;
}

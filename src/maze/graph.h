#pragma once

#include "GLFW/glfw3.h"
#include <iostream>
#include <vector>
#include <memory>
#include <cmath>
#include <algorithm>
#include "node.h"

class Graph {
private:
    int v;
    std::vector<int> *adj;
    std::vector<std::shared_ptr<Node>> nodes;

public:
    explicit Graph(int v);
    ~Graph();

    int get_v() const;
    std::vector<int> *get_adj() const;
    std::vector<std::shared_ptr<Node>> get_nodes() const;

    void add_edge(const std::shared_ptr<Node>& src, const std::shared_ptr<Node>& dest);
    void remove_edge(int src, int dest) const;
    void print_adj() const;

    static std::shared_ptr<Graph> create_orthogonal_grid_graph(int width, int height);
    static std::shared_ptr<Graph> create_hexagonal_grid_graph(int width, int height);
};

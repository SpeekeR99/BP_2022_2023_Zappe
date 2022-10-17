#pragma once

#include "GLFW/glfw3.h"
#include <iostream>
#include <vector>
#include <memory>
#include <cmath>
#include "../graphics/drawing.h"
#include "node.h"

class Graph {
private:
    int v;
    std::vector<int> *adj;
    std::vector<std::shared_ptr<Node>> nodes;

public:
    explicit Graph(int v);
    ~Graph();

    void add_edge(const std::shared_ptr<Node>& src, const std::shared_ptr<Node>& dest);
    void remove_edge(int src, int dest) const;
    void print_adj() const;
    void draw_grid_graph() const;

    static std::unique_ptr<Graph> create_grid_graph(int size);
};

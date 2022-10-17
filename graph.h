#pragma once

#include <glfw/glfw3.h>
#include <iostream>
#include <vector>
#include <memory>
#include <cmath>
#include "drawing.h"

class Graph {
private:
    int v;
    std::vector<int> *adj;

public:
    explicit Graph(int v);
    ~Graph();

    void add_edge(int src, int dest) const;
    void print_adj() const;
    void draw_grid_graph() const;

    static std::unique_ptr<Graph> create_grid_graph(int size);
};

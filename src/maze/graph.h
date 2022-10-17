#pragma once

#include "GLFW/glfw3.h"
#include <iostream>
#include <vector>
#include <memory>
#include <cmath>
#include "../graphics/drawing.h"

class Node {
private:
    int v;
    int x;
    int y;

public:
    Node(int v, int x, int y) : v(v), x(x), y(y) {}
    int get_v() const { return v; }
    int get_x() const { return x; }
    int get_y() const { return y; }
};

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

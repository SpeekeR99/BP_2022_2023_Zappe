#pragma once

#include <iostream>
#include <vector>
#include <memory>
#include <cmath>
#include <algorithm>
#include <random>
#include <chrono>
#include "GLFW/glfw3.h"
#include "../const.h"
#include "node.h"

class Graph {
private:
    int width;
    int height;
    int v;
    std::vector<int> *adj;
    std::vector<std::shared_ptr<Node>> nodes;

public:
    Graph(int width, int height);
    ~Graph();

    [[nodiscard]] int get_width() const;
    [[nodiscard]] int get_height() const;
    [[nodiscard]] int get_v() const;
    [[nodiscard]] std::vector<int> *get_adj() const;
    [[nodiscard]] std::vector<std::shared_ptr<Node>> get_nodes() const;
    [[nodiscard]] int get_nearest_node_to(int x, int y) const;
    void set_node(int index, int x, int y);

    void add_edge(int src, int dest);
    void remove_edge(int src, int dest) const;
    [[nodiscard]] bool is_adjacent(int src, int dest) const;

    void print_adj() const;

    [[nodiscard]] std::shared_ptr<Graph> create_copy() const;
};

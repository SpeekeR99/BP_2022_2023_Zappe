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

/**
 * Graph class
 */
class Graph {
private:
    /** Width of the graph */
    int width;
    /** Height of the graph */
    int height;
    /** Number of vertices */
    int v;
    /** Adjacency list */
    std::vector<int> *adj;
    /** Nodes */
    std::vector<std::shared_ptr<Node>> nodes;

public:
    /**
     * Constructor
     * @param width Width of the graph
     * @param height Height of the graph
     */
    Graph(int width, int height);

    /**
     * Destructor
     */
    ~Graph();

    /**
     * Getter for width
     * @return Width of the graph
     */
    [[nodiscard]] int get_width() const;

    /**
     * Getter for height
     * @return Height of the graph
     */
    [[nodiscard]] int get_height() const;

    /**
     * Getter for vertex count
     * @return Number of vertices
     */
    [[nodiscard]] int get_v() const;

    /**
     * Getter for adjacency list
     * @return Adjacency list
     */
    [[nodiscard]] std::vector<int> *get_adj() const;

    /**
     * Getter for nodes
     * @return Nodes list of references to nodes
     */
    [[nodiscard]] std::vector<std::shared_ptr<Node>> get_nodes() const;

    /**
     * Finds the nearest node to the given coordinates
     * @param x X coordinate
     * @param y Y coordinate
     * @return Vertex index of the nearest node
     */
    [[nodiscard]] int get_nearest_node_to(int x, int y) const;

    /**
     * Finds the nearest alive node to the given coordinates
     * @param x X coordinate
     * @param y Y coordinate
     * @return Vertex index of the nearest alive node
     */
    [[nodiscard]] int get_nearest_alive_node_to(int x, int y) const;

    /**
     * Sets the node at the given index to the given coordinates
     * @param index Index of the node (v)
     * @param x X coordinate
     * @param y Y coordinate
     */
    void set_node(int index, int x, int y);

    /**
     * Adds an edge to the graph
     * @param src Node index
     * @param dest Node index
     */
    void add_edge(int src, int dest);

    /**
     * Removes an edge from the graph
     * @param src Node index
     * @param dest Node index
     */
    void remove_edge(int src, int dest) const;

    /**
     * Checks if the given nodes are adjacent
     * @param src Node index
     * @param dest Node index
     * @return True if the nodes are adjacent, false otherwise
     */
    [[nodiscard]] bool is_adjacent(int src, int dest) const;

    /**
     * Prints the adjacency list to the console
     */
    void print_adj() const;

    /**
     * Creates a deep copy of the graph
     * @return Deep copy of the graph
     */
    [[nodiscard]] std::shared_ptr<Graph> create_copy() const;
};

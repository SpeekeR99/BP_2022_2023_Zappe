#pragma once

#include <utility>
#include "graph.h"
#include "generator.h"

/**
 * Cellular automata represented by a graph
 */
class CellularAutomata {
private:
    /** Original grid graph */
    std::shared_ptr<Graph> original_grid_graph;
    /** Current graph (changes with each generation) */
    std::shared_ptr<Graph> graph;
    /** Neighborhood graph (for checking alive nodes around) */
    std::shared_ptr<Graph> neighborhood_graph;
    /** Initial graph (for resetting) */
    std::shared_ptr<Graph> initialized_graph;
    /** String of rules */
    std::string rule_string;
    /** Born rule */
    std::vector<int> born_rule;
    /** Survival rule */
    std::vector<int> survive_rule;

public:
    /**
     * Constructor
     * @param rules Rules of the cellular automata (format B/S)
     * @param orig_graph Original graph
     * @param neighborhood Neighborhood graph
     * @param init_square_w Initial square width
     */
    CellularAutomata(std::string rules, std::shared_ptr<Graph>& orig_graph, const std::shared_ptr<Graph>& neighborhood = nullptr, const int init_square_w = -1);

    /**
     * Get current graph
     * @return Current graph
     */
    std::shared_ptr<Graph> &get_graph();

    /**
     * Do next generation
     */
    void next_generation();

    /**
     * Reset graph to initial state
     */
    void reset();
};

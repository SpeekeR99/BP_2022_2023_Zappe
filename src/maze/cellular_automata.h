#pragma once

#include "graph.h"
#include "generator.h"

class CellularAutomata {
private:
    std::shared_ptr<Graph> original_grid_graph;
    std::shared_ptr<Graph> graph;
    std::string rule_string;
    std::vector<int> born_rule;
    std::vector<int> survive_rule;

public:
    CellularAutomata(std::string rules, std::shared_ptr<Graph>& orig_graph);

    std::shared_ptr<Graph> get_graph();

    void next_generation();
};

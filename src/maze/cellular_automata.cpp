#include "cellular_automata.h"

#include <utility>

CellularAutomata::CellularAutomata(std::string rules, std::shared_ptr<Graph>& orig_graph) : rule_string(std::move(rules)), born_rule(), survive_rule() {
    original_grid_graph = orig_graph;
    graph = orig_graph->create_copy();

    std::default_random_engine gen(std::chrono::system_clock::now().time_since_epoch().count());
    std::uniform_int_distribution<> dis(0, 1);

    for (int i = 0; i < graph->get_v(); i++) {
        for (int j = 0; j < graph->get_v(); j++) {
            if (graph->is_adjacent(i, j) && dis(gen) % 2)
                graph->remove_edge(i, j);
        }
    }

    for (int i = 0; i < rule_string.length(); i++) {
        if (rule_string[i] == 'B') {
            for (int j = i + 1; j < rule_string.length(); j++) {
                if (rule_string[j] == '/')
                    break;
                born_rule.push_back(rule_string[j] - '0');
            }
        }
        else if (rule_string[i] == 'S') {
            for (int j = i + 1; j < rule_string.length(); j++) {
                if (rule_string[j] == '/')
                    break;
                survive_rule.push_back(rule_string[j] - '0');
            }
        }
    }
}

std::shared_ptr<Graph> CellularAutomata::get_graph() {
    return graph;
}

void CellularAutomata::next_generation() {
    std::default_random_engine gen(std::chrono::system_clock::now().time_since_epoch().count());
    std::uniform_int_distribution<> dis(0, RAND_MAX);

    auto new_graph = graph->create_copy();
    for (int i = 0; i < graph->get_v(); i++) {
        std::vector<int> possible_neighbors;
        for (int j = 0; j < graph->get_v(); j++) {
            if (original_grid_graph->is_adjacent(i, j))
                possible_neighbors.push_back(j);
        }
        std::vector<int> neighbors;
        for (int possible_neighbor : possible_neighbors) {
            if (graph->is_adjacent(i, possible_neighbor))
                neighbors.push_back(possible_neighbor);
        }

        if (std::count(survive_rule.begin(), survive_rule.end(), neighbors.size())) {
            continue;
        }

        else if (std::count(born_rule.begin(), born_rule.end(), neighbors.size())) {
            if (dis(gen) % 2)
                new_graph->add_edge(i, possible_neighbors[dis(gen) % possible_neighbors.size()]);
        }

        else {
            for (int neighbor : neighbors)
                new_graph->remove_edge(i, neighbor);
        }

    }
    graph = new_graph;
}

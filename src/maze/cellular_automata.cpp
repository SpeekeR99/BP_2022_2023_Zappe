#include "cellular_automata.h"

CellularAutomata::CellularAutomata(std::string rules, std::shared_ptr<Graph> &orig_graph,
                                   const std::shared_ptr<Graph> &neighborhood, const int init_square_w) : rule_string(
        std::move(rules)), born_rule(), survive_rule() {
    // Create copy of original graph
    original_grid_graph = orig_graph;
    graph = orig_graph->create_copy();
    // If neighborhood graph is not given, create one from original graph
    if (neighborhood)
        neighborhood_graph = neighborhood->create_copy();
    else
        neighborhood_graph = orig_graph->create_copy();

    std::default_random_engine gen(std::chrono::system_clock::now().time_since_epoch().count());
    std::uniform_int_distribution<> dis(0, 1);
    auto width = graph->get_width();
    if (init_square_w > -1)
        width = init_square_w;

    // Randomly set nodes to alive or dead
    for (int i = 1; i < graph->get_v() - 1; i++) {
        if (i / graph->get_width() < width && i % graph->get_width() < width) {
            if (dis(gen) == 1) {
                graph->get_nodes()[i]->set_alive(false);
                for (int j = 0; j < graph->get_v(); j++)
                    graph->remove_edge(i, j);
            }
        } else {
            graph->get_nodes()[i]->set_alive(false);
            for (int j = 0; j < graph->get_v(); j++)
                graph->remove_edge(i, j);
        }
    }

    // Save initial state
    initialized_graph = graph->create_copy();

    // Parse rules
    for (int i = 0; i < rule_string.length(); i++) {
        if (rule_string[i] == 'B') {
            for (int j = i + 1; j < rule_string.find_first_of('/'); j++) {
                born_rule.push_back(rule_string[j] - '0');
            }
        } else if (rule_string[i] == 'S') {
            for (int j = i + 1; j < rule_string.length(); j++) {
                survive_rule.push_back(rule_string[j] - '0');
            }
        }
    }
}

std::shared_ptr<Graph> &CellularAutomata::get_graph() {
    return graph;
}

void CellularAutomata::next_generation() {
    auto new_graph = graph->create_copy();
    for (int i = 0; i < graph->get_v(); i++) {
        auto alive_neighbors = 0;
        for (int j = 0; j < graph->get_v(); j++) {
            if (neighborhood_graph->is_adjacent(i, j) && graph->get_nodes()[j]->is_alive())
                alive_neighbors++;
        }

        if ((graph->get_nodes()[i]->is_alive() && std::count(survive_rule.begin(), survive_rule.end(), alive_neighbors))
            || (!graph->get_nodes()[i]->is_alive() && std::count(born_rule.begin(), born_rule.end(), alive_neighbors)))
            new_graph->get_nodes()[i]->set_alive(true);
        else
            new_graph->get_nodes()[i]->set_alive(false);
    }

    // Set start and end nodes to alive
    new_graph->get_nodes()[0]->set_alive(true);
    new_graph->get_nodes()[graph->get_v() - 1]->set_alive(true);

    // Resolve edges
    for (int i = 0; i < new_graph->get_v(); i++) {
        std::vector<int> possible_neighbors;
        for (int j = 0; j < new_graph->get_v(); j++) {
            if (original_grid_graph->is_adjacent(i, j))
                possible_neighbors.push_back(j);
        }
        if (new_graph->get_nodes()[i]->is_alive()) {
            for (int possible_neighbor: possible_neighbors) {
                if (new_graph->get_nodes()[possible_neighbor]->is_alive())
                    new_graph->add_edge(i, possible_neighbor);
            }
        } else {
            for (int j = 0; j < new_graph->get_v(); j++) {
                new_graph->remove_edge(i, j);
            }
        }
    }

    graph = new_graph;
}

void CellularAutomata::reset() {
    graph = initialized_graph->create_copy();
}

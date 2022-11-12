#pragma once

#include <memory>
#include <vector>
#include <random>
#include <ctime>
#include "graph.h"

class Generator {
public:
    static std::shared_ptr<Graph> generate_maze_dfs(std::shared_ptr<Graph> &maze);
};

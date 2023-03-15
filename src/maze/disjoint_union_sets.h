#pragma once

#include <vector>

class Disjoint_union_sets {
private:
    std::vector<int> parent;
    std::vector<int> rank;
    int size;

public:
    explicit Disjoint_union_sets(int size);

    int find(int x);
    void merge(int x, int y);
};

#include "disjoint_union_sets.h"

Disjoint_union_sets::Disjoint_union_sets(int size) {
    this->size = size;
    parent.resize(size);
    rank.resize(size);
    for (int i = 0; i < size; i++) {
        parent[i] = i;
        rank[i] = 0;
    }
}

int Disjoint_union_sets::find(int x) {
    if (parent[x] != x)
        parent[x] = find(parent[x]);
    return parent[x];
}

void Disjoint_union_sets::merge(int x, int y) {
    int x_root = find(x);
    int y_root = find(y);

    // Elements are already in the same set
    if (x_root == y_root)
        return;

    if (rank[x_root] < rank[y_root])
        parent[x_root] = y_root;
    else if (rank[x_root] > rank[y_root])
        parent[y_root] = x_root;
    else {
        parent[y_root] = x_root;
        rank[x_root]++;
    }
}

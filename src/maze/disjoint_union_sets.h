#pragma once

#include <vector>

/**
 * Disjoint union sets data structure
 */
class Disjoint_union_sets {
private:
    /** Parent of each element */
    std::vector<int> parent;
    /** Rank of each element */
    std::vector<int> rank;
    /** Size of the whole data structure */
    int size;

public:
    /**
     * Constructor
     * @param size Size of the data structure
     */
    explicit Disjoint_union_sets(int size);

    /**
     * Find the parent of the given element
     * @param x Element
     * @return Parent of the element
     */
    int find(int x);

    /**
     * Merge two elements
     * @param x Element
     * @param y Element
     */
    void merge(int x, int y);
};

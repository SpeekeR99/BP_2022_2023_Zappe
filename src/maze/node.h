#pragma once

/**
 * Node in a graph
 */
class Node {
private:
    /** Vertex number (== ID) */
    int v;
    /** X coordinate */
    int x;
    /** Y coordinate */
    int y;
    /** Alive flag (used for Cellular Automatas) */
    bool alive;
public:
    /**
     * Constructor
     * @param v Vertex number
     * @param x X coordinate
     * @param y Y coordinate
     */
    Node(int v, int x, int y);

    /**
     * Getter for vertex number
     * @return Vertex number
     */
    [[nodiscard]] int get_v() const;
    /**
     * Getter for X coordinate
     * @return X coordinate
     */
    [[nodiscard]] int get_x() const;
    /**
     * Getter for Y coordinate
     * @return Y coordinate
     */
    [[nodiscard]] int get_y() const;
    /**
     * Getter for alive flag
     * @return Alive flag
     */
    [[nodiscard]] bool is_alive() const;

    /**
     * Setter for alive flag
     * @param new_state New state of alive flag
     */
    void set_alive(bool new_state);
};

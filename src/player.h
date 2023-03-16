#pragma once

#include <vector>

/**
 * Player class
 */
class Player {
private:
    /** Player X coordinate */
    int x;
    /** Player Y coordinate */
    int y;
    /** Walked path */
    std::vector<std::pair<int, int>> path;

public:
    /**
     * Constructor
     * @param x X coordinate
     * @param y Y coordinate
     */
    Player(int x, int y);

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
     * Getter for path
     * @return Path
     */
    [[nodiscard]] std::vector<std::pair<int, int>> get_path() const;

    /**
     * Move player to new coordinates
     * Update path accordingly
     * @param x_new New X coordinate
     * @param y_new New Y coordinate
     */
    void move_to(int x_new, int y_new);
};

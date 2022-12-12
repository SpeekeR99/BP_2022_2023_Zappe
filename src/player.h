#pragma once

#include <vector>

class Player {
private:
    int x;
    int y;
    std::vector<std::pair<int, int>> path;

public:
    Player(int x, int y);

    [[nodiscard]] int get_x() const;
    [[nodiscard]] int get_y() const;
    [[nodiscard]] std::vector<std::pair<int, int>> get_path() const;

    void move_to(int x_new, int y_new);
};

#include "player.h"

Player::Player(int x, int y) : x{x}, y{y}, path{} {
    path.emplace_back(x, y);
}

int Player::get_x() const {
    return x;
}

int Player::get_y() const {
    return y;
}

std::vector<std::pair<int, int>> Player::get_path() const {
    return path;
}

void Player::move_to(int x_new, int y_new) {
    x = x_new;
    y = y_new;
    path.emplace_back(x, y);
}

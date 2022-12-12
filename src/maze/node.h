#pragma once

class Node {
private:
    int v;
    int x;
    int y;
public:
    Node(int v, int x, int y);

    [[nodiscard]] int get_v() const;
    [[nodiscard]] int get_x() const;
    [[nodiscard]] int get_y() const;
};

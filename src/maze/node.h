#pragma once

class Node {
private:
    int v;
    int x;
    int y;
public:
    Node(int v, int x, int y);

    int get_v() const;
    int get_x() const;
    int get_y() const;
};

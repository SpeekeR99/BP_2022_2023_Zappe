#include "node.h"

Node::Node(int v, int x, int y) : v{v}, x{x}, y{y} {
    // empty
}

int Node::get_v() const {
    return v;
}

int Node::get_x() const {
    return x;
}

int Node::get_y() const {
    return y;
}

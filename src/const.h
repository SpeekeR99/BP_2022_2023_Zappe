#pragma once

constexpr double PI = 3.14159265359;
constexpr int WINDOW_WIDTH = 800;
constexpr int WINDOW_HEIGHT = 800;
constexpr int GRID_SIZE = 50;
constexpr float BLACK_LINE_WIDTH = GRID_SIZE * 1.33f;
constexpr float WHITE_LINE_WIDTH = GRID_SIZE * 0.5f;
constexpr float BLACK_NODE_RADIUS = 2 * BLACK_LINE_WIDTH / std::max(WINDOW_WIDTH, WINDOW_HEIGHT);
constexpr float WHITE_NODE_RADIUS = 2 * WHITE_LINE_WIDTH / std::max(WINDOW_WIDTH, WINDOW_HEIGHT);
constexpr float PLAYER_RADIUS = GRID_SIZE * 0.5f / std::max(WINDOW_WIDTH, WINDOW_HEIGHT);
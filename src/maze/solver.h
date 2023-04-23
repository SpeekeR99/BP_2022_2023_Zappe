#pragma once

#include <array>
#include <vector>
#include "graph.h"

/**
 * Solver class
 */
class Solver {
public:
    /**
     * Checks if the maze is solvable using BFS (Breadth First Search)
     * @param maze Maze to be solved
     * @param start Start position
     * @param end End position
     * @return True if the maze is solvable, false otherwise
     */
    static bool is_maze_solvable_bfs(std::shared_ptr<Graph> &maze, const std::pair<int, int> &start, const std::pair<int, int> &end);
    /**
     * Solves the maze using BFS (Breadth First Search)
     * @param maze Maze to be solved
     * @param start Start position
     * @param end End position
     * @return Vector of X and Y coordinates of the path from start to end
     */
    static std::vector<std::pair<int, int>> solve_maze_bfs(std::shared_ptr<Graph> &maze, const std::pair<int, int> &start, const std::pair<int, int> &end);
    /**
     * Checks if the maze is solvable using Dijkstra's algorithm
     * (Dijkstra is a special case of A* algorithm, where the heuristic is constant 0)
     * @param maze Maze to be solved
     * @param start Start position
     * @param end End position
     * @return True if the maze is solvable, false otherwise
     */
    static bool is_maze_solvable_dijkstra(std::shared_ptr<Graph> &maze, const std::pair<int, int> &start, const std::pair<int, int> &end);
    /**
     * Solves the maze using Dijkstra's algorithm
     * (Dijkstra is a special case of A* algorithm, where the heuristic is constant 0)
     * @param maze Maze to be solved
     * @param start Start position
     * @param end End position
     * @return Vector of X and Y coordinates of the path from start to end
     */
    static std::vector<std::pair<int, int>> solve_maze_dijkstra(std::shared_ptr<Graph> &maze, const std::pair<int, int> &start, const std::pair<int, int> &end);
    /**
     * Checks if the maze is solvable using A* algorithm
     * @param maze Maze to be solved
     * @param start Start position
     * @param end End position
     * @param heuristic Heuristic function to be used
     * @return True if the maze is solvable, false otherwise
     */
    static bool is_maze_solvable_a_star(std::shared_ptr<Graph> &maze, const std::pair<int, int> &start, const std::pair<int, int> &end, HeuristicType heuristic = HeuristicType::EUCLIDEAN_DISTANCE);

    /**
     * Solves the maze using A* algorithm
     * @param maze Maze to be solved
     * @param start Start position
     * @param end End position
     * @param heuristic Heuristic function to be used
     * @return Vector of X and Y coordinates of the path from start to end
     */
    static std::vector<std::pair<int, int>> solve_maze_a_star(std::shared_ptr<Graph> &maze, const std::pair<int, int> &start, const std::pair<int, int> &end, HeuristicType heuristic = HeuristicType::EUCLIDEAN_DISTANCE);
};



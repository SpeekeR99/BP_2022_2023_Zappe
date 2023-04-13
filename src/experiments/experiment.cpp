#include <memory>
#include <fstream>
#include "../maze/generator.h"
#include "../maze/solver.h"
#include "../maze/cellular_automata.h"
#include "../const.h"

/** Window width */
int WINDOW_WIDTH = 1280;
/** Window height */
int WINDOW_HEIGHT = 720;
/** Window X offset - Maze is drawn on the right square, rest of the screen is GUI on the left */
int WINDOW_X_OFFSET = WINDOW_WIDTH - WINDOW_HEIGHT;
/** Grid size for drawing */
int GRID_SIZE = 50;
/** Width of the lines representing the paths */
float WHITE_LINE_WIDTH = (float) GRID_SIZE * 0.5f;
/** Radius of the nodes (used to make the paths junctions look smooth) */
float WHITE_NODE_RADIUS = WHITE_LINE_WIDTH / (float) WINDOW_HEIGHT;
/** Radius of the player */
float PLAYER_RADIUS = (float) GRID_SIZE * 0.25f / (float) WINDOW_HEIGHT;

/**
 * Runs the experiment for a given cellular automata
 * @param ca Cellular automata to run the experiment on
 * @return Number of generations it took to generate a solvable maze
 */
int experiment(std::unique_ptr<CellularAutomata> &ca) {
    int steps = 0;
    auto start = std::chrono::high_resolution_clock::now();
    while (true) {
        ca->next_generation();
        steps++;
        if (Solver::is_maze_solvable_bfs(ca->get_graph(), {ca->get_graph()->get_nodes()[0]->get_x(),
                                                           ca->get_graph()->get_nodes()[0]->get_y()},
                                         {ca->get_graph()->get_nodes()[
                                                  ca->get_graph()->get_nodes().size() - 1]->get_x(),
                                          ca->get_graph()->get_nodes()[
                                                  ca->get_graph()->get_nodes().size() - 1]->get_y()}))
            return steps;
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
        if (duration > 10000 || steps > 1000)
            return -1;
    }
}

/**
 * This file is exclusive with the main.cpp file
 * Use this file to run experiments only
 * @return EXIT_SUCCESS if the program finishes successfully
 */
int main() {
    auto graph = Generator::create_hexagonal_grid_graph(WINDOW_HEIGHT / GRID_SIZE - 1, WINDOW_HEIGHT / GRID_SIZE - 1);
    auto neighborhood = Generator::create_orthogonal_grid_graph_laplacian(WINDOW_HEIGHT / GRID_SIZE - 1, WINDOW_HEIGHT / GRID_SIZE - 1);
    auto initialize_square_size = -1;
    std::string rulestring = "B3/S1234";
    const int number_of_experiments = 100;
    std::string filename = rulestring + "_" + std::to_string(number_of_experiments) + "_2.txt";
    filename.erase(std::remove(filename.begin(), filename.end(), '/'), filename.end());
    std::ofstream output(filename);
    auto ca = std::make_unique<CellularAutomata>(rulestring, graph, neighborhood, initialize_square_size);
    const int max_i = WINDOW_HEIGHT / GRID_SIZE;
    std::cout << "max_i: " << max_i << std::endl;
    std::vector<std::vector<int>> results(max_i + 1, std::vector<int>(number_of_experiments, 0));

    for (; initialize_square_size < max_i - 1; initialize_square_size++) {
        std::cout << "sqr_size: " << initialize_square_size << std::endl;
        for (int i = 0; i < number_of_experiments; i++) {
            results[initialize_square_size+1][i] = experiment(ca);
            ca = std::make_unique<CellularAutomata>(rulestring, graph, neighborhood, initialize_square_size);
        }
    }

    std::cout << rulestring << std::endl;
    std::cout << number_of_experiments << std::endl;
    output << rulestring << std::endl;
    output << number_of_experiments << std::endl;

    int sum = 0;
    int finish = 0;
    std::vector<int> finished(max_i, 0);
    std::vector<float> averages(max_i, 0.0f);
    for (int i = 0; i < max_i; i++) {
        for (int j = 0; j < number_of_experiments; j++) {
            if (results[i][j] != -1) {
                finish++;
                sum += results[i][j];
            }
        }
        finished[i] = finish;
        averages[i] = (float) sum / (float) finished[i];
        sum = 0;
        finish = 0;
    }
    std::vector<float> std_devs(max_i, 0.0f);
    for (int i = 0; i < max_i; i++) {
        for (int j = 0; j < number_of_experiments; j++) {
            if (results[i][j] != -1) {
                std_devs[i] += ((float) results[i][j] - averages[i]) * ((float) results[i][j] - averages[i]);
            }
        }
        std_devs[i] = std::sqrt(std_devs[i] / (float) finished[i]);
    }

    for (int i = 0; i < max_i; i++) {
        std::cout << "init sqr size " << i-1 << " finished: " << finished[i] << " avg: " << averages[i] << " std dev: " << std_devs[i] << std::endl;
        output << i-1 << " " << finished[i] << " " << averages[i] << " " << std_devs[i] << std::endl;
    }

    output.close();

    return EXIT_SUCCESS;
}
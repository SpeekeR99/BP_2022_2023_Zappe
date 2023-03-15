#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <regex>
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "imgui_stdlib.h"
#include "graphics/shaders/shader.h"
#include "graphics/shaders/VAO.h"
#include "graphics/shaders/VBO.h"
#include "graphics/shaders/EBO.h"
#include "graphics/drawing.h"
#include "maze/generator.h"
#include "maze/solver.h"
#include "maze/cellular_automata.h"
#include "player.h"
#include "imgui_internal.h"

// Orthogonal grid graph
// B3/S1234
// B37/S1234
// B3/S12345
// B37/S12345

// Hexagonal grid graph
// B24/S12345
// B24/S1234

// Hexagonal grid graph + laplacean
// B23/S12345   i = 7
// B3/S1234     i = 7

// Orthogonal grid graph + hexagonal
// B3/S234    i = -1
// B2/S234    i = -1

std::shared_ptr<Graph> graph;
std::shared_ptr<Graph> maze;
std::shared_ptr<Graph> neighborhood;
std::shared_ptr<CellularAutomata> ca;
static GraphType graph_type = GraphType::ORTHOGONAL;
static NeighborhoodGraphType neighborhood_graph_type = NeighborhoodGraphType::SAME_AS_GRAPH_TYPE;
static MazeType maze_type = MazeType::STATIC;
static GeneratorType generator_algorithm = GeneratorType::KRUSKAL;
static SolverType solver_algorithm = SolverType::A_STAR;
static HeuristicType heuristic = HeuristicType::EUCLIDEAN_DISTANCE;
bool non_grid_version = false;
std::unique_ptr<Player> player;
bool draw = false;
bool paused = false;
bool is_solvable = false;
bool is_solved = false;
std::vector<std::pair<int, int>> solved_path;
bool show_solution = false;
std::string rulestring = "B3/S1234";
std::regex rulestring_regex("B([0-9]+)/S([0-9]+)");
int initialize_square_size = -1;
float font_size = 1.0f;
float speed = 0.6f;
bool is_solvable_from_player = false;
std::vector<std::pair<int, int>> solved_path_from_player;
bool show_solution_from_player = false;
float horizontal_bias = 0.5f;
float vertical_bias = 0.5f;
float cycle_bias = 0.0f;

int WINDOW_WIDTH = 1280;
int WINDOW_HEIGHT = 720;
int WINDOW_X_OFFSET = WINDOW_WIDTH - WINDOW_HEIGHT;
int GRID_SIZE = 50;
float WHITE_LINE_WIDTH = (float) GRID_SIZE * 0.5f;
float WHITE_NODE_RADIUS = WHITE_LINE_WIDTH / (float) WINDOW_HEIGHT;
float PLAYER_RADIUS = (float) GRID_SIZE * 0.25f / (float) WINDOW_HEIGHT;
bool fullscreen = false;
bool show_settings_window = false;
bool show_about_window = false;

ImVec4 background_color = {29. / 255, 29. / 255, 29. / 255, 1.0f};
std::shared_ptr<VAO> background_vao;
std::shared_ptr<VBO> background_vbo;
std::shared_ptr<EBO> background_ebo;
ImVec4 paths_color = {1.0f, 1.0f, 1.0f, 1.0f};
std::shared_ptr<VAO> paths_vao;
std::shared_ptr<VBO> paths_vbo;
std::shared_ptr<EBO> paths_ebo;
ImVec4 start_end_color = {0.0f, 0.0f, 1.0f, 1.0f};
ImVec4 player_color = {0.0f, 1.0f, 0.0f, 1.0f};
ImVec4 solution_color = {0.0f, 25. / 255, 185. / 255, 1.0f};
std::shared_ptr<VAO> solution_vao;
std::shared_ptr<VBO> solution_vbo;
std::shared_ptr<EBO> solution_ebo;
ImVec4 player_path_color = {1.0f, 0.0f, 0.0f, 1.0f};
std::shared_ptr<VAO> player_path_vao;
std::shared_ptr<VBO> player_path_vbo;
std::shared_ptr<EBO> player_path_ebo;
ImVec4 solution_from_player_color = {0.0f, 185. / 255, 110. / 255, 1.0f};
std::shared_ptr<VAO> solution_from_player_vao;
std::shared_ptr<VBO> solution_from_player_vbo;
std::shared_ptr<EBO> solution_from_player_ebo;

GLfloat background_vertices[] = {
        0.0f, 0.0f, background_color.x, background_color.y, background_color.z,
        0.0f, 0.0f, background_color.x, background_color.y, background_color.z,
        0.0f, 0.0f, background_color.x, background_color.y, background_color.z,
        0.0f, 0.0f, background_color.x, background_color.y, background_color.z
};

GLuint background_indices[] = {
        0, 1, 2, 3
};

void cursor_position_callback(GLFWwindow *window, double xpos, double ypos) {
    if (!draw) return;

    auto new_x = static_cast<int>(xpos) - WINDOW_X_OFFSET;
    auto new_y = static_cast<int>(ypos);
    auto moved = false;

    if (new_x < GRID_SIZE / 2 || new_x > WINDOW_HEIGHT - GRID_SIZE / 2 || new_y < GRID_SIZE / 2 || new_y > WINDOW_HEIGHT - GRID_SIZE / 2)
        return;

    if (maze_type == MazeType::STATIC) {
        auto nearest_to_player = maze->get_nearest_node_to(player->get_x(), player->get_y());
        auto nearest_to_mouse = maze->get_nearest_node_to(new_x, new_y);

        if (maze->is_adjacent(nearest_to_player, nearest_to_mouse)) {
            player->move_to(maze->get_nodes()[nearest_to_mouse]->get_x(), maze->get_nodes()[nearest_to_mouse]->get_y());
            moved = true;
        }
    }
    else if (maze_type == MazeType::DYNAMIC && paused) {
        auto nearest_to_player = ca->get_graph()->get_nearest_node_to(player->get_x(), player->get_y());
        auto nearest_to_mouse = ca->get_graph()->get_nearest_node_to(new_x, new_y);

        if (ca->get_graph()->is_adjacent(nearest_to_player, nearest_to_mouse) && ca->get_graph()->get_nodes()[nearest_to_mouse]->is_alive()) {
            player->move_to(ca->get_graph()->get_nodes()[nearest_to_mouse]->get_x(), ca->get_graph()->get_nodes()[nearest_to_mouse]->get_y());
            moved = true;
        }
    }

    if (!moved)
        return;

    // Check if the player has reached the end
    if (maze_type == MazeType::STATIC)
        is_solved = is_solved || (player->get_x() == maze->get_nodes()[maze->get_v() - 1]->get_x() &&
                                  player->get_y() == maze->get_nodes()[maze->get_v() - 1]->get_y());
    else if (maze_type == MazeType::DYNAMIC)
        is_solved = is_solved || (player->get_x() == ca->get_graph()->get_nodes()[ca->get_graph()->get_v() - 1]->get_x() &&
                                  player->get_y() == ca->get_graph()->get_nodes()[ca->get_graph()->get_v() - 1]->get_y());

    // Check the solvability of the maze from the player's position
    if (maze_type == MazeType::STATIC) {
        if (solver_algorithm == SolverType::BREATH_FIRST_SEARCH) {
            is_solvable_from_player = Solver::is_maze_solvable_bfs(maze, {player->get_x(), player->get_y()},
                                                                   {maze->get_nodes()[maze->get_nodes().size() - 1]->get_x(),
                                                                    maze->get_nodes()[maze->get_nodes().size() - 1]->get_y()});
            solved_path_from_player = Solver::solve_maze_bfs(maze, {player->get_x(), player->get_y()},
                                                            {maze->get_nodes()[maze->get_nodes().size() - 1]->get_x(),
                                                             maze->get_nodes()[maze->get_nodes().size() - 1]->get_y()});
        }
        else if (solver_algorithm == SolverType::DIJKSTRA) {
            is_solvable_from_player = Solver::is_maze_solvable_dijkstra(maze, {player->get_x(), player->get_y()},
                                                                   {maze->get_nodes()[maze->get_nodes().size() - 1]->get_x(),
                                                                    maze->get_nodes()[maze->get_nodes().size() - 1]->get_y()});
            solved_path_from_player = Solver::solve_maze_dijkstra(maze, {player->get_x(), player->get_y()},
                                                            {maze->get_nodes()[maze->get_nodes().size() - 1]->get_x(),
                                                             maze->get_nodes()[maze->get_nodes().size() - 1]->get_y()});
        }
        else if (solver_algorithm == SolverType::A_STAR) {
            is_solvable_from_player = Solver::is_maze_solvable_a_star(maze, {player->get_x(), player->get_y()},
                                                                   {maze->get_nodes()[maze->get_nodes().size() - 1]->get_x(),
                                                                    maze->get_nodes()[maze->get_nodes().size() - 1]->get_y()}, heuristic);
            solved_path_from_player = Solver::solve_maze_a_star(maze, {player->get_x(), player->get_y()},
                                                            {maze->get_nodes()[maze->get_nodes().size() - 1]->get_x(),
                                                             maze->get_nodes()[maze->get_nodes().size() - 1]->get_y()}, heuristic);
        }
    }
    if (maze_type == MazeType::DYNAMIC) {
        if (solver_algorithm == SolverType::BREATH_FIRST_SEARCH) {
            is_solvable_from_player = Solver::is_maze_solvable_bfs(ca->get_graph(), {player->get_x(), player->get_y()},
                                                                   {ca->get_graph()->get_nodes()[ca->get_graph()->get_nodes().size() - 1]->get_x(),
                                                                    ca->get_graph()->get_nodes()[ca->get_graph()->get_nodes().size() - 1]->get_y()});
            solved_path_from_player = Solver::solve_maze_bfs(ca->get_graph(), {player->get_x(), player->get_y()},
                                                            {ca->get_graph()->get_nodes()[ca->get_graph()->get_nodes().size() - 1]->get_x(),
                                                             ca->get_graph()->get_nodes()[ca->get_graph()->get_nodes().size() - 1]->get_y()});
        }
        else if (solver_algorithm == SolverType::DIJKSTRA) {
            is_solvable_from_player = Solver::is_maze_solvable_dijkstra(ca->get_graph(), {player->get_x(), player->get_y()},
                                                                        {ca->get_graph()->get_nodes()[ca->get_graph()->get_nodes().size() - 1]->get_x(),
                                                                         ca->get_graph()->get_nodes()[ca->get_graph()->get_nodes().size() - 1]->get_y()});
            solved_path_from_player = Solver::solve_maze_dijkstra(ca->get_graph(), {player->get_x(), player->get_y()},
                                                                 {ca->get_graph()->get_nodes()[ca->get_graph()->get_nodes().size() - 1]->get_x(),
                                                                  ca->get_graph()->get_nodes()[ca->get_graph()->get_nodes().size() - 1]->get_y()});
        }
        else if (solver_algorithm == SolverType::A_STAR) {
            is_solvable_from_player = Solver::is_maze_solvable_a_star(ca->get_graph(), {player->get_x(), player->get_y()},
                                                                      {ca->get_graph()->get_nodes()[ca->get_graph()->get_nodes().size() - 1]->get_x(),
                                                                       ca->get_graph()->get_nodes()[ca->get_graph()->get_nodes().size() - 1]->get_y()}, heuristic);
            solved_path_from_player = Solver::solve_maze_a_star(ca->get_graph(), {player->get_x(), player->get_y()},
                                                               {ca->get_graph()->get_nodes()[ca->get_graph()->get_nodes().size() - 1]->get_x(),
                                                                ca->get_graph()->get_nodes()[ca->get_graph()->get_nodes().size() - 1]->get_y()}, heuristic);
        }
    }

    Drawing::buffer_lines(player_path_vao, player_path_vbo, player_path_ebo, player->get_path(), player_path_color);
    Drawing::buffer_lines(solution_from_player_vao, solution_from_player_vbo, solution_from_player_ebo,
                          solved_path_from_player, solution_from_player_color);
}

static void glfw_error_callback(int error, const char* description) {
    fprintf(stderr, "GLFW Error %d: %s\n", error, description);
}

static void help_marker(const char* desc)
{
    ImGui::TextDisabled("(?)");
    if (ImGui::IsItemHovered(ImGuiHoveredFlags_DelayShort))
    {
        ImGui::BeginTooltip();
        ImGui::SetWindowFontScale(font_size);
        ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
        ImGui::TextUnformatted(desc);
        ImGui::PopTextWrapPos();
        ImGui::EndTooltip();
    }
}

void update_background_color() {
    background_vertices[2] = background_color.x;
    background_vertices[3] = background_color.y;
    background_vertices[4] = background_color.z;
    background_vertices[7] = background_color.x;
    background_vertices[8] = background_color.y;
    background_vertices[9] = background_color.z;
    background_vertices[12] = background_color.x;
    background_vertices[13] = background_color.y;
    background_vertices[14] = background_color.z;
    background_vertices[17] = background_color.x;
    background_vertices[18] = background_color.y;
    background_vertices[19] = background_color.z;

    background_vao = std::make_shared<VAO>();
    background_vao->bind();
    background_vbo = std::make_shared<VBO>(background_vertices, 4 * sizeof(background_vertices));
    background_ebo = std::make_shared<EBO>(background_indices, 4 * sizeof(background_indices));
    background_vao->link_attribute(background_vbo, 0, 2, GL_FLOAT, 5 * sizeof(float), (void*)0);
    background_vao->link_attribute(background_vbo, 1, 3, GL_FLOAT, 5 * sizeof(float), (void*)(2 * sizeof(float)));
    background_vao->unbind();
    background_vbo->unbind();
    background_ebo->unbind();
}

void clear_button_callback() {
    draw = false;
}

void solve_button_callback() {
    if (maze_type == MazeType::STATIC && maze) {
        if (solver_algorithm == SolverType::BREATH_FIRST_SEARCH) {
            is_solvable = Solver::is_maze_solvable_bfs(maze, {maze->get_nodes()[0]->get_x(), maze->get_nodes()[0]->get_y()},
                                                       {maze->get_nodes()[maze->get_nodes().size() - 1]->get_x(),
                                                        maze->get_nodes()[maze->get_nodes().size() - 1]->get_y()});

            solved_path = Solver::solve_maze_bfs(maze, {maze->get_nodes()[0]->get_x(), maze->get_nodes()[0]->get_y()},
                                                 {maze->get_nodes()[maze->get_nodes().size() - 1]->get_x(),
                                                  maze->get_nodes()[maze->get_nodes().size() - 1]->get_y()});

            is_solvable_from_player = Solver::is_maze_solvable_bfs(maze, {player->get_x(), player->get_y()},
                                                                  {maze->get_nodes()[maze->get_nodes().size() - 1]->get_x(),
                                                                   maze->get_nodes()[maze->get_nodes().size() - 1]->get_y()});

            solved_path_from_player = Solver::solve_maze_bfs(maze, {player->get_x(), player->get_y()},
                                                            {maze->get_nodes()[maze->get_nodes().size() - 1]->get_x(),
                                                             maze->get_nodes()[maze->get_nodes().size() - 1]->get_y()});
        }
        else if (solver_algorithm == SolverType::DIJKSTRA) {
            is_solvable = Solver::is_maze_solvable_dijkstra(maze, {maze->get_nodes()[0]->get_x(), maze->get_nodes()[0]->get_y()},
                                                            {maze->get_nodes()[maze->get_nodes().size() - 1]->get_x(),
                                                             maze->get_nodes()[maze->get_nodes().size() - 1]->get_y()});

            solved_path = Solver::solve_maze_dijkstra(maze, {maze->get_nodes()[0]->get_x(), maze->get_nodes()[0]->get_y()},
                                                        {maze->get_nodes()[maze->get_nodes().size() - 1]->get_x(),
                                                         maze->get_nodes()[maze->get_nodes().size() - 1]->get_y()});

            is_solvable_from_player = Solver::is_maze_solvable_dijkstra(maze, {player->get_x(), player->get_y()},
                                                                       {maze->get_nodes()[maze->get_nodes().size() - 1]->get_x(),
                                                                        maze->get_nodes()[maze->get_nodes().size() - 1]->get_y()});

            solved_path_from_player = Solver::solve_maze_dijkstra(maze, {player->get_x(), player->get_y()},
                                                                    {maze->get_nodes()[maze->get_nodes().size() - 1]->get_x(),
                                                                    maze->get_nodes()[maze->get_nodes().size() - 1]->get_y()});
        }
        else if (solver_algorithm == SolverType::A_STAR) {
            is_solvable = Solver::is_maze_solvable_a_star(maze, {maze->get_nodes()[0]->get_x(), maze->get_nodes()[0]->get_y()},
                                                          {maze->get_nodes()[maze->get_nodes().size() - 1]->get_x(),
                                                           maze->get_nodes()[maze->get_nodes().size() - 1]->get_y()}, heuristic);

            solved_path = Solver::solve_maze_a_star(maze, {maze->get_nodes()[0]->get_x(), maze->get_nodes()[0]->get_y()},
                                                    {maze->get_nodes()[maze->get_nodes().size() - 1]->get_x(),
                                                     maze->get_nodes()[maze->get_nodes().size() - 1]->get_y()}, heuristic);

            is_solvable_from_player = Solver::is_maze_solvable_a_star(maze, {player->get_x(), player->get_y()},
                                                                     {maze->get_nodes()[maze->get_nodes().size() - 1]->get_x(),
                                                                      maze->get_nodes()[maze->get_nodes().size() - 1]->get_y()}, heuristic);

            solved_path_from_player = Solver::solve_maze_a_star(maze, {player->get_x(), player->get_y()},
                                                                {maze->get_nodes()[maze->get_nodes().size() - 1]->get_x(),
                                                                 maze->get_nodes()[maze->get_nodes().size() - 1]->get_y()}, heuristic);
        }
    }
    else if (maze_type == MazeType::DYNAMIC && ca) {
        if (solver_algorithm == SolverType::BREATH_FIRST_SEARCH) {
            is_solvable = Solver::is_maze_solvable_bfs(ca->get_graph(), {ca->get_graph()->get_nodes()[0]->get_x(), ca->get_graph()->get_nodes()[0]->get_y()},
                                                       {ca->get_graph()->get_nodes()[ca->get_graph()->get_nodes().size() - 1]->get_x(),
                                                        ca->get_graph()->get_nodes()[ca->get_graph()->get_nodes().size() - 1]->get_y()});

            solved_path = Solver::solve_maze_bfs(ca->get_graph(), {ca->get_graph()->get_nodes()[0]->get_x(), ca->get_graph()->get_nodes()[0]->get_y()},
                                                 {ca->get_graph()->get_nodes()[ca->get_graph()->get_nodes().size() - 1]->get_x(),
                                                  ca->get_graph()->get_nodes()[ca->get_graph()->get_nodes().size() - 1]->get_y()});

            is_solvable_from_player = Solver::is_maze_solvable_bfs(ca->get_graph(), {player->get_x(), player->get_y()},
                                                                  {ca->get_graph()->get_nodes()[ca->get_graph()->get_nodes().size() - 1]->get_x(),
                                                                   ca->get_graph()->get_nodes()[ca->get_graph()->get_nodes().size() - 1]->get_y()});

            solved_path_from_player = Solver::solve_maze_bfs(ca->get_graph(), {player->get_x(), player->get_y()},
                                                            {ca->get_graph()->get_nodes()[ca->get_graph()->get_nodes().size() - 1]->get_x(),
                                                             ca->get_graph()->get_nodes()[ca->get_graph()->get_nodes().size() - 1]->get_y()});
        }
        else if (solver_algorithm == SolverType::DIJKSTRA) {
            is_solvable = Solver::is_maze_solvable_dijkstra(ca->get_graph(), {ca->get_graph()->get_nodes()[0]->get_x(), ca->get_graph()->get_nodes()[0]->get_y()},
                                                            {ca->get_graph()->get_nodes()[ca->get_graph()->get_nodes().size() - 1]->get_x(),
                                                             ca->get_graph()->get_nodes()[ca->get_graph()->get_nodes().size() - 1]->get_y()});

            solved_path = Solver::solve_maze_dijkstra(ca->get_graph(), {ca->get_graph()->get_nodes()[0]->get_x(), ca->get_graph()->get_nodes()[0]->get_y()},
                                                        {ca->get_graph()->get_nodes()[ca->get_graph()->get_nodes().size() - 1]->get_x(),
                                                         ca->get_graph()->get_nodes()[ca->get_graph()->get_nodes().size() - 1]->get_y()});

            is_solvable_from_player = Solver::is_maze_solvable_dijkstra(ca->get_graph(), {player->get_x(), player->get_y()},
                                                                       {ca->get_graph()->get_nodes()[ca->get_graph()->get_nodes().size() - 1]->get_x(),
                                                                        ca->get_graph()->get_nodes()[ca->get_graph()->get_nodes().size() - 1]->get_y()});

            solved_path_from_player = Solver::solve_maze_dijkstra(ca->get_graph(), {player->get_x(), player->get_y()},
                                                                 {ca->get_graph()->get_nodes()[ca->get_graph()->get_nodes().size() - 1]->get_x(),
                                                                  ca->get_graph()->get_nodes()[ca->get_graph()->get_nodes().size() - 1]->get_y()});
        }
        else if (solver_algorithm == SolverType::A_STAR) {
            is_solvable = Solver::is_maze_solvable_a_star(ca->get_graph(), {ca->get_graph()->get_nodes()[0]->get_x(), ca->get_graph()->get_nodes()[0]->get_y()},
                                                          {ca->get_graph()->get_nodes()[ca->get_graph()->get_nodes().size() - 1]->get_x(),
                                                           ca->get_graph()->get_nodes()[ca->get_graph()->get_nodes().size() - 1]->get_y()}, heuristic);

            solved_path = Solver::solve_maze_a_star(ca->get_graph(), {ca->get_graph()->get_nodes()[0]->get_x(), ca->get_graph()->get_nodes()[0]->get_y()},
                                                    {ca->get_graph()->get_nodes()[ca->get_graph()->get_nodes().size() - 1]->get_x(),
                                                     ca->get_graph()->get_nodes()[ca->get_graph()->get_nodes().size() - 1]->get_y()}, heuristic);

            is_solvable_from_player = Solver::is_maze_solvable_a_star(ca->get_graph(), {player->get_x(), player->get_y()},
                                                                     {ca->get_graph()->get_nodes()[ca->get_graph()->get_nodes().size() - 1]->get_x(),
                                                                      ca->get_graph()->get_nodes()[ca->get_graph()->get_nodes().size() - 1]->get_y()}, heuristic);

            solved_path_from_player = Solver::solve_maze_a_star(ca->get_graph(), {player->get_x(), player->get_y()},
                                                                {ca->get_graph()->get_nodes()[ca->get_graph()->get_nodes().size() - 1]->get_x(),
                                                                 ca->get_graph()->get_nodes()[ca->get_graph()->get_nodes().size() - 1]->get_y()}, heuristic);
        }
    }

    Drawing::buffer_lines(solution_vao, solution_vbo, solution_ebo, solved_path, solution_color);
    Drawing::buffer_lines(solution_from_player_vao, solution_from_player_vbo, solution_from_player_ebo, solved_path_from_player, solution_from_player_color);
}

void reset_player_button_callback() {
    if (maze_type == MazeType::STATIC && maze) {
        player = std::make_unique<Player>(maze->get_nodes()[0]->get_x(), maze->get_nodes()[0]->get_y());
    }
    else if (maze_type == MazeType::DYNAMIC && ca) {
        player = std::make_unique<Player>(ca->get_graph()->get_nodes()[0]->get_x(), ca->get_graph()->get_nodes()[0]->get_y());
        ca->reset();
        Drawing::buffer_graph(paths_vao, paths_vbo, paths_ebo, ca->get_graph(), paths_color);
    }
    is_solved = false;
    solve_button_callback();
    Drawing::buffer_lines(player_path_vao, player_path_vbo, player_path_ebo, player->get_path(), player_path_color);
}

void generate_button_callback() {
    draw = true;

    if (graph_type == GraphType::ORTHOGONAL)
        graph = Generator::create_orthogonal_grid_graph(WINDOW_HEIGHT / GRID_SIZE - 1, WINDOW_HEIGHT / GRID_SIZE - 1, non_grid_version);
    else if (graph_type == GraphType::HEXAGONAL)
        graph = Generator::create_hexagonal_grid_graph(WINDOW_HEIGHT / GRID_SIZE - 1, WINDOW_HEIGHT / GRID_SIZE - 1, non_grid_version);
    else if (graph_type == GraphType::LAPLACIAN)
        graph = Generator::create_orthogonal_grid_graph_laplacian(WINDOW_HEIGHT / GRID_SIZE - 1, WINDOW_HEIGHT / GRID_SIZE - 1, non_grid_version);

    if (maze_type == MazeType::STATIC) {
        if (generator_algorithm == GeneratorType::DEPTH_FIRST_SEARCH)
            maze = Generator::generate_maze_dfs(graph);
        else if (generator_algorithm == GeneratorType::KRUSKAL)
            maze = Generator::generate_maze_kruskal(graph, horizontal_bias, vertical_bias, cycle_bias);

        Drawing::buffer_graph(paths_vao, paths_vbo, paths_ebo, maze, paths_color);
    }
    else if (maze_type == MazeType::DYNAMIC) {
        if (neighborhood_graph_type == NeighborhoodGraphType::SAME_AS_GRAPH_TYPE)
            neighborhood = graph->create_copy();
        else if (neighborhood_graph_type == NeighborhoodGraphType::ORTHOGONAL)
            neighborhood = Generator::create_orthogonal_grid_graph(WINDOW_HEIGHT / GRID_SIZE - 1, WINDOW_HEIGHT / GRID_SIZE - 1, non_grid_version);
        else if (neighborhood_graph_type == NeighborhoodGraphType::HEXAGONAL)
            neighborhood = Generator::create_hexagonal_grid_graph(WINDOW_HEIGHT / GRID_SIZE - 1, WINDOW_HEIGHT / GRID_SIZE - 1, non_grid_version);
        else if (neighborhood_graph_type == NeighborhoodGraphType::LAPLACIAN)
            neighborhood = Generator::create_orthogonal_grid_graph_laplacian(WINDOW_HEIGHT / GRID_SIZE - 1, WINDOW_HEIGHT / GRID_SIZE - 1, non_grid_version);

        ca = std::make_unique<CellularAutomata>(rulestring, graph, neighborhood, initialize_square_size);

        Drawing::buffer_graph(paths_vao, paths_vbo, paths_ebo, ca->get_graph(), paths_color);
    }

    reset_player_button_callback();
}

int main(int argc, char **argv) {
    // Initialize the library
    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit())
        return -1;

    // GL 3.0 + GLSL 130
    const char* glsl_version = "#version 130";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

    // Create a windowed mode window and its OpenGL context
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    GLFWwindow *window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Maze", nullptr, nullptr);
    if (!window) {
        glfwTerminate();
        return -1;
    }

    // Make the window's context current
    glfwMakeContextCurrent(window);
    const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
    glfwSetWindowPos(window, (mode->width - WINDOW_WIDTH) / 2, (mode->height - WINDOW_HEIGHT) / 2);

    GLenum err = glewInit();
    if (err != GLEW_OK) {
        // Problem: glewInit failed, something is seriously wrong
        std::cerr << "Error: " << glewGetErrorString(err) << std::endl;
    }

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    (void)io;

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();

    // Print out some info about the graphics drivers
    std::cout << "OpenGL version: " << glGetString(GL_VERSION) << std::endl;
    std::cout << "GLEW version: " << glewGetString(GLEW_VERSION) << std::endl;

    // Set callbacks for user inputs
//    glfwSetKeyCallback(window, key_callback);
    glfwSetCursorPosCallback(window, cursor_position_callback);

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    // Background color
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

    // Load shader
    auto source_default = Shader::parse_shader("src/graphics/shaders/default.shader");
    auto shader_default = Shader::create_shader(source_default.vertex_source, source_default.fragment_source);

    // Background rectangle
    float rect_x, rect_y;
    float rect_width, rect_height;
    Drawing::transform_x_y_to_opengl(WINDOW_X_OFFSET, 0, rect_x, rect_y);
    Drawing::transform_x_y_to_opengl(WINDOW_WIDTH, WINDOW_HEIGHT, rect_width, rect_height);
    background_vertices[0] = rect_x;
    background_vertices[1] = rect_y;
    background_vertices[5] = rect_x;
    background_vertices[6] = rect_height;
    background_vertices[10] = rect_width;
    background_vertices[11] = rect_height;
    background_vertices[15] = rect_width;
    background_vertices[16] = rect_y;
    update_background_color();

    auto now = std::chrono::high_resolution_clock::now();
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Loop until the user closes the window
    while (!glfwWindowShouldClose(window)) {
        // Poll for and process events
        glfwPollEvents();

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // Render here
        glClear(GL_COLOR_BUFFER_BIT);

        // Setup shader
        glUseProgram(shader_default);

        // Draw background
        background_vao->bind();
        glDrawElements(GL_QUADS, background_ebo->num_elements, GL_UNSIGNED_INT, 0);
        background_vao->unbind();

        if (draw) {
            // Evolve the cellular automata
            if (!paused && maze_type == MazeType::DYNAMIC && std::chrono::duration_cast<std::chrono::milliseconds>(
                    std::chrono::high_resolution_clock::now() - now).count() > (int) (1000 * (1.0f - speed))) {
                ca->next_generation();
                Drawing::buffer_graph(paths_vao, paths_vbo, paths_ebo, ca->get_graph(), paths_color);
                now = std::chrono::high_resolution_clock::now();

                // Check if the player is standing on a dead node
                auto player_node = ca->get_graph()->get_nearest_node_to(player->get_x(), player->get_y());
                if (!(ca->get_graph()->get_nodes()[player_node]->is_alive())) {
                    // Move the player to the nearest alive node
                    auto nearest_alive_node = ca->get_graph()->get_nearest_alive_node_to(player->get_x(), player->get_y());
                    player->move_to(ca->get_graph()->get_nodes()[nearest_alive_node]->get_x(),
                                    ca->get_graph()->get_nodes()[nearest_alive_node]->get_y());
                    Drawing::buffer_lines(player_path_vao, player_path_vbo, player_path_ebo, player->get_path(), player_path_color);
                }

                // Do the solutions
                solve_button_callback();

                // Check if the player has reached the end
                is_solved = is_solved || (player->get_x() == ca->get_graph()->get_nodes()[ca->get_graph()->get_v() - 1]->get_x() &&
                                          player->get_y() == ca->get_graph()->get_nodes()[ca->get_graph()->get_v() - 1]->get_y());
            }

            // Draw maze
            glLineWidth(WHITE_LINE_WIDTH);
            paths_vao->bind();
            glDrawElements(GL_LINES, paths_ebo->num_elements, GL_UNSIGNED_INT, 0);
            paths_vao->unbind();

            if (maze_type == MazeType::STATIC) {
                for (auto &node: maze->get_nodes())
                    Drawing::draw_circle(node->get_x(), node->get_y(), WHITE_NODE_RADIUS, paths_color);
            }
            else if (maze_type == MazeType::DYNAMIC) {
                for (auto &node: ca->get_graph()->get_nodes())
                    if (node->is_alive())
                        Drawing::draw_circle(node->get_x(), node->get_y(), WHITE_NODE_RADIUS, paths_color);
            }

            // Color the start and end nodes
            if (maze_type == MazeType::STATIC) {
                Drawing::draw_circle(maze->get_nodes()[0]->get_x(), maze->get_nodes()[0]->get_y(), PLAYER_RADIUS * 1.5f, start_end_color);
                Drawing::draw_circle(maze->get_nodes()[maze->get_v() - 1]->get_x(), maze->get_nodes()[maze->get_v() - 1]->get_y(), PLAYER_RADIUS * 1.5f, start_end_color);
            }
            else if (maze_type == MazeType::DYNAMIC) {
                Drawing::draw_circle(ca->get_graph()->get_nodes()[0]->get_x(), ca->get_graph()->get_nodes()[0]->get_y(),
                                     PLAYER_RADIUS * 1.5f, start_end_color);
                Drawing::draw_circle(ca->get_graph()->get_nodes()[ca->get_graph()->get_v() - 1]->get_x(),
                                     ca->get_graph()->get_nodes()[ca->get_graph()->get_v() - 1]->get_y(),
                                     PLAYER_RADIUS * 1.5f, start_end_color);
            }

            // Draw solution if wanted
            glEnable(GL_BLEND);
            if (is_solvable && show_solution) {
                glLineWidth(0.33f * WHITE_LINE_WIDTH);
                solution_vao->bind();
                glDrawElements(GL_LINES, solution_ebo->num_elements, GL_UNSIGNED_INT, 0);
                solution_vao->unbind();
            }

            if (is_solvable_from_player && show_solution_from_player) {
                glLineWidth(0.33f * WHITE_LINE_WIDTH);
                solution_from_player_vao->bind();
                glDrawElements(GL_LINES, solution_from_player_ebo->num_elements, GL_UNSIGNED_INT, 0);
                solution_from_player_vao->unbind();
            }

            // Draw path that the player has walked
            glLineWidth(3.0f);
            player_path_vao->bind();
            glDrawElements(GL_LINES, player_path_ebo->num_elements, GL_UNSIGNED_INT, 0);
            player_path_vao->unbind();
            glDisable(GL_BLEND);

            // Draw player
            Drawing::draw_circle(player->get_x(), player->get_y(), PLAYER_RADIUS, player_color);
        }

        {
            const char *maze_types[] = {
                    "Static Maze",
                    "Dynamic Maze"
            };
            const char *graph_types[] = {
                    "Orthogonal Grid Graph",
                    "Hexagonal Grid Graph",
                    "Orthogonal Grid Graph with Diagonals"
            };
            const char *generator_algorithms[] = {
                    "Depth First Search",
                    "Kruskal's Modified Algorithm",
            };
            const char *neighborhood_graph_types[] = {
                    "Same as Graph Type",
                    "Orthogonal Grid Graph",
                    "Hexagonal Grid Graph",
                    "Orthogonal Grid Graph with Diagonals"
            };
            const char *solver_algorithms[] = {
                    "Breadth First Search",
                    "Dijkstra's Algorithm",
                    "A* Search"
            };
            const char *heuristics[] = {
                    "Manhattan Distance",
                    "Euclidean Distance",
                    "Cosine Distance"
            };

            ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
            ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);

            ImGui::Begin("Configuration", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus | ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoTitleBar);

            ImGui::SetWindowPos(ImVec2(0, 0));
            ImGui::SetWindowSize(ImVec2((float) WINDOW_X_OFFSET, (float) WINDOW_HEIGHT));
            ImGui::SetWindowFontScale(font_size);

            if (ImGui::BeginMenuBar()) {
                if (ImGui::BeginMenu("File")) {
                    if (ImGui::MenuItem("Exit", "Alt+F4"))
                        glfwSetWindowShouldClose(window, true);
                    ImGui::EndMenu();
                }
                if (ImGui::BeginMenu("Settings")) {
                    if (ImGui::MenuItem("Graphics Settings")) {
                        show_settings_window = true;
                    }
                    ImGui::EndMenu();
                }
                if (ImGui::BeginMenu("Help")) {
                    if (ImGui::MenuItem("About")) {
                        show_about_window = true;
                    }
                    ImGui::EndMenu();
                }
                ImGui::EndMenuBar();
            }

            ImGui::SeparatorText("Maze Type");
            if (ImGui::Combo("", (int *) &maze_type, maze_types, IM_ARRAYSIZE(maze_types))) {
                clear_button_callback();
                is_solvable = false;
                is_solvable_from_player = false;
                is_solved = false;
            }

            ImGui::SeparatorText("Graph Settings");
            if (ImGui::InputInt("Grid Size", &GRID_SIZE, 1, 5)) {
                if (GRID_SIZE < 10) GRID_SIZE = 10;
                if (GRID_SIZE > 100) GRID_SIZE = 100;

                WHITE_LINE_WIDTH = (float) GRID_SIZE * 0.5f;
                WHITE_NODE_RADIUS = WHITE_LINE_WIDTH / (float) WINDOW_HEIGHT;
                PLAYER_RADIUS = (float) GRID_SIZE * 0.25f / (float) WINDOW_HEIGHT;
                clear_button_callback();
            }
            ImGui::Combo("Base Graph", (int *) &graph_type, graph_types, IM_ARRAYSIZE(graph_types));
            ImGui::Checkbox("Non-Grid", &non_grid_version);
            ImGui::SameLine();
            help_marker("Non-Grid means that the graph appears as if it was not a grid\nEvery cell is randomly moved a little bit");

            if (maze_type == MazeType::STATIC) {
                ImGui::SeparatorText("Generator Settings");
                ImGui::Combo("Generator", (int *) &generator_algorithm, generator_algorithms, IM_ARRAYSIZE(generator_algorithms));

                if (generator_algorithm == GeneratorType::KRUSKAL) {
                    if (ImGui::SliderFloat("Horizontal Bias", &horizontal_bias, 0.0f, 1.0))
                        vertical_bias = 1.0f - horizontal_bias;
                    ImGui::SameLine();
                    help_marker("Horizontal Bias means that horizontal paths are more likely to be chosen over vertical ones\nIf the value is 0, only essential horizontal paths will be created\nIf the value is 1, only essential vertical paths will be created");
                    if (ImGui::SliderFloat("Vertical Bias", &vertical_bias, 0.0f, 1.0))
                        horizontal_bias = 1.0f - vertical_bias;
                    ImGui::SameLine();
                    help_marker("Vertical Bias means that vertical paths are more likely to be chosen over horizontal ones\nIf the value is 0, only essential vertical paths will be created\nIf the value is 1, only essential horizontal paths will be created");
                    ImGui::SliderFloat("Cycle Bias", &cycle_bias, 0.0f, 1.0f);
                    ImGui::SameLine();
                    help_marker("Cycle Bias refers to the probability of cycles being created\nIf the value is 0, no cycles will be created, resulting in a Perfect Maze\nIf the value is 1, cycles will be created as much as possible, resulting in Fully Connected Graph");
                }
            }

            else if (maze_type == MazeType::DYNAMIC) {
                ImGui::Combo("Neighborhood Graph", (int *) &neighborhood_graph_type, neighborhood_graph_types, IM_ARRAYSIZE(neighborhood_graph_types));
                ImGui::SameLine();
                help_marker("Neighborhood Graph is the graph that is used as reference for possible neighborhoods checking\nChecking doesn't necessarily mean that the original Base Graph is able to have these edges\nExample: Game of Life uses Base Graph with 4-neighborhood and Neighborhood Graph with 8-neighborhood");

                ImGui::SeparatorText("Cellular Automata Settings");
                auto temp_rulestring = rulestring;
                if (ImGui::InputText("Rulestring", &temp_rulestring)) {
                    if (std::regex_match(temp_rulestring, rulestring_regex))
                        rulestring = temp_rulestring;
                }
                ImGui::SameLine();
                help_marker("Rulestring format is B[0-9]+/S[0-9]+\nB is birth rule\nS is survival rule\nExample: Game of Life Rulestring is B3/S23");
                if (ImGui::InputInt("Initial Square Size", &initialize_square_size)) {
                    if (initialize_square_size < -1) initialize_square_size = -1;
                    if (initialize_square_size > WINDOW_HEIGHT / GRID_SIZE - 1) initialize_square_size = WINDOW_HEIGHT / GRID_SIZE - 1;
                }
                ImGui::SameLine();
                help_marker("Size of square of cells that is used to initialize the cellular automata\nInitial state is random\n-1 means that the whole grid is used");
                ImGui::SliderFloat("Speed", &speed, 0.0f, 1.0f);
                ImGui::Checkbox("Pause Evolution & Allow Movement", &paused);
                ImGui::SameLine();
                help_marker("Pausing the evolution not only stops the evolution of the Cellular Automata\nIt also allows the player to move!");
            }

            ImGui::SeparatorText("Solver Settings");
            if (ImGui::Combo("Solver", (int *) &solver_algorithm, solver_algorithms, IM_ARRAYSIZE(solver_algorithms)))
                solve_button_callback();
            if (solver_algorithm == SolverType::A_STAR) {
                if (ImGui::Combo("Heuristic", (int *) &heuristic, heuristics, IM_ARRAYSIZE(heuristics)))
                    solve_button_callback();
            }
            ImGui::Checkbox("Show Solution", &show_solution);
            ImGui::Checkbox("Show Solution from the Player", &show_solution_from_player);

            ImGui::SeparatorText("Maze Status");
            ImGui::Text("Maze");
            ImGui::SameLine();
            is_solvable ? ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(0,200,0,255)) : ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(200,0,0,255));
            is_solvable ? ImGui::Text("IS") : ImGui::Text("IS NOT");
            ImGui::PopStyleColor();
            ImGui::SameLine();
            ImGui::Text("solvable");
            ImGui::Text("Maze");
            ImGui::SameLine();
            is_solvable_from_player ? ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(0,200,0,255)) : ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(200,0,0,255));
            is_solvable_from_player ? ImGui::Text("IS") : ImGui::Text("IS NOT");
            ImGui::PopStyleColor();
            ImGui::SameLine();
            ImGui::Text("solvable from the player's position");
            ImGui::Text("Maze");
            ImGui::SameLine();
            is_solved ? ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(0,200,0,255)) : ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(200,0,0,255));
            is_solved ? ImGui::Text("IS") : ImGui::Text("IS NOT");
            ImGui::PopStyleColor();
            ImGui::SameLine();
            ImGui::Text("solved");

            ImGui::SeparatorText("Controls");
            if (ImGui::Button("Generate"))
                generate_button_callback();
            if (ImGui::Button("Reset"))
                reset_player_button_callback();
            if (ImGui::Button("Clear"))
                clear_button_callback();

            ImGui::End();
            ImGui::PopStyleVar(2);
        }

        {
            if (show_settings_window) {
                ImGui::Begin("Graphics Settings", &show_settings_window, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);

                ImGui::SetWindowSize(ImVec2(600, 400));

                ImGui::SeparatorText("Window Settings");

                if (ImGui::Button("Set 1280x720")) {
                    WINDOW_WIDTH = 1280;
                    WINDOW_HEIGHT = 720;
                    WINDOW_X_OFFSET = WINDOW_WIDTH - WINDOW_HEIGHT;
                    WHITE_NODE_RADIUS = WHITE_LINE_WIDTH / (float) WINDOW_HEIGHT;
                    PLAYER_RADIUS = (float) GRID_SIZE * 0.25f / (float) WINDOW_HEIGHT;
                    Drawing::transform_x_y_to_opengl(WINDOW_X_OFFSET, 0, rect_x, rect_y);
                    Drawing::transform_x_y_to_opengl(WINDOW_WIDTH, WINDOW_HEIGHT, rect_width, rect_height);
                    glfwSetWindowSize(window, WINDOW_WIDTH, WINDOW_HEIGHT);
                    glfwSetWindowPos(window, (mode->width - WINDOW_WIDTH) / 2, (mode->height - WINDOW_HEIGHT) / 2);
                    clear_button_callback();
                }
                ImGui::SameLine();
                if (ImGui::Button("Set 1600x900")) {
                    WINDOW_WIDTH = 1600;
                    WINDOW_HEIGHT = 900;
                    WINDOW_X_OFFSET = WINDOW_WIDTH - WINDOW_HEIGHT;
                    WHITE_NODE_RADIUS = WHITE_LINE_WIDTH / (float) WINDOW_HEIGHT;
                    PLAYER_RADIUS = (float) GRID_SIZE * 0.25f / (float) WINDOW_HEIGHT;
                    Drawing::transform_x_y_to_opengl(WINDOW_X_OFFSET, 0, rect_x, rect_y);
                    Drawing::transform_x_y_to_opengl(WINDOW_WIDTH, WINDOW_HEIGHT, rect_width, rect_height);
                    glfwSetWindowSize(window, WINDOW_WIDTH, WINDOW_HEIGHT);
                    glfwSetWindowPos(window, (mode->width - WINDOW_WIDTH) / 2, (mode->height - WINDOW_HEIGHT) / 2);
                    clear_button_callback();
                }
                ImGui::SameLine();
                if (!fullscreen) {
                    if (ImGui::Button("Fullscreen")) {
                        WINDOW_WIDTH = mode->width;
                        WINDOW_HEIGHT = mode->height;
                        WINDOW_X_OFFSET = WINDOW_WIDTH - WINDOW_HEIGHT;
                        WHITE_NODE_RADIUS = WHITE_LINE_WIDTH / (float) WINDOW_HEIGHT;
                        PLAYER_RADIUS = (float) GRID_SIZE * 0.25f / (float) WINDOW_HEIGHT;
                        Drawing::transform_x_y_to_opengl(WINDOW_X_OFFSET, 0, rect_x, rect_y);
                        Drawing::transform_x_y_to_opengl(WINDOW_WIDTH, WINDOW_HEIGHT, rect_width, rect_height);
                        glfwSetWindowMonitor(window, glfwGetPrimaryMonitor(), 0, 0, mode->width, mode->height, mode->refreshRate);
                        clear_button_callback();
                        fullscreen = true;
                    }
                }
                else {
                    if (ImGui::Button("Windowed")) {
                        WINDOW_WIDTH = 1280;
                        WINDOW_HEIGHT = 720;
                        WINDOW_X_OFFSET = WINDOW_WIDTH - WINDOW_HEIGHT;
                        WHITE_NODE_RADIUS = WHITE_LINE_WIDTH / (float) WINDOW_HEIGHT;
                        PLAYER_RADIUS = (float) GRID_SIZE * 0.25f / (float) WINDOW_HEIGHT;
                        Drawing::transform_x_y_to_opengl(WINDOW_X_OFFSET, 0, rect_x, rect_y);
                        Drawing::transform_x_y_to_opengl(WINDOW_WIDTH, WINDOW_HEIGHT, rect_width, rect_height);
                        glfwSetWindowMonitor(window, nullptr, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, 0);
                        glfwSetWindowPos(window, (mode->width - WINDOW_WIDTH) / 2, (mode->height - WINDOW_HEIGHT) / 2);
                        clear_button_callback();
                        fullscreen = false;
                    }
                }
                if (ImGui::InputFloat("Font Size", &font_size, 0.1f, 0.3f, "%.1f")) {
                    if (font_size < 1.0f) font_size = 1.0f;
                    if (font_size > 2.0f) font_size = 2.0f;
                }
                ImGui::SetWindowFontScale(font_size);
                ImGui::SeparatorText("Colors");
                static int style_idx = 0;
                if (ImGui::Combo("Style", &style_idx, "Dark\0Light\0Classic\0"))
                {
                    switch (style_idx) {
                        case 0:
                            ImGui::StyleColorsDark();
                            background_color = {29. / 255, 29. / 255, 29. / 255, 1.0f};
                            update_background_color();
                            break;
                        case 1:
                            ImGui::StyleColorsLight();
                            background_color = {240. / 255, 240. / 255, 240. / 255, 1.0f};
                            update_background_color();
                            break;
                        case 2:
                            ImGui::StyleColorsClassic();
                            background_color = {38. / 255, 38. / 255, 38. / 255, 1.0f};
                            update_background_color();
                            break;
                        default:
                            break;
                    }
                }
                if (ImGui::ColorEdit3("Background Color", (float*)&background_color)) {
                    update_background_color();
                }
                if (ImGui::ColorEdit3("Maze Paths Color", (float*)&paths_color)) {
                    if (maze_type == MazeType::STATIC && maze)
                        Drawing::buffer_graph(paths_vao, paths_vbo, paths_ebo, maze, paths_color);
                    else if (maze_type == MazeType::DYNAMIC && ca)
                        Drawing::buffer_graph(paths_vao, paths_vbo, paths_ebo, ca->get_graph(), paths_color);
                }
                ImGui::ColorEdit3("Start and End Color", (float*)&start_end_color);
                ImGui::ColorEdit3("Player Color", (float*)&player_color);
                if (ImGui::ColorEdit3("Player Path Color", (float*)&player_path_color)) {
                    if (player)
                        Drawing::buffer_lines(player_path_vao, player_path_vbo, player_path_ebo, player->get_path(), player_path_color);
                }
                if (ImGui::ColorEdit3("Solution Color", (float*)&solution_color)) {
                    if (!solved_path.empty())
                        Drawing::buffer_lines(solution_vao, solution_vbo, solution_ebo, solved_path, solution_color);
                }
                if (ImGui::ColorEdit3("Solution from Player Color", (float*)&solution_from_player_color)) {
                    if (!solved_path_from_player.empty())
                        Drawing::buffer_lines(solution_from_player_vao, solution_from_player_vbo, solution_from_player_ebo, solved_path_from_player, solution_from_player_color);
                }
                ImGui::End();
            }
        }

        {
            if (show_about_window) {
                ImGui::SetNextWindowPos(ImGui::GetMainViewport()->GetCenter(), ImGuiCond_Always, ImVec2(0.5f, 0.5f));
                ImGui::Begin("About", &show_about_window, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoCollapse);
                ImGui::SetWindowFontScale(font_size);
                ImGui::Text("This application was made by:\nDominik Zappe");
                ImGui::Separator();
                ImGui::Text("Application serves as Bachelors Thesis Project\nfor the University of Applied Sciences\nin the field of Computer Science");
                ImGui::End();
            }
        }

        // ImGui Rendering
        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        // Swap front and back buffers
        glfwSwapBuffers(window);
        glfwSwapInterval(1); // Enable vsync
    }

    // Cleanup
    glDeleteProgram(shader_default);
    if (background_vao) background_vao->del();
    if (background_vbo) background_vbo->del();
    if (background_ebo) background_ebo->del();
    if (paths_vao) paths_vao->del();
    if (paths_vbo) paths_vbo->del();
    if (paths_ebo) paths_ebo->del();
    if (solution_vao) solution_vao->del();
    if (solution_vbo) solution_vbo->del();
    if (solution_ebo) solution_ebo->del();
    if (player_path_vao) player_path_vao->del();
    if (player_path_vbo) player_path_vbo->del();
    if (player_path_ebo) player_path_ebo->del();
    if (solution_from_player_vao) solution_from_player_vao->del();
    if (solution_from_player_vbo) solution_from_player_vbo->del();
    if (solution_from_player_ebo) solution_from_player_ebo->del();

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}

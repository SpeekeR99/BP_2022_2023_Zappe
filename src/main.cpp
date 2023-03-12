#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <regex>
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "imgui_stdlib.h"
#include "graphics/shader.h"
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
static int graph_type = 0;
static int neighborhood_graph_type = 0;
static int maze_type = 0;
static int generator_algorithm = 0;
static int solver_algorithm = 0;
bool non_grid_version = false;
std::unique_ptr<Player> player;
int size_paths, buffer_paths;
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

int WINDOW_WIDTH = 1280;
int WINDOW_HEIGHT = 720;
int WINDOW_X_OFFSET = WINDOW_WIDTH - WINDOW_HEIGHT;
int GRID_SIZE = 50;
float BLACK_LINE_WIDTH = (float) GRID_SIZE * 1.33f;
float WHITE_LINE_WIDTH = (float) GRID_SIZE * 0.5f;
float BLACK_NODE_RADIUS = 2 * BLACK_LINE_WIDTH / (float) std::max(WINDOW_WIDTH, WINDOW_HEIGHT);
float WHITE_NODE_RADIUS = 2 * WHITE_LINE_WIDTH / (float) std::max(WINDOW_WIDTH, WINDOW_HEIGHT);
float PLAYER_RADIUS = (float) GRID_SIZE * 0.5f / (float) std::max(WINDOW_WIDTH, WINDOW_HEIGHT);
bool fullscreen = false;
bool language = true;
bool show_settings_window = false;
bool show_about_window = false;

void cursor_position_callback(GLFWwindow *window, double xpos, double ypos) {
    if (!draw) return;

    auto new_x = static_cast<int>(xpos) - WINDOW_X_OFFSET;
    auto new_y = static_cast<int>(ypos);

    if (new_x < GRID_SIZE / 2 || new_x > WINDOW_HEIGHT - GRID_SIZE / 2 || new_y < GRID_SIZE / 2 || new_y > WINDOW_HEIGHT - GRID_SIZE / 2)
        return;

    if (maze_type == 0) {
        auto nearest_to_player = maze->get_nearest_node_to(player->get_x(), player->get_y());
        auto nearest_to_mouse = maze->get_nearest_node_to(new_x, new_y);

        if (maze->is_adjacent(nearest_to_player, nearest_to_mouse))
            player->move_to(maze->get_nodes()[nearest_to_mouse]->get_x(), maze->get_nodes()[nearest_to_mouse]->get_y());
    }
    else if (maze_type == 1 && paused) {
        auto nearest_to_player = ca->get_graph()->get_nearest_node_to(player->get_x(), player->get_y());
        auto nearest_to_mouse = ca->get_graph()->get_nearest_node_to(new_x, new_y);

        if (ca->get_graph()->is_adjacent(nearest_to_player, nearest_to_mouse) && ca->get_graph()->get_nodes()[nearest_to_mouse]->is_alive())
            player->move_to(ca->get_graph()->get_nodes()[nearest_to_mouse]->get_x(), ca->get_graph()->get_nodes()[nearest_to_mouse]->get_y());
    }

    // Check if the player has reached the end
    if (maze_type == 0)
        is_solved = is_solved || (player->get_x() == maze->get_nodes()[maze->get_v() - 1]->get_x() &&
                                  player->get_y() == maze->get_nodes()[maze->get_v() - 1]->get_y());
    else if (maze_type == 1)
        is_solved = is_solved || (player->get_x() == ca->get_graph()->get_nodes()[ca->get_graph()->get_v() - 1]->get_x() &&
                                  player->get_y() == ca->get_graph()->get_nodes()[ca->get_graph()->get_v() - 1]->get_y());

    // Check the solvability of the maze from the player's position
//    if (maze_type == 0) {
//        if (solver_algorithm == 0) {
//            is_solvable_from_player = Solver::is_maze_solvable_bfs(maze, {player->get_x(), player->get_y()},
//                                                                   {maze->get_nodes()[maze->get_nodes().size() - 1]->get_x(),
//                                                                    maze->get_nodes()[maze->get_nodes().size() - 1]->get_y()});
//            solved_path_from_player = Solver::solve_maze_bfs(maze, {player->get_x(), player->get_y()},
//                                                            {maze->get_nodes()[maze->get_nodes().size() - 1]->get_x(),
//                                                             maze->get_nodes()[maze->get_nodes().size() - 1]->get_y()});
//        }
//    }
//    else if (maze_type == 1 && paused) {
//        if (solver_algorithm == 0) {
//            is_solvable_from_player = Solver::is_maze_solvable_bfs(ca->get_graph(), {player->get_x(), player->get_y()},
//                                                                   {ca->get_graph()->get_nodes()[ca->get_graph()->get_nodes().size() - 1]->get_x(),
//                                                                    ca->get_graph()->get_nodes()[ca->get_graph()->get_nodes().size() - 1]->get_y()});
//            solved_path_from_player = Solver::solve_maze_bfs(ca->get_graph(), {player->get_x(), player->get_y()},
//                                                            {ca->get_graph()->get_nodes()[ca->get_graph()->get_nodes().size() - 1]->get_x(),
//                                                             ca->get_graph()->get_nodes()[ca->get_graph()->get_nodes().size() - 1]->get_y()});
//        }
//    }
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

void reset_player_button_callback() {
    if (maze_type == 0 && maze != nullptr) {
        player = std::make_unique<Player>(maze->get_nodes()[0]->get_x(), maze->get_nodes()[0]->get_y());
    }
    else if (maze_type == 1 && ca != nullptr) {
        player = std::make_unique<Player>(ca->get_graph()->get_nodes()[0]->get_x(), ca->get_graph()->get_nodes()[0]->get_y());
    }
    is_solved = false;
}

void clear_button_callback() {
    draw = false;
}

void solve_button_callback() {
    if (maze == nullptr) return;
    if (maze_type == 0) {
        if (solver_algorithm == 0) {
            is_solvable = Solver::is_maze_solvable_bfs(maze, {maze->get_nodes()[0]->get_x(), maze->get_nodes()[0]->get_y()},
                                                    {maze->get_nodes()[maze->get_nodes().size() - 1]->get_x(),
                                                     maze->get_nodes()[maze->get_nodes().size() - 1]->get_y()});

            solved_path = Solver::solve_maze_bfs(maze, {maze->get_nodes()[0]->get_x(), maze->get_nodes()[0]->get_y()},
                                                     {maze->get_nodes()[maze->get_nodes().size() - 1]->get_x(),
                                                      maze->get_nodes()[maze->get_nodes().size() - 1]->get_y()});

        }

        is_solvable_from_player = is_solvable;
        solved_path_from_player = solved_path;
    }
}

void generate_button_callback() {
    draw = true;

    if (graph_type == 0)
        graph = Generator::create_orthogonal_grid_graph(WINDOW_HEIGHT / GRID_SIZE - 1, WINDOW_HEIGHT / GRID_SIZE - 1, non_grid_version);
    else if (graph_type == 1)
        graph = Generator::create_hexagonal_grid_graph(WINDOW_HEIGHT / GRID_SIZE - 1, WINDOW_HEIGHT / GRID_SIZE - 1, non_grid_version);
    else if (graph_type == 2)
        graph = Generator::create_orthogonal_grid_graph_laplacian(WINDOW_HEIGHT / GRID_SIZE - 1, WINDOW_HEIGHT / GRID_SIZE - 1, non_grid_version);

    if (maze_type == 0) {
        if (generator_algorithm == 0)
            maze = Generator::generate_maze_dfs(graph);

        reset_player_button_callback();

        solve_button_callback();

        Drawing::buffer_graph(maze, size_paths, buffer_paths);
    }
    else if (maze_type == 1) {
        if (neighborhood_graph_type == 0)
            neighborhood = graph->create_copy();
        else if (neighborhood_graph_type == 1)
            neighborhood = Generator::create_orthogonal_grid_graph(WINDOW_HEIGHT / GRID_SIZE - 1, WINDOW_HEIGHT / GRID_SIZE - 1, non_grid_version);
        else if (neighborhood_graph_type == 2)
            neighborhood = Generator::create_hexagonal_grid_graph(WINDOW_HEIGHT / GRID_SIZE - 1, WINDOW_HEIGHT / GRID_SIZE - 1, non_grid_version);
        else if (neighborhood_graph_type == 3)
            neighborhood = Generator::create_orthogonal_grid_graph_laplacian(WINDOW_HEIGHT / GRID_SIZE - 1, WINDOW_HEIGHT / GRID_SIZE - 1, non_grid_version);

        ca = std::make_unique<CellularAutomata>(rulestring, graph, neighborhood, initialize_square_size);

        reset_player_button_callback();

        solve_button_callback();

        Drawing::buffer_graph(ca->get_graph(), size_paths, buffer_paths);
    }
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

    // Load shaders
    auto source_paths = Shader::parse_shader("src/graphics/shaders/paths.shader");
    auto source_green = Shader::parse_shader("src/graphics/shaders/green.shader");
    auto source_red = Shader::parse_shader("src/graphics/shaders/red.shader");
    auto source_blue = Shader::parse_shader("src/graphics/shaders/blue.shader");
    auto source_background = Shader::parse_shader("src/graphics/shaders/background.shader");
    auto shader_paths = Shader::create_shader(source_paths.vertex_source, source_paths.fragment_source);
    auto shader_green = Shader::create_shader(source_green.vertex_source, source_green.fragment_source);
    auto shader_red = Shader::create_shader(source_red.vertex_source, source_red.fragment_source);
    auto shader_blue = Shader::create_shader(source_blue.vertex_source, source_blue.fragment_source);
    auto shader_background = Shader::create_shader(source_background.vertex_source, source_background.fragment_source);

    float rect_x, rect_y;
    float rect_width, rect_height;
    Drawing::transform_x_y_to_opengl(WINDOW_X_OFFSET, 0, rect_x, rect_y);
    Drawing::transform_x_y_to_opengl(WINDOW_WIDTH, WINDOW_HEIGHT, rect_width, rect_height);
    auto now = std::chrono::high_resolution_clock::now();

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

        glUseProgram(shader_background);
        glRectf(rect_x, rect_y, rect_width, rect_height);

        if (draw) {
            if (!paused && maze_type == 1 && std::chrono::duration_cast<std::chrono::milliseconds>(
                    std::chrono::high_resolution_clock::now() - now).count() > (int) (1000 * (1.0f - speed))) {
                ca->next_generation();
                Drawing::buffer_graph(ca->get_graph(), size_paths, buffer_paths);
                now = std::chrono::high_resolution_clock::now();

                auto player_node = ca->get_graph()->get_nearest_node_to(player->get_x(), player->get_y());
                if (!(ca->get_graph()->get_nodes()[player_node]->is_alive())) {
                    auto nearest_alive_node = ca->get_graph()->get_nearest_alive_node_to(player->get_x(), player->get_y());
                    player->move_to(ca->get_graph()->get_nodes()[nearest_alive_node]->get_x(),
                                    ca->get_graph()->get_nodes()[nearest_alive_node]->get_y());
                }

                if (solver_algorithm == 0) {
                    is_solvable = Solver::is_maze_solvable_bfs(ca->get_graph(),
                                                               {ca->get_graph()->get_nodes()[0]->get_x(),
                                                                ca->get_graph()->get_nodes()[0]->get_y()},
                                                               {ca->get_graph()->get_nodes()[
                                                                        ca->get_graph()->get_nodes().size() -
                                                                        1]->get_x(),
                                                                ca->get_graph()->get_nodes()[
                                                                        ca->get_graph()->get_nodes().size() -
                                                                        1]->get_y()});
                    solved_path = Solver::solve_maze_bfs(ca->get_graph(), {ca->get_graph()->get_nodes()[0]->get_x(),
                                                                           ca->get_graph()->get_nodes()[0]->get_y()},
                                                         {ca->get_graph()->get_nodes()[
                                                                  ca->get_graph()->get_nodes().size() -
                                                                  1]->get_x(),
                                                          ca->get_graph()->get_nodes()[
                                                                  ca->get_graph()->get_nodes().size() -
                                                                  1]->get_y()});
                }
            }

            // Draw maze
            glLineWidth(WHITE_LINE_WIDTH);
            glUseProgram(shader_paths);
            glBindBuffer(GL_ARRAY_BUFFER, buffer_paths);
            glEnableVertexAttribArray(0);
            glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), 0);
            glDrawArrays(GL_LINES, 0, size_paths);
            if (maze_type == 0) {
                for (auto &node: maze->get_nodes())
                    Drawing::draw_circle(node->get_x(), node->get_y(), WHITE_NODE_RADIUS);
            }
            else if (maze_type == 1) {
                for (auto &node: ca->get_graph()->get_nodes())
                    if (node->is_alive())
                        Drawing::draw_circle(node->get_x(), node->get_y(), WHITE_NODE_RADIUS);
            }

            // Color the start and end nodes
            glUseProgram(shader_blue);
            if (maze_type == 0) {
                Drawing::draw_circle(maze->get_nodes()[0]->get_x(), maze->get_nodes()[0]->get_y(), PLAYER_RADIUS * 1.5f);
                Drawing::draw_circle(maze->get_nodes()[maze->get_v() - 1]->get_x(), maze->get_nodes()[maze->get_v() - 1]->get_y(), PLAYER_RADIUS * 1.5f);
            }
            else if (maze_type == 1) {
                Drawing::draw_circle(ca->get_graph()->get_nodes()[0]->get_x(), ca->get_graph()->get_nodes()[0]->get_y(),
                                     PLAYER_RADIUS * 1.5f);
                Drawing::draw_circle(ca->get_graph()->get_nodes()[ca->get_graph()->get_v() - 1]->get_x(),
                                     ca->get_graph()->get_nodes()[ca->get_graph()->get_v() - 1]->get_y(),
                                     PLAYER_RADIUS * 1.5f);
            }

            // Draw solution if wanted
            if (is_solvable && show_solution) {
                glLineWidth(0.33f * WHITE_LINE_WIDTH);
                glUseProgram(shader_blue);
                for (int i = 0; i < solved_path.size() - 1; i++)
                    Drawing::draw_line(solved_path[i].first, solved_path[i].second,
                                       solved_path[i + 1].first, solved_path[i + 1].second);
            }

            if (is_solvable_from_player && show_solution_from_player) {
                glLineWidth(0.33f * WHITE_LINE_WIDTH);
                glUseProgram(shader_blue);
                for (int i = 0; i < solved_path_from_player.size() - 1; i++)
                    Drawing::draw_line(solved_path_from_player[i].first, solved_path_from_player[i].second,
                                       solved_path_from_player[i + 1].first, solved_path_from_player[i + 1].second);
            }

            // Draw path that the player has walked
            glLineWidth(3.0f);
            glUseProgram(shader_red);
            for (int i = 0; i < player->get_path().size() - 1; i++)
                Drawing::draw_line(player->get_path()[i].first, player->get_path()[i].second,
                                   player->get_path()[i + 1].first, player->get_path()[i + 1].second);

            // Draw player
            glUseProgram(shader_green);
            Drawing::draw_circle(player->get_x(), player->get_y(), PLAYER_RADIUS);
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
                    "Depth First Search"
            };
            const char *neighborhood_graph_types[] = {
                    "Same as Graph Type",
                    "Orthogonal Grid Graph",
                    "Hexagonal Grid Graph",
                    "Orthogonal Grid Graph with Diagonals"
            };
            const char *solver_algorithms[] = {
                    "Breadth First Search"
            };

            ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
            ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);

            ImGui::Begin("Configuration", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus | ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoTitleBar);

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
                    ImGui::Separator();
                    if (ImGui::MenuItem("CZ", nullptr, !language)) {
                        language = !language;
                    }
                    if (ImGui::MenuItem("EN", nullptr, language)) {
                        language = !language;
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

            ImGui::SetWindowPos(ImVec2(0, 0));
            ImGui::SetWindowSize(ImVec2((float) WINDOW_X_OFFSET, (float) WINDOW_HEIGHT));
            ImGui::SetWindowFontScale(font_size);

            if (ImGui::InputInt("Grid Size", &GRID_SIZE, 1, 5)) {
                if (GRID_SIZE < 10) GRID_SIZE = 10;
                if (GRID_SIZE > 100) GRID_SIZE = 100;

                BLACK_LINE_WIDTH = (float) GRID_SIZE * 1.33f;
                WHITE_LINE_WIDTH = (float) GRID_SIZE * 0.5f;
                BLACK_NODE_RADIUS = 2 * BLACK_LINE_WIDTH / (float) std::max(WINDOW_WIDTH, WINDOW_HEIGHT);
                WHITE_NODE_RADIUS = 2 * WHITE_LINE_WIDTH / (float) std::max(WINDOW_WIDTH, WINDOW_HEIGHT);
                PLAYER_RADIUS = (float) GRID_SIZE * 0.5f / (float) std::max(WINDOW_WIDTH, WINDOW_HEIGHT);
                clear_button_callback();
            }

            ImGui::SeparatorText("Maze Type");
            if (ImGui::ListBox("", &maze_type, maze_types, IM_ARRAYSIZE(maze_types))) {
                int tmp = maze_type;
                clear_button_callback();
                maze_type = tmp;
            }

            if (maze_type == 0) {
                ImGui::SeparatorText("Graph Settings");
                ImGui::Combo("Base Graph", &graph_type, graph_types, IM_ARRAYSIZE(graph_types));
                ImGui::Checkbox("Non-Grid", &non_grid_version);
                ImGui::SameLine();
                help_marker("Non-Grid means that the graph appears as if it was not a grid\nEvery cell is randomly moved a little bit");

                ImGui::SeparatorText("Generator Settings");
                ImGui::Combo("Generator", &generator_algorithm, generator_algorithms, IM_ARRAYSIZE(generator_algorithms));
            }

            else if (maze_type == 1) {
                ImGui::SeparatorText("Graph Settings");
                ImGui::Combo("Base Graph", &graph_type, graph_types, IM_ARRAYSIZE(graph_types));
                ImGui::Checkbox("Non-Grid", &non_grid_version);
                ImGui::SameLine();
                help_marker("Non-Grid means that the graph appears as if it was not a grid\nEvery cell is randomly moved a little bit");
                ImGui::Combo("Neighborhood Graph", &neighborhood_graph_type, neighborhood_graph_types, IM_ARRAYSIZE(neighborhood_graph_types));
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
            ImGui::Combo("Solver", &solver_algorithm, solver_algorithms, IM_ARRAYSIZE(solver_algorithms));
//            if (ImGui::Button("Solve"))
//                solve_button_callback();
            ImGui::Checkbox("Show Solution", &show_solution);
//            ImGui::Checkbox("Show Solution from the Player", &show_solution_from_player);

            ImGui::SeparatorText("Maze Status");
            ImGui::Text("Maze");
            ImGui::SameLine();
            is_solvable ? ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(0,200,0,255)) : ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(200,0,0,255));
            is_solvable ? ImGui::Text("IS") : ImGui::Text("IS NOT");
            ImGui::PopStyleColor();
            ImGui::SameLine();
            ImGui::Text("solvable");
//            ImGui::Text("Maze");
//            ImGui::SameLine();
//            is_solvable_from_player ? ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(0,200,0,255)) : ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(200,0,0,255));
//            is_solvable_from_player ? ImGui::Text("IS") : ImGui::Text("IS NOT");
//            ImGui::PopStyleColor();
//            ImGui::SameLine();
//            ImGui::Text("solvable from the player's position");
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
            if (ImGui::Button("Reset Player"))
                reset_player_button_callback();
            if (ImGui::Button("Clear"))
                clear_button_callback();

            ImGui::End();
            ImGui::PopStyleVar(2);
        }

        {
            if (show_settings_window) {
                ImGui::Begin("Graphics Settings", &show_settings_window, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);

                ImGui::SetWindowSize(ImVec2(600, 300));

                if (ImGui::Button("Set 1280x720")) {
                    WINDOW_WIDTH = 1280;
                    WINDOW_HEIGHT = 720;
                    WINDOW_X_OFFSET = WINDOW_WIDTH - WINDOW_HEIGHT;
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
                        Drawing::transform_x_y_to_opengl(WINDOW_X_OFFSET, 0, rect_x, rect_y);
                        Drawing::transform_x_y_to_opengl(WINDOW_WIDTH, WINDOW_HEIGHT, rect_width, rect_height);
                        glfwSetWindowMonitor(window, nullptr, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, 0);
                        glfwSetWindowPos(window, (mode->width - WINDOW_WIDTH) / 2, (mode->height - WINDOW_HEIGHT) / 2);
                        clear_button_callback();
                        fullscreen = false;
                    }
                }
                static int style_idx = 0;
                if (ImGui::Combo("Style", &style_idx, "Dark\0Light\0Classic\0"))
                {
                    switch (style_idx) {
                        case 0: ImGui::StyleColorsDark(); break;
                        case 1: ImGui::StyleColorsLight(); break;
                        case 2: ImGui::StyleColorsClassic(); break;
                        default: break;
                    }
                }
                ImGui::SetWindowFontScale(font_size);
                if (ImGui::InputFloat("Font Size", &font_size, 0.1f, 0.3f, "%.1f")) {
                    if (font_size < 1.0f) font_size = 1.0f;
                    if (font_size > 2.0f) font_size = 2.0f;
                }

                ImGui::End();
            }
        }

        {
            if (show_about_window) {
                ImGui::SetNextWindowPos(ImGui::GetMainViewport()->GetCenter(), ImGuiCond_Always, ImVec2(0.5f, 0.5f));
                ImGui::SetNextWindowBgAlpha(0.35f);
                ImGui::Begin("About", &show_about_window, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoCollapse);
                ImGui::SetWindowFontScale(font_size);
                ImGui::Text("This application was made by:\nDominik Zappe");
                ImGui::Separator();
                ImGui::Text("Application serves as Bachelors Thesis Project\nfor the University of Applied Sciences\nin the field of Computer Science");
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
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}

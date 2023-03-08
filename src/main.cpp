#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <regex>
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "imgui_stdlib.h"
#include "const.h"
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
std::vector<std::pair<int, int>> solved_path;
bool show_solution = false;
std::string rulestring = "B3/S1234";
int initialize_square_size = -1;
float font_size = 1.0f;
float speed = 0.6f;

int WINDOW_WIDTH = 1280;
int WINDOW_HEIGHT = 720;
int WINDOW_X_OFFSET = WINDOW_WIDTH - WINDOW_HEIGHT;
int GRID_SIZE = 30;
float BLACK_LINE_WIDTH = GRID_SIZE * 1.33f;
float WHITE_LINE_WIDTH = GRID_SIZE * 0.5f;
float BLACK_NODE_RADIUS = 2 * BLACK_LINE_WIDTH / std::max(WINDOW_WIDTH, WINDOW_HEIGHT);
float WHITE_NODE_RADIUS = 2 * WHITE_LINE_WIDTH / std::max(WINDOW_WIDTH, WINDOW_HEIGHT);
float PLAYER_RADIUS = GRID_SIZE * 0.5f / std::max(WINDOW_WIDTH, WINDOW_HEIGHT);
bool fullscreen = false;

//void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods) {
//    // Escape exits the app
//    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
//        glfwSetWindowShouldClose(window, GLFW_TRUE);
//
//    int new_x = player->get_x();
//    int new_y = player->get_y();
//
//    if (!draw) return;
//
//    // WSAD or arrow keys move the player
//    if ((key == GLFW_KEY_W || key == GLFW_KEY_UP) && action == GLFW_PRESS)
//        new_y -= 10;
//    if ((key == GLFW_KEY_S || key == GLFW_KEY_DOWN) && action == GLFW_PRESS)
//        new_y += 10;
//    if ((key == GLFW_KEY_A || key == GLFW_KEY_LEFT) && action == GLFW_PRESS)
//        new_x -= 10;
//    if ((key == GLFW_KEY_D || key == GLFW_KEY_RIGHT) && action == GLFW_PRESS)
//        new_x += 10;
//
//    // Check if the new position is valid
//    GLubyte pixel[3];
//    glReadPixels(new_x + WINDOW_X_OFFSET, WINDOW_HEIGHT - new_y, 1, 1, GL_RGB, GL_UNSIGNED_BYTE, &pixel);
//    if (pixel[0] == 0 && pixel[1] == 0 && pixel[2] == 0) // Black == Wall
//        return;
//
//    player->move_to(new_x, new_y);
//}

void cursor_position_callback(GLFWwindow *window, double xpos, double ypos) {
    if (!draw) return;

    auto new_x = static_cast<int>(xpos);
    auto new_y = static_cast<int>(ypos);

    // Check if the new position is valid
    if (abs(new_x - (player->get_x() + WINDOW_X_OFFSET)) < GRID_SIZE * 0.5 && abs(new_y - player->get_y()) < GRID_SIZE * 0.5)
        return;

    // Check line of sight between player and cursor
    int x0 = player->get_x() + WINDOW_X_OFFSET;
    int y0 = player->get_y();
    int x1 = new_x;
    int y1 = new_y;

    int dx = abs(x1 - x0);
    int sx = x0 < x1 ? 1 : -1;
    int dy = -abs(y1 - y0);
    int sy = y0 < y1 ? 1 : -1;
    int err = dx + dy, e2;

    while (x0 != x1 || y0 != y1) {
        // Check if there is a wall in the way of line of sight
        GLubyte pixel[3];
        glReadPixels(x0, WINDOW_HEIGHT - y0, 1, 1, GL_RGB, GL_UNSIGNED_BYTE, &pixel);
        if (pixel[0] == 0 && pixel[1] == 0 && pixel[2] == 0) // Black == Wall
            return;

        // Update
        e2 = 2 * err;
        if (e2 >= dy) {
            err += dy;
            x0 += sx;
        }
        if (e2 <= dx) {
            err += dx;
            y0 += sy;
        }
    }

    player->move_to(new_x - WINDOW_X_OFFSET, new_y);
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

void reset_button_callback() {
    graph = nullptr;
    maze = nullptr;
    neighborhood = nullptr;
    ca = nullptr;
    player = nullptr;
    graph_type = 0;
    neighborhood_graph_type = 0;
    maze_type = 0;
    generator_algorithm = 0;
    solver_algorithm = 0;
    non_grid_version = false;
    size_paths = 0;
    buffer_paths = 0;
    draw = false;
    paused = false;
    is_solvable = false;
    solved_path.clear();
    show_solution = false;
    rulestring = "B3/S1234";
    initialize_square_size = -1;
    speed = 0.6f;
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

        player = std::make_unique<Player>(maze->get_nodes()[0]->get_x(), maze->get_nodes()[0]->get_y());

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

        player = std::make_unique<Player>(ca->get_graph()->get_nodes()[0]->get_x(), ca->get_graph()->get_nodes()[0]->get_y());

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
    ImGui::StyleColorsClassic();

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
    auto source_walls = Shader::parse_shader("src/graphics/shaders/walls.shader");
    auto source_green = Shader::parse_shader("src/graphics/shaders/green.shader");
    auto source_red = Shader::parse_shader("src/graphics/shaders/red.shader");
    auto source_blue = Shader::parse_shader("src/graphics/shaders/blue.shader");
    auto shader_paths = Shader::create_shader(source_paths.vertex_source, source_paths.fragment_source);
    auto shader_walls = Shader::create_shader(source_walls.vertex_source, source_walls.fragment_source);
    auto shader_green = Shader::create_shader(source_green.vertex_source, source_green.fragment_source);
    auto shader_red = Shader::create_shader(source_red.vertex_source, source_red.fragment_source);
    auto shader_blue = Shader::create_shader(source_blue.vertex_source, source_blue.fragment_source);

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

        if (draw) {
            if (!paused && maze_type == 1 && std::chrono::duration_cast<std::chrono::milliseconds>(
                    std::chrono::high_resolution_clock::now() - now).count() > (int) (1000 * (1.0f - speed))) {
                ca->next_generation();
                Drawing::buffer_graph(ca->get_graph(), size_paths, buffer_paths);
                now = std::chrono::high_resolution_clock::now();

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
            glLineWidth(BLACK_LINE_WIDTH);
            glUseProgram(shader_walls);
            glBindBuffer(GL_ARRAY_BUFFER, buffer_paths);
            glEnableVertexAttribArray(0);
            glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), 0);
            glDrawArrays(GL_LINES, 0, size_paths);
            if (maze_type == 0) {
                for (auto &node: maze->get_nodes())
                    Drawing::draw_circle(node->get_x(), node->get_y(), BLACK_NODE_RADIUS);
            }
            else if (maze_type == 1) {
                for (auto &node: ca->get_graph()->get_nodes())
                    Drawing::draw_circle(node->get_x(), node->get_y(), BLACK_NODE_RADIUS);
            }

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

            // Draw path that the player has walked
            glLineWidth(2.0f);
            glUseProgram(shader_red);
            for (int i = 0; i < player->get_path().size() - 1; i++)
                Drawing::draw_line(player->get_path()[i].first, player->get_path()[i].second,
                                   player->get_path()[i + 1].first, player->get_path()[i + 1].second);

            // Draw player
            glUseProgram(shader_green);
            Drawing::draw_circle(player->get_x(), player->get_y(), PLAYER_RADIUS);
        }

        // Check if the player has reached the end
//        if (abs(player->get_x() - maze->get_nodes()[maze->get_v() - 1]->get_x()) < 10 &&
//            abs(player->get_y() - maze->get_nodes()[maze->get_v() - 1]->get_y()) < 10) {
//            std::cout << "Maze solved!" << std::endl;
//        }

//        {
//            static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_PassthruCentralNode | ImGuiDockNodeFlags_AutoHideTabBar;
//
//            ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDocking;
//
//            ImGuiViewport* viewport = ImGui::GetMainViewport();
//            ImGui::SetNextWindowPos(viewport->Pos);
//            ImGui::SetNextWindowSize(viewport->Size);
//            ImGui::SetNextWindowViewport(viewport->ID);
//            ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
//            ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
//            window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
//            window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
//
//            if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
//                window_flags |= ImGuiWindowFlags_NoBackground ;
//
//            ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
//            ImGui::Begin("DockSpace", nullptr, window_flags);
//            ImGui::PopStyleVar();
//            ImGui::PopStyleVar(2);
//
//            ImGuiIO& io = ImGui::GetIO();
//            if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable) {
//                ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
//                ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
//
//                static auto first_time = true;
//                if (first_time) {
//                    first_time = false;
//
//                    ImGui::DockBuilderRemoveNode(dockspace_id); // clear any previous layout
//                    ImGui::DockBuilderAddNode(dockspace_id, dockspace_flags | ImGuiDockNodeFlags_DockSpace);
//                    ImGui::DockBuilderSetNodeSize(dockspace_id, viewport->Size);
//
//                    auto dock_id_left = ImGui::DockBuilderSplitNode(dockspace_id, ImGuiDir_Left, (float) (WINDOW_WIDTH - WINDOW_HEIGHT - static_cast<int>(GRID_SIZE / 2)) / WINDOW_WIDTH, nullptr, &dockspace_id);
//
//                    ImGui::DockBuilderDockWindow("Configuration", dock_id_left);
//                    ImGui::DockBuilderDockWindow("Okno 2", dock_id_left);
//                    ImGui::DockBuilderFinish(dockspace_id);
//                }
//            }
//            ImGui::End();
//
//            {
//                ImGui::PushStyleVar(ImGuiStyleVar_WindowMinSize, ImVec2((WINDOW_WIDTH - WINDOW_HEIGHT - static_cast<int>(GRID_SIZE / 2)), -1));
//                ImGui::Begin("Configuration");
//                ImGui::SliderFloat("Speed", &speed, 0.0f, 1.0f);
//                ImGui::Checkbox("Paused", &paused);
//                ImGui::Checkbox("Show Solution", &show_solution);
//                is_solvable ? ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(0,200,0,255)) : ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(200,0,0,255));
//                is_solvable ? ImGui::Text("Maze IS solvable") : ImGui::Text("Maze IS NOT solvable");
//                ImGui::End();
//            }
//            {
//                ImGui::Begin("Okno 2");
//                ImGui::Text("Hello, world!");
//                ImGui::End();
//            }
//        }

        {
            const char *maze_types[] = {
                    "Normal",
                    "Changing in Time"
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

            ImGui::Begin("Configuration", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus);

            ImGui::SetWindowPos(ImVec2(0, 0));
            ImGui::SetWindowSize(ImVec2((float) (WINDOW_WIDTH - WINDOW_HEIGHT - (float) GRID_SIZE / 2), WINDOW_HEIGHT));

            ImGui::SeparatorText("Graphics Settings");
            if (ImGui::Button("Set 1280x720")) {
                WINDOW_WIDTH = 1280;
                WINDOW_HEIGHT = 720;
                WINDOW_X_OFFSET = WINDOW_WIDTH - WINDOW_HEIGHT;
                glfwSetWindowSize(window, WINDOW_WIDTH, WINDOW_HEIGHT);
                glfwSetWindowPos(window, (mode->width - WINDOW_WIDTH) / 2, (mode->height - WINDOW_HEIGHT) / 2);
            }
            ImGui::SameLine();
            if (ImGui::Button("Set 1600x900")) {
                WINDOW_WIDTH = 1600;
                WINDOW_HEIGHT = 900;
                WINDOW_X_OFFSET = WINDOW_WIDTH - WINDOW_HEIGHT;
                glfwSetWindowSize(window, WINDOW_WIDTH, WINDOW_HEIGHT);
                glfwSetWindowPos(window, (mode->width - WINDOW_WIDTH) / 2, (mode->height - WINDOW_HEIGHT) / 2);
            }
            ImGui::SameLine();
            if (!fullscreen) {
                if (ImGui::Button("Fullscreen")) {
                    WINDOW_WIDTH = mode->width;
                    WINDOW_HEIGHT = mode->height;
                    WINDOW_X_OFFSET = WINDOW_WIDTH - WINDOW_HEIGHT;
                    glfwSetWindowMonitor(window, glfwGetPrimaryMonitor(), 0, 0, mode->width, mode->height, mode->refreshRate);
                    fullscreen = true;
                }
            }
            else {
                if (ImGui::Button("Windowed")) {
                    WINDOW_WIDTH = 1280;
                    WINDOW_HEIGHT = 720;
                    WINDOW_X_OFFSET = WINDOW_WIDTH - WINDOW_HEIGHT;
                    glfwSetWindowMonitor(window, nullptr, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, 0);
                    glfwSetWindowPos(window, (mode->width - WINDOW_WIDTH) / 2, (mode->height - WINDOW_HEIGHT) / 2);
                    fullscreen = false;
                }
            }
            static int style_idx = 2;
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
            ImGui::InputFloat("Font Size", &font_size, 0.1f, 0.3f, "%.1f");
            if (ImGui::InputInt("Grid Size", &GRID_SIZE, 1, 5)) {
                BLACK_LINE_WIDTH = GRID_SIZE * 1.33f;
                WHITE_LINE_WIDTH = GRID_SIZE * 0.5f;
                BLACK_NODE_RADIUS = 2 * BLACK_LINE_WIDTH / std::max(WINDOW_WIDTH, WINDOW_HEIGHT);
                WHITE_NODE_RADIUS = 2 * WHITE_LINE_WIDTH / std::max(WINDOW_WIDTH, WINDOW_HEIGHT);
                PLAYER_RADIUS = GRID_SIZE * 0.5f / std::max(WINDOW_WIDTH, WINDOW_HEIGHT);
                reset_button_callback();
            }

            ImGui::SeparatorText("Maze Type");
            if (ImGui::ListBox("", &maze_type, maze_types, IM_ARRAYSIZE(maze_types))) {
                int tmp = maze_type;
                reset_button_callback();
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

                ImGui::SeparatorText("Cellular Automata Settings");
                ImGui::InputText("Rulestring", &rulestring);
                ImGui::SameLine();
                help_marker("Rulestring format is B[0-9]+/S[0-9]+\nB is birth rule\nS is survival rule\nExample: B3/S23");
                ImGui::InputInt("Initial Square Size", &initialize_square_size);
                ImGui::SameLine();
                help_marker("Size of square of cells that is used to initialize the cellular automata\nInitial state is random\n-1 means that the whole grid is used");
                ImGui::SliderFloat("Speed", &speed, 0.0f, 1.0f);
                ImGui::Checkbox("Paused", &paused);
            }

            ImGui::SeparatorText("Solver Settings");
            ImGui::Combo("Solver", &solver_algorithm, solver_algorithms, IM_ARRAYSIZE(solver_algorithms));
//            if (ImGui::Button("Solve"))
//                solve_button_callback();
            ImGui::Checkbox("Show Solution", &show_solution);

            ImGui::SeparatorText("Maze Status");
            is_solvable ? ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(0,200,0,255)) : ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(200,0,0,255));
            is_solvable ? ImGui::Text("Maze IS solvable") : ImGui::Text("Maze IS NOT solvable");
            ImGui::PopStyleColor();

            ImGui::SeparatorText("Controls");
            if (ImGui::Button("Generate"))
                generate_button_callback();
            ImGui::SameLine();
            if (ImGui::Button("Reset"))
                reset_button_callback();

            ImGui::End();
            ImGui::PopStyleVar(2);
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

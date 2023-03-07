#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "const.h"
#include "graphics/shader.h"
#include "graphics/drawing.h"
#include "maze/generator.h"
#include "maze/solver.h"
#include "maze/cellular_automata.h"
#include "player.h"

std::unique_ptr<Player> player;
bool paused = false;
bool show_solution = false;

void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods) {
    // Escape exits the app
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);

    int new_x = player->get_x();
    int new_y = player->get_y();

    // WSAD or arrow keys move the player
    if ((key == GLFW_KEY_W || key == GLFW_KEY_UP) && action == GLFW_PRESS)
        new_y -= 10;
    if ((key == GLFW_KEY_S || key == GLFW_KEY_DOWN) && action == GLFW_PRESS)
        new_y += 10;
    if ((key == GLFW_KEY_A || key == GLFW_KEY_LEFT) && action == GLFW_PRESS)
        new_x -= 10;
    if ((key == GLFW_KEY_D || key == GLFW_KEY_RIGHT) && action == GLFW_PRESS)
        new_x += 10;

    // Spaces for pausing the animation
    if ((key == GLFW_KEY_SPACE) && action == GLFW_PRESS)
        paused = !paused;

    // Enter for showing the solution
    if ((key == GLFW_KEY_ENTER) && action == GLFW_PRESS)
        show_solution = !show_solution;

    // Check if the new position is valid
    GLubyte pixel[3];
    glReadPixels(new_x , WINDOW_HEIGHT - new_y, 1, 1, GL_RGB, GL_UNSIGNED_BYTE, &pixel);
    if (pixel[0] == 0 && pixel[1] == 0 && pixel[2] == 0) // Black == Wall
        return;

    player->move_to(new_x, new_y);
}

void cursor_position_callback(GLFWwindow *window, double xpos, double ypos) {
    auto new_x = static_cast<int>(xpos);
    auto new_y = static_cast<int>(ypos);

    // Check if the new position is valid
    if (abs(new_x - player->get_x()) < 8 && abs(new_y - player->get_y()) < 8)
        return;

    // Check line of sight between player and cursor
    int x0 = player->get_x();
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

    player->move_to(new_x, new_y);
}

static void glfw_error_callback(int error, const char* description) {
    fprintf(stderr, "GLFW Error %d: %s\n", error, description);
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
    glfwSetWindowPos(window, (1920 - WINDOW_WIDTH) / 2, (1080 - WINDOW_HEIGHT) / 2);

    GLenum err = glewInit();
    if (err != GLEW_OK) {
        // Problem: glewInit failed, something is seriously wrong
        std::cerr << "Error: " << glewGetErrorString(err) << std::endl;
    }

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows
    //io.ConfigViewportsNoAutoMerge = true;
    //io.ConfigViewportsNoTaskBarIcon = true;

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();

    // When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
    ImGuiStyle& style = ImGui::GetStyle();
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        style.WindowRounding = 0.0f;
        style.Colors[ImGuiCol_WindowBg].w = 1.0f;
    }

    // Print out some info about the graphics drivers
    std::cout << "OpenGL version: " << glGetString(GL_VERSION) << std::endl;
    std::cout << "GLEW version: " << glewGetString(GLEW_VERSION) << std::endl;

    // Set callbacks for user inputs
    glfwSetKeyCallback(window, key_callback);
    glfwSetCursorPosCallback(window, cursor_position_callback);

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    // White background
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

    // Generate maze
    auto graph = Generator::create_hexagonal_grid_graph(WINDOW_WIDTH / GRID_SIZE - 1, WINDOW_HEIGHT / GRID_SIZE - 1, true);
//    auto graph = Generator::create_orthogonal_grid_graph(WINDOW_WIDTH / GRID_SIZE - 1, WINDOW_HEIGHT / GRID_SIZE - 1, false);
//    auto maze = Generator::generate_maze_dfs(graph);
//    player = std::make_unique<Player>(maze->get_nodes()[0]->get_x(), maze->get_nodes()[0]->get_y());
//    auto neighbourhood = Generator::create_orthogonal_grid_graph_laplacian(WINDOW_WIDTH / GRID_SIZE - 1, WINDOW_HEIGHT / GRID_SIZE - 1);

    // Orthogonal grid graph
    // B3/S1234
    // B37/S1234
    // B3/S12345
    // B7/S12345

    // Hexagonal grid graph
    // B24/S12345
    std::shared_ptr<CellularAutomata> ca = std::make_shared<CellularAutomata>("B24/S1234", graph, nullptr, 10);
    player = std::make_unique<Player>(ca->get_graph()->get_nodes()[0]->get_x(), ca->get_graph()->get_nodes()[0]->get_y());

    // Solve maze
//    auto is_solvable = Solver::is_maze_solvable_bfs(maze, {maze->get_nodes()[0]->get_x(), maze->get_nodes()[0]->get_y()},
//                                                    {maze->get_nodes()[maze->get_nodes().size() - 1]->get_x(),
//                                                     maze->get_nodes()[maze->get_nodes().size() - 1]->get_y()});
    auto is_solvable = Solver::is_maze_solvable_bfs(ca->get_graph(), {ca->get_graph()->get_nodes()[0]->get_x(), ca->get_graph()->get_nodes()[0]->get_y()},
                                                   {ca->get_graph()->get_nodes()[ca->get_graph()->get_nodes().size() - 1]->get_x(),
                                                    ca->get_graph()->get_nodes()[ca->get_graph()->get_nodes().size() - 1]->get_y()});
    std::cout << "Is maze solvable: " << (is_solvable ? "Yes" : "No") << std::endl;

//    auto solved_path = Solver::solve_maze_bfs(maze, {maze->get_nodes()[0]->get_x(), maze->get_nodes()[0]->get_y()},
//                                             {maze->get_nodes()[maze->get_nodes().size() - 1]->get_x(),
//                                              maze->get_nodes()[maze->get_nodes().size() - 1]->get_y()});
    auto solved_path = Solver::solve_maze_bfs(ca->get_graph(), {ca->get_graph()->get_nodes()[0]->get_x(), ca->get_graph()->get_nodes()[0]->get_y()},
                                             {ca->get_graph()->get_nodes()[ca->get_graph()->get_nodes().size() - 1]->get_x(),
                                              ca->get_graph()->get_nodes()[ca->get_graph()->get_nodes().size() - 1]->get_y()});

    // Buffer maze
    int size_paths, buffer_paths;
//    Drawing::buffer_graph(maze, size_paths, buffer_paths);

    Drawing::buffer_graph(ca->get_graph(), size_paths, buffer_paths);

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
    float speed = 0.0f;

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

        if (!paused && std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - now).count() > (int) (1000 * (1.0f - speed))) {
            ca->next_generation();
            Drawing::buffer_graph(ca->get_graph(), size_paths, buffer_paths);
            now = std::chrono::high_resolution_clock::now();

            is_solvable = Solver::is_maze_solvable_bfs(ca->get_graph(), {ca->get_graph()->get_nodes()[0]->get_x(), ca->get_graph()->get_nodes()[0]->get_y()},
                                                       {ca->get_graph()->get_nodes()[ca->get_graph()->get_nodes().size() - 1]->get_x(),
                                                        ca->get_graph()->get_nodes()[ca->get_graph()->get_nodes().size() - 1]->get_y()});
            std::cout << "Is maze solvable: " << (is_solvable ? "Yes" : "No") << std::endl;
            solved_path = Solver::solve_maze_bfs(ca->get_graph(), {ca->get_graph()->get_nodes()[0]->get_x(), ca->get_graph()->get_nodes()[0]->get_y()},
                                                 {ca->get_graph()->get_nodes()[ca->get_graph()->get_nodes().size() - 1]->get_x(),
                                                  ca->get_graph()->get_nodes()[ca->get_graph()->get_nodes().size() - 1]->get_y()});
        }

        // Draw maze
        glLineWidth(BLACK_LINE_WIDTH);
        glUseProgram(shader_walls);
        glBindBuffer(GL_ARRAY_BUFFER, buffer_paths);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), 0);
        glDrawArrays(GL_LINES, 0, size_paths);
//        for (auto &node: maze->get_nodes())
//            Drawing::draw_circle(node->get_x(), node->get_y(), BLACK_NODE_RADIUS);
        for (auto &node: ca->get_graph()->get_nodes())
            Drawing::draw_circle(node->get_x(), node->get_y(), BLACK_NODE_RADIUS);

        glLineWidth(WHITE_LINE_WIDTH);
        glUseProgram(shader_paths);
        glBindBuffer(GL_ARRAY_BUFFER, buffer_paths);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), 0);
        glDrawArrays(GL_LINES, 0, size_paths);
//        for (auto &node: maze->get_nodes())
//            Drawing::draw_circle(node->get_x(), node->get_y(), WHITE_NODE_RADIUS);
        for (auto &node: ca->get_graph()->get_nodes())
            if (node->is_alive())
                Drawing::draw_circle(node->get_x(), node->get_y(), WHITE_NODE_RADIUS);

        // Color the start and end nodes
        glUseProgram(shader_blue);
//        Drawing::draw_circle(maze->get_nodes()[0]->get_x(), maze->get_nodes()[0]->get_y(), PLAYER_RADIUS * 1.5f);
//        Drawing::draw_circle(maze->get_nodes()[maze->get_v() - 1]->get_x(), maze->get_nodes()[maze->get_v() - 1]->get_y(), PLAYER_RADIUS * 1.5f);
        Drawing::draw_circle(ca->get_graph()->get_nodes()[0]->get_x(), ca->get_graph()->get_nodes()[0]->get_y(), PLAYER_RADIUS * 1.5f);
        Drawing::draw_circle(ca->get_graph()->get_nodes()[ca->get_graph()->get_v() - 1]->get_x(), ca->get_graph()->get_nodes()[ca->get_graph()->get_v() - 1]->get_y(), PLAYER_RADIUS * 1.5f);

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

        // Check if the player has reached the end
//        if (abs(player->get_x() - maze->get_nodes()[maze->get_v() - 1]->get_x()) < 10 &&
//            abs(player->get_y() - maze->get_nodes()[maze->get_v() - 1]->get_y()) < 10) {
//            std::cout << "Maze solved!" << std::endl;
//        }

        {
            ImGui::Begin("Config Window");
            ImGui::SliderFloat("Speed", &speed, 0.0f, 1.0f);
            ImGui::Checkbox("Paused", &paused);      // Edit bools storing our window open/close state
            ImGui::Checkbox("Show Solution", &show_solution);
            ImGui::End();
        }

        // ImGui Rendering
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        // Swap front and back buffers
        glfwSwapBuffers(window);
        glfwSwapInterval(1); // Enable vsync
    }

    glfwTerminate();
    return 0;
}

/**
 * @mainpage Maze Solver Visualization
 *
 * @section intro_sec Introduction
 * This project demonstrates maze generation and solving with an animated
 * visualization built using C++ and the SFML graphics library.
 *
 * @section algo_sec Algorithms
 * - Maze generation: Recursive Backtracking (Depth-First Search)
 * - Maze solving: Breadth-First Search (BFS)
 *
 * @section viz_sec Visualization
 * - Maze walls are drawn once into a static layer.
 * - Visited cells are shown in light blue as the solver explores.
 * - The final shortest path is shown in red once discovered.
 *
 * @section deps_sec Dependencies
 * - C++17 or newer
 * - SFML 3.0+ (graphics, window, system)
 * - Dear ImGui + ImGui-SFML (settings UI)
 *
 * @section usage_sec Usage
 * Build and run the program. Configure maze size in the settings panel,
 * then click Generate. Press R to instantly regenerate with the same settings,
 * or Esc to return to the settings panel.
 */

/**
 * @file App.cpp
 * @brief Maze generation and solver visualization using SFML and Dear ImGui.
 */

#include "Maze.hpp"
#include "Solver.hpp"
#include <SFML/Graphics.hpp>
#include <imgui.h>
#include <imgui-SFML.h>
#include <memory>

int main() {
    // ---------------------------------------------------------------------
    // Settings (configurable via ImGui panel)
    // ---------------------------------------------------------------------
    int cols = 201;
    int rows = 151;

    // ---------------------------------------------------------------------
    // Scale all UI dimensions relative to 1080p
    // ---------------------------------------------------------------------
    const float scale = std::max(0.5f, std::min(
        (float)sf::VideoMode::getDesktopMode().size.y / 1080.f, 3.0f
    ));

    const int   taskbarHeight = (int)(80  * scale);
    const int   uiW           = (int)(400 * scale);
    const int   uiH           = (int)(285 * scale);
    const float uiLabelW      = std::round(120.f * scale);
    const float uiSliderW     = std::round(200.f * scale);
    const float uiBtnW        = std::round(120.f * scale);
    const float uiBtnH        = std::round(32.f  * scale);

    // ---------------------------------------------------------------------
    // Initial window - sized to settings panel only, resized on generate
    // ---------------------------------------------------------------------
    sf::RenderWindow window(
        sf::VideoMode({ (unsigned)uiW, (unsigned)uiH }),
        "Maze Solver - Settings"
    );
    window.setFramerateLimit(60);

    // Center window on screen, leaving room for the taskbar at the bottom
    auto centerWindow = [&](int w, int h) {
        auto desktop = sf::VideoMode::getDesktopMode();
        int x = ((int)desktop.size.x - w) / 2;
        int y = ((int)desktop.size.y - taskbarHeight - h) / 2;
        window.setPosition(sf::Vector2i(std::max(0, x), std::max(0, y)));
    };

    centerWindow(uiW, uiH);

    (void)ImGui::SFML::Init(window);

    // ---------------------------------------------------------------------
    // Font - Segoe UI, fall back to ImGui default if unavailable
    // ---------------------------------------------------------------------
    {
        ImGuiIO& io = ImGui::GetIO();
        (void)io.Fonts->Clear();
        ImFont* font = io.Fonts->AddFontFromFileTTF("C:/Windows/Fonts/consola.ttf", std::round(17.f * scale));
        if (font) (void)ImGui::SFML::UpdateFontTexture();
    }

    // ---------------------------------------------------------------------
    // Style - dark theme matching maze colors
    // Palette:
    //   bg         #141414  (maze wall black)
    //   frame      #282828  (slightly lighter)
    //   accent     #96C8FF  (150,200,255 - visited cell blue)
    //   btn        #3D6FA0  (deeper blue for resting button)
    //   text       #FFFFFF  (open cell white)
    // ---------------------------------------------------------------------
    {
        ImGui::StyleColorsDark();
        auto& s = ImGui::GetStyle();

        s.WindowRounding    = 0.0f;
        s.FrameRounding     = 0.0f;
        s.GrabRounding      = 0.0f;
        s.ScrollbarRounding = 0.0f;
        s.TabRounding       = 0.0f;
        s.PopupRounding     = 0.0f;
        s.ChildRounding     = 0.0f;
        s.WindowBorderSize  = 0.0f;
        s.FrameBorderSize   = 0.0f;
        s.WindowPadding     = ImVec2(std::round(18.f * scale), std::round(18.f * scale));
        s.FramePadding      = ImVec2(std::round(8.f  * scale), std::round(5.f  * scale));
        s.ItemSpacing       = ImVec2(std::round(10.f * scale), std::round(10.f * scale));
        s.GrabMinSize       = std::round(10.f * scale);

        ImVec4* c = s.Colors;

        // Background / panels
        c[ImGuiCol_WindowBg]         = ImVec4(0.08f, 0.08f, 0.08f, 1.00f);
        c[ImGuiCol_PopupBg]          = ImVec4(0.08f, 0.08f, 0.08f, 1.00f);

        // Text
        c[ImGuiCol_Text]             = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
        c[ImGuiCol_TextDisabled]     = ImVec4(0.45f, 0.55f, 0.65f, 1.00f);

        // Frames (slider bg, input bg)
        c[ImGuiCol_FrameBg]          = ImVec4(0.16f, 0.16f, 0.16f, 1.00f);
        c[ImGuiCol_FrameBgHovered]   = ImVec4(0.22f, 0.22f, 0.22f, 1.00f);
        c[ImGuiCol_FrameBgActive]    = ImVec4(0.28f, 0.28f, 0.28f, 1.00f);

        // Separator / border
        c[ImGuiCol_Separator]        = ImVec4(0.22f, 0.22f, 0.22f, 1.00f);
        c[ImGuiCol_SeparatorHovered] = ImVec4(0.59f, 0.78f, 1.00f, 1.00f);
        c[ImGuiCol_SeparatorActive]  = ImVec4(0.59f, 0.78f, 1.00f, 1.00f);

        // Slider grab - visited cell blue
        c[ImGuiCol_SliderGrab]       = ImVec4(0.45f, 0.65f, 0.90f, 1.00f);
        c[ImGuiCol_SliderGrabActive] = ImVec4(0.59f, 0.78f, 1.00f, 1.00f);

        // Scrollbar
        c[ImGuiCol_ScrollbarBg]      = ImVec4(0.10f, 0.10f, 0.10f, 1.00f);
        c[ImGuiCol_ScrollbarGrab]    = ImVec4(0.45f, 0.65f, 0.90f, 1.00f);
        c[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.59f, 0.78f, 1.00f, 1.00f);
        c[ImGuiCol_ScrollbarGrabActive]  = ImVec4(0.59f, 0.78f, 1.00f, 1.00f);

        // Buttons - resting is a muted blue, hover/active rise to visited-cell blue
        c[ImGuiCol_Button]           = ImVec4(0.24f, 0.44f, 0.63f, 1.00f);
        c[ImGuiCol_ButtonHovered]    = ImVec4(0.39f, 0.60f, 0.86f, 1.00f);
        c[ImGuiCol_ButtonActive]     = ImVec4(0.59f, 0.78f, 1.00f, 1.00f);

        // Headers
        c[ImGuiCol_Header]           = ImVec4(0.24f, 0.44f, 0.63f, 1.00f);
        c[ImGuiCol_HeaderHovered]    = ImVec4(0.39f, 0.60f, 0.86f, 1.00f);
        c[ImGuiCol_HeaderActive]     = ImVec4(0.59f, 0.78f, 1.00f, 1.00f);

        // Check mark
        c[ImGuiCol_CheckMark]        = ImVec4(0.59f, 0.78f, 1.00f, 1.00f);
    }

    // ---------------------------------------------------------------------
    // App state
    // ---------------------------------------------------------------------
    enum class AppState { Settings, Running };
    AppState state = AppState::Settings;

    std::unique_ptr<Maze>   maze;
    std::unique_ptr<Solver> solver;

    sf::RenderTexture            staticLayer;
    sf::RenderTexture            dynamicLayer;
    std::optional<sf::Sprite>    mazeBackground;
    std::optional<sf::Sprite>    dynamicOverlay;

    sf::Clock animClock;
    float stepMs = 0.5f;
    std::size_t visitedIndex = 0;
    std::size_t pathIndex    = 0;
    std::shared_ptr<const std::vector<Cell*>> pathPtr;

    enum class Phase { Visiting, Path };
    Phase phase = Phase::Visiting;

    sf::Clock deltaClock; // for ImGui::SFML::Update

    // ---------------------------------------------------------------------
    // Lambda: start a new maze with current settings
    // ---------------------------------------------------------------------
    auto generate = [&]() {
        if (solver) { solver->join(); solver.reset(); }

        auto desktop = sf::VideoMode::getDesktopMode();
        const int cellSize = std::max(1, std::min(
            (int)desktop.size.x / cols,
            ((int)desktop.size.y - taskbarHeight) / rows
        ));

        maze = std::make_unique<Maze>(cols, rows, cellSize);
        maze->generate();

        const unsigned int w = cols * cellSize;
        const unsigned int h = rows * cellSize;

        (void)staticLayer.resize({ w, h });
        staticLayer.clear(sf::Color::White);
        maze->draw(staticLayer);
        staticLayer.display();
        mazeBackground.emplace(staticLayer.getTexture());

        (void)dynamicLayer.resize({ w, h });
        dynamicLayer.clear(sf::Color::Transparent);
        dynamicLayer.display();
        dynamicOverlay.emplace(dynamicLayer.getTexture());

        visitedIndex = 0;
        pathIndex    = 0;
        pathPtr      = nullptr;
        phase        = Phase::Visiting;
        animClock.restart();

        solver = std::make_unique<Solver>(*maze);
        solver->start();
        window.setTitle("Maze Solver - Solving...");

        window.setSize({ w, h });
        window.setView(sf::View(sf::FloatRect({ 0.f, 0.f }, { (float)w, (float)h })));
        centerWindow((int)w, (int)h);

        state = AppState::Running;
    };

    // ---------------------------------------------------------------------
    // Main loop
    // ---------------------------------------------------------------------
    while (window.isOpen()) {
        while (const std::optional event = window.pollEvent()) {
            ImGui::SFML::ProcessEvent(window, *event);

            if (event->is<sf::Event::Closed>())
                window.close();

            if (const auto* key = event->getIf<sf::Event::KeyPressed>()) {
                if (state == AppState::Settings) {
                    if (key->code == sf::Keyboard::Key::Enter)
                        generate();
                }
                else if (state == AppState::Running) {
                    if (key->code == sf::Keyboard::Key::R)
                        generate();
                    else if (key->code == sf::Keyboard::Key::Escape) {
                        window.setSize({ (unsigned)uiW, (unsigned)uiH });
                        window.setView(sf::View(sf::FloatRect({ 0.f, 0.f }, { (float)uiW, (float)uiH })));
                        centerWindow(uiW, uiH);
                        window.setTitle("Maze Solver - Settings");
                        state = AppState::Settings;
                    }
                }
            }
        }

        ImGui::SFML::Update(window, deltaClock.restart());

        // ------------------------------------------------------------------
        // Settings panel
        // ------------------------------------------------------------------
        if (state == AppState::Settings) {
            ImGui::SetNextWindowPos({ 0, 0 });
            ImGui::SetNextWindowSize({ (float)uiW, (float)uiH });
            ImGui::Begin("Maze Settings", nullptr,
                ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
                ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar |
                ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);

            ImGui::Text("Maze Solver");
            ImGui::Separator();
            ImGui::Spacing();

            ImGui::Text("Columns"); ImGui::SameLine(uiLabelW);
            ImGui::SetNextItemWidth(uiSliderW);
            ImGui::SliderInt("##cols", &cols, 21, 601);

            ImGui::Text("Rows"); ImGui::SameLine(uiLabelW);
            ImGui::SetNextItemWidth(uiSliderW);
            ImGui::SliderInt("##rows", &rows, 21, 401);

            ImGui::Text("Speed"); ImGui::SameLine(uiLabelW);
            ImGui::SetNextItemWidth(uiSliderW);
            ImGui::SliderFloat("##speed", &stepMs, 0.1f, 20.0f, "%.1f ms/step");

            // Ensure odd values (maze algorithm requires it)
            if (cols % 2 == 0) cols++;
            if (rows % 2 == 0) rows++;

            // Preview computed cell size and window resolution
            {
                auto desktop = sf::VideoMode::getDesktopMode();
                int preview = std::max(1, std::min((int)desktop.size.x / cols, ((int)desktop.size.y - taskbarHeight) / rows));
                ImGui::Spacing();
                ImGui::TextDisabled("Cell size: %dpx  |  Window: %d x %d", preview, cols * preview, rows * preview);
            }
            ImGui::Spacing();

            if (ImGui::Button("Generate", { uiBtnW, uiBtnH }))
                generate();

            ImGui::End();

            window.clear(sf::Color(30, 30, 30));
            ImGui::SFML::Render(window);
            window.display();
            continue;
        }

        // ------------------------------------------------------------------
        // Running: animate solver - batch steps to stay frame-rate independent
        // ------------------------------------------------------------------
        if (phase == Phase::Visiting) {
            auto currentVisited = solver->getVisited();
            int steps = std::max(1, (int)(animClock.getElapsedTime().asMilliseconds() / stepMs));
            bool drew = false;
            for (int i = 0; i < steps && visitedIndex < currentVisited.size(); ++i) {
                Cell* c = currentVisited[visitedIndex++];
                c->shape.setFillColor(sf::Color(150, 200, 255));
                c->shape.setOutlineColor(sf::Color(150, 200, 255));
                dynamicLayer.draw(c->shape);
                drew = true;
            }
            if (drew) { dynamicLayer.display(); animClock.restart(); }

            if (visitedIndex >= currentVisited.size() && solver->isFinished() && solver->foundPath()) {
                pathPtr = solver->getPath();
                pathIndex = 0;
                phase = Phase::Path;
                animClock.restart();
            }
            else if (visitedIndex >= currentVisited.size() && solver->isFinished() && !solver->foundPath()) {
                window.setTitle("Maze Solver - No path found!");
            }
        }
        else if (phase == Phase::Path && pathPtr) {
            int steps = std::max(1, (int)(animClock.getElapsedTime().asMilliseconds() / stepMs));
            bool drew = false;
            for (int i = 0; i < steps && pathIndex < pathPtr->size(); ++i) {
                Cell* c = (*pathPtr)[pathIndex++];
                c->shape.setFillColor(sf::Color::Red);
                c->shape.setOutlineColor(sf::Color::Red);
                dynamicLayer.draw(c->shape);
                drew = true;
            }
            if (drew) { dynamicLayer.display(); animClock.restart(); }
            if (pathIndex >= pathPtr->size())
                window.setTitle("Maze Solver - Done!");
        }

        window.clear();
        window.draw(*mazeBackground);
        window.draw(*dynamicOverlay);
        ImGui::SFML::Render(window);
        window.display();
    }

    if (solver) solver->join();
    ImGui::SFML::Shutdown();
    return 0;
}

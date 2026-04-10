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
 * then click Generate. Press R at any time to open settings and regenerate.
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
    int cols     = 201;
    int rows     = 151;
    int cellSize = 5;

    // ---------------------------------------------------------------------
    // Initial window — sized to settings panel only, resized on generate
    // ---------------------------------------------------------------------
    sf::RenderWindow window(
        sf::VideoMode({ 400, 220 }),
        "Maze Solver"
    );
    window.setFramerateLimit(60);
    (void)ImGui::SFML::Init(window);

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
    const float stepMs = 0.5f;
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

        window.setSize({ w, h });
        window.setView(sf::View(sf::FloatRect({ 0.f, 0.f }, { (float)w, (float)h })));

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
                if (key->code == sf::Keyboard::Key::R && state == AppState::Running) {
                    window.setSize({ 400, 220 });
                    window.setView(sf::View(sf::FloatRect({ 0.f, 0.f }, { 400.f, 220.f })));
                    state = AppState::Settings;
                }
            }
        }

        ImGui::SFML::Update(window, deltaClock.restart());

        // ------------------------------------------------------------------
        // Settings panel
        // ------------------------------------------------------------------
        if (state == AppState::Settings) {
            ImGui::SetNextWindowPos({ 0, 0 });
            ImGui::SetNextWindowSize({ 400, 220 });
            ImGui::Begin("Maze Settings", nullptr,
                ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
                ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar);

            ImGui::Text("Maze Solver");
            ImGui::Separator();
            ImGui::Spacing();

            ImGui::Text("Columns"); ImGui::SameLine(120);
            ImGui::SetNextItemWidth(200);
            ImGui::SliderInt("##cols", &cols, 21, 601);

            ImGui::Text("Rows"); ImGui::SameLine(120);
            ImGui::SetNextItemWidth(200);
            ImGui::SliderInt("##rows", &rows, 21, 401);

            ImGui::Text("Cell size"); ImGui::SameLine(120);
            ImGui::SetNextItemWidth(200);
            ImGui::SliderInt("##cell", &cellSize, 2, 12);

            // Ensure odd values (maze algorithm requires it)
            if (cols % 2 == 0) cols++;
            if (rows % 2 == 0) rows++;

            ImGui::Spacing();
            ImGui::Text("Window: %d x %d px", cols * cellSize, rows * cellSize);
            ImGui::Spacing();

            if (ImGui::Button("Generate", { 120, 32 }))
                generate();

            ImGui::End();

            window.clear(sf::Color(30, 30, 30));
            ImGui::SFML::Render(window);
            window.display();
            continue;
        }

        // ------------------------------------------------------------------
        // Running: animate solver — batch steps to stay frame-rate independent
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

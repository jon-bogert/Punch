#include "pch.h"

#include "Application.h"

int main()
{
    sf::RenderWindow window(sf::VideoMode(Application::GetDimensions().x, Application::GetDimensions().y), Application::GetTitle(), sf::Style::Close);
    window.setFramerateLimit(60);
    ImGui::SFML::Init(window);

    Application::Start();

    sf::Clock deltaClock;
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            ImGui::SFML::ProcessEvent(window, event);

            if (event.type == sf::Event::Closed) {
                window.close();
            }
        }
        sf::Time deltaTime = deltaClock.restart();
        Application::Update(deltaTime.asSeconds());
        ImGui::SFML::Update(window, deltaTime);

        ImGui::SetNextWindowPos(ImVec2(0.0f, 0.0f));
        ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize);
        ImGui::Begin("MainWindow", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoResize);
        Application::OnGui();
        ImGui::End();

        window.clear();
        ImGui::SFML::Render(window);
        window.display();
    }
    Application::Shutdown();
    ImGui::SFML::Shutdown();
}
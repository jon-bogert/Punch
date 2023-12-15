#pragma once

class Application final
{
    static Application& Get() { static Application instance; return instance; }
    Application() {}

    std::string m_title = "Punch";
    sf::Vector2i m_dimensions = { 800, 1000 };

public:

    ~Application() = default;
    Application(const Application& other) = delete;
    Application(const Application&& other) = delete;
    Application operator=(const Application& other) = delete;
    Application operator=(const Application&& other) = delete;

    static void Start();
    static void Update(const float deltaTime);
    static void OnGui();
    static void Shutdown();

    static std::string& GetTitle() { return Get().m_title; }
    static sf::Vector2i& GetDimensions() { return Get().m_dimensions; }
};

#pragma once
#include "Project.h"

class Application final
{
    enum class Page { Start, Project };
    struct Data
    {
        std::string lastDirectory = "";
    };

    static Application& Get() { static Application instance; return instance; }
    Application() {}

    std::string m_title = "Punch";
    sf::Vector2i m_dimensions = { 800, 1000 };
    std::string m_dataPath = _APPDATA_ + "/Punch";
    Data m_data;

    std::deque<std::string> m_projHistory;
    Page m_page = Page::Start;

    std::unique_ptr<Project> m_activeProject = nullptr;

    //Start Page
    int m_startSelectionIndex = -1;

    inline static int MAX_HISTORY = 10;

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

private:
    void LoadProjectHistory();
    void SaveProjectHistory();
    void LoadData();
    void SaveData();

    void StartPage();
    void AddHistory(const std::string& file);
    std::string PathNoFile(const std::string& path);
};

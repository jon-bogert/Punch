#include "pch.h"
#include "Application.h"

#include "Utils.h"
#include "Serializer.h"
#include "Project.h"

void Application::Start()
{	
	Get().LoadProjectHistory();
	Get().LoadData();
}

void Application::Update(const float deltaTime)
{
}

void Application::OnGui()
{
	switch (Get().m_page)
	{
	case Page::Start:
		Get().StartPage();
		break;
	}
}

void Application::Shutdown()
{
	Get().SaveProjectHistory();
	Get().SaveData();
}

void Application::LoadProjectHistory()
{
	if (!std::filesystem::exists(m_dataPath + "/history.txt"))
		return;

	std::ifstream file(m_dataPath + "/history.txt");
	std::string line;

	m_projHistory.clear();
	int counter = 1;
	while (std::getline(file, line))
	{
		if (std::filesystem::exists(line))
			m_projHistory.push_back(line);
	}
}

void Application::SaveProjectHistory()
{
	if (!std::filesystem::exists(m_dataPath))
		std::filesystem::create_directories(m_dataPath);

	std::ofstream file(m_dataPath + "/history.txt");
	if (!file.is_open())
		throw std::exception("history.txt is inaccessible");

	for (const std::string& path : m_projHistory)
	{
		file << path << '\n';
	}
}

void Application::LoadData()
{
	if (!std::filesystem::exists(m_dataPath + "/data.yaml"))
		return;

	YAML::Node yaml = YAML::LoadFile(m_dataPath + "/data.yaml");

	m_data.lastDirectory = yaml["last-dir"].as<std::string>();
}

void Application::SaveData()
{
	if (!std::filesystem::exists(m_dataPath))
		std::filesystem::create_directories(m_dataPath);

	std::ofstream file(m_dataPath + "/data.yaml");
	if (!file.is_open())
		throw std::exception("data.yaml is inaccessible");

	YAML::Node yaml;
	yaml["last-dir"] = m_data.lastDirectory;

	file << yaml;
}

void Application::StartPage()
{
	std::string fileToOpen;
	bool doLoad = true;

	ImGui::Text("History:");
	ImGui::ListBox("##HItems", &m_startSelectionIndex, CStrVect(m_projHistory).data(), m_projHistory.size());
	if (m_startSelectionIndex >= 0)
	{
		if (ImGui::Button("Open Selected"))
		{
			fileToOpen = m_projHistory[m_startSelectionIndex];
		}
	}

	if (ImGui::Button("Browse..."))
	{
		xe::FileBrowser browser;
		browser.PushFileType(L"*.punch", L"Punch File");
		browser.PushFileType(L"*.yaml;*.yml", L"YAML File");
		browser.PushFileType(L"*.*", L"All File Types");
		browser.SetStartPath(ToWideString(m_data.lastDirectory));
		fileToOpen = ToShortString(browser.GetFile());
		std::string path = PathNoFile(fileToOpen);
		if (path != "")
			m_data.lastDirectory = path;

	}

	if (ImGui::Button("Create New"))
	{
		xe::FileBrowser browser;
		browser.PushFileType(L"*.punch", L"Punch File");
		browser.PushFileType(L"*.yaml;*.yml", L"YAML File");
		browser.PushFileType(L"*.*", L"All File Types");
		browser.SetDefaultExtension(L"punch");
		browser.SetStartPath(ToWideString(m_data.lastDirectory));
		fileToOpen = ToShortString(browser.SaveFile());
		std::string path = PathNoFile(fileToOpen);
		if (path != "")
			m_data.lastDirectory = path;

		doLoad = false;
	}

	if (!fileToOpen.empty())
	{
		m_activeProject = std::make_unique<Project>();
		Serializer serializer;
		if (doLoad)
		{
			try
			{
				*m_activeProject = serializer.Load(fileToOpen);
			}
			catch (std::exception e)
			{
				std::cout << "[EXCEPTION]: " << e.what() << std::endl;
				fileToOpen = "";
				return;
			}
		}
		else
		{
			std::ofstream newFile(fileToOpen);
			serializer.Save(*m_activeProject, newFile);
		}
		AddHistory(fileToOpen);
		m_page = Page::Project;
	}
}

void Application::AddHistory(const std::string& file)
{
	auto iter = std::find(m_projHistory.begin(), m_projHistory.end(), file);

	if (iter != m_projHistory.end())
	{
		m_projHistory.erase(iter);
		m_projHistory.push_front(file);
		return;
	}

	while (m_projHistory.size() >= MAX_HISTORY)
		m_projHistory.pop_back();

	m_projHistory.push_front(file);
}

std::string Application::PathNoFile(const std::string& path)
{
	if (path.empty())
		return path;

	std::string result = path;

	while (result.back() != '/' && result.back() != '\\')
		result.pop_back();

	return result;
}

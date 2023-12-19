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
	case Page::Project:
		Get().ProjectPage();
		break;
	case Page::Edit:
		Get().EditPage();
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
		bool isSaved = true;
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

namespace
{
	std::string TwoDig(uint8_t num)
	{
		std::string result;
		if (num < 10)
			result = "0";
		result += std::to_string(num);
		return result;
	}

	std::string FltStr(float flt)
	{
		std::string result = std::to_string(flt);
		result = result.substr(0, result.size() - 4);
		return result;
	}
}

void Application::ProjectPage()
{
	const char* backName = (m_hasSaved) ? "Back" : "Back*";
	if (ImGui::Button(backName))
	{
		m_page = Page::Start;
		m_activeProject = nullptr;
		m_entryIndex = -1;
	}
	ImGui::SameLine();
	if (ImGui::Button("Save"))
	{
		std::ofstream file(m_activeProject->filePath);
		Serializer serializer;
		serializer.Save(*m_activeProject, file);
		m_hasSaved = true;
	}
	ImGui::Text("Entries:");

	std::vector<std::string> entryDates;

	ImGui::Columns(2, "##MainEntryColumn", true);

	for (const Entry& e : m_activeProject->entries)
	{
		entryDates.push_back(((e.isCharged) ? "" : "*") + std::to_string(e.start.year) + "-" + TwoDig(e.start.month) + "-" + TwoDig(e.start.day));
	}

	ImGui::ListBox("##EntryList", &m_entryIndex, CStrVect(entryDates).data(), entryDates.size(), 10);

	ImGui::NextColumn();

	if (m_entryIndex >= 0)
	{
		const Entry& entry = m_activeProject->entries[m_entryIndex];
		bool isIncomplete = (entry.end.year == 0);
		ImGui::Text("START");
		ImGui::Text(("\tDay: " + std::to_string(entry.start.year) + "-" + TwoDig(entry.start.month) + "-" + TwoDig(entry.start.day)).c_str());
		ImGui::Text(("\tTime: " + std::to_string(entry.start.hour) + ":" + TwoDig(entry.start.minute)).c_str());
		ImGui::Text("END");
		if (isIncomplete)
		{
			ImGui::Text("\tDay: -");
			ImGui::Text("\tTime: -");
		}
		else
		{
			ImGui::Text(("\tDay: " + std::to_string(entry.end.year) + "-" + TwoDig(entry.end.month) + "-" + TwoDig(entry.end.day)).c_str());
			ImGui::Text(("\tTime: " + std::to_string(entry.end.hour) + ":" + TwoDig(entry.end.minute)).c_str());
		}
		ImGui::NewLine();

		if (isIncomplete)
		{
			ImGui::Text("Hours: -");
			ImGui::Text("Rate: -");
			ImGui::Text("Cost: -");
			ImGui::Text("Charged: -");
		}
		else
		{
			ImGui::Text(("Hours: " + FltStr(entry.GetElapsed().GetHours())).c_str());
			ImGui::Text(("Rate: $" + FltStr(entry.rate)).c_str());
			ImGui::Text(("Cost: $" + FltStr(entry.GetElapsed().GetHours() * entry.rate)).c_str());
			ImGui::Text(("Charged: " + ((entry.isCharged) ? std::string("Yes") : std::string("No"))).c_str());
		}
		ImGui::NewLine();
		if (ImGui::Button("Edit##CurrentEntry"))
		{
			m_workingEntry = entry;
			m_page = Page::Edit;
		}
		if (isIncomplete)
		{
			ImGui::SameLine();
			if (ImGui::Button("End Now##CurrentEntry"))
			{
				m_activeProject->entries[m_entryIndex].end = DateTime::Now();
				m_activeProject->entries[m_entryIndex].end.RoundMinute();
			}
		}
	}

	ImGui::Columns(1);
	if (ImGui::Button("New"))
	{
		m_workingEntry = Entry();
		m_entryIndex = m_activeProject->entries.size();
		m_activeProject->entries.emplace_back();
		m_page = Page::Edit;
	}
	ImGui::SameLine();
	if (ImGui::Button("New Now"))
	{
		m_workingEntry = Entry();
		m_entryIndex = m_activeProject->entries.size();
		m_workingEntry.start = DateTime::Now();
		m_workingEntry.start.RoundMinute();
		m_activeProject->entries.push_back(m_workingEntry);
	}
	
}

void Application::EditPage()
{
	ImGui::Columns(2, "##EditCols");
	ImGui::Text("START:");
	int tmpInt = m_workingEntry.start.year;
	ImGui::SetNextItemWidth(100);
	if (ImGui::InputInt("Year##Start", &tmpInt))
	{
		m_workingEntry.start.year = tmpInt;
	}
	tmpInt = m_workingEntry.start.month;
	ImGui::SetNextItemWidth(100);
	if (ImGui::InputInt("Month##Start", &tmpInt))
	{
		m_workingEntry.start.month = tmpInt;
	}
	tmpInt = m_workingEntry.start.day;
	ImGui::SetNextItemWidth(100);
	if (ImGui::InputInt("Day##Start", &tmpInt))
	{
		m_workingEntry.start.day = tmpInt;
	}
	ImGui::NewLine();
	tmpInt = m_workingEntry.start.hour;
	ImGui::SetNextItemWidth(100);
	if (ImGui::InputInt("##HourStart", &tmpInt))
	{
		m_workingEntry.start.hour = tmpInt;
	}
	ImGui::SameLine();
	ImGui::Text(":");
	ImGui::SameLine();
	tmpInt = m_workingEntry.start.minute;
	ImGui::SetNextItemWidth(100);
	if (ImGui::InputInt("Time##MinuteStart", &tmpInt))
	{
		m_workingEntry.start.minute = tmpInt;
		m_workingEntry.start.RoundMinute();
	}
	if (ImGui::Button("Now##Start"))
	{
		m_workingEntry.start = DateTime::Now();
	}

	ImGui::NextColumn();

	ImGui::Text("END:");
	tmpInt = m_workingEntry.end.year;
	ImGui::SetNextItemWidth(100);
	if (ImGui::InputInt("Year##End", &tmpInt))
	{
		m_workingEntry.end.year = tmpInt;
	}
	tmpInt = m_workingEntry.end.month;
	ImGui::SetNextItemWidth(100);
	if (ImGui::InputInt("Month##End", &tmpInt))
	{
		m_workingEntry.end.month = tmpInt;
	}
	tmpInt = m_workingEntry.end.day;
	ImGui::SetNextItemWidth(100);
	if (ImGui::InputInt("Day##End", &tmpInt))
	{
		m_workingEntry.end.day = tmpInt;
	}
	ImGui::NewLine();
	tmpInt = m_workingEntry.end.hour;
	ImGui::SetNextItemWidth(100);
	if (ImGui::InputInt("##HourEnd", &tmpInt))
	{
		m_workingEntry.end.hour = tmpInt;
	}
	ImGui::SameLine();
	ImGui::Text(":");
	ImGui::SameLine();
	tmpInt = m_workingEntry.end.minute;
	ImGui::SetNextItemWidth(100);
	if (ImGui::InputInt("Time##MinuteEnd", &tmpInt))
	{
		m_workingEntry.end.minute = tmpInt;
		m_workingEntry.end.RoundMinute();
	}
	if (ImGui::Button("Now##End"))
	{
		m_workingEntry.end = DateTime::Now();
		m_workingEntry.end.RoundMinute();
	}
	ImGui::SameLine();
	if (ImGui::Button("Clear##End"))
	{
		m_workingEntry.end = DateTime();
	}

	ImGui::Columns(1);

	ImGui::NewLine();

	ImGui::NewLine();



	if (ImGui::Button("Apply"))
	{
		m_activeProject->entries[m_entryIndex] = m_workingEntry;
	}
	ImGui::SameLine();
	if (ImGui::Button("Cancel"))
	{
		m_page = Page::Project;
	}
	ImGui::SameLine();
	if (ImGui::Button("OK"))
	{
		m_activeProject->entries[m_entryIndex] = m_workingEntry;
		m_page = Page::Project;
	}
}

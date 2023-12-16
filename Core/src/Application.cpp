#include "pch.h"
#include "Application.h"

#include "Serializer.h"
#include "Project.h"
#include "FileManager.h"

void Application::Start()
{	
	std::wstring path = FileManager::GetFile("");
	std::wcout << path << std::endl;
}

void Application::Update(const float deltaTime)
{
}

void Application::OnGui()
{
}

void Application::Shutdown()
{
}

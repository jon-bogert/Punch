#pragma once

struct Project;
class Serializer
{
public:
	Project Load(const std::string& output);
	void Save(const Project& project, std::ofstream& output);

private:
	std::string m_fileVer = "1.0.0";
	Project ReadVersion0(const YAML::Node& yaml); // 1.0.0;
};
#pragma once

class Project;
class Serializer
{
public:
	Project Load(std::istream& output);
	void Save(const Project& project, std::ostream& output);

private:
	std::string m_fileVer = "1.0.0";

	Project ReadVersion0(const YAML::Node& yaml); // 1.0.0;
};
#include "pch.h"
#include "Serializer.h"

#include "Entry.h"
#include "Project.h"

Project Serializer::Load(const std::string& input)
{
	Project project;
	YAML::Node yaml = YAML::LoadFile(input);
	if (yaml["punch-ver"].as<std::string>() == "1.0.0")
	{
		project = ReadVersion0(yaml["project"]);
	}
	return project;
}

void Serializer::Save(const Project& project, std::ofstream& output)
{
	YAML::Node yaml;
	yaml["punch-ver"] = m_fileVer.c_str();

	YAML::Node& projNode = yaml["project"];

	projNode["name"] = project.name.c_str();
	projNode["rate"] = project.currentRate;

	for (size_t i = 0; i < project.entries.size(); ++i)
	{
		YAML::Node entryNode;

		std::string typeStr;
		switch (project.entries[i].type)
		{
		case Entry::Meeting:
			typeStr = "Meeting";
			break;
		default:
			typeStr = "Work";
			break;
		}
		entryNode["type"] = typeStr.c_str();
		entryNode["rate"] = project.entries[i].rate;

		entryNode["start"]["year"] = (int)project.entries[i].start.year;
		entryNode["start"]["month"] = (int)project.entries[i].start.month;
		entryNode["start"]["day"] = (int)project.entries[i].start.day;
		entryNode["start"]["hour"] = (int)project.entries[i].start.hour;
		entryNode["start"]["minute"] = (int)project.entries[i].start.minute;

		entryNode["end"]["year"] = (int)project.entries[i].end.year;
		entryNode["end"]["month"] = (int)project.entries[i].end.month;
		entryNode["end"]["day"] = (int)project.entries[i].end.day;
		entryNode["end"]["hour"] = (int)project.entries[i].end.hour;
		entryNode["end"]["minute"] = (int)project.entries[i].end.minute;

		projNode["entries"].push_back(entryNode);
	}

	output << yaml;
}

Project Serializer::ReadVersion0(const YAML::Node& yaml)
{
	Project project;

	project.name = yaml["name"].as<std::string>();
	project.currentRate = yaml["rate"].as<float>();
	if (!yaml["entries"].IsDefined())
		return project;

	size_t entryLength = yaml["entries"].size();
	project.entries.resize(entryLength);
	for (size_t i = 0; i < entryLength; ++i)
	{
		Entry entry;
		entry.start.year =	 yaml["entries"][i]["start"]["year"].as<uint16_t>();
		entry.start.month =	 yaml["entries"][i]["start"]["month"].as<uint8_t>();
		entry.start.day =	 yaml["entries"][i]["start"]["day"].as<uint8_t>();
		entry.start.hour =	 yaml["entries"][i]["start"]["hour"].as<uint8_t>();
		entry.start.minute = yaml["entries"][i]["start"]["minute"].as<uint8_t>();
							 
		entry.end.year =	 yaml["entries"][i]["end"]["year"].as<uint16_t>();
		entry.end.month =	 yaml["entries"][i]["end"]["month"].as<uint8_t>();
		entry.end.day =	     yaml["entries"][i]["end"]["day"].as<uint8_t>();
		entry.end.hour =	 yaml["entries"][i]["end"]["hour"].as<uint8_t>();
		entry.end.minute =   yaml["entries"][i]["end"]["minute"].as<uint8_t>();

		entry.rate = yaml["entries"][i]["rate"].as<float>();
		std::string typeStr = yaml["entries"][i]["type"].as<std::string>();

		if (typeStr == "Meeting")
			entry.type = Entry::Meeting;
		else
			entry.type = Entry::Work;

		project.entries[i] = entry;
	}


	return project;
}

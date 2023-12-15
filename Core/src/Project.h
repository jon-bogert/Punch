#pragma once
#include <string>
#include <vector>
#include "Entry.h"

struct Project
{
	std::string name;
	float currentRate = 0.f;
	std::vector<Entry> entries;
};
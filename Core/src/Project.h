#pragma once
#include <string>
#include <vector>
#include "DateTime.h"

struct Project
{
	std::string name;
	float currentRate = 0.f;
	std::vector<DateTime> entries;
};
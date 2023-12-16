#pragma once

#include <string>
#include <cstring>
#include <cstdlib>

std::string ToShortString(const std::wstring& wstr)
{
	const wchar_t* cwstr = wstr.c_str();
	const size_t size = wstr.size() + 1;
	char* buffer = new char[size];
	wcstombs(buffer, cwstr, size);
	std::string result(buffer);
	delete[] buffer;
	return result;
}

std::wstring ToWideString(const std::string& str)
{
    const char* cstr = str.c_str();
    const size_t size = str.size() + 1;
    wchar_t* buffer = new wchar_t[size];
    mbstowcs(buffer, cstr, size);
    std::wstring result(buffer);
    delete[] buffer;
    return result;
}

inline std::vector<const char*> CStrVect(const std::vector<std::string>& input)
{
	std::vector<const char*> inputCStr;
	for (const std::string& item : input)
	{
		inputCStr.push_back(item.c_str());
	}
	return inputCStr;
}

inline std::vector<const char*> CStrVect(const std::deque<std::string>& input)
{
	std::vector<const char*> inputCStr;
	for (const std::string& item : input)
	{
		inputCStr.push_back(item.c_str());
	}
	return inputCStr;
}
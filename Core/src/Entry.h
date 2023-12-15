#pragma once
#include "DateTime.h"

struct Entry
{
	enum Type { Work, Meeting };
	DateTime start;
	DateTime end;
	float rate = 0;

	DateTime GetElapsed() const
	{
		return end - start;
	}

	float GetCost() const
	{
		GetElapsed().GetHours() * rate;
	}

	bool IsComplete()
	{
		return (end != DateTime());
	}
};
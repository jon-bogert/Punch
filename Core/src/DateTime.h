#pragma once
#include <cstdint>
#include <chrono>

struct DateTime
{
	uint8_t hour;
	uint8_t minute;
	uint16_t year;
	uint8_t month;
	uint8_t day;

	DateTime() = default;
	inline DateTime(uint8_t hour, uint8_t minute, uint16_t year, uint8_t month, uint8_t day)
		: hour(hour)
		, minute(minute)
		, year(year)
		, month(month)
		, day(day) {}

	long GetMinutes() const
	{
		return this->minute + this->hour * 60 + this->day * 24 * 60 + this->month * 30 * 24 * 60 + this->year * 12 * 30 * 24 * 60;
	}

	float GetHours() const
	{
		return GetMinutes() / 60.0f;
	}

	DateTime operator-(const DateTime& rhs) const
	{
		DateTime result;

		long total1 = GetMinutes();
		long total2 = rhs.GetMinutes();

		long diff = total1 - total2;

		result.year = diff / (12 * 30 * 24 * 60);
		diff %= 12 * 30 * 24 * 60;
		result.month = diff / (30 * 24 * 60);
		diff %= 30 * 24 * 60;
		result.day = diff / (24 * 60);
		diff %= 24 * 60;
		result.hour = diff / 60;
		result.minute = diff % 60;
		return result;
	}

	DateTime operator+(const DateTime& rhs) const
	{
		DateTime result;

		long total1 = GetMinutes();
		long total2 = rhs.GetMinutes();

		long diff = total1 + total2;

		result.year = diff / (12 * 30 * 24 * 60);
		diff %= 12 * 30 * 24 * 60;
		result.month = diff / (30 * 24 * 60);
		diff %= 30 * 24 * 60;
		result.day = diff / (24 * 60);
		diff %= 24 * 60;
		result.hour = diff / 60;
		result.minute = diff % 60;
		return result;
	}

	bool operator<(const DateTime& rhs) const { return GetMinutes() < rhs.GetMinutes(); }
	bool operator<=(const DateTime& rhs) const { return GetMinutes() <= rhs.GetMinutes(); }
	bool operator>(const DateTime& rhs) const { return GetMinutes() > rhs.GetMinutes(); }
	bool operator>=(const DateTime& rhs) const { return GetMinutes() >= rhs.GetMinutes(); }
	bool operator==(const DateTime& rhs) const { return GetMinutes() == rhs.GetMinutes(); }
	bool operator!=(const DateTime& rhs) const { return GetMinutes() != rhs.GetMinutes(); }

	void RoundMinute()
	{
		if (minute % 15 > 7)
		{
			minute = (minute / 15) * 15 + 15;
			return;
		}
		minute = (minute / 15) * 15;
	}

	static DateTime Now()
	{
		DateTime result{};

		std::chrono::system_clock::time_point timePoint = std::chrono::system_clock::now();
		std::time_t timeT = std::chrono::system_clock::to_time_t(timePoint);
		std::tm* now  = std::localtime(&timeT);

		// Print year, month, day, hour, and minute
		result.year = static_cast<uint16_t>(now->tm_year + 1900);
		result.month = static_cast<uint8_t>(now->tm_mon + 1);
		result.day = static_cast<uint8_t>(now->tm_mday);
		result.hour = static_cast<uint8_t>(now->tm_hour);
		result.minute = static_cast<uint8_t>(now->tm_min);

		return result;
	}
};
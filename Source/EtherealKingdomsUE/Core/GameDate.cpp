// Copyright © 2025 CronoGames. All Rights Reserved.
// Author: Matt Brock

#include "GameDate.h"

FGameDate::FGameDate()
{
	Year = 1200;
	Month = 1;
	Day = 1;
}

void FGameDate::AdvanceOneDay()
{
	Day++;

	const int32 DaysInMonth[] = {
		31, // January
		28, // February
		31, // March
		30, // April
		31, // May
		30, // June
		31, // July
		31, // August
		30, // September
		31, // October
		30, // November
		31  // December
	};

	// Check for end of month
	if (Day > DaysInMonth[Month - 1])
	{
		Day = 1;
		Month++;

		// Check for end of year
		if (Month > 12)
		{
			Month = 1;
			Year++;
		}
	}
}

FString FGameDate::ToString() const
{
	static const FString MonthNames[] = {
		TEXT("January"), TEXT("February"), TEXT("March"), TEXT("April"),
		TEXT("May"), TEXT("June"), TEXT("July"), TEXT("August"),
		TEXT("September"), TEXT("October"), TEXT("November"), TEXT("December")
	};

	return FString::Printf(TEXT("%s %d, %d"), *MonthNames[Month - 1], Day, Year);
}

int32 FGameDate::GetDaysInMonth(int32 InMonth, int32 InYear) const
{
	// Handle leap years for February
	if (InMonth == 2)
	{
		if ((InYear % 4 == 0 && InYear % 100 != 0) || (InYear % 400 == 0))
		{
			return 29; // Leap year
		}
		return 28; // Non-leap year
	}
	static const int32 DaysInMonth[] = {
		31, // January
		28, // February
		31, // March
		30, // April
		31, // May
		30, // June
		31, // July
		31, // August
		30, // September
		31, // October
		30, // November
		31  // December
	};
	return DaysInMonth[InMonth - 1];
}

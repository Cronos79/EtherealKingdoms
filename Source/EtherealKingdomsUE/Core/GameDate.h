// Copyright © 2025 CronoGames. All Rights Reserved.
// Author: Matt Brock
#pragma once

#include "CoreMinimal.h"
#include "GameDate.generated.h"

USTRUCT(BlueprintType)
struct FGameDate
{
    GENERATED_BODY()

public:
    FGameDate();

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 Year = 1200;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 Month = 1; // 1 - 12

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 Day = 1;   // 1 - 31 depending on month

    void AdvanceOneDay();

    FString ToString() const;

private:
    int32 GetDaysInMonth(int32 InMonth, int32 InYear) const;
};
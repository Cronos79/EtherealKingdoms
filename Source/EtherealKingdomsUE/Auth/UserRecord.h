// Copyright © 2025 CronoGames. All Rights Reserved.
// Author: Matt Brock

#pragma once

#include "CoreMinimal.h"
#include "UserRecord.generated.h"

USTRUCT(BlueprintType)
struct FUserRecord
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly)
    FString Username;

    UPROPERTY(BlueprintReadOnly)
    FString PasswordHash;

    UPROPERTY(BlueprintReadOnly)
    FString Salt;

    UPROPERTY(BlueprintReadOnly)
    int32 KingdomID;

    FUserRecord()
        : Username(TEXT("")), PasswordHash(TEXT("")), Salt(TEXT("")), KingdomID(-1) { }
};

// Copyright © 2025 CronoGames. All Rights Reserved.
// Author: Matt Brock
#pragma once

#include "CoreMinimal.h"
#include "KingdomTypes.generated.h"

USTRUCT(BlueprintType)
struct FResource
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite)
    FString Name;

    UPROPERTY(BlueprintReadWrite)
    int32 Amount;

    FResource() : Name(TEXT("")), Amount(0) {}
    FResource(FString InName, int32 InAmount) : Name(InName), Amount(InAmount) {}
};

USTRUCT(BlueprintType)
struct FKingdomRecord
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite)
    int32 KingdomID;

    UPROPERTY(BlueprintReadWrite)
    FString KingdomName;

    UPROPERTY(BlueprintReadWrite)
    FString OwnerUsername;

    UPROPERTY(BlueprintReadWrite)
    TArray<FResource> Resources;

    // Future:
    // TArray<FBuilding> Buildings;
    // int32 Population;

    FKingdomRecord()
        : KingdomID(-1), KingdomName(TEXT("")), OwnerUsername(TEXT("")) {}
};
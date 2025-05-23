// Copyright © 2025 CronoGames. All Rights Reserved.  
// Author: Matt Brock
#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "../Buildings/EtherealBuilding.h"
#include "EtherealKingdom.generated.h"

USTRUCT(BlueprintType)
struct FKingdomStats
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite)
    int32 Population = 10;

    UPROPERTY(BlueprintReadWrite)
    int32 PopulationCap = 10;

    UPROPERTY(BlueprintReadWrite)
    int32 Happiness = 100;

    UPROPERTY(BlueprintReadWrite)
    int32 MilitaryStrength = 0;

    // Add more as needed: Mana, Science, Pollution, etc.

    void Reset()
    {
        *this = FKingdomStats(); // Resets to default values
    }
};

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
struct FKingdomData
{
    GENERATED_BODY()

public:
    UPROPERTY(BlueprintReadWrite)
    int32 KingdomID;

    UPROPERTY(BlueprintReadWrite)
    FString KingdomName;

    UPROPERTY(BlueprintReadWrite)
    FString OwnerUsername;

    UPROPERTY(BlueprintReadWrite)
    TArray<FResource> Resources;

    UPROPERTY(BlueprintReadWrite)
    FKingdomStats Stats;
    // Add more as needed
};

UCLASS()
class ETHEREALKINGDOMSUE_API UEtherealKingdom : public UObject
{
	GENERATED_BODY()
	
public:
    UEtherealKingdom();

	UPROPERTY(BlueprintReadWrite)
    int32 KingdomID;

    UPROPERTY(BlueprintReadWrite)
    FString KingdomName;

    UPROPERTY(BlueprintReadWrite)
    FString OwnerUsername;

    UPROPERTY(BlueprintReadWrite)
    TArray<FResource> Resources;

    UPROPERTY(BlueprintReadWrite)
    TArray<UEtherealBuilding*> Buildings;

    UPROPERTY(BlueprintReadWrite)
    FKingdomStats Stats;
	
	UFUNCTION()
    void AdvanceKingdomDay();

    UFUNCTION()
    void IncreasePopulationCap(int32 Amount)
    {
        Stats.PopulationCap += Amount;
    }

    UFUNCTION()
	void DecreasePopulationCap(int32 Amount)
	{

		// Minimum cap check
		if (Stats.PopulationCap - Amount < 0)
		{
            Stats.PopulationCap = 0;
			return;
		}
		Stats.PopulationCap -= Amount;
	}

    UFUNCTION()
	void IncreasePopulation(int32 Amount)
	{
		Stats.Population += Amount;
	}

    UFUNCTION()
	void DecreasePopulation(int32 Amount)
	{
		// Minimum population check
		if (Stats.Population - Amount < 0)
		{
			Stats.Population = 0;
			return;
		}
		Stats.Population -= Amount;
	}

	// Add resource functions
	UFUNCTION()
	void AddResource(const FString& ResourceName, int32 Amount)
	{
		for (FResource& Resource : Resources)
		{
			if (Resource.Name == ResourceName)
			{
				Resource.Amount += Amount;
				return;
			}
		}
		// If resource not found, add it
		Resources.Add(FResource(ResourceName, Amount));
	}

	UFUNCTION()
	void RemoveResource(const FString& ResourceName, int32 Amount)
	{
		for (FResource& Resource : Resources)
		{
			if (Resource.Name == ResourceName)
			{
				if (Resource.Amount < Amount)
				{
					UE_LOG(LogTemp, Warning, TEXT("Not enough %s!"), *ResourceName);
					return; // Not enough resource
				}
				Resource.Amount -= Amount;
				break;
			}
		}
	}
};

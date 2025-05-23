// Copyright © 2025 CronoGames. All Rights Reserved.  Author: Matt Brock

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "EtherealBuilding.generated.h"

/**
 * 
 */
UCLASS(Abstract, Blueprintable)
class ETHEREALKINGDOMSUE_API UEtherealBuilding : public UObject
{
	GENERATED_BODY()

public:
    // Unique database ID
    UPROPERTY()
    int32 BuildingID;

    // Building name or internal type identifier
    UPROPERTY()
    FString BuildingName;

    // Building's level
    UPROPERTY()
    int32 Level;

    // Maximum number of workers that can be assigned to the building
    UPROPERTY()
    int32 MaxWorkers;

    // Current number of workers assigned
    UPROPERTY()
    int32 WorkersAssigned;

	// Minimum number of workers needed for the building to function
	UPROPERTY()
	int32 WorkersNeeded;

    // Whether the building is constructed and operational
    UPROPERTY()
    int32 ConstructionDaysLeft;

     // Add this property to track if a one-time effect has been applied
    UPROPERTY(BlueprintReadWrite)
    bool bOneTimeEffectApplied = false;

    // Constructor
    UEtherealBuilding()
    {
        Level = 1;
        MaxWorkers = 0;
        WorkersAssigned = 0;
		ConstructionDaysLeft = 0;
    }

	bool IsConstructed() const
	{
		return ConstructionDaysLeft <= 0;
	}

	bool IsOperational() const
	{
		return IsConstructed() && WorkersAssigned > 0;
	}

    // Advance logic for a single game day, to be implemented in child classes
    virtual void AdvanceDay(class UEtherealKingdom* Kingdom) PURE_VIRTUAL(UEtherealBuilding::AdvanceDay, );
};

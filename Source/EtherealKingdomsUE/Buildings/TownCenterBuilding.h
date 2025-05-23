// Copyright © 2025 CronoGames. All Rights Reserved.  Author: Matt Brock

#pragma once

#include "CoreMinimal.h"
#include "EtherealBuilding.h"
#include "TownCenterBuilding.generated.h"

/**
 * 
 */
UCLASS()
class ETHEREALKINGDOMSUE_API UTownCenterBuilding : public UEtherealBuilding
{
	GENERATED_BODY()
	
public:
	// Passive coins per day
	UPROPERTY(BlueprintReadWrite)
	int32 BaseCoinsPerDay = 4;

	// Population cap boost per level
  UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 PopulationCapBonus = 10;

public:
	UTownCenterBuilding();

	virtual void AdvanceDay(class UEtherealKingdom* Kingdom) override;
	
	
};

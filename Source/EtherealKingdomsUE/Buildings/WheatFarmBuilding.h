// Copyright © 2025 CronoGames. All Rights Reserved.  Author: Matt Brock

#pragma once

#include "CoreMinimal.h"
#include "EtherealBuilding.h"
#include "WheatFarmBuilding.generated.h"

/**
 * 
 */
UCLASS()
class ETHEREALKINGDOMSUE_API UWheatFarmBuilding : public UEtherealBuilding
{
	GENERATED_BODY()
	
public:
    // Base food produced per worker per day
    UPROPERTY(BlueprintReadWrite)
    int32 BaseFoodPerWorker = 5;
	
public:
	UWheatFarmBuilding();

	virtual void AdvanceDay(class UEtherealKingdom* Kingdom) override;
};

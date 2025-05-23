// Copyright © 2025 CronoGames. All Rights Reserved.  Author: Matt Brock


#include "WheatFarmBuilding.h"
#include "../Kingdom/EtherealKingdom.h"

UWheatFarmBuilding::UWheatFarmBuilding()
{
	BuildingName = TEXT("WheatFarm");
	Level = 1; // Starting level
	WorkersAssigned = 0; // No workers assigned initially
	WorkersNeeded = 2; // Minimum workers needed for operation
	MaxWorkers = 5; // Can support 5 workers
	ConstructionDaysLeft = 4; // Takes 4 days to construct
}

void UWheatFarmBuilding::AdvanceDay(UEtherealKingdom* Kingdom)
{
	if (!IsConstructed())
	{
		ConstructionDaysLeft--;
		if (ConstructionDaysLeft <= 0)
		{
			// Building is now constructed
			UE_LOG(LogTemp, Log, TEXT("Wheat Farm construction completed."));
		}
		else
		{
			// Log the remaining construction days
			UE_LOG(LogTemp, Log, TEXT("Wheat Farm construction in progress. Days left: %d"), ConstructionDaysLeft);
		}	
		return;
	}

	if (IsOperational())
	{
		// Calculate food produced
		int32 WheatProduced = BaseFoodPerWorker * WorkersAssigned * (Level * 1.25);
		Kingdom->AddResource("Wheat", WheatProduced);

		// Log the food produced
		UE_LOG(LogTemp, Log, TEXT("Wheat Farm produced %d food."), WheatProduced);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Wheat Farm is not operational. No workers assigned."));
	}
}

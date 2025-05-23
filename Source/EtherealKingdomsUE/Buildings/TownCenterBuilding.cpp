// Copyright © 2025 CronoGames. All Rights Reserved.  Author: Matt Brock


#include "TownCenterBuilding.h"
#include "../Kingdom/EtherealKingdom.h"

UTownCenterBuilding::UTownCenterBuilding()
{
	BuildingName = TEXT("TownCenter");
	Level = 1;
	WorkersAssigned = 1;
	WorkersNeeded = 1;
	MaxWorkers = 1;
	ConstructionDaysLeft = 0; // Starts already built
}

void UTownCenterBuilding::AdvanceDay(UEtherealKingdom* Kingdom)
{
	if (!IsConstructed())
	{
		// Shouldn't really happen, but safe check
		UE_LOG(LogTemp, Warning, TEXT("Town Center under construction? Should be pre-built."));
		return;
	}
	else
	{
		if (!bOneTimeEffectApplied)
		{
			Kingdom->IncreasePopulationCap(PopulationCapBonus);
			bOneTimeEffectApplied = true;
			UE_LOG(LogTemp, Log, TEXT("Town Center boosted population cap by %d."), PopulationCapBonus);
		}
	}

	if (IsOperational())
	{
		// Grant passive gold per day
		int32 CoinsEarned = BaseCoinsPerDay + (Level * 2); // Scales with level
		Kingdom->AddResource("Coins", CoinsEarned);
		UE_LOG(LogTemp, Log, TEXT("Town Center produced %d coins."), CoinsEarned);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Town Center is not operational. No workers assigned."));
	}
}
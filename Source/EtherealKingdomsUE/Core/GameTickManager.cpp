// Copyright © 2025 CronoGames. All Rights Reserved. 
// Author: Matt Brock


#include "GameTickManager.h"
#include <EtherealKingdomsUE/Auth/KingdomTypes.h>
#include <EtherealKingdomsUE/Database/DatabaseManager.h>
#include "EKGameModeBase.h"

UGameTickManager::UGameTickManager()
{
}

void UGameTickManager::Initialize(AEKGameModeBase* InGameMode)
{
	GameModeRef = InGameMode;

	// Optionally load saved date later
	CurrentDate = FGameDate(); // Fresh campaign starts at Jan 1, 1200
}

void UGameTickManager::TickOneDay()
{
	if (!GameModeRef)
	{
		UE_LOG(LogTemp, Warning, TEXT("GameTickManager: GameModeRef not set."));
		return;
	}

	CurrentDate.AdvanceOneDay();
	UE_LOG(LogTemp, Log, TEXT("Game Date advanced to %s"), *CurrentDate.ToString());

	ProcessKingdoms();
	ProcessWorldEvents();

	GameModeRef->GetKingdomManager()->SaveAllKingdoms();
}

void UGameTickManager::ProcessKingdoms()
{
	if (!GameModeRef || !GameModeRef->GetKingdomManager()) return;

	GameModeRef->GetKingdomManager()->TickKingdoms();
}

void UGameTickManager::ProcessWorldEvents()
{
}

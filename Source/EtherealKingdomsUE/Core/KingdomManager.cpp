// Copyright © 2025 CronoGames. All Rights Reserved.
// Author: Matt Brock

#include "KingdomManager.h"
#include "EKGameModeBase.h"
#include "../Database/DatabaseManager.h"
#include "sqlite3.h"

void UKingdomManager::Initialize(AEKGameModeBase* InGameMode)
{
    GameModeRef = InGameMode;

    if (!GameModeRef)
    {
        UE_LOG(LogTemp, Error, TEXT("KingdomManager: Invalid GameMode reference during initialization."));
        return;
    }

    LoadAllKingdoms();
}

void UKingdomManager::LoadAllKingdoms()
{
    Kingdoms.Empty();

    if (!GameModeRef || !GameModeRef->GetDatabaseManager())
    {
        UE_LOG(LogTemp, Error, TEXT("KingdomManager: Cannot load kingdoms, missing GameMode or DBManager."));
        return;
    }

    DatabaseManager* DB = GameModeRef->GetDatabaseManager();

    // Load all Kingdom IDs
    sqlite3_stmt* Statement = nullptr;
    if (!DB->SQLite.PrepareStatement(TEXT("SELECT KingdomID FROM Kingdoms;"), Statement))
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to prepare query to load all kingdoms."));
        return;
    }

    while (sqlite3_step(Statement) == SQLITE_ROW)
    {
        int32 KingdomID = sqlite3_column_int(Statement, 0);
        UEtherealKingdom* Kingdom = NewObject<UEtherealKingdom>();
        if (DB->LoadKingdomData(KingdomID, Kingdom))
        {
            Kingdoms.Add(KingdomID, Kingdom);
            UE_LOG(LogTemp, Log, TEXT("Loaded Kingdom ID %d: %s"), KingdomID, *Kingdom->KingdomName);
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("Failed to load kingdom data for ID: %d"), KingdomID);
        }
    }

    DB->SQLite.FinalizeStatement(Statement);
}

void UKingdomManager::TickKingdoms()
{
	if (!GameModeRef || !GameModeRef->GetDatabaseManager()) return;
	DatabaseManager* DB = GameModeRef->GetDatabaseManager();
	for (auto& Elem : Kingdoms)
	{
        if (Elem.Value)
        {
            UEtherealKingdom* Kingdom = Elem.Value;
            UE_LOG(LogTemp, Log, TEXT("Advancing Kingdom ID %d: %s"),
                Kingdom->KingdomID,
                Kingdom->KingdomName.IsEmpty() ? TEXT("<Unnamed>") : *Kingdom->KingdomName);
            Kingdom->AdvanceKingdomDay();
            DB->SaveKingdomData(Kingdom);
        }	
        else
        {
			UE_LOG(LogTemp, Warning, TEXT("Kingdom ID %d is invalid."), Elem.Key);
        }
	}
}

void UKingdomManager::SaveAllKingdoms()
{
    if (!GameModeRef || !GameModeRef->GetDatabaseManager()) return;

    DatabaseManager* DB = GameModeRef->GetDatabaseManager();
    for (auto& Elem : Kingdoms)
    {
        DB->SaveKingdomData(Elem.Value);
    }

    UE_LOG(LogTemp, Log, TEXT("All kingdoms saved."));
}

TMap<int32, UEtherealKingdom*>& UKingdomManager::GetKingdoms()
{
    return Kingdoms;
}

bool UKingdomManager::GetKingdomByID(int32 KingdomID, UEtherealKingdom* OutRecord)
{
    if (UEtherealKingdom** Found = Kingdoms.Find(KingdomID))
    {
        OutRecord = *Found;
        return true;
    }
    return false;
}

bool UKingdomManager::UpdateKingdom(UEtherealKingdom* UpdatedRecord)
{
    if (!Kingdoms.Contains(UpdatedRecord->KingdomID)) return false;
    Kingdoms[UpdatedRecord->KingdomID] = UpdatedRecord;
    return true;
}
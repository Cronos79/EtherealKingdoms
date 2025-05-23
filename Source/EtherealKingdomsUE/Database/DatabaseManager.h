// Copyright © 2025 CronoGames. All Rights Reserved.
// Author: Matt Brock

#pragma once

#include "CoreMinimal.h"
#include "../Auth/UserRecord.h"
#include "SQLiteWrapper.h"
#include "../Auth/KingdomTypes.h"
#include "../Kingdom/EtherealKingdom.h"



/**
 * 
 */
class ETHEREALKINGDOMSUE_API DatabaseManager
{
public:
	DatabaseManager();
	~DatabaseManager();

	bool OpenDatabase(const FString& DatabasePath);
	bool EnsureTablesExists();

	// User stuff
	bool GetUserRecord(const FString& Username, const FString& Password, FUserRecord& OutRecord);
	FString HashPassword(const FString& Password, const FString& Salt);
	FString GenerateSalt();
	bool RegisterUser(const FString& Username, const FString& Password);

	// Kingdom stuff
	bool CreateKingdom(const FString& Username, const FString& KingdomName, int32& OutKingdomID);
	bool LoadKingdomData(int32 KingdomID, UEtherealKingdom* OutRecord);
	bool SaveKingdomData(const UEtherealKingdom* Kingdom);
	bool SaveKingdomStats(const UEtherealKingdom* Kingdom);
	bool LoadKingdomStats(UEtherealKingdom* Kingdom);

	bool SaveKingdomResources(const UEtherealKingdom* Kingdom);
	bool LoadKingdomResources(UEtherealKingdom* Kingdom);
	bool UpdateResourceAmount(int32 KingdomID, const FString& ResourceName, int32 NewAmount);
	bool HasEnoughResource(int32 KingdomID, const FString& ResourceName, int32 RequiredAmount);
	bool ConsumeResource(int32 KingdomID, const FString& ResourceName, int32 Amount);
	bool AddResource(int32 KingdomID, const FString& ResourceName, int32 Amount);
	bool LoadKingdomBuildings(UEtherealKingdom* Kingdom);
	bool SaveKingdomBuildings(const UEtherealKingdom* Kingdom);
//private:
	SQLiteWrapper SQLite;
};

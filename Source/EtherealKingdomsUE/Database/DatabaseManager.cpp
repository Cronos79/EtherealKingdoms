// Copyright © 2025 CronoGames. All Rights Reserved.
// Author: Matt Brock

#include "DatabaseManager.h"
#include "Misc/Paths.h"
#include "../ThirdParty/SQLite/sqlite3.h"
#include "../Buildings/TownCenterBuilding.h"
#include "../Buildings/WheatFarmBuilding.h"

DatabaseManager::DatabaseManager()
{
}

DatabaseManager::~DatabaseManager()
{
}

bool DatabaseManager::OpenDatabase(const FString& DatabasePath)
{
    return SQLite.Open(*DatabasePath) && EnsureTablesExists();
}

bool DatabaseManager::EnsureTablesExists()
{
    const FString CreateTablesQuery = TEXT(R"(
        CREATE TABLE IF NOT EXISTS Users (
            Username TEXT PRIMARY KEY,
            PasswordHash TEXT NOT NULL,
            Salt TEXT NOT NULL,
            KingdomID INTEGER
        );

        CREATE TABLE IF NOT EXISTS Kingdoms (
            KingdomID INTEGER PRIMARY KEY AUTOINCREMENT,
            Name TEXT NOT NULL UNIQUE,
            OwnerUsername TEXT NOT NULL
        );

        CREATE TABLE IF NOT EXISTS KingdomResources (
            KingdomID INTEGER,
            ResourceName TEXT,
            Amount INTEGER,
            PRIMARY KEY (KingdomID, ResourceName),
            FOREIGN KEY (KingdomID) REFERENCES Kingdoms(KingdomID)
        );

          CREATE TABLE IF NOT EXISTS KingdomBuildings (
            KingdomID INTEGER,
            BuildingID INTEGER PRIMARY KEY AUTOINCREMENT,
            BuildingType TEXT,
            Level INTEGER,
            WorkersNeeded INTEGER,
            WorkersAssigned INTEGER,
            MaxWorkers INTEGER,
            ConstructionDaysLeft INTEGER,
            OneTimeEffectApplied INTEGER DEFAULT 0,
            FOREIGN KEY (KingdomID) REFERENCES Kingdoms(KingdomID)
        );

        CREATE TABLE IF NOT EXISTS KingdomStats (
            KingdomID INTEGER PRIMARY KEY,
            Population INTEGER DEFAULT 10,
            PopulationCap INTEGER DEFAULT 10,
            Happiness INTEGER DEFAULT 100,
            MilitaryStrength INTEGER DEFAULT 0,
            FOREIGN KEY (KingdomID) REFERENCES Kingdoms(KingdomID)
        );
    )");

    return SQLite.ExecuteQuery(CreateTablesQuery);
}

bool DatabaseManager::GetUserRecord(const FString& Username, const FString& Password, FUserRecord& OutRecord)
{
    if (!SQLite.IsOpen()) return false;

    sqlite3_stmt* Statement = nullptr;
    FString Query = FString::Printf(TEXT("SELECT PasswordHash, Salt, KingdomID FROM Users WHERE Username = '%s';"), *Username.ReplaceCharWithEscapedChar());

    if (!SQLite.PrepareStatement(Query, Statement)) return false;

    bool bSuccess = false;
    if (sqlite3_step(Statement) == SQLITE_ROW)
    {
        FString StoredHash = SQLite.GetColumnText(Statement, 0);
        FString StoredSalt = SQLite.GetColumnText(Statement, 1);
        int32 StoredKingdomID = SQLite.GetColumnInt(Statement, 2);

        FString AttemptHash = HashPassword(Password, StoredSalt);
        if (AttemptHash == StoredHash)
        {
            OutRecord.Username = Username;
            OutRecord.PasswordHash = StoredHash;
            OutRecord.Salt = StoredSalt;
            OutRecord.KingdomID = StoredKingdomID;
            bSuccess = true;
        }
    }

    SQLite.FinalizeStatement(Statement);
    return bSuccess;
}

FString DatabaseManager::HashPassword(const FString& Password, const FString& Salt)
{
    FString Combined = Password + Salt;
    FTCHARToUTF8 UTF8Combined(*Combined);

    FSHAHash Hash;
    FSHA1::HashBuffer(UTF8Combined.Get(), UTF8Combined.Length(), Hash.Hash);

    return BytesToHex(Hash.Hash, UE_ARRAY_COUNT(Hash.Hash));
}

FString DatabaseManager::GenerateSalt()
{
    FGuid NewGuid = FGuid::NewGuid();
    uint8 SaltBytes[16];
    FMemory::Memcpy(SaltBytes, &NewGuid, 16);

    FString Salt;
    for (int32 i = 0; i < 16; ++i)
    {
        Salt += FString::Printf(TEXT("%02x"), SaltBytes[i]);
    }

    return Salt;
}

bool DatabaseManager::RegisterUser(const FString& Username, const FString& Password)
{
    if (!SQLite.IsOpen()) return false;

    FString CheckQuery = FString::Printf(TEXT("SELECT COUNT(*) FROM Users WHERE Username = '%s';"), *Username.ReplaceCharWithEscapedChar());
    int32 Count = 0;
    if (!SQLite.GetSingleIntResult(CheckQuery, Count) || Count > 0) return false;

    FString Salt = GenerateSalt();
    FString PasswordHash = HashPassword(Password, Salt);

    FString InsertQuery = FString::Printf(
        TEXT("INSERT INTO Users (Username, PasswordHash, Salt, KingdomID) VALUES ('%s', '%s', '%s', -1);"),
        *Username.ReplaceCharWithEscapedChar(), *PasswordHash, *Salt
    );

    return SQLite.ExecuteQuery(InsertQuery);
}

bool DatabaseManager::CreateKingdom(const FString& Username, const FString& KingdomName, int32& OutKingdomID)
{
    UE_LOG(LogTemp, Log, TEXT("CreateKingdom called with Username: %s, KingdomName: %s"), *Username, *KingdomName);
    if (!SQLite.IsOpen()) return false;

    FString CheckQuery = FString::Printf(TEXT("SELECT COUNT(*) FROM Kingdoms WHERE Name = '%s';"), *KingdomName.ReplaceCharWithEscapedChar());
    int32 Count = 0;
    if (!SQLite.GetSingleIntResult(CheckQuery, Count) || Count > 0) return false;

    FString InsertQuery = FString::Printf(
        TEXT("INSERT INTO Kingdoms (Name, OwnerUsername) VALUES ('%s', '%s');"),
        *KingdomName.ReplaceCharWithEscapedChar(),
        *Username.ReplaceCharWithEscapedChar()
    );
    if (!SQLite.ExecuteQuery(InsertQuery)) return false;

    sqlite3_stmt* Statement = nullptr;
    if (!SQLite.PrepareStatement(TEXT("SELECT last_insert_rowid();"), Statement)) return false;

    bool bSuccess = false;
    if (sqlite3_step(Statement) == SQLITE_ROW)
    {
        OutKingdomID = sqlite3_column_int(Statement, 0);
        bSuccess = true;
    }
    SQLite.FinalizeStatement(Statement);
    if (!bSuccess) return false;

    FString UpdateQuery = FString::Printf(
        TEXT("UPDATE Users SET KingdomID = %d WHERE Username = '%s';"),
        OutKingdomID,
        *Username.ReplaceCharWithEscapedChar()
    );
    if (!SQLite.ExecuteQuery(UpdateQuery)) return false;

    // Initialize Kingdom Object
    UEtherealKingdom* NewKingdom = NewObject<UEtherealKingdom>();
    NewKingdom->KingdomID = OutKingdomID;
    NewKingdom->KingdomName = KingdomName;
    NewKingdom->OwnerUsername = Username;

    SaveKingdomStats(NewKingdom);

    // Starting resources
    NewKingdom->Resources = {
        FResource("Coins", 250),
        FResource("Wood", 100),
        FResource("Stone", 50),
        FResource("Wheat", 200)
    };
    SaveKingdomResources(NewKingdom);

    // Starting building
    UTownCenterBuilding* TownCenter = NewObject<UTownCenterBuilding>();

    NewKingdom->Buildings.Add(TownCenter);
    SaveKingdomBuildings(NewKingdom);

    return true;
}

bool DatabaseManager::LoadKingdomData(int32 KingdomID, UEtherealKingdom* OutRecord)
{
    if (!SQLite.IsOpen())
        return false;

    FString Query = FString::Printf(TEXT("SELECT Name, OwnerUsername FROM Kingdoms WHERE KingdomID = %d;"), KingdomID);
    sqlite3_stmt* Statement = nullptr;

    if (!SQLite.PrepareStatement(Query, Statement))
        return false;

    bool bSuccess = false;

    if (sqlite3_step(Statement) == SQLITE_ROW)
    {
        OutRecord->KingdomID = KingdomID;
        OutRecord->KingdomName = SQLite.GetColumnText(Statement, 0);
        OutRecord->OwnerUsername = SQLite.GetColumnText(Statement, 1);

        bSuccess = LoadKingdomStats(OutRecord);
        if (!bSuccess)
        {
            UE_LOG(LogTemp, Warning, TEXT("Failed to load kingdom stats for KingdomID: %d"), KingdomID);
        }

		bSuccess = LoadKingdomResources(OutRecord);
		if (!bSuccess)
		{
			UE_LOG(LogTemp, Warning, TEXT("Failed to load kingdom resources for KingdomID: %d"), KingdomID);
		}

        if (!LoadKingdomBuildings(OutRecord))
        {
            UE_LOG(LogTemp, Warning, TEXT("Failed to load buildings for KingdomID: %d"), KingdomID);
        }
    }    

    SQLite.FinalizeStatement(Statement);
    return bSuccess;
}

bool DatabaseManager::SaveKingdomData(const UEtherealKingdom* Kingdom)
{
    if (!SQLite.IsOpen())
        return false;

    // Save main Kingdom info
    FString Query = FString::Printf(
        TEXT("UPDATE Kingdoms SET Name = '%s', OwnerUsername = '%s' WHERE KingdomID = %d;"),
        *Kingdom->KingdomName.ReplaceCharWithEscapedChar(),
        *Kingdom->OwnerUsername.ReplaceCharWithEscapedChar(),
        Kingdom->KingdomID
    );

    if (!SQLite.ExecuteQuery(Query))
        return false;

    // Save resources and buildings (can fail independently, so guard each)
    const bool bSavedStats = SaveKingdomStats(Kingdom);
    const bool bSavedResources = SaveKingdomResources(Kingdom);
    const bool bSavedBuildings = SaveKingdomBuildings(Kingdom);

    return bSavedStats && bSavedResources && bSavedBuildings;
}


bool DatabaseManager::SaveKingdomStats(const UEtherealKingdom* Kingdom)
{
    if (!SQLite.IsOpen() || !Kingdom) return false;

    FString Query = FString::Printf(
        TEXT("INSERT INTO KingdomStats (KingdomID, Population, PopulationCap, Happiness, MilitaryStrength) "
            "VALUES (%d, %d, %d, %d, %d) "
            "ON CONFLICT(KingdomID) DO UPDATE SET "
            "Population=excluded.Population, "
            "PopulationCap=excluded.PopulationCap, "
            "Happiness=excluded.Happiness, "
            "MilitaryStrength=excluded.MilitaryStrength;"),
        Kingdom->KingdomID,
        Kingdom->Stats.Population,
        Kingdom->Stats.PopulationCap,
        Kingdom->Stats.Happiness,
        Kingdom->Stats.MilitaryStrength
    );

    return SQLite.ExecuteQuery(Query);
}


bool DatabaseManager::LoadKingdomStats(UEtherealKingdom* Kingdom)
{
    if (!SQLite.IsOpen() || !Kingdom) return false;

    FString Query = FString::Printf(
        TEXT("SELECT Population, PopulationCap, Happiness, MilitaryStrength FROM KingdomStats WHERE KingdomID = %d;"),
        Kingdom->KingdomID
    );

    sqlite3_stmt* Statement = nullptr;
    if (!SQLite.PrepareStatement(Query, Statement))
        return false;

    bool bSuccess = false;
    if (sqlite3_step(Statement) == SQLITE_ROW)
    {
        Kingdom->Stats.Population = sqlite3_column_int(Statement, 0);
        Kingdom->Stats.PopulationCap = sqlite3_column_int(Statement, 1);
        Kingdom->Stats.Happiness = sqlite3_column_int(Statement, 2);
        Kingdom->Stats.MilitaryStrength = sqlite3_column_int(Statement, 3);
        bSuccess = true;
    }
    SQLite.FinalizeStatement(Statement);
    return bSuccess;
}

bool DatabaseManager::SaveKingdomResources(const UEtherealKingdom* Kingdom)
{
    if (!SQLite.IsOpen()) return false;

    if (!SQLite.BeginTransaction())
        return false;

    for (const FResource& Resource : Kingdom->Resources)
    {
        FString Query = FString::Printf(
            TEXT("INSERT INTO KingdomResources (KingdomID, ResourceName, Amount) VALUES (%d, '%s', %d) ")
            TEXT("ON CONFLICT(KingdomID, ResourceName) DO UPDATE SET Amount = excluded.Amount;"),
            Kingdom->KingdomID,
            *Resource.Name.ReplaceCharWithEscapedChar(),
            Resource.Amount
        );

        if (!SQLite.ExecuteQuery(Query))
        {
            SQLite.RollbackTransaction();
            return false;
        }
    }

    return SQLite.CommitTransaction();
}

bool DatabaseManager::LoadKingdomResources(UEtherealKingdom* Kingdom)
{
    if (!SQLite.IsOpen()) return false;

    FString Query = FString::Printf(TEXT("SELECT ResourceName, Amount FROM KingdomResources WHERE KingdomID = %d;"), Kingdom->KingdomID);
    sqlite3_stmt* Statement = nullptr;

    if (!SQLite.PrepareStatement(Query, Statement))
        return false;

    Kingdom->Resources.Empty(); // Clear old data

    while (sqlite3_step(Statement) == SQLITE_ROW)
    {
        FString Name = SQLite.GetColumnText(Statement, 0);
        int32 Amount = SQLite.GetColumnInt(Statement, 1);

        Kingdom->Resources.Add(FResource(Name, Amount));
    }

    SQLite.FinalizeStatement(Statement);
    return true;
}

bool DatabaseManager::UpdateResourceAmount(int32 KingdomID, const FString& ResourceName, int32 NewAmount)
{
    if (!SQLite.IsOpen()) return false;

    FString Query = FString::Printf(
        TEXT("INSERT INTO KingdomResources (KingdomID, ResourceName, Amount) VALUES (%d, '%s', %d) ")
        TEXT("ON CONFLICT(KingdomID, ResourceName) DO UPDATE SET Amount = excluded.Amount;"),
        KingdomID, *ResourceName.ReplaceCharWithEscapedChar(), NewAmount
    );

    return SQLite.ExecuteQuery(Query);
}

bool DatabaseManager::HasEnoughResource(int32 KingdomID, const FString& ResourceName, int32 RequiredAmount)
{
    if (!SQLite.IsOpen()) return false;

    FString Query = FString::Printf(
        TEXT("SELECT Amount FROM KingdomResources WHERE KingdomID = %d AND ResourceName = '%s';"),
        KingdomID, *ResourceName.ReplaceCharWithEscapedChar()
    );

    int32 CurrentAmount = 0;
    if (!SQLite.GetSingleIntResult(Query, CurrentAmount))
        return false;

    return CurrentAmount >= RequiredAmount;
}

bool DatabaseManager::ConsumeResource(int32 KingdomID, const FString& ResourceName, int32 Amount)
{
    if (!HasEnoughResource(KingdomID, ResourceName, Amount)) return false;

    FString Query = FString::Printf(
        TEXT("UPDATE KingdomResources SET Amount = Amount - %d WHERE KingdomID = %d AND ResourceName = '%s';"),
        Amount, KingdomID, *ResourceName.ReplaceCharWithEscapedChar()
    );

    return SQLite.ExecuteQuery(Query);
}

bool DatabaseManager::AddResource(int32 KingdomID, const FString& ResourceName, int32 Amount)
{
    if (!SQLite.IsOpen()) return false;

    FString Query = FString::Printf(
        TEXT("INSERT INTO KingdomResources (KingdomID, ResourceName, Amount) VALUES (%d, '%s', %d) ")
        TEXT("ON CONFLICT(KingdomID, ResourceName) DO UPDATE SET Amount = Amount + %d;"),
        KingdomID, *ResourceName.ReplaceCharWithEscapedChar(), Amount, Amount
    );

    return SQLite.ExecuteQuery(Query);
}

bool DatabaseManager::LoadKingdomBuildings(UEtherealKingdom* Kingdom)
{
    if (!SQLite.IsOpen() || !Kingdom) return false;

    FString Query = FString::Printf(TEXT(
        "SELECT BuildingID, BuildingType, Level, WorkersNeeded, WorkersAssigned, MaxWorkers, ConstructionDaysLeft, OneTimeEffectApplied "
        "FROM KingdomBuildings WHERE KingdomID = %d;"), Kingdom->KingdomID);

    sqlite3_stmt* Statement = nullptr;
    if (!SQLite.PrepareStatement(Query, Statement))
        return false;

    Kingdom->Buildings.Empty(); // Clear existing list

    while (sqlite3_step(Statement) == SQLITE_ROW)
    {
        FString BuildingType = FString((const char*)sqlite3_column_text(Statement, 1));
        UEtherealBuilding* Building = nullptr;

        if (BuildingType == TEXT("TownCenter"))
        {
            Building = NewObject<UTownCenterBuilding>();
        }
        else if (BuildingType == TEXT("WheatFarm"))
        {
            Building = NewObject<UWheatFarmBuilding>();
        }
        // Add more else ifs for other building types

        if (Building)
        {
            Building->BuildingID = sqlite3_column_int(Statement, 0);
            Building->BuildingName = BuildingType;
            Building->Level = sqlite3_column_int(Statement, 2);
            Building->WorkersNeeded = sqlite3_column_int(Statement, 3);
            Building->WorkersAssigned = sqlite3_column_int(Statement, 4);
            Building->MaxWorkers = sqlite3_column_int(Statement, 5);
            Building->ConstructionDaysLeft = sqlite3_column_int(Statement, 6);
            Building->bOneTimeEffectApplied = sqlite3_column_int(Statement, 7) != 0;

            Kingdom->Buildings.Add(Building);
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("Unknown building type: %s"), *BuildingType);
        }
    }

    SQLite.FinalizeStatement(Statement);
    return true;
}

bool DatabaseManager::SaveKingdomBuildings(const UEtherealKingdom* Kingdom)
{
    if (!SQLite.IsOpen() || !Kingdom) return false;

    if (!SQLite.BeginTransaction())
        return false;

    FString DeleteQuery = FString::Printf(TEXT("DELETE FROM KingdomBuildings WHERE KingdomID = %d;"), Kingdom->KingdomID);
    if (!SQLite.ExecuteQuery(DeleteQuery))
    {
        SQLite.RollbackTransaction();
        return false;
    }

    FString InsertQuery = TEXT(
        "INSERT INTO KingdomBuildings (KingdomID, BuildingID, BuildingType, Level, WorkersNeeded, WorkersAssigned, MaxWorkers, ConstructionDaysLeft, OneTimeEffectApplied) "
        "VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?);"
    );
    sqlite3_stmt* InsertStmt = nullptr;
    if (!SQLite.PrepareStatement(InsertQuery, InsertStmt))
    {
        SQLite.RollbackTransaction();
        return false;
    }

    for (UEtherealBuilding* Building : Kingdom->Buildings)
    {
        sqlite3_reset(InsertStmt);

        // If BuildingID is 0, let SQLite assign it
        if (Building->BuildingID == 0)
        {
            FString InsertNoIDQuery = TEXT(
                "INSERT INTO KingdomBuildings (KingdomID, BuildingType, Level, WorkersNeeded, WorkersAssigned, MaxWorkers, ConstructionDaysLeft, OneTimeEffectApplied) "
                "VALUES (?, ?, ?, ?, ?, ?, ?, ?);"
            );
            sqlite3_stmt* InsertNoIDStmt = nullptr;
            if (!SQLite.PrepareStatement(InsertNoIDQuery, InsertNoIDStmt))
            {
                SQLite.RollbackTransaction();
                return false;
            }
            sqlite3_bind_int(InsertNoIDStmt, 1, Kingdom->KingdomID);
            sqlite3_bind_text(InsertNoIDStmt, 2, TCHAR_TO_UTF8(*Building->BuildingName), -1, SQLITE_TRANSIENT);
            sqlite3_bind_int(InsertNoIDStmt, 3, Building->Level);
            sqlite3_bind_int(InsertNoIDStmt, 4, Building->WorkersNeeded);
            sqlite3_bind_int(InsertNoIDStmt, 5, Building->WorkersAssigned);
            sqlite3_bind_int(InsertNoIDStmt, 6, Building->MaxWorkers);
            sqlite3_bind_int(InsertNoIDStmt, 7, Building->ConstructionDaysLeft);
            sqlite3_bind_int(InsertNoIDStmt, 8, Building->bOneTimeEffectApplied ? 1 : 0);

            if (sqlite3_step(InsertNoIDStmt) != SQLITE_DONE)
            {
                SQLite.FinalizeStatement(InsertNoIDStmt);
                SQLite.RollbackTransaction();
                return false;
            }

            // Get the new BuildingID
            Building->BuildingID = (int32)sqlite3_last_insert_rowid(SQLite.GetHandle());
            SQLite.FinalizeStatement(InsertNoIDStmt);
        }
        else
        {
            // Existing building, use the original statement
            sqlite3_bind_int(InsertStmt, 1, Kingdom->KingdomID);
            sqlite3_bind_int(InsertStmt, 2, Building->BuildingID);
            sqlite3_bind_text(InsertStmt, 3, TCHAR_TO_UTF8(*Building->BuildingName), -1, SQLITE_TRANSIENT);
            sqlite3_bind_int(InsertStmt, 4, Building->Level);
            sqlite3_bind_int(InsertStmt, 5, Building->WorkersNeeded);
            sqlite3_bind_int(InsertStmt, 6, Building->WorkersAssigned);
            sqlite3_bind_int(InsertStmt, 7, Building->MaxWorkers);
            sqlite3_bind_int(InsertStmt, 8, Building->ConstructionDaysLeft);
            sqlite3_bind_int(InsertStmt, 9, Building->bOneTimeEffectApplied ? 1 : 0);

            if (sqlite3_step(InsertStmt) != SQLITE_DONE)
            {
                SQLite.RollbackTransaction();
                SQLite.FinalizeStatement(InsertStmt);
                return false;
            }
        }
    }

    SQLite.FinalizeStatement(InsertStmt);
    return SQLite.CommitTransaction();
}
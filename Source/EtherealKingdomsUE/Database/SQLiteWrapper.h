// Copyright © 2025 CronoGames. All Rights Reserved.
// Author: Matt Brock

#pragma once

#include "CoreMinimal.h"

// Forward declaration of SQLite
struct sqlite3;
struct sqlite3_stmt;

class SQLiteWrapper
{
public:
    SQLiteWrapper();
    ~SQLiteWrapper();

    bool Open(const FString& DatabasePath);
    void Close();

    bool IsOpen() const
    {
        return DB != nullptr;
    }

    sqlite3* GetHandle() const
    {
        return DB;
    }

    bool ExecuteQuery(const FString& Query);
    bool PrepareStatement(const FString& Query, sqlite3_stmt*& OutStatement);
    void FinalizeStatement(sqlite3_stmt* Statement);

    FString GetColumnText(sqlite3_stmt* Statement, int ColumnIndex);
    int32 GetColumnInt(sqlite3_stmt* Statement, int ColumnIndex);

    bool GetSingleIntResult(const FString& Query, int32& OutValue);

    bool BindText(sqlite3_stmt* Statement, int Index, const FString& Value);

    bool BeginTransaction();
    bool CommitTransaction();
    bool RollbackTransaction();

private:
    sqlite3* DB;
};

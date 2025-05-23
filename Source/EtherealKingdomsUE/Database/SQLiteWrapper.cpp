// Copyright © 2025 CronoGames. All Rights Reserved.
// Author: Matt Brock

#include "SQLiteWrapper.h"
#include "Misc/Paths.h"
#include "HAL/PlatformFilemanager.h"

#include "sqlite3.h"

SQLiteWrapper::SQLiteWrapper()
    : DB(nullptr)
{
}

SQLiteWrapper::~SQLiteWrapper()
{
    Close();
}

bool SQLiteWrapper::Open(const FString& DatabasePath)
{
    Close(); // ensure no previous connection

    const FString FullPath = FPaths::ConvertRelativePathToFull(DatabasePath);
    int32 Result = sqlite3_open(TCHAR_TO_UTF8(*FullPath), &DB);
    return Result == SQLITE_OK;
}

void SQLiteWrapper::Close()
{
    if (DB)
    {
        sqlite3_close(DB);
        DB = nullptr;
    }
}

bool SQLiteWrapper::ExecuteQuery(const FString& Query)
{
    if (!DB) return false;

    char* ErrMsg = nullptr;
    int32 Result = sqlite3_exec(DB, TCHAR_TO_UTF8(*Query), nullptr, nullptr, &ErrMsg);

    if (ErrMsg)
    {
        UE_LOG(LogTemp, Error, TEXT("SQLite Error: %s"), UTF8_TO_TCHAR(ErrMsg));
        sqlite3_free(ErrMsg);
    }

    return Result == SQLITE_OK;
}

bool SQLiteWrapper::PrepareStatement(const FString& Query, sqlite3_stmt*& OutStatement)
{
    if (!DB) return false;

    const int32 Result = sqlite3_prepare_v2(DB, TCHAR_TO_UTF8(*Query), -1, &OutStatement, nullptr);
    return Result == SQLITE_OK;
}

void SQLiteWrapper::FinalizeStatement(sqlite3_stmt* Statement)
{
    if (Statement)
    {
        sqlite3_finalize(Statement);
    }
}

FString SQLiteWrapper::GetColumnText(sqlite3_stmt* Statement, int ColumnIndex)
{
    const unsigned char* Text = sqlite3_column_text(Statement, ColumnIndex);
    return Text ? UTF8_TO_TCHAR(reinterpret_cast<const char*>(Text)) : TEXT("");
}

int32 SQLiteWrapper::GetColumnInt(sqlite3_stmt* Statement, int ColumnIndex)
{
    return sqlite3_column_int(Statement, ColumnIndex);
}

bool SQLiteWrapper::GetSingleIntResult(const FString& Query, int32& OutValue)
{
    sqlite3_stmt* Statement = nullptr;
    if (!PrepareStatement(Query, Statement))
        return false;

    bool bResult = false;
    if (sqlite3_step(Statement) == SQLITE_ROW)
    {
        OutValue = sqlite3_column_int(Statement, 0);
        bResult = true;
    }

    FinalizeStatement(Statement);
    return bResult;
}

bool SQLiteWrapper::BindText(sqlite3_stmt* Statement, int Index, const FString& Value)
{
    return sqlite3_bind_text(Statement, Index, TCHAR_TO_UTF8(*Value), -1, SQLITE_TRANSIENT) == SQLITE_OK;
}

bool SQLiteWrapper::BeginTransaction()
{
    return ExecuteQuery(TEXT("BEGIN TRANSACTION;"));
}

bool SQLiteWrapper::CommitTransaction()
{
    return ExecuteQuery(TEXT("COMMIT;"));
}

bool SQLiteWrapper::RollbackTransaction()
{
    return ExecuteQuery(TEXT("ROLLBACK;"));
}

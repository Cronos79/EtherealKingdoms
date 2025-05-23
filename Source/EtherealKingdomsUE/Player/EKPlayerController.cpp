// Copyright © 2025 CronoGames. All Rights Reserved.
// Author: Matt Brock

#include "EKPlayerController.h"
#include <Kismet/GameplayStatics.h>
#include <EtherealKingdomsUE/Core/EKGameModeBase.h>

AEKPlayerController::AEKPlayerController()
{
    bShowMouseCursor = true;
    bEnableClickEvents = true;
    bEnableMouseOverEvents = true;
}

void AEKPlayerController::BeginPlay()
{
    Super::BeginPlay();

    if (IsLocalController() && GetNetMode() == NM_Standalone)
    {
#if !UE_BUILD_SHIPPING
        const FString ServerIP = TEXT("127.0.0.1");
        const FString Command = FString::Printf(TEXT("open %s"), *ServerIP);
        ConsoleCommand(Command, true);
#endif
    }
}

/*********************************************************************************************/
// User: Public Methods

void AEKPlayerController::AttemptLogin(const FString& Username, const FString& Password)
{
    if (!HasAuthority())
    {
        ServerAttemptLogin(Username, Password);
    }
}

void AEKPlayerController::RegisterUser(const FString& Username, const FString& Password)
{
    if (!HasAuthority())
    {
        ServerAttemptRegisterUser(Username, Password);
    }
}

bool AEKPlayerController::IsLoggedIn() const
{
    return bIsLoggedIn;
}


void AEKPlayerController::LogoutUser()
{
    if (HasAuthority())
    {
        if (AEKGameModeBase* GameMode = Cast<AEKGameModeBase>(UGameplayStatics::GetGameMode(this)))
        {
            if (DatabaseManager* DB = GameMode->GetDatabaseManager())
            {
                if (!DB->SaveKingdomData(CurrentKingdom))
                {
                    UE_LOG(LogTemp, Warning, TEXT("Failed to save kingdom data on logout."));
                }
            }
        }
    }

    bIsLoggedIn = false;
    SetUserProfile(FUserRecord());
    SetKingdomData(NewObject<UEtherealKingdom>());

    UE_LOG(LogTemp, Log, TEXT("Player logged out."));
}

const FUserRecord& AEKPlayerController::GetUserProfile() const
{
    return CurrentUser;
}

/*********************************************************************************************/
// User: Server RPCs

void AEKPlayerController::ServerAttemptLogin_Implementation(const FString& Username, const FString& Password)
{
    if (AEKGameModeBase* GameMode = Cast<AEKGameModeBase>(UGameplayStatics::GetGameMode(this)))
    {
        if (DatabaseManager* DBManager = GameMode->GetDatabaseManager())
        {
            FUserRecord UserRecord;
            const bool bSuccess = DBManager->GetUserRecord(Username, Password, UserRecord);
            HandleLoginResult(bSuccess, UserRecord);
        }
    }
}

void AEKPlayerController::ServerAttemptRegisterUser_Implementation(const FString& Username, const FString& Password)
{
    if (AEKGameModeBase* GameMode = Cast<AEKGameModeBase>(UGameplayStatics::GetGameMode(this)))
    {
        if (DatabaseManager* DBManager = GameMode->GetDatabaseManager())
        {
            const bool bSuccess = DBManager->RegisterUser(Username, Password);
            HandleRegisterResult(bSuccess);
        }
    }
}

/*********************************************************************************************/
// User: Server Logic

void AEKPlayerController::HandleLoginResult(bool bSuccess, const FUserRecord& User)
{
    UE_LOG(LogTemp, Log, TEXT("HandleLoginResult: bSuccess=%d, KingdomID=%d"), bSuccess, User.KingdomID);
    bIsLoggedIn = bSuccess;
    if (bSuccess)
    {
        SetUserProfile(User);
    }

    // Attempt to load their kingdom
    if (User.KingdomID != -1)
    {
        UE_LOG(LogTemp, Log, TEXT("Attempting to load kingdom for ID: %d"), User.KingdomID);
		UEtherealKingdom* LoadedKingdom = NewObject<UEtherealKingdom>();
        if (AEKGameModeBase* GameMode = Cast<AEKGameModeBase>(UGameplayStatics::GetGameMode(this)))
        {
            if (DatabaseManager* DB = GameMode->GetDatabaseManager())
            {
                if (DB->LoadKingdomData(User.KingdomID, LoadedKingdom))
                {
                    SetKingdomData(LoadedKingdom);
                    UE_LOG(LogTemp, Log, TEXT("Loaded Kingdom: %s"), *CurrentKingdom->KingdomName);
                }
                else
                {
                    UE_LOG(LogTemp, Warning, TEXT("Failed to load kingdom data for KingdomID: %d"), User.KingdomID);
                }
            }
        }
    }
    ClientReceiveLoginResult(bSuccess, User);
}

void AEKPlayerController::HandleRegisterResult(bool bSuccess)
{
    ClientReceiveRegisterResult(bSuccess);
}

/*********************************************************************************************/
// User: Client RPCs

void AEKPlayerController::ClientReceiveLoginResult_Implementation(bool bWasSuccessful, const FUserRecord& User)
{
    bIsLoggedIn = bWasSuccessful;
    SetUserProfile(bWasSuccessful ? User : FUserRecord());
    OnLoginResult.Broadcast(bWasSuccessful);
}

void AEKPlayerController::ClientReceiveRegisterResult_Implementation(bool bWasSuccessful)
{
    OnRegisterResult.Broadcast(bWasSuccessful);
}

void AEKPlayerController::ClientUpdateUserProfile_Implementation(const FUserRecord& UpdatedUser)
{
    SetUserProfile(UpdatedUser);
}

/*********************************************************************************************/
// Internal Utility

void AEKPlayerController::SetUserProfile(const FUserRecord& User)
{
    CurrentUser = User;

    // Always sync to client if we're on the server
    if (HasAuthority())
    {
        ClientUpdateUserProfile(User);
    }
}

/*********************************************************************************************/
// Kingdom: Public Methods

void AEKPlayerController::CreateKingdom(const FString& KingdomName)
{
    if (!HasAuthority())
        ServerCreateKingdom(KingdomName);
}

const UEtherealKingdom* AEKPlayerController::GetKingdom() const
{
    return CurrentKingdom;
}

bool AEKPlayerController::SpendResource(const FString& ResourceName, int32 Amount)
{
    if (!HasAuthority()) return false;

    if (AEKGameModeBase* GameMode = Cast<AEKGameModeBase>(UGameplayStatics::GetGameMode(this)))
    {
        if (DatabaseManager* DB = GameMode->GetDatabaseManager())
        {
            return DB->ConsumeResource(CurrentUser.KingdomID, ResourceName, Amount);
        }
    }
    return false;
}

void AEKPlayerController::SetKingdomData(UEtherealKingdom* Kingdom)
{
	// logging kingdom data
	UE_LOG(LogTemp, Log, TEXT("Setting Kingdom Data: %s"), *Kingdom->KingdomName);
    CurrentKingdom = Kingdom;

    // Always update the client from server
    if (HasAuthority())
    {
		// Logging kingdom data
		UE_LOG(LogTemp, Log, TEXT("Setting Kingdom Data: %s"), *Kingdom->KingdomName);
	
		// Update the client with the new kingdom data
        FKingdomData Data;
        Data.KingdomID = Kingdom->KingdomID;
        Data.KingdomName = Kingdom->KingdomName;
        Data.OwnerUsername = Kingdom->OwnerUsername;
        Data.Resources = Kingdom->Resources;
        Data.Stats = Kingdom->Stats;
        ClientUpdateKingdomData(Data);
    }
}


void AEKPlayerController::SaveKingdom()
{
    if (!HasAuthority()) return;

    if (AEKGameModeBase* GameMode = Cast<AEKGameModeBase>(UGameplayStatics::GetGameMode(this)))
    {
        if (DatabaseManager* DB = GameMode->GetDatabaseManager())
        {
            if (DB->SaveKingdomData(CurrentKingdom))
            {
                UE_LOG(LogTemp, Log, TEXT("Manual save succeeded."));
            }
            else
            {
                UE_LOG(LogTemp, Warning, TEXT("Manual save failed."));
            }
        }
    }
}

/*********************************************************************************************/
// Kingdom: Server RPCs

void AEKPlayerController::ServerCreateKingdom_Implementation(const FString& KingdomName)
{
    if (AEKGameModeBase* GameMode = Cast<AEKGameModeBase>(UGameplayStatics::GetGameMode(this)))
    {
        if (DatabaseManager* DB = GameMode->GetDatabaseManager())
        {
            UE_LOG(LogTemp, Log, TEXT("Attempting to create kingdom for user: %s"), *CurrentUser.Username);
            int32 NewKingdomID = -1;
            const bool bSuccess = DB->CreateKingdom(CurrentUser.Username, KingdomName, NewKingdomID);

            if (bSuccess)
            {
                FUserRecord UpdatedUser = CurrentUser;
                UpdatedUser.KingdomID = NewKingdomID;
                SetUserProfile(UpdatedUser);
                bIsLoggedIn = true;
                ClientUpdateUserProfile(CurrentUser);

                // Load the new kingdom data
                UEtherealKingdom* NewKingdom = NewObject<UEtherealKingdom>();
                if (DB->LoadKingdomData(NewKingdomID, NewKingdom))
                {
                    SetKingdomData(NewKingdom);
                }
                else
                {
                    UE_LOG(LogTemp, Warning, TEXT("Failed to load new kingdom data after creation!"));
                }
            }
		
            ClientReceiveCreateKingdomResult(bSuccess);
        }
    }
    else
    {
        ClientReceiveCreateKingdomResult(false);
    }
}

/*********************************************************************************************/
// Kingdom: Client RPCs

void AEKPlayerController::ClientReceiveCreateKingdomResult_Implementation(bool bSuccess)
{
    OnCreateKingdomResult.Broadcast(bSuccess);
}

void AEKPlayerController::ClientUpdateKingdomData_Implementation(const FKingdomData& Data)
{
    if (!CurrentKingdom)
        CurrentKingdom = NewObject<UEtherealKingdom>(this);

    CurrentKingdom->KingdomID = Data.KingdomID;
    CurrentKingdom->KingdomName = Data.KingdomName;
    CurrentKingdom->OwnerUsername = Data.OwnerUsername;
    CurrentKingdom->Resources = Data.Resources;
    CurrentKingdom->Stats = Data.Stats;
	// Log the Current Kingdom Name
	UE_LOG(LogTemp, Log, TEXT("Current Kingdom Name: %s"), *CurrentKingdom->KingdomName);
}
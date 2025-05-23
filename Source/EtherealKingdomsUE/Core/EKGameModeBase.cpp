// Copyright © 2025 CronoGames. All Rights Reserved.
// Author: Matt Brock


#include "EKGameModeBase.h"
#include "GameFramework/PlayerController.h"
#include "EKGameStateBase.h"
#include "Kismet/GameplayStatics.h"
#include "../Player/EKPlayerController.h"
#include "../Player/EKPlayerPawn.h"



AEKGameModeBase::AEKGameModeBase()
{
    DBManager = MakeUnique<DatabaseManager>();
}


AEKGameModeBase::~AEKGameModeBase()
{
    
}

void AEKGameModeBase::BeginPlay()
{
    Super::BeginPlay();

	World = GetWorld();

    if (IsRunningDedicatedServer())
    {
        FString DBPath = FPaths::Combine(FPaths::ProjectSavedDir(), TEXT("EtherealGameData.db"));
        if (!DBManager->OpenDatabase(DBPath))
        {
            UE_LOG(LogTemp, Error, TEXT("Failed to open database: %s"), *DBPath);
        }
        else
        {
            UE_LOG(LogTemp, Log, TEXT("Database opened successfully at: %s"), *DBPath);
        }

        if (!KingdomManager)
        {
            KingdomManager = NewObject<UKingdomManager>(this);
            KingdomManager->Initialize(this);
            UE_LOG(LogTemp, Log, TEXT("KingdomManager initialized and kingdoms loaded."));
            UE_LOG(LogTemp, Log, TEXT("Loaded %d kingdoms."), KingdomManager->GetKingdoms().Num());
        }

        if (!TickManager)
        {
            TickManager = NewObject<UGameTickManager>(this);
            TickManager->Initialize(this);
            UE_LOG(LogTemp, Log, TEXT("GameTickManager initialized with starting date: %s"), *TickManager->GetCurrentDate().ToString());
        }

        if (World)
        {
            World->GetTimerManager().SetTimer(
                TickTimerHandle, this, &AEKGameModeBase::AdvanceGameDay, RealTimePerDay, true
            );
        }
    }
}

void AEKGameModeBase::PostLogin(APlayerController* NewPlayer)
{
    Super::PostLogin(NewPlayer);
}

void AEKGameModeBase::Logout(AController* Exiting)
{
    Super::Logout(Exiting);

    if (AEKPlayerController* PC = Cast<AEKPlayerController>(Exiting))
    {
        const FUserRecord& Profile = PC->GetUserProfile();
        if (DatabaseManager* DB = GetDatabaseManager())
        {
            // Optional future logic: Save stats, location, etc.
            UE_LOG(LogTemp, Log, TEXT("User '%s' disconnected."), *Profile.Username);
        }
    }
}

void AEKGameModeBase::SaveAllKingdoms()
{
    if (KingdomManager)
    {
        KingdomManager->SaveAllKingdoms();
        UE_LOG(LogTemp, Log, TEXT("All kingdoms saved manually."));
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("KingdomManager is null. Cannot save."));
    }
}

void AEKGameModeBase::AdvanceGameDay()
{
    if (TickManager)
    {
        TickManager->TickOneDay();
    }

    AEKGameStateBase* EKGameState = Cast<AEKGameStateBase>(UGameplayStatics::GetGameState(this));
    if (EKGameState && TickManager)
    {
        EKGameState->CurrentDate = TickManager->GetCurrentDate();
    }
}

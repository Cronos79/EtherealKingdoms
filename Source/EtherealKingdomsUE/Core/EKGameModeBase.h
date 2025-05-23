// Copyright © 2025 CronoGames. All Rights Reserved.
// Author: Matt Brock

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include <EtherealKingdomsUE/Database/DatabaseManager.h>
#include "GameTickManager.h"
#include "KingdomManager.h"
#include "EKGameModeBase.generated.h"

/**
 * 
 */
UCLASS()
class ETHEREALKINGDOMSUE_API AEKGameModeBase : public AGameModeBase
{
	GENERATED_BODY()
	
public:
	AEKGameModeBase();
	virtual ~AEKGameModeBase();

protected:
	virtual void BeginPlay() override;

	virtual void PostLogin(APlayerController* NewPlayer) override;

	// Logout
	virtual void Logout(AController* Exiting) override;
	
public:
	DatabaseManager* GetDatabaseManager() const	{ return DBManager.Get(); }

	UGameTickManager* GetTickManager() const { return TickManager; }

	UKingdomManager* GetKingdomManager() const { return KingdomManager; }

	UFUNCTION(BlueprintCallable, Category = "Debug")
	void SaveAllKingdoms(); // Manual save trigger

	UPROPERTY()
	UGameTickManager* TickManager;

protected:
	void AdvanceGameDay();

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Game Settings")
	float RealTimePerDay = 3600.0f; // Time in seconds for one game day
private:
	TUniquePtr<DatabaseManager> DBManager;

	// Timer handle for game tick
	FTimerHandle TickTimerHandle;

	UPROPERTY()
    UKingdomManager* KingdomManager;

	UWorld* World;
};


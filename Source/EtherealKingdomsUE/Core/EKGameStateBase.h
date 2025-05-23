// Copyright © 2025 CronoGames. All Rights Reserved.
// Author: Matt Brock

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "GameDate.h"
#include "Net/UnrealNetwork.h"
#include "EKGameStateBase.generated.h"

/**
 * 
 */
UCLASS()
class ETHEREALKINGDOMSUE_API AEKGameStateBase : public AGameStateBase
{
	GENERATED_BODY()
	
public:
    AEKGameStateBase();

protected:
    virtual void Tick(float DeltaSeconds) override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

public:
	// GameDate CurrentDate;
	UPROPERTY(Replicated, BlueprintReadOnly)
	FGameDate CurrentDate;

	UFUNCTION(BlueprintCallable, Category = "Game Date")
	FString GetFormattedDate() const;
};

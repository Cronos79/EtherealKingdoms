// Copyright © 2025 CronoGames. All Rights Reserved.  
// Author: Matt Brock

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "GameDate.h"
#include "GameTickManager.generated.h"

class AEKGameModeBase;

UCLASS()
class ETHEREALKINGDOMSUE_API UGameTickManager : public UObject
{
	GENERATED_BODY()
	
public:
	UGameTickManager();

	void Initialize(AEKGameModeBase* InGameMode);
	void TickOneDay();

	const FGameDate& GetCurrentDate() const
	{
		return CurrentDate;
	}

private:
	FGameDate CurrentDate;
	AEKGameModeBase* GameModeRef;

	void ProcessKingdoms();
	void ProcessWorldEvents();	
};

// Copyright © 2025 CronoGames. All Rights Reserved.  
// Author: Matt Brock

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "../Kingdom/EtherealKingdom.h"
#include "KingdomManager.generated.h"

class AEKGameModeBase;
//class DatabaseManager;

UCLASS()
class ETHEREALKINGDOMSUE_API UKingdomManager : public UObject
{
	GENERATED_BODY()	

public:
    void Initialize(AEKGameModeBase* InGameMode);

    void LoadAllKingdoms();
    void TickKingdoms();
    void SaveAllKingdoms();

    TMap<int32, UEtherealKingdom*>& GetKingdoms();
    bool GetKingdomByID(int32 KingdomID, UEtherealKingdom* OutRecord);
    bool UpdateKingdom(UEtherealKingdom* UpdatedRecord);

private:
    AEKGameModeBase* GameModeRef = nullptr;

    UPROPERTY()
    TMap<int32, UEtherealKingdom*> Kingdoms;
};

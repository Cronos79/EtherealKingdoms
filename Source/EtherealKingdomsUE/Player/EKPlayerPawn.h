// Copyright © 2025 CronoGames. All Rights Reserved.
// Author: Matt Brock

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "EKPlayerPawn.generated.h"

UCLASS()
class ETHEREALKINGDOMSUE_API AEKPlayerPawn : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AEKPlayerPawn();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;	
};

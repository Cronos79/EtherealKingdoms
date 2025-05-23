// Copyright © 2025 CronoGames. All Rights Reserved.
// Author: Matt Brock


#include "EKGameStateBase.h"

AEKGameStateBase::AEKGameStateBase()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AEKGameStateBase::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);
}

void AEKGameStateBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AEKGameStateBase, CurrentDate);
}

FString AEKGameStateBase::GetFormattedDate() const
{
	return CurrentDate.ToString();
}

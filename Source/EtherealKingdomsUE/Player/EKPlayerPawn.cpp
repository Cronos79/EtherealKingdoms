// Copyright © 2025 CronoGames. All Rights Reserved.
// Author: Matt Brock


#include "EKPlayerPawn.h"


// Sets default values
AEKPlayerPawn::AEKPlayerPawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	// Root component just so it doesn't crash
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
}

// Called when the game starts or when spawned
void AEKPlayerPawn::BeginPlay()
{
	Super::BeginPlay();
	UE_LOG(LogTemp, Log, TEXT("EKPlayerPawn::BeginPlay triggered"));
}


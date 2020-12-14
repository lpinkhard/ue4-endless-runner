// Copyright Likotech Software. All Rights Reserved.

#include "TP_EndlessRunnerGameMode.h"
#include "TP_EndlessRunnerCharacter.h"
#include "TP_EndlessRunnerFloorTile.h"
#include "TP_EndlessRunnerPlayerController.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "UObject/ConstructorHelpers.h"

ATP_EndlessRunnerGameMode::ATP_EndlessRunnerGameMode()
{
	// set default player controller to Blueprinted controller
	static ConstructorHelpers::FClassFinder<APlayerController> PlayerControllerBPClass(TEXT("/Game/EndlessRunner/Blueprints/EndlessRunnerPlayerController"));
	if (PlayerControllerBPClass.Class != NULL)
	{
		PlayerControllerClass = PlayerControllerBPClass.Class;
	}

	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/EndlessRunner/Blueprints/EndlessRunnerCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}

	// default spawn point
	NextSpawnPoint = FTransform();

}

// Called when the game starts
void ATP_EndlessRunnerGameMode::BeginPlay()
{
	Super::BeginPlay();

	// Spawn safe tiles in the beginning
	for (int i = 0; i <= 2; i++)
	{
		AddSafeFloorTile();
	}

	// Spawn normal tiles afterwards
	for (int i = 0; i <= 4; i++)
	{
		AddFloorTile();
	}

}

void ATP_EndlessRunnerGameMode::AddFloorTile()
{
	FActorSpawnParameters SpawnInfo;
	ATP_EndlessRunnerFloorTile* NewTile;

	// Abort if there are no tiles
	if (FloorTiles.Num() < 1)
	{
		return;
	}

	// Select a class of tile to spawn
	float Selection = FMath::RandRange(0, FloorTiles.Num() - 1);
	TSubclassOf<ATP_EndlessRunnerFloorTile> TileClass = FloorTiles[Selection];

	// Spawn the tile
	NewTile = GetWorld()->SpawnActor<ATP_EndlessRunnerFloorTile>(TileClass, NextSpawnPoint, SpawnInfo);

	// Save the next spawn point
	NextSpawnPoint = NewTile->GetAttachTransform();

}

void ATP_EndlessRunnerGameMode::AddSafeFloorTile()
{
	FActorSpawnParameters SpawnInfo;
	ATP_EndlessRunnerFloorTile* NewTile;

	// Abort if there are no tiles
	if (SafeFloorTiles.Num() < 1)
	{
		return;
	}

	// Select a class of tile to spawn
	float Selection = FMath::RandRange(0, SafeFloorTiles.Num() - 1);
	TSubclassOf<ATP_EndlessRunnerFloorTile> TileClass = SafeFloorTiles[Selection];

	// Spawn the tile
	NewTile = GetWorld()->SpawnActor<ATP_EndlessRunnerFloorTile>(TileClass, NextSpawnPoint, SpawnInfo);

	// Save the next spawn point
	NextSpawnPoint = NewTile->GetAttachTransform();

}
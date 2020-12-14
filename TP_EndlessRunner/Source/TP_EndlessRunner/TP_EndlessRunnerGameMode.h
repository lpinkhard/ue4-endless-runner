// Copyright Likotech Software. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "TP_EndlessRunnerGameMode.generated.h"

/**
 * 
 */
UCLASS(minimalapi)
class ATP_EndlessRunnerGameMode : public AGameModeBase
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, Category = Spawning)
	TArray<TSubclassOf<class ATP_EndlessRunnerFloorTile>> FloorTiles;

	UPROPERTY(EditDefaultsOnly, Category = Spawning)
	TArray<TSubclassOf<class ATP_EndlessRunnerFloorTile>> SafeFloorTiles;

	/** Transform indicating the next tile spawn point */
	FTransform NextSpawnPoint;

public:
	ATP_EndlessRunnerGameMode();

	// Called to spawn the next tile
	void AddFloorTile();

	// Called to spawn a safe tile
	void AddSafeFloorTile();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

};

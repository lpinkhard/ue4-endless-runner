// Copyright Likotech Software. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TP_EndlessRunnerFloorTile.generated.h"

UCLASS(minimalapi)
class ATP_EndlessRunnerFloorTile : public AActor
{
	GENERATED_BODY()
	
	/** Root scene component */
	class USceneComponent* Scene;

	/** Floor mesh */
	UPROPERTY(EditDefaultsOnly, Category = "Meshes")
	class UStaticMeshComponent* Floor;

	/** Tile attachment point */
	UPROPERTY(EditDefaultsOnly, Category = "Attachment")
	class UArrowComponent* AttachPoint;

	/** End trigger volume */
	UPROPERTY(EditDefaultsOnly, Category = "Attachment")
	class UBoxComponent* EndTrigger;

	/** Blocker spawn, middle lane */
	UPROPERTY(EditDefaultsOnly, Category = "Blockers")
	UArrowComponent* SpawnPoint;

	/** Blocker spawn, left lane */
	UPROPERTY(EditDefaultsOnly, Category = "Blockers")
	UArrowComponent* SpawnPointL;

	/** Blocker spawn, right lane */
	UPROPERTY(EditDefaultsOnly, Category = "Blockers")
	UArrowComponent* SpawnPointR;

	/** Whether to spawn left and right blockers */
	UPROPERTY(EditDefaultsOnly, Category = "Blockers")
	bool bSpawnOnSides;

	/** Lane spline, middle lane */
	UPROPERTY(EditDefaultsOnly, Category = "Splines")
	class USplineComponent* Spline;

	/** Lane spline, left lane */
	UPROPERTY(EditDefaultsOnly, Category = "Splines")
	USplineComponent* SplineL;

	/** Lane spline, right lane */
	UPROPERTY(EditDefaultsOnly, Category = "Splines")
	USplineComponent* SplineR;

	/** Item spawn area */
	UPROPERTY(EditDefaultsOnly, Category = "Items")
	UBoxComponent* ItemArea;

	/** Whether to spawn items on splines */
	UPROPERTY(EditDefaultsOnly, Category = "Items")
	bool bSpawnOnSplines;

	/** Spawn point transforms for object spawning */
	TArray<FTransform> SpawnPoints;

public:	
	// Sets default values for this actor's properties
	ATP_EndlessRunnerFloorTile();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Called when the tile should be destroyed
	virtual void DestroyTile();

public:	
	// Return the attachment transform */
	FTransform GetAttachTransform();

	// Items to spawn
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Spawning)
	TArray<TSubclassOf<class ATP_EndlessRunnerItem>> ItemsToSpawn;

	// Blockers to spawn
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Spawning)
	TArray<TSubclassOf<class ATP_EndlessRunnerBlocker>> BlockersToSpawn;

	// Called to spawn blockers
	void SpawnBlocker();

	// Called to spawn items
	void SpawnItems();

	// Returns the spline for the specified lane
	USplineComponent* GetSpline(int8 Lane);

	UFUNCTION()
	virtual void OnEndTriggerOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

};

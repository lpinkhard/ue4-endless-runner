// Copyright Likotech Software. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TP_EndlessRunnerItem.generated.h"

UCLASS(minimalapi)
class ATP_EndlessRunnerItem : public AActor
{
	GENERATED_BODY()

	/** Root scene component */
	class USceneComponent* Scene;

	/** Mesh for the item */
	UPROPERTY(EditDefaultsOnly, Category = "Mesh")
	class UStaticMeshComponent* StaticMesh;

	/** Particle system component for idle item */
	UPROPERTY(EditDefaultsOnly, Category = "Game Item | Particles")
	class UParticleSystemComponent* IdleParticlesComponent;

	/** Particle system for item interaction */
	UPROPERTY(EditDefaultsOnly, Category = "Game Item | Particles")
	class UParticleSystem* InteractParticles;

public:	
	// Sets default values for this actor's properties
	ATP_EndlessRunnerItem();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	/** Score value when collected */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Game Item | Score")
	int32 ScoreValue;

	// Called on overlap with item
	UFUNCTION()
	virtual void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
};

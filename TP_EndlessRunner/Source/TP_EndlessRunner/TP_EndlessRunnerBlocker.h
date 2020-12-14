// Copyright Likotech Software. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TP_EndlessRunnerBlocker.generated.h"

UCLASS(minimalapi)
class ATP_EndlessRunnerBlocker : public AActor
{
	GENERATED_BODY()

	/** Static mesh for blocker */
	UPROPERTY(EditDefaultsOnly, Category = Mesh)
	class UStaticMeshComponent* StaticMesh;

	/** Should object spawn rotated */
	UPROPERTY(EditDefaultsOnly, Category = Spawning)
	bool bRandomRotation;

public:	
	// Sets default values for this actor's properties
	ATP_EndlessRunnerBlocker();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called when the object is hit
	UFUNCTION()
	void OnComponentHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

};

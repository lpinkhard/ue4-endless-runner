// Copyright Likotech Software. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TP_EndlessRunnerMovingPlatform.generated.h"

UCLASS()
class TP_ENDLESSRUNNER_API ATP_EndlessRunnerMovingPlatform : public AActor
{
	GENERATED_BODY()

	/** Platform state */
	FVector StartPoint;
	FTimerHandle MoveTimer;
	float Distance;
	bool bMoving;

public:	
	// Sets default values for this actor's properties
	ATP_EndlessRunnerMovingPlatform();

	/** Mesh for the platform */
	UPROPERTY(EditDefaultsOnly, Category = Mesh)
	class UStaticMeshComponent* Mesh;

	UPROPERTY(EditAnywhere, meta = (MakeEditWidget = "true"))
	FVector EndPoint;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Motion)
	float MoveSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Motion)
	float ReverseTime;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to start or stop the platform motion
	void ToggleMovement();

};

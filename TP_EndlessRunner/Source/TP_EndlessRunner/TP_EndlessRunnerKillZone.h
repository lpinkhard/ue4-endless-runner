// Copyright Likotech Software. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TP_EndlessRunnerKillZone.generated.h"

UCLASS()
class TP_ENDLESSRUNNER_API ATP_EndlessRunnerKillZone : public AActor
{
	GENERATED_BODY()

	/** Kill zone volume */
	UPROPERTY(EditDefaultsOnly, Category = Zone)
	class UBoxComponent* KillZone;
	
public:	
	// Sets default values for this actor's properties
	ATP_EndlessRunnerKillZone();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called on overlap with kill zone
	UFUNCTION()
	virtual void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
};

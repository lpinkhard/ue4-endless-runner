// Copyright Likotech Software. All Rights Reserved.

#include "TP_EndlessRunnerKillZone.h"
#include "TP_EndlessRunnerCharacter.h"
#include "Components/BoxComponent.h"

// Sets default values
ATP_EndlessRunnerKillZone::ATP_EndlessRunnerKillZone()
{
	// Kill zones don't need to call Tick() every frame.
	PrimaryActorTick.bCanEverTick = false;

	// Create kill zone box
	KillZone = CreateDefaultSubobject<UBoxComponent>(TEXT("KillZone"));
	SetRootComponent(KillZone);
	
	// Set default extent
	KillZone->SetBoxExtent(FVector(1050.f, 1050.f, 50.f));

}

// Called when the game starts or when spawned
void ATP_EndlessRunnerKillZone::BeginPlay()
{
	Super::BeginPlay();
	
	// Bind overlap event
	KillZone->OnComponentBeginOverlap.AddDynamic(this, &ATP_EndlessRunnerKillZone::OnOverlapBegin);

}

// Called on overlap with kill zone
void ATP_EndlessRunnerKillZone::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// Check if other actor is player and cast as appropriate
	if (OtherActor)
	{
		ATP_EndlessRunnerCharacter* Player = Cast<ATP_EndlessRunnerCharacter>(OtherActor);
		if (Player)
		{
			// kill player
			Player->Death();
		}
	}

}
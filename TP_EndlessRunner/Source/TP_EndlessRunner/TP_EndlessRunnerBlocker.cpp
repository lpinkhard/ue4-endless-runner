// Copyright Likotech Software. All Rights Reserved.

#include "TP_EndlessRunnerBlocker.h"
#include "TP_EndlessRunnerCharacter.h"
#include "Components/StaticMeshComponent.h"

// Sets default values
ATP_EndlessRunnerBlocker::ATP_EndlessRunnerBlocker()
{
	// Blockers don't need to call Tick() every frame.
	PrimaryActorTick.bCanEverTick = false;

	// Create static mesh and set as root
	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	SetRootComponent(StaticMesh);

	// Set up collision on mesh
	StaticMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	StaticMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	StaticMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Block);

	// Do not allow objects to be rotated by default
	bRandomRotation = false;

}

// Called when the game starts or when spawned
void ATP_EndlessRunnerBlocker::BeginPlay()
{
	Super::BeginPlay();
	
	// Bind hit event
	StaticMesh->OnComponentHit.AddDynamic(this, &ATP_EndlessRunnerBlocker::OnComponentHit);

	// Rotate object randomly, if applicable
	if (bRandomRotation)
	{
		SetActorRotation(FRotator(0.f, FMath::RandRange(0.f, 360.f), 0.f));
	}

}

// Called when the object is hit
void ATP_EndlessRunnerBlocker::OnComponentHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
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
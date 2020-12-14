// Copyright Likotech Software. All Rights Reserved.

#include "TP_EndlessRunnerItem.h"
#include "TP_EndlessRunnerCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"

// Sets default values
ATP_EndlessRunnerItem::ATP_EndlessRunnerItem()
{
 	// Items don't call Tick() every frame
	PrimaryActorTick.bCanEverTick = false;

	// Create static mesh for item.  Create a derived class to use a skeletal mesh instead.
	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	SetRootComponent(StaticMesh);

	// Set up the static mesh
	StaticMesh->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	StaticMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	StaticMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);

	// Create idle particles component
	IdleParticlesComponent = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("IdleParticlesComponent"));
	IdleParticlesComponent->SetupAttachment(GetRootComponent());

	// Default to 1 point value
	ScoreValue = 1;
}

// Called when the game starts or when spawned
void ATP_EndlessRunnerItem::BeginPlay()
{
	Super::BeginPlay();
	
	// Bind overlap event
	StaticMesh->OnComponentBeginOverlap.AddDynamic(this, &ATP_EndlessRunnerItem::OnOverlapBegin);

	// Make sure the item doesn't overlap something on spawn
	TArray<AActor*> OutActors;
	GetOverlappingActors(OutActors);
	for (AActor* OverActor : OutActors)
	{
		// Make sure not overlapping itself
		if (OverActor->GetUniqueID() != GetUniqueID())
		{
			// Overlapping, destroy
			Destroy();
		}
	}
}

// Called on overlap with item
void ATP_EndlessRunnerItem::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// Check other actor and cast if player
	if (OtherActor)
	{
		ATP_EndlessRunnerCharacter* Player = Cast<ATP_EndlessRunnerCharacter>(OtherActor);
		if (Player)
		{
			// Spawn emitter for interaction particles
			if (InteractParticles)
			{
				UGameplayStatics::SpawnEmitterAtLocation(this, InteractParticles, GetActorLocation());
			}

			// Increase player score and destroy item
			Player->IncreaseScore(ScoreValue);
			Destroy();
		}
	}
}
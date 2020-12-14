// Copyright Likotech Software. All Rights Reserved.

#include "TP_EndlessRunnerFloorTile.h"
#include "TP_EndlessRunnerCharacter.h"
#include "TP_EndlessRunnerGameMode.h"
#include "TP_EndlessRunnerBlocker.h"
#include "TP_EndlessRunnerItem.h"
#include "Components/ArrowComponent.h"
#include "Components/BoxComponent.h"
#include "Components/ChildActorComponent.h"
#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SplineComponent.h"
#include "TimerManager.h"
#include "UObject/ConstructorHelpers.h"

// Sets default values
ATP_EndlessRunnerFloorTile::ATP_EndlessRunnerFloorTile()
{
 	// Tiles do not need to call Tick() every frame
	PrimaryActorTick.bCanEverTick = false;

	// Create root scene component
	Scene = CreateDefaultSubobject<USceneComponent>(TEXT("Scene"));
	SetRootComponent(Scene);

	// Create the floor mesh
	Floor = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Floor"));
	Floor->SetupAttachment(GetRootComponent());
	Floor->SetRelativeLocation(FVector(1000.f, 0.f, 0.f));

	// Create the attachment point
	AttachPoint = CreateDefaultSubobject<UArrowComponent>(TEXT("AttachPoint"));
	AttachPoint->SetupAttachment(GetRootComponent());
	AttachPoint->SetRelativeLocation(FVector(2000.f, 0.f, 0.f));

	// Create the end trigger volume
	EndTrigger = CreateDefaultSubobject<UBoxComponent>(TEXT("EndTrigger"));
	EndTrigger->SetupAttachment(GetRootComponent());
	EndTrigger->SetRelativeLocation(FVector(2000.f, 0.f, 380.f));
	EndTrigger->SetBoxExtent(FVector(32.f, 1000.f, 400.f));

	// Setup collision for end trigger
	EndTrigger->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	EndTrigger->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);

	// Create object spawn points
	SpawnPoint = CreateDefaultSubobject<UArrowComponent>(TEXT("SpawnPoint"));
	SpawnPoint->SetupAttachment(GetRootComponent());
	SpawnPoint->SetRelativeLocation(FVector(890.f, 0.f, 30.f));

	SpawnPointL = CreateDefaultSubobject<UArrowComponent>(TEXT("SpawnPointL"));
	SpawnPointL->SetupAttachment(GetRootComponent());
	SpawnPointL->SetRelativeLocation(FVector(890.f, -290.f, 30.f));

	SpawnPointR = CreateDefaultSubobject<UArrowComponent>(TEXT("SpawnPointR"));
	SpawnPointR->SetupAttachment(GetRootComponent());
	SpawnPointR->SetRelativeLocation(FVector(890.f, 290.f, 30.f));

	// Default to spawning in all lanes
	bSpawnOnSides = true;

	// Create lane splines
	Spline = CreateDefaultSubobject<USplineComponent>(TEXT("Spline"));
	Spline->SetupAttachment(GetRootComponent());

	SplineL = CreateDefaultSubobject<USplineComponent>(TEXT("SplineL"));
	SplineL->SetupAttachment(GetRootComponent());
	SplineL->SetRelativeLocation(FVector(0.f, -290.f, 0.f));

	SplineR = CreateDefaultSubobject<USplineComponent>(TEXT("SplineR"));
	SplineR->SetupAttachment(GetRootComponent());
	SplineR->SetRelativeLocation(FVector(0.f, 290.f, 0.f));

	// Create item spawn area
	ItemArea = CreateDefaultSubobject<UBoxComponent>(TEXT("ItemArea"));
	ItemArea->SetRelativeLocation(FVector(1000.f, 0.f, 85.f));
	ItemArea->SetBoxExtent(FVector(540.f, 450.f, 2.f));

	// Default to spawning items on splines
	bSpawnOnSplines = true;
}

// Called when the game starts or when spawned
void ATP_EndlessRunnerFloorTile::BeginPlay()
{
	Super::BeginPlay();
	
	// Bind end trigger
	EndTrigger->OnComponentBeginOverlap.AddDynamic(this, &ATP_EndlessRunnerFloorTile::OnEndTriggerOverlap);

	// Add spawn points to array
	if (bSpawnOnSides)
	{
		SpawnPoints.Add(SpawnPointR->GetRelativeTransform());
		SpawnPoints.Add(SpawnPointL->GetRelativeTransform());
	}
	SpawnPoints.Add(SpawnPoint->GetRelativeTransform());

	// Randomly spawn either items or blockers
	if (FMath::RandRange(0.f, 1.f) < 0.5f)
	{
		SpawnBlocker();
	}
	else
	{
		SpawnItems();
	}

}

// Returns the attachment transform
FTransform ATP_EndlessRunnerFloorTile::GetAttachTransform()
{
	return AttachPoint->GetComponentTransform();
}

// Called on overlap with the end trigger
void ATP_EndlessRunnerFloorTile::OnEndTriggerOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor)
	{
		// Cast to character
		ATP_EndlessRunnerCharacter* Player = Cast<ATP_EndlessRunnerCharacter>(OtherActor);
		if (Player)
		{
			// Get the game mode
			AGameModeBase* GameMode = GetWorld()->GetAuthGameMode();
			if (GameMode)
			{
				ATP_EndlessRunnerGameMode* RunnerGameMode = Cast<ATP_EndlessRunnerGameMode>(GameMode);
				if (RunnerGameMode)
				{
					// Add new tile and destroy old tile with 2 second delay
					RunnerGameMode->AddFloorTile();
					FTimerHandle DelayHandle;
					GetWorldTimerManager().SetTimer(DelayHandle, this, &ATP_EndlessRunnerFloorTile::DestroyTile, 2.f);
				}
			}
		}
	}
}

// Called to destroy the tile
void ATP_EndlessRunnerFloorTile::DestroyTile()
{
	Destroy();
}

// Returns the spline for the lane
USplineComponent* ATP_EndlessRunnerFloorTile::GetSpline(int8 Lane)
{
	switch (Lane)
	{
	case 0:
		return SplineL;
	case 2:
		return SplineR;
	default:
		return Spline;
	}
}

// Called to spawn one or more blockers
void ATP_EndlessRunnerFloorTile::SpawnBlocker()
{
	FActorSpawnParameters SpawnInfo;

	// Abort if there are no blockers to spawn
	if (BlockersToSpawn.Num() < 1)
	{
		return;
	}

	// Select a class of blocker to spawn
	float ClassSelection = FMath::RandRange(0, BlockersToSpawn.Num() - 1);
	TSubclassOf<ATP_EndlessRunnerBlocker> BlockerClass = BlockersToSpawn[ClassSelection];

	float LocationSelection = FMath::RandRange(0.f, 1.f);
	if (LocationSelection < 0.8f || !bSpawnOnSides)
	{
		// Spawn single blocker
		UChildActorComponent* Blocker = NewObject<UChildActorComponent>(this);
		Blocker->bEditableWhenInherited = true;
		Blocker->RegisterComponent();
		Blocker->SetChildActorClass(BlockerClass);
		Blocker->CreateChildActor();
		Blocker->SetRelativeTransform(GetActorTransform() + SpawnPoints[FMath::RandRange(0, SpawnPoints.Num() - 1)]);
	}
	else // Spawn two blockers
	{
		// Find locations to spawn
		FTransform Transform1 = SpawnPoints[FMath::RandRange(0, SpawnPoints.Num() - 1)];
		FTransform Transform2;

		// Make sure second location isn't the same as the first
		do {
			Transform2 = SpawnPoints[FMath::RandRange(0, SpawnPoints.Num() - 1)];
		} while (Transform1.Equals(Transform2));

		// First blocker
		UChildActorComponent* Blocker1 = NewObject<UChildActorComponent>(this);
		Blocker1->bEditableWhenInherited = true;
		Blocker1->RegisterComponent();
		Blocker1->SetChildActorClass(BlockerClass);
		Blocker1->CreateChildActor();
		Blocker1->SetRelativeTransform(GetActorTransform() + Transform1);

		// Second blocker
		UChildActorComponent* Blocker2 = NewObject<UChildActorComponent>(this);
		Blocker2->bEditableWhenInherited = true;
		Blocker2->RegisterComponent();
		Blocker2->SetChildActorClass(BlockerClass);
		Blocker2->CreateChildActor();
		Blocker2->SetRelativeTransform(GetActorTransform() + Transform2);
	}
}

// Called to spawn one or more blockers
void ATP_EndlessRunnerFloorTile::SpawnItems()
{
	// Abort if there are no items to spawn
	if (ItemsToSpawn.Num() < 1)
	{
		return;
	}

	// Select a class of item to spawn
	float ClassSelection = FMath::RandRange(0, ItemsToSpawn.Num() - 1);
	TSubclassOf<ATP_EndlessRunnerItem> ItemClass = ItemsToSpawn[ClassSelection];

	// Select random number to spawn
	int8 NumItems = FMath::RandRange(0.f, 5.f);

	for (int8 i = 0; i <= NumItems; i++)
	{
		FVector ItemPoint = FMath::RandPointInBox(ItemArea->Bounds.GetBox());

		// Constrain to the lanes in spline mode
		if (bSpawnOnSplines)
		{
			float Lane = FMath::RandRange(0.f, 3.f);
			if (Lane < 1.f)
			{
				ItemPoint.Y = SpawnPointL->GetRelativeLocation().Y;
			}
			else if (Lane < 2.f)
			{
				ItemPoint.Y = SpawnPoint->GetRelativeLocation().Y;
			}
			else
			{
				ItemPoint.Y = SpawnPointR->GetRelativeLocation().Y;
			}
		}

		// Set the location
		FTransform ItemTransform;
		ItemTransform.SetLocation(ItemPoint);

		// Spawn the item
		UChildActorComponent* Item = NewObject<UChildActorComponent>(this);
		Item->bEditableWhenInherited = true;
		Item->RegisterComponent();
		Item->SetChildActorClass(ItemClass);
		Item->CreateChildActor();
		Item->SetRelativeTransform(GetActorTransform() + ItemTransform);
	}
}

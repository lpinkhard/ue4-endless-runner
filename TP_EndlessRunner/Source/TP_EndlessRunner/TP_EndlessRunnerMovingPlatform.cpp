// Copyright Likotech Software. All Rights Reserved.

#include "TP_EndlessRunnerMovingPlatform.h"

// Sets default values
ATP_EndlessRunnerMovingPlatform::ATP_EndlessRunnerMovingPlatform()
{
	// Set this actor to call Tick() every frame.  Required for platform movement.
	PrimaryActorTick.bCanEverTick = true;

	// Create the platform mesh
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	SetRootComponent(Mesh);

	// Start and end point defaults
	StartPoint = FVector(0.f);
	EndPoint = FVector(0.f);

	// Motion variable defaults
	MoveSpeed = 5.0f;
	ReverseTime = 1.f;

	// Default to not moving
	bMoving = false;

}

// Called when the game starts or when spawned
void ATP_EndlessRunnerMovingPlatform::BeginPlay()
{
	Super::BeginPlay();

	// Store start location and calculate world end location
	StartPoint = GetActorLocation();
	EndPoint += StartPoint;

	// Set up motion toggle for reverse timer
	GetWorldTimerManager().SetTimer(MoveTimer, this, &ATP_EndlessRunnerMovingPlatform::ToggleMovement, ReverseTime);

	// Calculate travel distance
	Distance = (EndPoint - StartPoint).Size();

}

// Called every frame
void ATP_EndlessRunnerMovingPlatform::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Move platform, if appropriate
	if (bMoving)
	{
		// Get location and interpolate to new location
		FVector CurrentLocation = GetActorLocation();
		FVector Interp = FMath::VInterpTo(CurrentLocation, EndPoint, DeltaTime, MoveSpeed);
		SetActorLocation(Interp);

		// Check whether destination has been reached
		float DistanceTraveled = (GetActorLocation() - StartPoint).Size();
		if (Distance - DistanceTraveled <= 1.f)
		{
			// Stop moving and set up timer to reverse
			ToggleMovement();
			GetWorldTimerManager().SetTimer(MoveTimer, this, &ATP_EndlessRunnerMovingPlatform::ToggleMovement, ReverseTime);
			
			// Reverse direction of movement
			FVector Temp = StartPoint;
			StartPoint = EndPoint;
			EndPoint = Temp;
		}
	}

}

// Called to start or stop the platform motion
void ATP_EndlessRunnerMovingPlatform::ToggleMovement()
{
	bMoving = !bMoving;
}
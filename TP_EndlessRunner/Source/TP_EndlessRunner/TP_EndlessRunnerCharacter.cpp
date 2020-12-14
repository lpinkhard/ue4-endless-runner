// Copyright Likotech Software. All Rights Reserved.

#include "TP_EndlessRunnerCharacter.h"
#include "TP_EndlessRunnerFloorTile.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/SplineComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Controller.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/SpringArmComponent.h"
#include "Engine/World.h"

#ifdef WWISE_AUDIO_ENABLED
	#include "AkGameplayStatics.h"
#else
	#include "Sound/SoundCue.h"
#endif

// Sets default values
ATP_EndlessRunnerCharacter::ATP_EndlessRunnerCharacter()
{
 	// Set this character to call Tick() every frame.  Required for raytracing and input.
	PrimaryActorTick.bCanEverTick = true;

	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// Set default mesh parameters
	GetMesh()->SetRelativeLocation(FVector(0.f, 0.f, -90.f));
	GetMesh()->SetRelativeRotation(FRotator(0.f, -90.f, 0.f));

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = true;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f); // ...at this rotation rate
	GetCharacterMovement()->JumpZVelocity = 600.f;
	GetCharacterMovement()->AirControl = 1.0f; // Provide full air control
	GetCharacterMovement()->MaxWalkSpeed = 1500.f; // Make character movement faster

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 500.0f; // The camera follows at this distance behind the character	
	CameraBoom->SocketOffset = FVector(0.f, 0.f, 80.f);
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Set portrait aspect ratio on mobile
	if (UGameplayStatics::GetPlatformName().Compare(FString("IOS"), ESearchCase::IgnoreCase) == 0
		|| UGameplayStatics::GetPlatformName().Compare(FString("Android"), ESearchCase::IgnoreCase) == 0)
	{
		FollowCamera->SetAspectRatio(0.5625f);	// Portrait aspect ratio

		// Adjust camera distance
		CameraBoom->TargetArmLength = 200.0f;
		CameraBoom->SocketOffset = FVector(0.f, 0.f, 260.f);
	}

	// Initial target rotation is zero
	TargetRotation = FRotator(0.f, 0.f, 0.f);

	// Start a bit forward of origin
	SetActorLocation(FVector(10.f, 0.f, 0.f));

	// Character defaults
	bAlive = true;
	Score = 0;
	Distance = 0.f;
	CurrentLane = 1;
	TargetLane = 1;
	LaneOffset = 290.f;

	LateralMovementMode = ELateralMovementMode::ELM_Spline;

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named EndlessRunnerCharacter (to avoid direct content references in C++)

}

// Called when the game starts or when spawned
void ATP_EndlessRunnerCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ATP_EndlessRunnerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Don't do anything with a dead character
	if (!bAlive)
	{
		return;
	}

	// Get the player controller
	APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();

	// Adjust rotation to target, if needed
	FRotator Rotation = PlayerController->GetControlRotation();
	if (Rotation != TargetRotation)
	{
		PlayerController->SetControlRotation(FMath::RInterpTo(Rotation, TargetRotation, DeltaTime, 10.f));
	}

	// get yaw rotation
	const FRotator YawRotation(0, Rotation.Yaw, 0);

	// get forward vector and add direction/tangent to spline
	const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

	if (LateralMovementMode == ELateralMovementMode::ELM_Spline) // follow splines in spline mode
	{
		// Do a line trace to find the surface below the character
		FHitResult TraceHit;
		FVector TraceStart = GetActorLocation();
		FVector TraceEnd = GetActorLocation();
		TraceEnd.Z -= 1000.f;
		FCollisionQueryParams TraceParams;
		TraceParams.AddIgnoredActor(this);

		FCollisionResponseParams TraceResponse;
		if (GetWorld()->LineTraceSingleByChannel(TraceHit, TraceStart, TraceEnd, ECollisionChannel::ECC_Visibility, TraceParams, TraceResponse))
		{
			ATP_EndlessRunnerFloorTile* CurrentTile = Cast<ATP_EndlessRunnerFloorTile>(TraceHit.Actor);
			// check parent if cast failed
			if (!CurrentTile)
			{
				CurrentTile = Cast<ATP_EndlessRunnerFloorTile>(TraceHit.Actor.Get()->GetOwner());
			}

			if (CurrentTile)
			{
				// find the spline for the lane
				USplineComponent* Path = CurrentTile->GetSpline(TargetLane);

				// follow spline
				FVector SplineLocation = Path->FindLocationClosestToWorldLocation(GetActorLocation(), ESplineCoordinateSpace::World);
				FVector SplineDirection = (SplineLocation - GetActorLocation()).GetSafeNormal();

				FVector PathDirection = Path->FindTangentClosestToWorldLocation(TraceStart, ESplineCoordinateSpace::World).GetSafeNormal();

				// calculate the movement vector
				if (FMath::IsNearlyZero(PathDirection.Size()) && FMath::IsNearlyZero(SplineDirection.Size()))
				{
					AddMovementInput(Direction, 1.f);
				}
				else
				{
					AddMovementInput((SplineDirection + PathDirection + (Direction / 2)).GetSafeNormal(), 1.f);
				}
			}
			else // cannot find a tile below, just move forward
			{
				AddMovementInput(Direction, 1.f);
			}
		}
	}
	else // all other modes, just move forward
	{
		AddMovementInput(Direction, 1.f);
	}

	Distance += GetCharacterMovement()->Velocity.Size() * DeltaTime;
}

// Called to bind functionality to input
void ATP_EndlessRunnerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// Set up gameplay key bindings
	check(PlayerInputComponent);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	switch (LateralMovementMode)
	{
	case ELateralMovementMode::ELM_Spline:
		PlayerInputComponent->BindAction("MoveLeft", IE_Pressed, this, &ATP_EndlessRunnerCharacter::MoveLeft);
		PlayerInputComponent->BindAction("MoveRight", IE_Pressed, this, &ATP_EndlessRunnerCharacter::MoveRight);
		break;
	case ELateralMovementMode::ELM_Free:
		PlayerInputComponent->BindAxis("MoveRight", this, &ATP_EndlessRunnerCharacter::MoveRight);
	}

	// handle touch devices
	PlayerInputComponent->BindTouch(IE_Pressed, this, &ATP_EndlessRunnerCharacter::TouchStarted);
	PlayerInputComponent->BindTouch(IE_Released, this, &ATP_EndlessRunnerCharacter::TouchStopped);

}

// Called when touch is started
void ATP_EndlessRunnerCharacter::TouchStarted(ETouchIndex::Type FingerIndex, FVector Location)
{
	// Record touch location
	if (FingerIndex == ETouchIndex::Touch1)
	{
		TouchLocation = Location;
	}
}

// Called when touch is stopped
void ATP_EndlessRunnerCharacter::TouchStopped(ETouchIndex::Type FingerIndex, FVector Location)
{
	// Evaluate touch movement
	if (FingerIndex == ETouchIndex::Touch1)
	{
		if (FMath::Abs(Location.X - TouchLocation.X) > FMath::Abs(Location.Y - TouchLocation.Y))
		{
			// Horizontal movement, only in spline mode
			if (LateralMovementMode == ELateralMovementMode::ELM_Spline)
			{
				if (Location.X < TouchLocation.X)
				{
					MoveLeft();
				}
				else
				{
					MoveRight();
				}
			}
		}
		else
		{
			// Vertical movement
			if (Location.Y < TouchLocation.Y)
			{
				Jump();
			}
		}
	}
}

// Called to move character forward
void ATP_EndlessRunnerCharacter::MoveForward(float Value)
{
	if ((Controller != NULL) && (Value != 0.0f))
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);
	}
}

// Called to change lanes to the left
void ATP_EndlessRunnerCharacter::MoveLeft()
{
	// change lane to the left
	TargetLane = FMath::Clamp(TargetLane - 1, 0, 1);
}

void ATP_EndlessRunnerCharacter::MoveRight()
{
	// change lane to the right
	TargetLane = FMath::Clamp(TargetLane + 1, 1, 2);
}

// Free side to side movement
void ATP_EndlessRunnerCharacter::MoveRight(float Value)
{
	if ((Controller != NULL) && (Value != 0.0f))
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get right vector
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		AddMovementInput(Direction, Value);
	}
}


// Called to turn character to the left (not bound by default)
void ATP_EndlessRunnerCharacter::TurnLeft()
{
	// turn 90 degrees to the left
	TargetRotation = FRotator(FQuat(TargetRotation) * FQuat(FRotator(0.f, -90.f, 0.f)));
}

// Called to turn character to the right (not bound by default)
void ATP_EndlessRunnerCharacter::TurnRight()
{
	// turn 90 degrees to the right
	TargetRotation = FRotator(FQuat(TargetRotation) * FQuat(FRotator(0.f, 90.f, 0.f)));
}

// Called when character dies
void ATP_EndlessRunnerCharacter::Death()
{
	DetachFromControllerPendingDestroy();

	// Disable capsule collision
	UCapsuleComponent* CapsuleComp = GetCapsuleComponent();
	CapsuleComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	CapsuleComp->SetCollisionResponseToAllChannels(ECR_Ignore);

	// Set up ragdoll
	GetMesh()->SetCollisionProfileName(TEXT("Ragdoll"));
	SetActorEnableCollision(true);

	// Check to make sure this code is only executed once
	if (bAlive)
	{
		// Ragdoll animation
		GetMesh()->SetAllBodiesSimulatePhysics(true);
		GetMesh()->SetSimulatePhysics(true);
		GetMesh()->WakeAllRigidBodies();
		GetMesh()->bBlendPhysics = true;

		// Stop character movement
		UCharacterMovementComponent* CharacterComp = Cast<UCharacterMovementComponent>(GetMovementComponent());
		if (CharacterComp)
		{
			CharacterComp->StopMovementImmediately();
			CharacterComp->DisableMovement();
			CharacterComp->SetComponentTickEnabled(false);
		}

		// Clean up and mark character dead
		SetLifeSpan(10.0f);
		bAlive = false;

		// Play death sound
#ifdef WWISE_AUDIO_ENABLED
		FAkAudioDevice::Get()->PostEvent(DeathEvent, this);
#else
		UGameplayStatics::PlaySoundAtLocation(this, DeathCue, GetActorLocation());
#endif
	}
}

// Called to increase the player score
void ATP_EndlessRunnerCharacter::IncreaseScore(int32 Value)
{
	Score += Value;
}
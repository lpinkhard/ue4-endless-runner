// Copyright Likotech Software. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "TP_EndlessRunnerCharacter.generated.h"

/** Uncomment to enable Wwise audio */
// #define WWISE_AUDIO_ENABLED

UENUM(BlueprintType)
enum class ELateralMovementMode : uint8
{
	ELM_Fixed UMETA(DisplayName = "Fixed"),
	ELM_Spline UMETA(DisplayName = "Spline"),
	ELM_Free UMETA(DisplayName = "Free"),

	ELM_MAX UMETA(DisplayName = "DefaultMAX")
};


UCLASS(config=Game)
class ATP_EndlessRunnerCharacter : public ACharacter
{
	GENERATED_BODY()

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;

	/** Distance between lanes */
	UPROPERTY(EditDefaultsOnly, Category = World)
	float LaneOffset;

	/** Death sound - Uncomment this property if using Wwise */
	/*
	UPROPERTY(EditAnywhere, Category = Sound)
	class UAkAudioEvent* DeathEvent;
	*/

	/** Death sound - Remove this property if using Wwise */
	UPROPERTY(EditAnywhere, Category = Sound)
	class USoundCue* DeathCue;

	/** Movement mode */
	UPROPERTY(EditDefaultsOnly, Category = Movement)
	ELateralMovementMode LateralMovementMode;

	/** Player state */
	bool bAlive;
	int8 CurrentLane;
	int8 TargetLane;
	FRotator TargetRotation;

	/** Touch input */
	FVector TouchLocation;

public:
	// Sets default values for this character's properties
	ATP_EndlessRunnerCharacter();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = State)
	int32 Score;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = State)
	float Distance;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	/** Called for forwards/backward input */
	void MoveForward(float Value);

	/** Called for side to side input */
	void MoveLeft();
	void MoveRight();
	void MoveRight(float Value);
	void TurnLeft();
	void TurnRight();

	/** Handler for when a touch input begins. */
	void TouchStarted(ETouchIndex::Type FingerIndex, FVector Location);

	/** Handler for when a touch input stops. */
	void TouchStopped(ETouchIndex::Type FingerIndex, FVector Location);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }

	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

	// Called on character death
	virtual void Death();

	// Called to increase player score
	virtual void IncreaseScore(int32 Value);

	// Returns current player score
	UFUNCTION(BlueprintPure)
	FORCEINLINE int32 GetScore() { return Score; }

	// Returns current player distance
	UFUNCTION(BlueprintPure)
	FORCEINLINE int32 GetDistance() { return (int32) (Distance / 100.f); }
};

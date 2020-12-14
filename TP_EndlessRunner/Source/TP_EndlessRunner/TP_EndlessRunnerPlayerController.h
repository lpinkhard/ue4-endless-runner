// Copyright Likotech Software. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "TP_EndlessRunnerPlayerController.generated.h"

UCLASS(minimalapi)
class ATP_EndlessRunnerPlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
	/** Reference to UMG asset class */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = HUD)
	TSubclassOf<class UUserWidget> HUDOverlayAsset;

	/** HUD overlay instance */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = HUD)
	UUserWidget* HUDOverlay;

protected:
	virtual void BeginPlay() override;
};

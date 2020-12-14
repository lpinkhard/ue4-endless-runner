// Copyright Likotech Software. All Rights Reserved.

#include "TP_EndlessRunnerPlayerController.h"
#include "Blueprint/UserWidget.h"

void ATP_EndlessRunnerPlayerController::BeginPlay()
{
	Super::BeginPlay();

	// Create the HUD overlay and add it to the viewport
	if (IsLocalPlayerController() && HUDOverlayAsset)
	{
		HUDOverlay = CreateWidget<UUserWidget>(this, HUDOverlayAsset);
		HUDOverlay->AddToViewport();
		HUDOverlay->SetVisibility(ESlateVisibility::Visible);
	}
}

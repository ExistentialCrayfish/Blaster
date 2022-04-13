// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "OverheadWidget.generated.h"

/**
 * 
 */
UCLASS()
class BLASTER_API UOverheadWidget : public UUserWidget
{
	GENERATED_BODY()
	
	// Assign variables for the widget
public:
	// We want to have text in this widget,
	// so we define a text block.

	// Assigns this variable to the variable in the blueprint.
	// Anything we do to modify in c++ will modify the blueprint.
	// NOTE: it MUST have the same name in the blueprint as in
	// c++
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* DisplayText;

	// Set the text value for DisplayText
	void SetDisplayText(FString TextToDisplay);

	// Get a pawn's server related role
	UFUNCTION(BlueprintCallable)
	void ShowPlayerName(APawn* InPawn);

protected:
	virtual void OnLevelRemovedFromWorld(ULevel* InLevel, UWorld* InWorld) override;
};

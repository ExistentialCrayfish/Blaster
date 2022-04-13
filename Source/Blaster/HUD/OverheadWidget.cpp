// Fill out your copyright notice in the Description page of Project Settings.


#include "OverheadWidget.h"
#include "Components/TextBlock.h"
#include "GameFramework/PlayerState.h" 

void UOverheadWidget::SetDisplayText(FString TextToDisplay)
{
	if (DisplayText) 
	{
		DisplayText->SetText(FText::FromString(TextToDisplay));
	}
}

void UOverheadWidget::ShowPlayerName(APawn* InPawn)
{
	APlayerState* State = InPawn->GetPlayerState();
	if (State == nullptr) {

#if UE_BUILD_DEBUG
		if (GEngine)
			GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("State is null!"));
#endif
		return;
	}

	FString PlayerName = State->GetPlayerName();

	if (!PlayerName.IsEmpty()) {
		FString RoleString = FString::Printf(TEXT("%s"), *PlayerName);

		SetDisplayText(RoleString);
	}
	else {
#if UE_BUILD_DEBUG
		if (GEngine)
			GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("Player doens't have a name!"));
#endif
	}
}

void UOverheadWidget::OnLevelRemovedFromWorld(ULevel* InLevel, UWorld* InWorld)
{
	RemoveFromParent();

	Super::OnLevelRemovedFromWorld(InLevel, InWorld);

	// Remove it from the viewport, and remember to call the super as we're overriding.
}

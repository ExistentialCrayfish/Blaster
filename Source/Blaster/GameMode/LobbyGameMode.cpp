// Fill out your copyright notice in the Description page of Project Settings.


#include "LobbyGameMode.h"
#include "GameFramework/GameStateBase.h"

void ALobbyGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	// Here it's safe to access the player who just joined.

	// We check how many players we have in the game
	int32 NumberOfPlayers = GameState.Get()->PlayerArray.Num();

	Say(FString("Welcome Player!"));

	if (NumberOfPlayers > MinServerPlayerCount)
	{
		// Get the world (which allows the server to travel)
		UWorld* World = GetWorld();
		

		// If the world exists, we can travel to the new world
		if (World != nullptr) {
			// We allow seamless travel
			bUseSeamlessTravel = true;

			// We want to go to the map, but make sure it's 
			// on lisen so it can be used as a server map 
			// clients can connect to.
			// NOTE: The path is slightly different!
			World->ServerTravel("/Game/Maps/BlasterMap?listen");
		}
	}
}

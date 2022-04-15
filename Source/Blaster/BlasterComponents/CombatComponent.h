// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Blaster/HUD/BlasterHUD.h"
#include "CombatComponent.generated.h"

#define TRACE_LENGTH 20000.f


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class BLASTER_API UCombatComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UCombatComponent();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	// Give class full access to this class
	friend class ABlasterCharacter;

	void EquipWeapon(class AWeapon* WeaponToEquip);

protected:
	virtual void BeginPlay() override;

	// This is called by the user (Character)
	// who wants to set aiming state.
	// 
	// This will be synced to server+client
	void SetAiming(bool bIsAiming);

	// RPC to set aiming on the server
	UFUNCTION(Server, Reliable)
	void ServerSetAiming(bool bIsAiming);

	UFUNCTION()
	void OnRep_EquippedWeapon();

	// Handle firing
	void FireButtonPressed(bool bPressed);

	// We want to fire - call this and it'll be handled
	// (so long as we have a weapon ofc)
	void Fire(float DeltaTime);

	// We want this to run on all clients and the server
	// So first we ask the server to fire.
	//
	// We provide the spread factor as the server is unaware of client's spread factor
	// and it's not synced
	UFUNCTION(Server, Reliable)
	void ServerFire(const FVector_NetQuantize& TraceHitTarget, const float TraceHitDistance, const float CrosshairSpread);

	// Then the server sends it out the the other clients.
	UFUNCTION(NetMulticast, Reliable)
	void MulticastFire(const FVector_NetQuantize& TraceHitTarget);

	void TraceUnderCrosshairs(FHitResult& TraceHitResult);

	void SetHUDCrosshairs(float DeltaTime);

private:
	class ABlasterCharacter* Character;
	class ABlasterPlayerController* Controller;
	class ABlasterHUD* HUD;

	UPROPERTY(ReplicatedUsing = OnRep_EquippedWeapon)
	class AWeapon* EquippedWeapon;

	UPROPERTY(Replicated)
	bool bAiming;

	UPROPERTY(EditAnywhere)
	float BaseWalkSpeed;

	UPROPERTY(EditAnywhere)
	float AimWalkSpeed;

	bool bFireButtonPressed;

	// Time elapsed since last shot
	// Replicated for server verification
	UPROPERTY(Replicated)
	float TimeSinceLastFire; 

	FVector HitTarget;
	float HitDistance;

	/*
		HUD and Crosshairs
	*/

	FHUDPackage HUDPackage;

	// Spread based on movement
	float CrosshairVelocityFactor;
	// Spread based on whether or not we're falling
	float CrosshairInAirFactor;
	// Aim offset for spread
	float CrosshairAimFactor;
	// Aim offset for shooting
	float CrosshairShootingFactor;
	// Overall spread
	float CrosshairSpreadFactor;


	/*
		Aiming and FOV
	*/

	// FOV when not aiming, set to camera base FOV in BeginPlay
	float DefaultFOV;

	float CurrentFOV;

	UPROPERTY(EditAnywhere, Category = "Combat")
		float ZoomedFOV = 30.f;

	UPROPERTY(EditAnywhere, Category = "Combat")
		float ZoomInterpSpeed = 20.f;

	void InterpolateFOV(float DeltaTime);
public:	
	void InterpolateCrosshairVelocityFactor(float NewValue, float DeltaTime, float InterpSpeed);
	void InterpolateCrosshairInAirFactor(float NewValue, float DeltaTime, float InterpSpeed);
	void InterpolateCrosshairAimFactor(float NewValue, float DeltaTime, float InterpSpeed);
	void InterpolateCrosshairShootingFactor(float NewValue, float DeltaTime, float InterpSpeed);
};

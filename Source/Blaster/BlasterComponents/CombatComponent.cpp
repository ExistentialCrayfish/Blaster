// Fill out your copyright notice in the Description page of Project Settings.


#include "CombatComponent.h"
#include "Blaster/Weapon/Weapon.h"
#include "Blaster/Character/BlasterCharacter.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Components/SphereComponent.h"
#include "Net/UnrealNetwork.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Blaster/BlasterTypes/OrientationMode.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"
#include "Blaster/PlayerController/BlasterPlayerController.h"
#include "Camera/CameraComponent.h"

UCombatComponent::UCombatComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	BaseWalkSpeed = 600.f;
	AimWalkSpeed = 450.f;

	bFireButtonPressed = false;
}


void UCombatComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// This is how we replicate a variable
	// It is however conditional on the condition being met,
	// otherwise it's not matched
	// Here it's set to whoever "owns" the current pawn. Or,
	// the current user.

	// ENSURE ALL VARIABLES TO BE REPLICATED ARE SET

	DOREPLIFETIME(UCombatComponent, EquippedWeapon);
	DOREPLIFETIME(UCombatComponent, bAiming);
	DOREPLIFETIME(UCombatComponent, TimeSinceLastFire);
}

void UCombatComponent::BeginPlay()
{
	Super::BeginPlay();

	if (Character) 
	{
		Character->GetCharacterMovement()->MaxWalkSpeed = BaseWalkSpeed;

		if (Character->GetFollowCamera())
		{
			DefaultFOV = Character->GetFollowCamera()->FieldOfView;
			CurrentFOV = DefaultFOV;
		}
	}
}


void UCombatComponent::SetHUDCrosshairs(float DeltaTime)
{
	if (Character == nullptr || Character->Controller == nullptr) return;
	// Efficient way to set vars that need be ticked
	Controller = Controller == nullptr ? Cast<ABlasterPlayerController>(Character->Controller) : Controller;

	if (Controller)
	{

		HUD = HUD == nullptr ? Cast<ABlasterHUD>(Controller->GetHUD()) : HUD;

		if (HUD)
		{
			
			if (EquippedWeapon)
			{
				HUDPackage.CrosshairsCenter = EquippedWeapon->CrosshairsCenter;
				HUDPackage.CrosshairsLeft = EquippedWeapon->CrosshairsLeft;
				HUDPackage.CrosshairsRight = EquippedWeapon->CrosshairsRight;
				HUDPackage.CrosshairsTop = EquippedWeapon->CrosshairsTop;
				HUDPackage.CrosshairsBottom = EquippedWeapon->CrosshairsBottom;
			}
			else
			{
				HUDPackage.CrosshairsCenter = nullptr;
				HUDPackage.CrosshairsLeft = nullptr;
				HUDPackage.CrosshairsRight = nullptr;
				HUDPackage.CrosshairsTop = nullptr;
				HUDPackage.CrosshairsBottom = nullptr;
			}

			// Calculate crosshairs spread
			// This will be interpolated based on velocity and whether 
			// the character is firing
			
			float InterpAmount = !Character->bIsCrouched ? 1.f : .5f;


			if (Character->GetVelocity().Size() > 0.f)
			{

				// Map [0 - max speed) to [0 - (1 || 0.5 based on crouch)] (normalize values)
				FVector2D WalkSpeedRange(0, Character->GetCharacterMovement()->MaxWalkSpeed);
				FVector2D VelocityNormalizedRange(0, InterpAmount);
				FVector Velocity = Character->GetVelocity();
				Velocity.Z = 0;

				InterpolateCrosshairVelocityFactor(FMath::GetMappedRangeValueClamped(WalkSpeedRange, VelocityNormalizedRange, Velocity.Size()), DeltaTime, 6.f);
			}
			else
			{
				InterpolateCrosshairVelocityFactor(0.f, DeltaTime, 6.f);
			}


			// Falling offset
			if (Character->GetCharacterMovement()->IsFalling())
			{
				InterpolateCrosshairInAirFactor(2.25f, DeltaTime, 2.25f);
			}
			else
			{
				InterpolateCrosshairInAirFactor(0.f, DeltaTime, 30.f);
			}

			// Aiming offset
			if (bAiming)
			{
				InterpolateCrosshairAimFactor(0.58f, DeltaTime, 30.f);
			}
			else
			{
				InterpolateCrosshairAimFactor(0.f, DeltaTime, 30.f);
			}

			// Firing offset
			if (bFireButtonPressed)
			{
				InterpolateCrosshairShootingFactor(.9f * InterpAmount, DeltaTime, 25.f);
			}
			else
			{
				InterpolateCrosshairShootingFactor(0.f, DeltaTime, 40.f);
			}


			// base spread, then add modifiers
			CrosshairSpreadFactor = 0.5f +  CrosshairShootingFactor + CrosshairVelocityFactor + CrosshairInAirFactor - CrosshairAimFactor;

			// Clamp to a max value
			CrosshairSpreadFactor = (CrosshairSpreadFactor < 0.f) ? 0.f : (CrosshairSpreadFactor > 2.3f) ? 2.3f : CrosshairSpreadFactor;


			HUDPackage.CrosshairsSpread = CrosshairSpreadFactor;

			HUD->SetHUDPackage(HUDPackage);
		}
	}
}




void UCombatComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);


	if (Character)
	{
		if (Character->IsLocallyControlled())
		{
			Fire(DeltaTime);
			SetHUDCrosshairs(DeltaTime);
			InterpolateFOV(DeltaTime);
		}

		if (Character->HasAuthority())
		{
			// Update timers based on delta time
			TimeSinceLastFire += DeltaTime;
		}
	}


	
}

void UCombatComponent::SetAiming(bool bIsAiming)
{
	bAiming = bIsAiming;
	ServerSetAiming(bIsAiming);
	if (Character && EquippedWeapon)
	{
		Character->GetCharacterMovement()->MaxWalkSpeed = bIsAiming ? AimWalkSpeed : BaseWalkSpeed;
	}
}

void UCombatComponent::ServerSetAiming_Implementation(bool bIsAiming)
{
	bAiming = bIsAiming;
	if (Character && EquippedWeapon) 
	{
		Character->GetCharacterMovement()->MaxWalkSpeed = bIsAiming ? AimWalkSpeed : BaseWalkSpeed;
	}
}

void UCombatComponent::OnRep_EquippedWeapon()
{
	if (EquippedWeapon) {
		Character->SetOrientationMode(EOrientationMode::EOM_LockToYaw);
	}
	else 
	{
		Character->SetOrientationMode(EOrientationMode::EOM_OrientWithMovement);
	}
}

void UCombatComponent::FireButtonPressed(bool bPressed)
{
	bFireButtonPressed = bPressed;

	Fire(0.f);
}

void UCombatComponent::Fire(float DeltaTime)
{
	if (Character == nullptr || EquippedWeapon == nullptr) return;

	FHitResult HitResult;
	TraceUnderCrosshairs(HitResult);
	HitTarget = HitResult.ImpactPoint;
	HitDistance = HitResult.Distance;

	if (bFireButtonPressed && TimeSinceLastFire >= EquippedWeapon->GetFireRate())
	{
		// If we're the client, send an RPC to the 
		// server to fire (along with the impact point 
		// so we know where to fire for the weapon/FX)
		//
		// If we're the server, RPC anyway since it'll just 
		// get run locally


		ServerFire(HitTarget, HitDistance, CrosshairSpreadFactor);
	}
}

void UCombatComponent::ServerFire_Implementation(const FVector_NetQuantize& TraceHitTarget, const float TraceHitDistance, const float CrosshairSpread)
{
	if (EquippedWeapon == nullptr) return;
	if (TimeSinceLastFire >= EquippedWeapon->GetFireRate())
	{
		// Reset fire timer
		TimeSinceLastFire = 0.f;


		// Offset the bullet based on the crosshair spread
		FVector BulletOffset(FMath::RandRange(-CrosshairSpread, CrosshairSpread), 0.f, FMath::RandRange(-CrosshairSpread, CrosshairSpread));
		// we multiply by 100 so we see a sizable difference (as a value of 1 is 1mm);
		BulletOffset *= (TraceHitDistance / 35.f);


		MulticastFire(TraceHitTarget + BulletOffset);
	}
}

void UCombatComponent::MulticastFire_Implementation(const FVector_NetQuantize& TraceHitTarget)
{
	if (Character && EquippedWeapon)
	{
		Character->PlayFireMontage(bAiming);
		EquippedWeapon->Fire(TraceHitTarget);
	}
}

void UCombatComponent::TraceUnderCrosshairs(FHitResult& TraceHitResult)
{
	if (GEngine == nullptr || GEngine->GameViewport == nullptr) 
	{
		return;
	}

	FVector2D ViewportSize;
	GEngine->GameViewport->GetViewportSize(ViewportSize);

	FVector2D CrosshairLocation(ViewportSize.X / 2.f, ViewportSize.Y / 2.f);


	FVector CrosshairWorldPosition;
	FVector CrosshairWorldDirection;
	bool bScreenToWorldSuccessful = UGameplayStatics::DeprojectScreenToWorld(
		UGameplayStatics::GetPlayerController(this, 0),
		CrosshairLocation,
		CrosshairWorldPosition,
		CrosshairWorldDirection
	);

	if (bScreenToWorldSuccessful) 
	{
		FVector Start = CrosshairWorldPosition;

		if (Character)
		{
			float DistanceToCharacter = (Character->GetActorLocation() - Start).Size();
			Start += (CrosshairWorldDirection * (DistanceToCharacter + 100.f));
		}

		FVector End = Start + (CrosshairWorldDirection * TRACE_LENGTH);

		GetWorld()->LineTraceSingleByChannel(TraceHitResult, Start, End, ECollisionChannel::ECC_Visibility);

		// In case we hit nothing, make sure to set the impact point
		if (!TraceHitResult.bBlockingHit) TraceHitResult.ImpactPoint = End;

		if (TraceHitResult.GetActor() && TraceHitResult.GetActor()->Implements<UInteractWithCrosshairsInterface>())
		{
			// Tell blaster hud to draw red crosshairs
			HUDPackage.CrosshairsColor = FLinearColor::Red;
		}
		else
		{
			HUDPackage.CrosshairsColor = FLinearColor::White;
		}
	}
}


void UCombatComponent::EquipWeapon(AWeapon* WeaponToEquip)
{
	if (Character == nullptr || WeaponToEquip == nullptr || EquippedWeapon) 
	{
		return;
	}

	EquippedWeapon = WeaponToEquip;

	EquippedWeapon->SetWeaponState(EWeaponState::EWS_Equipped);

	const USkeletalMeshSocket* HandSocket = Character->GetMesh()->GetSocketByName(FName("RightHandSocket"));

	if (HandSocket) 
	{
		HandSocket->AttachActor(EquippedWeapon, Character->GetMesh());
	}

	EquippedWeapon->SetOwner(Character);

	// For strafing - we don't want to orient the player
	// based on movement.
	Character->SetOrientationMode(EOrientationMode::EOM_LockToYaw);
}

void UCombatComponent::InterpolateFOV(float DeltaTime)
{
	if (EquippedWeapon == nullptr) return;

	if (bAiming)
	{
		CurrentFOV = FMath::FInterpTo(
			CurrentFOV,
			EquippedWeapon->GetZoomedFOV(),
			DeltaTime,
			EquippedWeapon->GetZoomedInterpSpeed()
		);
	}
	else
	{
		CurrentFOV = FMath::FInterpTo(
			CurrentFOV,
			DefaultFOV,
			DeltaTime,
			ZoomInterpSpeed
		);
	}

	if (Character && Character->GetFollowCamera())
	{
		Character->GetFollowCamera()->SetFieldOfView(CurrentFOV);
	}
}

void UCombatComponent::InterpolateCrosshairVelocityFactor(float NewValue, float DeltaTime, float InterpSpeed)
{
	CrosshairVelocityFactor = FMath::FInterpTo(
		CrosshairVelocityFactor,
		NewValue,
		DeltaTime,
		InterpSpeed
	);
}

void UCombatComponent::InterpolateCrosshairInAirFactor(float NewValue, float DeltaTime, float InterpSpeed)
{
	CrosshairInAirFactor = FMath::FInterpTo(
		CrosshairInAirFactor,
		NewValue,
		DeltaTime,
		InterpSpeed
	);
}

void UCombatComponent::InterpolateCrosshairAimFactor(float NewValue, float DeltaTime, float InterpSpeed)
{
	CrosshairAimFactor = FMath::FInterpTo(
		CrosshairAimFactor,
		NewValue,
		DeltaTime,
		InterpSpeed
	);
}

void UCombatComponent::InterpolateCrosshairShootingFactor(float NewValue, float DeltaTime, float InterpSpeed)
{
	CrosshairShootingFactor = FMath::FInterpTo(
		CrosshairShootingFactor,
		NewValue,
		DeltaTime,
		InterpSpeed
	);
}


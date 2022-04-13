// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Weapon.h"
#include "ProjectileWeapon.generated.h"

/**
 * 
 */
UCLASS()
class BLASTER_API AProjectileWeapon : public AWeapon
{
	GENERATED_BODY()
public:
	virtual void Fire(const FVector& HitTarget) override;

protected:
	UFUNCTION(NetMulticast, Reliable)
		void MulticastSpawnProjectile(const FVector_NetQuantize& SpawnPosition, const FRotator& SpawnRotation);
		virtual void MulticastSpawnProjectile_Implementation(const FVector_NetQuantize& SpawnPosition, const FRotator& SpawnRotation);


	UPROPERTY(EditAnywhere)
	TSubclassOf<class AProjectile> ProjectileClass;
};
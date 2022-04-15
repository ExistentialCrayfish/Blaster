// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectileWeapon.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Projectile.h"

void AProjectileWeapon::Fire(const FVector& HitTarget)
{
	Super::Fire(HitTarget);

	// Only fire ON THE SERVER
	if (!HasAuthority()) return;

	// The instigator is the owner. Must be a pawn, so we cast.
	APawn* InstigatorPawn = Cast<APawn>(GetOwner());

	// Spawn a projectile at the tip of the barrel
	const USkeletalMeshSocket* MuzzleFlashSocket = GetWeaponMesh()->GetSocketByName(FName("MuzzleFlash"));

	if (MuzzleFlashSocket)
	{
		FTransform SocketTransform = MuzzleFlashSocket->GetSocketTransform(GetWeaponMesh());

		// Get the direction from the muzzle flash socket to the hit location
		// traced from the center of the screen
		FVector DirToTarget = HitTarget - SocketTransform.GetLocation();
		// We now conver the direction into a rotation
		// that we can use for spawning the projectile.
		FRotator TargetRotation = DirToTarget.Rotation();

		if (ProjectileClass && InstigatorPawn)
		{
			UWorld* World = GetWorld();

			FActorSpawnParameters SpawnParameters;
			SpawnParameters.Owner = GetOwner();
			SpawnParameters.Instigator = InstigatorPawn;
			
			MulticastSpawnProjectile(SocketTransform.GetLocation(), TargetRotation);

			// Create a server version (with params)
			if (World)
			{
				World->SpawnActor<AProjectile>(
					ProjectileClass,
					SocketTransform.GetLocation(),
					TargetRotation,
					SpawnParameters
					);
			}
		}
	}
}

void AProjectileWeapon::MulticastSpawnProjectile_Implementation(const FVector_NetQuantize& SpawnPosition, const FRotator& SpawnRotation)
{
	// Only for clients
	if (!HasAuthority())
	{
		UWorld* World = GetWorld();
		if (World)
		{
			FActorSpawnParameters SpawnParameters;
			SpawnParameters.Owner = GetOwner();
			SpawnParameters.Instigator = Cast<APawn>(GetOwner());;

			World->SpawnActor<AProjectile>(
				ProjectileClass,
				SpawnPosition,
				SpawnRotation,
				SpawnParameters
				);
		}
	}
}
// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon.h"

#include "Components/SphereComponent.h"
#include "Components/WidgetComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Blaster/Character/BlasterCharacter.h"
#include "Net/UnrealNetwork.h"
#include "Animation/AnimationAsset.h"
#include "BulletCasing.h"
#include "Engine/SkeletalMeshSocket.h"


// Sets default values
AWeapon::AWeapon()
{
	PrimaryActorTick.bCanEverTick = false;
	// We make the weapon have authority only on the server.
	// this must be set to ensure that this is the case
	// This ensures that it is spawned on the server,
	// and propogated to clients.
	bReplicates = true;

	WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMesh"));
	SetRootComponent(WeaponMesh);

	// We set collision values.
	// We also disable collision (but can renable on drop)
	WeaponMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);
	WeaponMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
	WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	// Set the weapon to replicate so that we can have server authority.

	AreaSphere = CreateDefaultSubobject<USphereComponent>(TEXT("AreaSphere"));
	AreaSphere->SetupAttachment(RootComponent);

	// We check for overlap. If overlap with weapon, we can allow for pickup
	// This should be done SERVER ONLY as it's MP, so we disable it
	AreaSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	AreaSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);


	PickupWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("PickupWidget"));
	PickupWidget->SetupAttachment(RootComponent);


}

void AWeapon::BeginPlay()
{
	Super::BeginPlay();
	
	// If we're the server, then allow for overlap collision
	// on the area sphere. Basically server side gameplay.
	// Good anti-cheat!
	if (HasAuthority()) {
		AreaSphere->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		AreaSphere->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
	
		// Bind collision to the overlap event. THis should only be server side.
		AreaSphere->OnComponentBeginOverlap.AddDynamic(this, &AWeapon::OnSphereOverlap);
		AreaSphere->OnComponentEndOverlap.AddDynamic(this, &AWeapon::OnSphereEndOverlap);
	}

	if (PickupWidget) 
	{
		PickupWidget->SetVisibility(false);
	}
}

void AWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// MAKE SURE YOU DO THIS FOR EVERY CLASS WE WANT VARIABLES REPLICATED IN
void AWeapon::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AWeapon, WeaponState);
}

void AWeapon::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepOverlap)
{
	// Cast other actor to blaster character.
	ABlasterCharacter* BlasterCharacter = Cast<ABlasterCharacter>(OtherActor);

	// if not null, it's a player.
	if (BlasterCharacter) {
		// Set our weapon to the player
		BlasterCharacter->SetOverlappingWeapon(this);
	}
}

void AWeapon::OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	// Cast other actor to blaster character.
	ABlasterCharacter* BlasterCharacter = Cast<ABlasterCharacter>(OtherActor);

	// if not null, it's a player.
	if (BlasterCharacter) 
	{
		// Set our weapon to the player
		BlasterCharacter->SetOverlappingWeapon(nullptr);
	}
}

void AWeapon::SetWeaponState(EWeaponState State)
{
	WeaponState = State;

	switch (WeaponState) 
	{
	case EWeaponState::EWS_Equipped:
		// Hide widget
		ShowPickupWidget(false);
		AreaSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		break;
	case EWeaponState::EWS_Dropped:
		AreaSphere->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		break;
	case EWeaponState::EWS_Initial:
		AreaSphere->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		break;
	}
}

void AWeapon::OnRep_WeaponState()
{
	switch (WeaponState) 
	{
	case EWeaponState::EWS_Equipped:
		// Hide widget
		ShowPickupWidget(false);
		AreaSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		break;
	case EWeaponState::EWS_Dropped:
		AreaSphere->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		break;
	case EWeaponState::EWS_Initial:
		AreaSphere->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		break;
	}
}


void AWeapon::ShowPickupWidget(bool bShowWidget)
{
	if (PickupWidget) 
	{
		PickupWidget->SetVisibility(bShowWidget);
	}
}

void AWeapon::Fire(const FVector& HitTarget)
{
	if (FireAnimation) 
	{
		WeaponMesh->PlayAnimation(FireAnimation, false);
	}

	if (BulletCasingClass)
	{	
		const USkeletalMeshSocket* AmmoEjectSocket = GetWeaponMesh()->GetSocketByName(FName("AmmoEject"));

		if (AmmoEjectSocket)
		{
			FTransform SocketTransform = AmmoEjectSocket->GetSocketTransform(GetWeaponMesh());

			float RandomRotOffsetY = FMath::RandRange(-20.f, 20.f);
			FRotator SocketRotation = SocketTransform.Rotator();
			SocketRotation.Add(0.f, RandomRotOffsetY, 0.f);


			// Spawn actor at transform
			if (BulletCasingClass)
			{
				UWorld* World = GetWorld();

				if (World)
				{
					World->SpawnActor<ABulletCasing>(
						BulletCasingClass,
						SocketTransform.GetLocation(),
						SocketRotation
					);
				}
			}
		}
	}
}


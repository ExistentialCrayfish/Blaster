// Fill out your copyright notice in the Description page of Project Settings.


#include "BulletCasing.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"


ABulletCasing::ABulletCasing()
{
	PrimaryActorTick.bCanEverTick = false;

	CasingMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CasingMesh"));
	SetRootComponent(CasingMesh);

	CasingMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	CasingMesh->SetSimulatePhysics(true);
	CasingMesh->SetEnableGravity(true);
	// Enable collision events (for OnHit)
	CasingMesh->SetNotifyRigidBodyCollision(true);

	ShellEjectionImpulse = 10.f;

	bHasPlayedImpactSound = false;
}

void ABulletCasing::BeginPlay()
{
	Super::BeginPlay();
	CasingMesh->OnComponentHit.AddDynamic(this, &ABulletCasing::OnHit);
	CasingMesh->AddImpulse(GetActorForwardVector() * ShellEjectionImpulse);

	float RandomRotOffsetY = FMath::RandRange(-90.f, 90.f);
	FVector RandomTorque(0.f, RandomRotOffsetY, 0.f);
	CasingMesh->AddTorqueInDegrees(RandomTorque);

	SetLifeSpan(5.f);
}

void ABulletCasing::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (ShellImpactSound && !bHasPlayedImpactSound)
	{
		UGameplayStatics::PlaySoundAtLocation(
			this,
			ShellImpactSound,
			GetActorLocation()
		);

		bHasPlayedImpactSound = true;
	}
}


// Fill out your copyright notice in the Description page of Project Settings.


#include "TPSPickupActor.h"
#include "Components/SphereComponent.h"
#include "Components/DecalComponent.h"
#include "TPSPowerupActor.h"
#include "TimerManager.h"

// Sets default values
ATPSPickupActor::ATPSPickupActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	//PrimaryActorTick.bCanEverTick = true;

	SphereComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	SphereComp->SetSphereRadius(75.0f);
	RootComponent = SphereComp;


	DecalComp = CreateDefaultSubobject<UDecalComponent>(TEXT("DecalComp"));
	DecalComp->SetRelativeRotation(FRotator(90, 0.0f, 0.0f));
	DecalComp->DecalSize = FVector(64,75,75);
	DecalComp->SetupAttachment(RootComponent);

	CooldownDuration = 10.0f;

	SetReplicates(true);
}

// Called when the game starts or when spawned
void ATPSPickupActor::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority())
	{
		Respawn();
	}	
}

void ATPSPickupActor::Respawn()
{
	if (PowerupClass == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("PowerupClass is nullptr in %s. Assign in blueprint!"),*GetName());
		return;
	}

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	PowerupInstance = GetWorld()->SpawnActor<ATPSPowerupActor>(PowerupClass, GetTransform(), SpawnParams);

}

void ATPSPickupActor::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);
	//grant powerup to player if available

	if (HasAuthority() && PowerupInstance)
	{
		PowerupInstance->ActivatePowerup(OtherActor);
		PowerupInstance = nullptr;//we no longer own the powerup instance

		//set timer to respawn
		GetWorldTimerManager().SetTimer(TimerHandle_RespawnTimer, this, &ATPSPickupActor::Respawn, CooldownDuration);
	}
}

// Called every frame
/*void ATPSPickupActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}*/


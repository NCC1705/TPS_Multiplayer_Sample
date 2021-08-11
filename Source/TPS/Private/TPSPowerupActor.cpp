// Fill out your copyright notice in the Description page of Project Settings.


#include "TPSPowerupActor.h"
#include "Net/UnrealNetwork.h"

// Sets default values
ATPSPowerupActor::ATPSPowerupActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	//PrimaryActorTick.bCanEverTick = true;

	PowerupInterval = 0.0f;
	TickTotalCount = 0;

	bIsPowerupActive = false;
	SetReplicates(true);
}

// Called when the game starts or when spawned
/*void ATPSPowerupActor::BeginPlay()
{
	Super::BeginPlay();
}*/

void ATPSPowerupActor::OnTickPowerup()
{
	TickCounter++;

	OnPowerupTicked();

	if (TickCounter >= TickTotalCount)
	{
		OnExpired();

		bIsPowerupActive = false;
		OnRep_PowerupActive();

		GetWorldTimerManager().ClearTimer(TimerHandle_PowerupTick);
	}
}

void ATPSPowerupActor::OnRep_PowerupActive()
{
	/*if (bIsPowerupActive)
	{

	}
	else
	{

	}*/
	OnPowerupStateChanged(bIsPowerupActive);
}

void ATPSPowerupActor::ActivatePowerup(AActor* BoostedActor)
{
	OnActivated(BoostedActor);

	bIsPowerupActive = true;
	OnRep_PowerupActive();

	if (PowerupInterval > 0.0f)
	{
		GetWorldTimerManager().SetTimer(TimerHandle_PowerupTick, this, &ATPSPowerupActor::OnTickPowerup,
			PowerupInterval, true);
	}
	else
	{
		OnTickPowerup();
	}
}

// Called every frame
/*void ATPSPowerupActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}*/

void ATPSPowerupActor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ATPSPowerupActor, bIsPowerupActive);
}

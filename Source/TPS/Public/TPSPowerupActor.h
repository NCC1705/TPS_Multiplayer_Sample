// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TPSPowerupActor.generated.h"

UCLASS()
class TPS_API ATPSPowerupActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATPSPowerupActor();

protected:
	// Called when the game starts or when spawned
	//virtual void BeginPlay() override;

	//Time between powerup ticks = frequency
	UPROPERTY(EditDefaultsOnly, Category="Powerups")
	float PowerupInterval;

	//Total times we apply the powerup effect
	UPROPERTY(EditDefaultsOnly, Category = "Powerups")
	int32 TickTotalCount;

	int32 TickCounter;

	FTimerHandle TimerHandle_PowerupTick;


	
	UFUNCTION()
	void OnTickPowerup();

	UPROPERTY(ReplicatedUsing = OnRep_PowerupActive)
	bool bIsPowerupActive;

	UFUNCTION()
	void OnRep_PowerupActive();

	UFUNCTION(BlueprintImplementableEvent, Category = "Powerups")
	void OnPowerupStateChanged(bool bNewIsActive);

public:	
	// Called every frame
	//virtual void Tick(float DeltaTime) override;
	
	void ActivatePowerup(AActor* BoostedActor);

	UFUNCTION(BlueprintImplementableEvent, Category = "Powerups")//no implementation in cpp
	void OnActivated(AActor* BoostedActor);

	UFUNCTION(BlueprintImplementableEvent, Category = "Powerups")//no implementation in cpp	
	void OnPowerupTicked();

	UFUNCTION(BlueprintImplementableEvent, Category = "Powerups")
	void OnExpired();


};

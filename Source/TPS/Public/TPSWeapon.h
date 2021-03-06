// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TPSWeapon.generated.h"


class USkeletalMeshComponent;
class UDamageType;
class UParticleSystem;

//contains info of a single hitscan weapon linetrace
USTRUCT()
struct FHitScanTrace
{
	GENERATED_BODY()

public:
	UPROPERTY()
		TEnumAsByte< EPhysicalSurface> SurfaceType;
	//FVector_NetQuantize TraceFrom;//zero decimal point vector precision

	UPROPERTY()
	FVector_NetQuantize TraceTo;
};

UCLASS()
class TPS_API ATPSWeapon : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATPSWeapon();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	USkeletalMeshComponent* MeshComp;
		
	void PlayFireEffects(FVector TraceEnd);

	void PlayImpactEffects(EPhysicalSurface SurfaceType, FVector ImpactPoint);

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Weapon")
	TSubclassOf<UDamageType> DamageType;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	FName MuzzleSocketName;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	FName TracerTargetName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	UParticleSystem* MuzzleEffect;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	UParticleSystem* DefaultImpactEffect;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	UParticleSystem* FleshImpactEffect;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	UParticleSystem* TracerEffect;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	TSubclassOf<UCameraShakeBase> FireCamShake;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	float BaseDamage;

	//UFUNCTION(BlueprintCallable, Category = "Weapon")
	virtual void Fire();//virtual ~ override

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerFire();



	FTimerHandle TimerHandle_TimeBetweenShots;

	float LastFireTime;	

	/* RPM - Bullets per minute fired by this weapon*/
	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	float RateOfFire;

	/* Bullet spread in degrees */
	UPROPERTY(EditDefaultsOnly, Category = "Weapon", meta=(ClampMin=0.0f))
	float BulletSpread;

	//derived from rate of fire
	float TimeBetweenShots;

	UPROPERTY(ReplicatedUsing=OnRep_HitScanTrace)
	FHitScanTrace HitScanTrace;

	UFUNCTION()
	void OnRep_HitScanTrace();

public:	
	// Called every frame
	//virtual void Tick(float DeltaTime) override;
	
	


	void StartFire();
	void StopFire ();
	//BlueprintCallable, BlueprintProtected, Category = "Weapon" - 
	//otherwise blueprints can call this even if protected
};

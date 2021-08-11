// Fill out your copyright notice in the Description page of Project Settings.

//prefix A actor U uobject 
#include "Components/TPSHealthComponent.h"
#include "TPSGameMode.h"
#include "Net/UnrealNetwork.h"

// Sets default values for this component's properties
UTPSHealthComponent::UTPSHealthComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	//PrimaryComponentTick.bCanEverTick = true;

	// ...

	DefaultHealth = 100;	
	bIsDead = false;

	TeamNum = 255;//max

	//SetReplicates(true);
	SetIsReplicatedByDefault(true);
}


// Called when the game starts
void UTPSHealthComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	if (GetOwnerRole()==ROLE_Authority)//we are server
	{
		AActor* MyOwner = GetOwner();
		if (MyOwner)
		{
			MyOwner->OnTakeAnyDamage.AddDynamic(this, &UTPSHealthComponent::HandleTakeAnyDamage);
		}
	}

	Health = DefaultHealth;
}

void UTPSHealthComponent::OnRep_Health(float OldHealth)
{
	float Damage = Health - OldHealth;
	OnHealthChanged.Broadcast(this, Health, Damage, nullptr, nullptr, nullptr);

}

void UTPSHealthComponent::HandleTakeAnyDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser)
{
	if (Damage <= 0.0f || bIsDead)
	{
		return;
	}
	if (DamageCauser != DamagedActor && IsFriendly(DamagedActor, DamageCauser))//GetOwner() 
	{
		return;
	}
	//update health clamped
	Health = FMath::Clamp(Health - Damage, 0.0f, DefaultHealth);
	//UE_LOG(LogTemp, Log, TEXT("Health Changed:%s"), *FString::SanitizeFloat(Health));

	bIsDead = Health <= 0.0f;

	OnHealthChanged.Broadcast(this, Health, Damage, DamageType, InstigatedBy, DamageCauser);

	if (bIsDead)
	{
		ATPSGameMode* GM = Cast<ATPSGameMode>(GetWorld()->GetAuthGameMode());//only valid on server

		if (GM)
		{
			GM->OnActorKilled.Broadcast(GetOwner(), DamageCauser, InstigatedBy);//victim
		}
	}

	
}

void UTPSHealthComponent::Heal(float HealAmount)
{
	if (HealAmount <= 0.0f || Health <= 0)
	{
		return;
	}
	Health = FMath::Clamp(Health + HealAmount,0.0f, DefaultHealth);
	UE_LOG(LogTemp, Log, TEXT("Health Changed:%s+(%s)"), *FString::SanitizeFloat(Health),*FString::SanitizeFloat(HealAmount));
	OnHealthChanged.Broadcast(this, Health, -HealAmount, nullptr, nullptr, nullptr);
}

bool UTPSHealthComponent::IsFriendly(AActor* ActorA, AActor* ActorB)
{
	if (ActorA == nullptr || ActorB == nullptr)
	{
		//assume friendly
		return true;
	}

	UTPSHealthComponent* HealthCompA = Cast<UTPSHealthComponent>(ActorA->GetComponentByClass(UTPSHealthComponent::StaticClass()));
	UTPSHealthComponent* HealthCompB = Cast<UTPSHealthComponent>(ActorB->GetComponentByClass(UTPSHealthComponent::StaticClass()));
	
	if (HealthCompA == nullptr || HealthCompB == nullptr)
	{
		//assume friendly
		return true;
	}

	return HealthCompA->TeamNum == HealthCompB->TeamNum;
	//return false;
}

float UTPSHealthComponent::GetHealth() const//const = read only acces to class/variable
{
	return Health;
}


void UTPSHealthComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UTPSHealthComponent, Health);
}

// Called every frame
//void UTPSHealthComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
//{
//	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
//
//	// ...
//}


// Fill out your copyright notice in the Description page of Project Settings.


#include "TPSGameMode.h"
#include "TimerManager.h"
#include "TPSGameState.h"
#include "TPSPlayerState.h"
#include "Components/TPSHealthComponent.h"


ATPSGameMode::ATPSGameMode()
{
	TimeBetweenWaves = 2.0f;
	
	GameStateClass = ATPSGameState::StaticClass();
	PlayerStateClass = ATPSPlayerState::StaticClass();


	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.TickInterval = 1.0f;
}

void ATPSGameMode::StartWave()
{
	WaveCount++;

	NrOfBotsToSpawn = 2 * WaveCount;

	GetWorldTimerManager().SetTimer(TimerHandle_BotSpawner,this,&ATPSGameMode::SpawnBotTimerElapsed,1.0f,true,0.0f);

	SetWaveState(EWaveState::WaveInProgress);
}

void ATPSGameMode::EndWave()
{
	GetWorldTimerManager().ClearTimer(TimerHandle_BotSpawner);
	PrepareForNextWave();
	SetWaveState(EWaveState::WaveComplete);
}

void ATPSGameMode::PrepareForNextWave()
{
	//not use this anywhere else, cancel it, check its timer etc	
	GetWorldTimerManager().SetTimer(TimerHandle_NextWaveStart, this, &ATPSGameMode::StartWave, TimeBetweenWaves , false);
	SetWaveState(EWaveState::WaitingToStart);
	RestartDeadPlayers();
}

void ATPSGameMode::CheckWaveState()
{
	bool bIsPreparingForWave = GetWorldTimerManager().IsTimerActive(TimerHandle_NextWaveStart);

	if (NrOfBotsToSpawn > 0|| bIsPreparingForWave)//wave not finished / next already preparing
	{
		return;
	}

	bool bIsAnyBotAlive = false;

	for (FConstPawnIterator It = GetWorld()->GetPawnIterator(); It; ++It)
	{
		APawn* TestPawn = It->Get();
		if (TestPawn == nullptr||TestPawn->IsPlayerControlled()) 
		{
			continue;//we only search non player controlled / AI controlled
		}

		UTPSHealthComponent* HealthComp = Cast<UTPSHealthComponent>(TestPawn->GetComponentByClass(UTPSHealthComponent::StaticClass()));

		if (HealthComp&& HealthComp->GetHealth()>0.0f)
		{
			bIsAnyBotAlive = true;
			break;
		}
	}

	if (!bIsAnyBotAlive)
	{
		PrepareForNextWave();
		SetWaveState(EWaveState::WaveComplete);
	}

}

void ATPSGameMode::CheckAnyPlayerAlive()
{
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator();It;++It)
	{
		APlayerController* PC = It->Get();

		if (PC && PC->GetPawn())
		{
			APawn* MyPawn = PC->GetPawn();
			UTPSHealthComponent* HealthComp = 
				Cast<UTPSHealthComponent>( MyPawn->GetComponentByClass(UTPSHealthComponent::StaticClass()));
			if (ensure(HealthComp) && HealthComp->GetHealth() > 0.0f)//breaks code when check returns false
			{
				//a player is still alive
				return;
			}
		}
	}

	//no player alive
	
	GameOver();
}

void ATPSGameMode::GameOver()
{
	EndWave();
	//TODO finish match, display game over to players
	SetWaveState(EWaveState::GameOver);

	UE_LOG(LogTemp, Log, TEXT("GAME OVER"));


}

void ATPSGameMode::SetWaveState(EWaveState NewState)
{
	ATPSGameState* GS = GetGameState<ATPSGameState>();
	if (ensureAlways(GS))
	{
		GS->SetWaveState(NewState);
	}
}

void ATPSGameMode::RestartDeadPlayers()
{
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		APlayerController* PC = It->Get();

		if (PC && PC->GetPawn()==nullptr)//unpossessed
		{
			RestartPlayer(PC);
		}
	}
}



void ATPSGameMode::StartPlay()
{
	Super::StartPlay();

	PrepareForNextWave();
}

void ATPSGameMode::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	CheckWaveState();
	CheckAnyPlayerAlive();	
}

void ATPSGameMode::SpawnBotTimerElapsed()
{
	SpawnNewBot();

	NrOfBotsToSpawn--;

	if (NrOfBotsToSpawn <= 0)
	{
		EndWave();		
	}
}
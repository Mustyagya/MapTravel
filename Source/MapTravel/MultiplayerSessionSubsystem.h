#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "MultiplayerSessionSubsystem.generated.h"


/**
 * 
 */
UCLASS()
class MAPTRAVEL_API UMultiplayerSessionSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	public:
		UMultiplayerSessionSubsystem();
		void Initialize(FSubsystemCollectionBase& Collection ) override;
		void Deinitialize() override;

		IOnlineSessionPtr SessionInterface;
UFUNCTION(BlueprintCallable)
void CreateServer(FString ServerName);
	
UFUNCTION(BlueprintCallable)
void JoinServer(FString ServerName);

//void OnCreateSessionComplete(FName SessionName, bool WasSuccessful);
void OnCreateSessionComplete(FName SessionName, bool WasSuccessful);
};
// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "OnlineSessionSettings.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "LevelTransitionBox.generated.h"

UCLASS()
class MAPTRAVEL_API ALevelTransitionBox : public AActor
{
	GENERATED_BODY()
	
public:
    ALevelTransitionBox();

    static void PrintString(const FString& Message);

    // Ссылка на сессию
    IOnlineSessionPtr SessionInterface;

    // Делегаты для перехода на новую карту
    void OnCreateSessionComplete(FName SessionName, bool bWasSuccessful );
    
    void OnTravelToMap(bool bWasSuccessful, int32 ControllerId, FUniqueNetIdPtr UserId, const FOnlineSessionSearchResult& SearchResult);
   

protected:
    virtual void BeginPlay() override;

    // Коробка коллизии
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Collision")
    class UBoxComponent* CollisionBox;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Level")
    FString LevelName;
    FString FirstMap;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collision")
    FVector BoxExtent;

    UFUNCTION()
    void OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor,
        class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
        const FHitResult& SweepResult);

public:
    virtual void Tick(float DeltaTime) override;

    UFUNCTION(BlueprintCallable, Category = "Collision")
    void SetBoxExtent(FVector NewExtent);
    UFUNCTION(BlueprintCallable, Category = "MapTravel")
    void ReturnToFirstMap();

private:
    // Переход хоста на новую карту
    void HostTravelToNewMap();
};

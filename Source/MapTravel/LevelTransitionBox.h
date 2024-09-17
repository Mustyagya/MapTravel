// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "OnlineSessionSettings.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "Engine/TextureDefines.h"
#include "LevelTransitionBox.generated.h"

UCLASS()
class MAPTRAVEL_API ALevelTransitionBox : public AActor
{
    GENERATED_BODY()

public:
    // Sets default values for this actor's properties
    ALevelTransitionBox();

protected:
    // Called when the game starts or when spawned
    virtual void BeginPlay() override;

   /*

    // Переход хоста на новую карту
    void HostTravelToNewMap();

    // Возврат хоста на первую карту
    void ReturnToFirstMap();

    // Логика для клиента: предложение перейти на новую карту
    void OfferClientToTravel(const FString& CurrentLevel);

    // Показ UI клиенту с предложением перехода
    void ShowTravelUI(const FString& TargetMap);

    // Переход клиента на новую карту (после выбора в UI)
    void ClientTravelToNewMap(const FString& TargetMap);
    */
    // Вспомогательная функция для вывода сообщений на экран
    void PrintString(const FString& Message);
    
public:
    // Called every frame
    virtual void Tick(float DeltaTime) override;
    // Вызывается при пересечении границ зоны перехода
    
    // Установка размеров зоны
    void SetBoxExtent(FVector NewExtent);

    
    
    UFUNCTION()
    void OnOverlapBegin
    (UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
        UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

    // Компонент коллизии для зоны перехода
    UPROPERTY(VisibleAnywhere)
    class UBoxComponent* CollisionBox;


    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Level")
    FString LevelName;  // Имя нового уровня

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Level")
    FString FirstMap;  // Имя первой карты (возврат)

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Level")
    FVector BoxExtent;  // Размеры коллизии бокса

    //---------------------------------------------------------
    // Новая функция для перехода на другой уровень
    UFUNCTION(BlueprintCallable, Category = "Level Transition")
    void SwitchLevel(FString NewLevelName);  // Указание нового уровня

    //---------------------------------------------------------
    // Новая функция для выгрузки уровня
    UFUNCTION(BlueprintCallable, Category = "Level Transition")
    void UnloadLevel(FString UnloadLevelName);

    //---------------------------------------------------------
    // Обновление уровня на сервере
    UFUNCTION(Server, Reliable, WithValidation)
    void Server_UpdatePlayerLevel(const FString& ServerNewLevel);  // Передача нового уровня

    //---------------------------------------------------------
    // Multicast для обновления уровня на всех клиентах
    UFUNCTION(NetMulticast, Reliable)
    void Multicast_UpdatePlayerLevel(const FString& MulticasNewLevel);

    //---------------------------------------------------------
    // Вспомогательные функции для вывода сообщений
    void PrintString(const FString& Message);

    
private:
    //

    //// Размеры зоны
    //FVector BoxExtent;

    //// Имя карты для перехода (вторая карта)
    //FString LevelName;

    //// Имя первой карты
    //FString FirstMap;

    //// Интерфейс сессий для работы с сетевыми сессиями
    //IOnlineSessionPtr SessionInterface;
};



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

    // ������� ����� �� ����� �����
    void HostTravelToNewMap();

    // ������� ����� �� ������ �����
    void ReturnToFirstMap();

    // ������ ��� �������: ����������� ������� �� ����� �����
    void OfferClientToTravel(const FString& CurrentLevel);

    // ����� UI ������� � ������������ ��������
    void ShowTravelUI(const FString& TargetMap);

    // ������� ������� �� ����� ����� (����� ������ � UI)
    void ClientTravelToNewMap(const FString& TargetMap);
    */
    // ��������������� ������� ��� ������ ��������� �� �����
    void PrintString(const FString& Message);
    
public:
    // Called every frame
    virtual void Tick(float DeltaTime) override;
    // ���������� ��� ����������� ������ ���� ��������
    
    // ��������� �������� ����
    void SetBoxExtent(FVector NewExtent);

    
    
    UFUNCTION()
    void OnOverlapBegin
    (UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
        UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

    // ��������� �������� ��� ���� ��������
    UPROPERTY(VisibleAnywhere)
    class UBoxComponent* CollisionBox;


    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Level")
    FString LevelName;  // ��� ������ ������

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Level")
    FString FirstMap;  // ��� ������ ����� (�������)

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Level")
    FVector BoxExtent;  // ������� �������� �����

    //---------------------------------------------------------
    // ����� ������� ��� �������� �� ������ �������
    UFUNCTION(BlueprintCallable, Category = "Level Transition")
    void SwitchLevel(FString NewLevelName);  // �������� ������ ������

    //---------------------------------------------------------
    // ����� ������� ��� �������� ������
    UFUNCTION(BlueprintCallable, Category = "Level Transition")
    void UnloadLevel(FString UnloadLevelName);

    //---------------------------------------------------------
    // ���������� ������ �� �������
    UFUNCTION(Server, Reliable, WithValidation)
    void Server_UpdatePlayerLevel(const FString& ServerNewLevel);  // �������� ������ ������

    //---------------------------------------------------------
    // Multicast ��� ���������� ������ �� ���� ��������
    UFUNCTION(NetMulticast, Reliable)
    void Multicast_UpdatePlayerLevel(const FString& MulticasNewLevel);

    //---------------------------------------------------------
    // ��������������� ������� ��� ������ ���������
    void PrintString(const FString& Message);

    
private:
    //

    //// ������� ����
    //FVector BoxExtent;

    //// ��� ����� ��� �������� (������ �����)
    //FString LevelName;

    //// ��� ������ �����
    //FString FirstMap;

    //// ��������� ������ ��� ������ � �������� ��������
    //IOnlineSessionPtr SessionInterface;
};



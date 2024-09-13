// Fill out your copyright notice in the Description page of Project Settings.


#include "LevelTransitionBox.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"
#include "Engine/Texture.h"
// Sets default values

ALevelTransitionBox::ALevelTransitionBox()
{
    PrimaryActorTick.bCanEverTick = true;
    CollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionBox"));
    RootComponent = CollisionBox;
    BoxExtent = FVector(100.0f, 100.0f, 100.0f);
    CollisionBox->SetBoxExtent(BoxExtent);
    CollisionBox->SetGenerateOverlapEvents(true);
    CollisionBox->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
    CollisionBox->OnComponentBeginOverlap.AddDynamic(this, &ALevelTransitionBox::OnOverlapBegin);
}

void ALevelTransitionBox::BeginPlay()
{
    Super::BeginPlay();
    LevelName = "/Game/ThirdPerson/Maps/ThirdPersonMap1";
     FirstMap = "/Game/ThirdPerson/Maps/ThirdPersonMap";  // ����� ���� � ������ �����
    // ����������� � ������
    IOnlineSubsystem* OnlineSubsystem = IOnlineSubsystem::Get();
    if (OnlineSubsystem)
    {
        SessionInterface = OnlineSubsystem->GetSessionInterface();
        if (SessionInterface.IsValid())
        {
            SessionInterface->OnCreateSessionCompleteDelegates.AddUObject(this, &ALevelTransitionBox::OnCreateSessionComplete);
        }
    }

    CollisionBox->SetBoxExtent(BoxExtent);
}

void ALevelTransitionBox::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    if (OtherActor && OtherActor != this)
    {
        if (ACharacter* Character = Cast<ACharacter>(OtherActor))
        {
            // ��������, �� ����� ����� ����� ��������� ������
            FString CurrentLevel = GetWorld()->GetMapName();

            // ������� �������� ������, ����� �������� ������ ��� �����
            CurrentLevel.RemoveFromStart(GetWorld()->StreamingLevelsPrefix);

            if (HasAuthority()) // ��������, ���� ��� ����
            {
                if (CurrentLevel == "ThirdPersonMap") // ���� �� ������ �����
                {
                    PrintString("Host is traveling to the second map...");
                    HostTravelToNewMap();  // ������� �� ������ �����
                }
                else if (CurrentLevel == "ThirdPersonMap1") // ���� �� ������ �����
                {
                    PrintString("Host is returning to the first map...");
                    ReturnToFirstMap();  // ������� �� ������ �����
                }
            }
            else // ���������� �������
            {
                if (CurrentLevel == "ThirdPersonMap") // ���� �� ������ �����
                {
                    PrintString("Client is traveling to the second map...");
                    APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
                    if (PC)
                    {
                        PC->ClientTravel(LevelName, TRAVEL_Absolute);  // ������� ������� �� ������ �����
                    }
                }
                else if (CurrentLevel == "ThirdPersonMap1") // ���� �� ������ �����
                {
                    PrintString("Client is returning to the first map...");
                    APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
                    if (PC)
                    {
                        PC->ClientTravel(FirstMap, TRAVEL_Absolute);  // ������� ������� �� ������ �����
                    }
                }
            }
        }
    }
}

void ALevelTransitionBox::HostTravelToNewMap()
{
  
    if (!LevelName.IsEmpty())
    {
        // ���������, �������� �� ������ ����� ������
        if (HasAuthority())// ���� ����
        {
            // ������� ����� �� ����� ����� � ������� ServerTravel
            PrintString("Server traveling to new map...");
            GetWorld()->ServerTravel(LevelName + TEXT("?listen"));// ���� ��������� �� ����� �����
        }
        else
        {
            // ���� ��� ������, ������ ��������� �� ����� ����� ��������������
            APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
            if (PC)
            {
                PrintString("Client trevel to new map!");
                PC->ClientTravel(LevelName, TRAVEL_Absolute);// ������ ��������� �� ����� �����
            }
        }
        
    }
    else
    {
        PrintString("Level Name is empty. Cannot travel to new map.");
    }
}

void ALevelTransitionBox::OnCreateSessionComplete(FName SessionName, bool bWasSuccessful)
{
    if (bWasSuccessful)
    {
        PrintString(FString::Printf(TEXT("Session %s created successfully"), *SessionName.ToString()));

        // ��������� ������� ��� ���������� ��������
        if (SessionInterface.IsValid())
        {
            // ������� ����� ���������� ��� ������ �����, �� �� ��� �������� �������� �� ������ �����
            SessionInterface->OnSessionUserInviteAcceptedDelegates.AddUObject(this, &ALevelTransitionBox::OnTravelToMap);
        }
    }
}
// ������� ����������, ����� ���������� ������� �������� �� ����� �����
void ALevelTransitionBox::OnTravelToMap(bool bWasSuccessful, int32 ControllerId, FUniqueNetIdPtr UserId, const FOnlineSessionSearchResult& SearchResult)
{
   

    if (bWasSuccessful)
    {
        PrintString("Map transition request received.");
       

        if (!LevelName.IsEmpty())
        {
            if (HasAuthority())//���� ���������
            {
                PrintString("Host is Traveling to new map");
                    // ������� ��������� �� �� �� �����, ��� � ����
                    UGameplayStatics::OpenLevel(GetWorld(), FName(LevelName), true);
            }
            else
            {
                APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(),0);
                if (PC)
                {
                    PrintString("Client travel to the new map");
                    PC->ClientTravel(LevelName, TRAVEL_Absolute);
                }
            }
        }
        else
        {
            PrintString("Level Name is empty. Cannot travel to new map");
        }
    }
    else
    {
        PrintString("Failed to transition to the new map.");
    }
}

void ALevelTransitionBox::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

void ALevelTransitionBox::SetBoxExtent(FVector NewExtent)
{
    BoxExtent = NewExtent;
    if (CollisionBox)
    {
        CollisionBox->SetBoxExtent(BoxExtent);
    }
}
// ������� ��� �������� �� ������ �����, �������� ��� ��� �����, ��� � ��� �������
void ALevelTransitionBox::ReturnToFirstMap()
{
    
    PrintString("Trying to return to the first map...");
    if(!FirstMap.IsEmpty())
    { 

        if (HasAuthority())  // ���� ��� ����
        {
            PrintString("Returning to the first map...");
            GetWorld()->ServerTravel(FirstMap + TEXT("?listen"));
        }
        else//���� ��� ������
        {
            APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
            if (PC)
            {
                PrintString("Client returning to the first map...");
                PC->ClientTravel(FirstMap, TRAVEL_Absolute);
            }
        }
    }
    else
    {
        PrintString("First map name is empty. Cannot return to the first map.");

    }
}

void ALevelTransitionBox::PrintString(const FString& Message)
{
    if (GEngine)
    {
        GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, Message);
    }
}

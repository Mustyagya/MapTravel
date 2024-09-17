#include "LevelTransitionBox.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"
#include "Engine/Texture.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "OnlineSubsystem.h"
#include "Interfaces/OnlineSessionInterface.h"

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
    LevelName = "/Game/ThirdPerson/Maps/ThirdPersonMap1";  // ���� � ������ �����
    FirstMap = "/Game/ThirdPerson/Maps/ThirdPersonMap";    // ���� � ������ �����
}
void ALevelTransitionBox::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}
void ALevelTransitionBox::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    if (OtherActor && OtherActor != this)
    {
        if (ACharacter* Character = Cast<ACharacter>(OtherActor))
        {
            FString CurrentLevel = GetWorld()->GetMapName();
            CurrentLevel.RemoveFromStart(GetWorld()->StreamingLevelsPrefix);

            // ��������� � ��������: ��������� ���� ������� ��� ������� � ������� GetWorld()->IsServer()
             //------------------------------------------------

            if (GetNetMode() == NM_DedicatedServer || GetNetMode() == NM_ListenServer) // ��������, ���� ��� ������
            {

                if (CurrentLevel == "ThirdPersonMap") // ���� �� ������ �����
                {
                    PrintString("Host is traveling to the second map...");
                    /*HostTravelToNewMap();  // ������� �� ������ �����*/
                    GetWorld()->ServerTravel(LevelName + TEXT("?listen"));
                }
                else if (CurrentLevel == "ThirdPersonMap1") // ���� �� ������ �����
                {
                    PrintString("Host is returning to the first map...");
                    /*ReturnToFirstMap();  // ������� �� ������ �����*/
                    GetWorld()->ServerTravel(LevelName + TEXT("?listen"));
                }
            }
            else // ���������� �������
            {
                APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
                if (PC && PC->IsLocalController()) // ��������, ��� ��� ��������� ������
                {
                    if (CurrentLevel == "ThirdPersonMap") // ���� �� ������ �����
                    {
                        PrintString("Client is traveling to the second map...");
                        PC->ClientTravel(LevelName, TRAVEL_Absolute);  // ������� ������� �� ������ �����
                    }
                    else if (CurrentLevel == "ThirdPersonMap1") // ���� �� ������ �����
                    {
                        PrintString("Client is returning to the first map...");
                        PC->ClientTravel(FirstMap, TRAVEL_Absolute);  // ������� ������� �� ������ �����
                    }
                }
            }
            //------------------------------------------------
        }
    }
}
//---------------------------------------------------------
// ������� �� ����� �������
void ALevelTransitionBox::SwitchLevel(FString NewLevelName)
{
    if (GetNetMode() == NM_DedicatedServer || GetNetMode() == NM_ListenServer)
    {
        GetWorld()->ServerTravel(NewLevelName + TEXT("?listen"));
    }
    else
    {
        APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(),0);
        if (PC)
        {
            PC->ClientTravel(NewLevelName, TRAVEL_Absolute);
        }
    }
}
//---------------------------------------------------------
// �������� ������
void ALevelTransitionBox::UnloadLevel(FString UnloadLevelName)
{
}
//---------------------------------------------------------
// ������ ��������� ������� ������
void ALevelTransitionBox::Server_UpdatePlayerLevel(const FString& ServerNewLevel)
{
    if (HasAuthority())
    {
        LevelName = ServerNewLevel;
        Multicast_UpdatePlayerLevel(ServerNewLevel);// ����� Multicast
    }
}
//---------------------------------------------------------
// Multicast ��� ���������� ������ �� ���� ��������
void ALevelTransitionBox::Multicast_UpdatePlayerLevel(const FString& MulticasNewLevel)
{
    LevelName = MulticasNewLevel;
}




/*void ALevelTransitionBox::OfferClientToTravel(const FString& CurrentLevel)
{
    // ���������� ������� UI � ������������ ������� �� ������ �����
    // ��� �����, ��� ������ ���� ������ ���������������� ��������� ��� ������ ��������
    if (CurrentLevel == "ThirdPersonMap") // �� ������ �����
    {
        PrintString("Offering client travel to the second map...");
        ShowTravelUI(LevelName); // �������� UI ��� ������ �������� �� ������ �����
    }
    else if (CurrentLevel == "ThirdPersonMap1") // �� ������ �����
    {
        PrintString("Offering client travel back to the first map...");
        ShowTravelUI(FirstMap); // �������� UI ��� �������� �� ������ �����
    }
}*/

/*void ALevelTransitionBox::ShowTravelUI(const FString& TargetMap)
{
    // ���� ����� ���������� UI, ��� ������ ��������, ����� �� �� ���������� �� ����� �����
    // ����������� ����� UMG ��� ������ �������� ����������
    PrintString(FString::Printf(TEXT("Showing UI for client to travel to: %s"), *TargetMap));

    // ���� ����� �����������, �������� ������� ��� ��������:
    // ClientTravelToNewMap(TargetMap);
}*/

/*void ALevelTransitionBox::ClientTravelToNewMap(const FString& TargetMap)
{
    APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
    if (PC)
    {
        PrintString(FString::Printf(TEXT("Client traveling to map: %s"), *TargetMap));
        PC->ClientTravel(TargetMap, TRAVEL_Absolute); // ������ ��������� �� ��������� �����
    }
}*/

/*void ALevelTransitionBox::HostTravelToNewMap()
{
    if (!LevelName.IsEmpty())
    {
        // ��������� � ��������: ��������� ���� ������� � ������� GetWorld()->IsServer()
        //------------------------------------------------
        if (GetNetMode() == NM_DedicatedServer || GetNetMode() == NM_ListenServer) // ���� ��� ������
        {
            PrintString("Server traveling to new map...");
            GetWorld()->ServerTravel(LevelName + TEXT("?listen")); // ���� ��������� �� ����� �����
        }
        else
        {
            APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
            if (PC && PC->IsLocalController()) // ��������, ��� ��� ��������� ������
            {
                PrintString("Client traveling to new map...");
                PC->ClientTravel(LevelName, TRAVEL_Absolute); // ������ ��������� �� ����� �����
            }
        }
        //------------------------------------------------
    }
    else
    {
        PrintString("Level Name is empty. Cannot travel to new map.");
    }
}*/
/*
void ALevelTransitionBox::ReturnToFirstMap()
{
    if (!FirstMap.IsEmpty())
    {
        // ���������: �������� �� ������ ����� GetWorld()->IsServer()
         //------------------------------------------------
        if (GetNetMode() == NM_DedicatedServer || GetNetMode() == NM_ListenServer)  // ���� ��� ������
        {
            PrintString("Returning to the first map...");
            GetWorld()->ServerTravel(FirstMap + TEXT("?listen"));
        }
        else
        {
            APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
            if (PC && PC->IsLocalController()) // ��������, ��� ��� ��������� ������
            {
                PrintString("Client returning to the first map...");
                PC->ClientTravel(FirstMap, TRAVEL_Absolute);
            }
        }
        //------------------------------------------------
    }
    else
    {
        PrintString("First map name is empty. Cannot return to the first map.");
    }
}
*/




void ALevelTransitionBox::SetBoxExtent(FVector NewExtent)
{
    BoxExtent = NewExtent;
    if (CollisionBox)
    {
        CollisionBox->SetBoxExtent(BoxExtent);
    }
}

void ALevelTransitionBox::PrintString(const FString& Message)
{
    FString RoleMessage;

    switch (GetNetMode())
    {
    case NM_Client:
        RoleMessage = "[Client] ";
        break;
    case NM_ListenServer:
    case NM_DedicatedServer:
        RoleMessage = "[Server] ";
        break;
    default:
        RoleMessage = "[Standalone] ";
        break;
    }
    if (GEngine)
    {
        GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, Message);
    }
}

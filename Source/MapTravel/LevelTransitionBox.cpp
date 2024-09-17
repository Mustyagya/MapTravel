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
    LevelName = "/Game/ThirdPerson/Maps/ThirdPersonMap1";  // Путь к второй карте
    FirstMap = "/Game/ThirdPerson/Maps/ThirdPersonMap";    // Путь к первой карте
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

            // Изменение в проверке: проверяем роль сервера или клиента с помощью GetWorld()->IsServer()
             //------------------------------------------------

            if (GetNetMode() == NM_DedicatedServer || GetNetMode() == NM_ListenServer) // Проверка, если это сервер
            {

                if (CurrentLevel == "ThirdPersonMap") // Если на первой карте
                {
                    PrintString("Host is traveling to the second map...");
                    /*HostTravelToNewMap();  // Переход на вторую карту*/
                    GetWorld()->ServerTravel(LevelName + TEXT("?listen"));
                }
                else if (CurrentLevel == "ThirdPersonMap1") // Если на второй карте
                {
                    PrintString("Host is returning to the first map...");
                    /*ReturnToFirstMap();  // Возврат на первую карту*/
                    GetWorld()->ServerTravel(LevelName + TEXT("?listen"));
                }
            }
            else // Клиентская сторона
            {
                APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
                if (PC && PC->IsLocalController()) // Проверка, что это локальный клиент
                {
                    if (CurrentLevel == "ThirdPersonMap") // Если на первой карте
                    {
                        PrintString("Client is traveling to the second map...");
                        PC->ClientTravel(LevelName, TRAVEL_Absolute);  // Переход клиента на вторую карту
                    }
                    else if (CurrentLevel == "ThirdPersonMap1") // Если на второй карте
                    {
                        PrintString("Client is returning to the first map...");
                        PC->ClientTravel(FirstMap, TRAVEL_Absolute);  // Возврат клиента на первую карту
                    }
                }
            }
            //------------------------------------------------
        }
    }
}
//---------------------------------------------------------
// Переход на новый уровень
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
// Выгрузка уровня
void ALevelTransitionBox::UnloadLevel(FString UnloadLevelName)
{
}
//---------------------------------------------------------
// Сервер обновляет уровень игрока
void ALevelTransitionBox::Server_UpdatePlayerLevel(const FString& ServerNewLevel)
{
    if (HasAuthority())
    {
        LevelName = ServerNewLevel;
        Multicast_UpdatePlayerLevel(ServerNewLevel);// Вызов Multicast
    }
}
//---------------------------------------------------------
// Multicast для обновления уровня на всех клиентах
void ALevelTransitionBox::Multicast_UpdatePlayerLevel(const FString& MulticasNewLevel)
{
    LevelName = MulticasNewLevel;
}




/*void ALevelTransitionBox::OfferClientToTravel(const FString& CurrentLevel)
{
    // Показываем клиенту UI с предложением перейти на другую карту
    // Это место, где должен быть вызван пользовательский интерфейс для выбора перехода
    if (CurrentLevel == "ThirdPersonMap") // На первой карте
    {
        PrintString("Offering client travel to the second map...");
        ShowTravelUI(LevelName); // Вызываем UI для выбора перехода на вторую карту
    }
    else if (CurrentLevel == "ThirdPersonMap1") // На второй карте
    {
        PrintString("Offering client travel back to the first map...");
        ShowTravelUI(FirstMap); // Вызываем UI для возврата на первую карту
    }
}*/

/*void ALevelTransitionBox::ShowTravelUI(const FString& TargetMap)
{
    // Этот метод показывает UI, где клиент выбирает, хочет ли он переходить на новую карту
    // Реализуется через UMG или другой механизм интерфейса
    PrintString(FString::Printf(TEXT("Showing UI for client to travel to: %s"), *TargetMap));

    // Если игрок соглашается, вызываем функцию для перехода:
    // ClientTravelToNewMap(TargetMap);
}*/

/*void ALevelTransitionBox::ClientTravelToNewMap(const FString& TargetMap)
{
    APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
    if (PC)
    {
        PrintString(FString::Printf(TEXT("Client traveling to map: %s"), *TargetMap));
        PC->ClientTravel(TargetMap, TRAVEL_Absolute); // Клиент переходит на указанную карту
    }
}*/

/*void ALevelTransitionBox::HostTravelToNewMap()
{
    if (!LevelName.IsEmpty())
    {
        // Изменение в проверке: проверяем роль сервера с помощью GetWorld()->IsServer()
        //------------------------------------------------
        if (GetNetMode() == NM_DedicatedServer || GetNetMode() == NM_ListenServer) // Если это сервер
        {
            PrintString("Server traveling to new map...");
            GetWorld()->ServerTravel(LevelName + TEXT("?listen")); // Хост переходит на новую карту
        }
        else
        {
            APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
            if (PC && PC->IsLocalController()) // Проверка, что это локальный клиент
            {
                PrintString("Client traveling to new map...");
                PC->ClientTravel(LevelName, TRAVEL_Absolute); // Клиент переходит на новую карту
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
        // Изменение: проверка на сервер через GetWorld()->IsServer()
         //------------------------------------------------
        if (GetNetMode() == NM_DedicatedServer || GetNetMode() == NM_ListenServer)  // Если это сервер
        {
            PrintString("Returning to the first map...");
            GetWorld()->ServerTravel(FirstMap + TEXT("?listen"));
        }
        else
        {
            APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
            if (PC && PC->IsLocalController()) // Проверка, что это локальный клиент
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

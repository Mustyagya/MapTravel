// Fill out your copyright notice in the Description page of Project Settings.


#include "MultiplayerSessionSubsystem.h"
#include "OnlineSubsystem.h"
#include "OnlineSessionSettings.h"


void PrintString(const FString& str)
{
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Cyan, str);
	}
}
UMultiplayerSessionSubsystem::UMultiplayerSessionSubsystem()
{
	//PrintString("MSS Constructor");

	CreateServerAfterDestroy = false;
	DestroyServerName = "";

}

void UMultiplayerSessionSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	//PrintString("MSS Initialize");
	// 
	// Получаем подсистему онлайн-сервисов (например, Steam, NULL или другие).
	IOnlineSubsystem* OnlineSubsystem = IOnlineSubsystem::Get();

		// Проверяем, удалось ли получить подсистему (например, если Steam доступен, то он должен быть здесь).
		if (OnlineSubsystem)
		{
			// Получаем имя используемой подсистемы и конвертируем его в строку для вывода.
			FString SubsystemName = OnlineSubsystem->GetSubsystemName().ToString();
			// Выводим название подсистемы (например, "Steam" или "NULL").
			PrintString(SubsystemName);
			// Получаем интерфейс сессий из подсистемы.
			SessionInterface = OnlineSubsystem->GetSessionInterface();

					// Проверяем, что интерфейс сессий действителен (не `nullptr`).
					if (SessionInterface.IsValid())
					{
						PrintString("Session interface is valid!");

						//делегат
						SessionInterface->OnCreateSessionCompleteDelegates.AddUObject(this, &UMultiplayerSessionSubsystem::OnCreateSessionComplete);
						SessionInterface->OnDestroySessionCompleteDelegates.AddUObject(this, &UMultiplayerSessionSubsystem::OnDestroySessionComplete);
					}
		
	}
}

void UMultiplayerSessionSubsystem::Deinitialize()
{
	//UE_LOG(LogTemp,Warning ,TEXT("MSS Deinitialize"));
}

void UMultiplayerSessionSubsystem::CreateServer(FString ServerName)
{
	PrintString("Map Create!");
	if (ServerName.IsEmpty())
	{
		PrintString("Server Name can not bee empty");
		return;
	}

	// Создаём объект FName для хранения имени сессии, которое будет использовано в игре.
	FName MySessionName = FName("Session name for Game");

	FNamedOnlineSession* ExisteingSession = SessionInterface->GetNamedSession(MySessionName);//находим существующую сесию чтобы удалить

	if (ExisteingSession)
	{
		FString Msg = FString::Printf(TEXT("Session with name %s alredy exists, destroying it"), *MySessionName.ToString());
		PrintString(Msg);

		CreateServerAfterDestroy = true;
		DestroyServerName = ServerName;
		SessionInterface->DestroySession(MySessionName);
		return;

	}

	// Создаём настройки для новой сессии.
	FOnlineSessionSettings SessionSettings;

	SessionSettings.bAllowJoinInProgress = true;// Разрешаем присоединение к сессии после её начала.
	SessionSettings.bIsDedicated = false;// Устанавливаем, что это не выделенный сервер (игра будет работать и на клиенте).
	SessionSettings.bShouldAdvertise = true;// Разрешаем рекламу сессии, чтобы её могли находить другие игроки.
	SessionSettings.NumPrivateConnections = 2;// Задаём количество приватных подключений (2 частных слота).
	SessionSettings.bUseLobbiesIfAvailable = true;// Используем лобби, если оно доступно (например, для Steam-сессий).
	SessionSettings.bUsesPresence = true;// Указываем, что сессия использует информацию о присутствии игроков (например, статус в Steam).
	SessionSettings.bAllowJoinViaPresence = true;// Разрешаем игрокам присоединяться через систему присутствия (например, через список друзей).
	bool IsLan = false;// Локальная переменная для хранения информации о том, является ли это LAN-сессией.


	// Проверяем, используется ли локальная подсистема (например, NULL — это LAN-сессия).
	if (IOnlineSubsystem::Get()->GetSubsystemName() == "NULL")
	{
		IsLan = true;
	}
	// Устанавливаем, что это LAN-сессия (если это локальная сеть).
	SessionSettings.bIsLANMatch = IsLan;
	// Создаём сессию с указанными параметрами: локальный игрок (index 0), имя сессии, и настройки сессии.
	SessionInterface->CreateSession(0, MySessionName, SessionSettings);
}

void UMultiplayerSessionSubsystem::JoinServer(FString ServerName)
{
	PrintString("Join to Map!");

	
}

void UMultiplayerSessionSubsystem::OnCreateSessionComplete(FName SessionName, bool WasSuccessful)
{
	PrintString(FString::Printf(TEXT("OnCreateSessionComplete: %d"), WasSuccessful));

	if (WasSuccessful)
	{
		GetWorld()->ServerTravel("/Game/ThirdPerson/Maps/ThirdPersonMap?listen");
	}

}

void UMultiplayerSessionSubsystem::OnDestroySessionComplete(FName SessionName, bool WasSuccessful)
{
	//Zanovo sozdanie sesii
	FString Msg = FString::Printf(TEXT("OnDestroySessionComplete, SessionName: %s, Success: %d"), *SessionName.ToString(), WasSuccessful);
	PrintString(Msg);

	if (CreateServerAfterDestroy)
	{
		CreateServerAfterDestroy = false;
		CreateServer(DestroyServerName);
	}

}



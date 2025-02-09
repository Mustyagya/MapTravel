// Fill out your copyright notice in the Description page of Project Settings.


#include "MultiplayerSessionSubsystem.h"
#include "OnlineSubsystem.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "Online/OnlineSessionNames.h"//��� ��� �� ����� �������� SEARCH_PRESENCE ��� ��������� �� ����� � 5,2 ������ �� ����





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
	ServerNameToFind = "";
	// ������ ������ FName ��� �������� ����� ������, ������� ����� ������������ � ����.
	 MySessionName = FName("Session name for Game");

}

void UMultiplayerSessionSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	//PrintString("MSS Initialize");
	// 
	// �������� ���������� ������-�������� (��������, Steam, NULL ��� ������).
	IOnlineSubsystem* OnlineSubsystem = IOnlineSubsystem::Get();

		// ���������, ������� �� �������� ���������� (��������, ���� Steam ��������, �� �� ������ ���� �����).
		if (OnlineSubsystem)
		{
			// �������� ��� ������������ ���������� � ������������ ��� � ������ ��� ������.
			FString SubsystemName = OnlineSubsystem->GetSubsystemName().ToString();
			// ������� �������� ���������� (��������, "Steam" ��� "NULL").
			PrintString(SubsystemName);
			// �������� ��������� ������ �� ����������.
			SessionInterface = OnlineSubsystem->GetSessionInterface();

			// ���������, ��� ��������� ������ ������������ (�� `nullptr`).
			if (SessionInterface.IsValid())
			{
			PrintString("Session interface is valid!");

			//�������
			SessionInterface->OnCreateSessionCompleteDelegates.AddUObject(this, &UMultiplayerSessionSubsystem::OnCreateSessionComplete);
			SessionInterface->OnDestroySessionCompleteDelegates.AddUObject(this, &UMultiplayerSessionSubsystem::OnDestroySessionComplete);
			SessionInterface->OnFindSessionsCompleteDelegates.AddUObject(this, &UMultiplayerSessionSubsystem::OnFindSessionsComplete);
			SessionInterface->OnJoinSessionCompleteDelegates.AddUObject(this,&UMultiplayerSessionSubsystem::OnJoinSessionComplete );
			
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

	

	FNamedOnlineSession* ExisteingSession = SessionInterface->GetNamedSession(MySessionName);//������� ������������ ����� ����� �������

	if (ExisteingSession)
	{
		FString Msg = FString::Printf(TEXT("Session with name %s alredy exists, destroying it"), *MySessionName.ToString());
		PrintString(Msg);

		CreateServerAfterDestroy = true;
		DestroyServerName = ServerName;
		SessionInterface->DestroySession(MySessionName);
		return;

	}

	// ������ ��������� ��� ����� ������.
	FOnlineSessionSettings SessionSettings;

	SessionSettings.bAllowJoinInProgress = true;// ��������� ������������� � ������ ����� � ������.
	SessionSettings.bIsDedicated = false;// �������������, ��� ��� �� ���������� ������ (���� ����� �������� � �� �������).
	SessionSettings.bShouldAdvertise = true;// ��������� ������� ������, ����� � ����� �������� ������ ������.
	SessionSettings.NumPublicConnections = 2;// ����� ���������� ��������� ����������� (2 ������� �����).
	SessionSettings.bUseLobbiesIfAvailable = true;// ���������� �����, ���� ��� �������� (��������, ��� Steam-������).
	SessionSettings.bUsesPresence = true;// ���������, ��� ������ ���������� ���������� � ����������� ������� (��������, ������ � Steam).
	SessionSettings.bAllowJoinViaPresence = true;// ��������� ������� �������������� ����� ������� ����������� (��������, ����� ������ ������).
	bool IsLan = false;// ��������� ���������� ��� �������� ���������� � ���, �������� �� ��� LAN-�������.


	// ���������, ������������ �� ��������� ���������� (��������, NULL � ��� LAN-������).
	if (IOnlineSubsystem::Get()->GetSubsystemName() == "NULL")
	{
		IsLan = true;
	}
	// �������������, ��� ��� LAN-������ (���� ��� ��������� ����).
	SessionSettings.bIsLANMatch = IsLan;
	SessionSettings.Set(FName("SERVER_NAME"), ServerName,EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);


	// ������ ������ � ���������� �����������: ��������� ����� (index 0), ��� ������, � ��������� ������.
	SessionInterface->CreateSession(0, MySessionName, SessionSettings);
}

void UMultiplayerSessionSubsystem::JoinServer(FString ServerName)
{
	PrintString("Join to Map!");
	if (ServerName.IsEmpty())
	{
		PrintString("Server name cannot be empty!");
		return;
	}

	SessionSearch = MakeShareable(new FOnlineSessionSearch());
	bool IsLAN = false;
	if (IOnlineSubsystem::Get()->GetSubsystemName() == "NULL")
	{
		IsLAN = true;
	}

	SessionSearch->bIsLanQuery = IsLAN;
	SessionSearch->MaxSearchResults = 999999;
	SessionSearch->QuerySettings.Set(SEARCH_PRESENCE, true, EOnlineComparisonOp::Equals);
	
	ServerNameToFind = ServerName;
	
	SessionInterface->FindSessions(0, SessionSearch.ToSharedRef());



	
}



void UMultiplayerSessionSubsystem::OnFindSessionsComplete(bool WasSuccessful)
{
	if (!WasSuccessful) return;
	if (ServerNameToFind.IsEmpty()) return;

	TArray<FOnlineSessionSearchResult> Results = SessionSearch->SearchResults;
	
	FOnlineSessionSearchResult *CorrectResult = 0;

	if (Results.Num() > 0)
	{
		FString Msg = FString::Printf(TEXT("%d sessions found."), Results.Num());
		PrintString(Msg);
		for (FOnlineSessionSearchResult Result : Results)
		{
			if (Result.IsValid())
			{
				FString ServerName = "No-Name";
				Result.Session.SessionSettings.Get(FName("SERVER_NAME"), ServerName);

				//����������� �������
				if (ServerName.Equals(ServerNameToFind))
				{
					CorrectResult = &Result;

					FString Msg2 = FString::Printf(TEXT("Find Server Name: %s"), *ServerName);
					PrintString(Msg2);
					break;

				}
			}
		}

		if (CorrectResult)
		{
			SessionInterface->JoinSession(0, MySessionName, *CorrectResult);

		}
		else
		{
			PrintString(FString::Printf(TEXT("Couldn`t find server: %s!!"), *ServerNameToFind));
			ServerNameToFind = "";
		}

	}
	else
	{
		PrintString("Zero sessions found.");
	}

}

//������������� � �����
void UMultiplayerSessionSubsystem::OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result)
{
	if (Result == EOnJoinSessionCompleteResult::Success)
	{
		FString Msg = FString::Printf(TEXT("Successfully jpined session %s"), *SessionName.ToString());
		PrintString(Msg);

		FString Address = "";
		bool Success = SessionInterface->GetResolvedConnectString(MySessionName, Address);

		if (Success)
		{
			PrintString(FString::Printf(TEXT("Adress: %s"), *Address));
			APlayerController* PlayerController = GetGameInstance()->GetFirstLocalPlayerController();
			if (PlayerController)
			{
				PlayerController->ClientTravel(Address, ETravelType::TRAVEL_Absolute);
			}
		}
		else
		{
			PrintString("GetResolvedConnectString returned false!");
		}
	}
	else
	{
		PrintString("OnJoinSessionComplete failed");
	}

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



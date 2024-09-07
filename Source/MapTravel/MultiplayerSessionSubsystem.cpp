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

					}
		// ������ ������ FName ��� �������� ����� ������, ������� ����� ������������ � ����.
		FName MySessionName = FName("Session name for Game");

		// ������ ��������� ��� ����� ������.
		FOnlineSessionSettings SessionSettings;

		SessionSettings.bAllowJoinInProgress = true;// ��������� ������������� � ������ ����� � ������.
		SessionSettings.bIsDedicated = false;// �������������, ��� ��� �� ���������� ������ (���� ����� �������� � �� �������).
		SessionSettings.bShouldAdvertise = true;// ��������� ������� ������, ����� � ����� �������� ������ ������.
		SessionSettings.NumPrivateConnections = 2;// ����� ���������� ��������� ����������� (2 ������� �����).
		SessionSettings.bUseLobbiesIfAvailable = true;// ���������� �����, ���� ��� �������� (��������, ��� Steam-������).
		SessionSettings.bUsesPresence = true;// ���������, ��� ������ ���������� ���������� � ����������� ������� (��������, ������ � Steam).
		SessionSettings.bAllowJoinViaPresence = true;// ��������� ������� �������������� ����� ������� ����������� (��������, ����� ������ ������).
		bool IsLan = false;// ��������� ���������� ��� �������� ���������� � ���, �������� �� ��� LAN-�������.


		// ���������, ������������ �� ��������� ���������� (��������, NULL � ��� LAN-������).
				if (IOnlineSubsystem::Get()->GetSubsystemName()=="NULL")
				{
					IsLan = true;
				}
		// �������������, ��� ��� LAN-������ (���� ��� ��������� ����).
		SessionSettings.bIsLANMatch = IsLan;
		// ������ ������ � ���������� �����������: ��������� ����� (index 0), ��� ������, � ��������� ������.
		SessionInterface->CreateSession(0, MySessionName, SessionSettings);
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



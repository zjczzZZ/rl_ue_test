// Fill out your copyright notice in the Description page of Project Settings.


#include "tcpclient.h"
#include <thread>
#include <chrono>

// Sets default values
Atcpclient::Atcpclient()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	SocketClient = nullptr;
}

// Called when the game starts or when spawned
void Atcpclient::BeginPlay()
{
	Super::BeginPlay();
	
}

void Atcpclient::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (SocketClient) {
		SocketClient->Close();
	}
	SocketClient = nullptr;
}

// Called every frame
void Atcpclient::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

bool Atcpclient::SocketCreate(FString IPStr, int32 port)
{
	//创建远程地址
	RemoteAddr = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateInternetAddr();
	bool bIsValid;
	RemoteAddr->SetIp(*IPStr, bIsValid);
	RemoteAddr->SetPort(port);
	if (!bIsValid)
	{
		UE_LOG(LogTemp, Warning, TEXT("Rama UDP Sender>> IP address was not valid! "), *IPStr);
		return false;
	}
	SocketClient = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateSocket(NAME_Stream, TEXT("default"), false);

	if (SocketClient->Connect(*RemoteAddr)) {
		UE_LOG(LogTemp, Warning, TEXT("Connect Succ!"));
		return true;
	}
	else {
		UE_LOG(LogTemp, Warning, TEXT("Connect failed!"));

		return false;
	}
	return false;
}

void Atcpclient::SocketSend(FString meesage)
{
	TCHAR* seriallizedChar = meesage.GetCharArray().GetData();

	int32 size = FCString::Strlen(seriallizedChar) + 1;

	int32 sent = 0;

	if (SocketClient->Send((uint8*)TCHAR_TO_UTF8(seriallizedChar), size, sent))
	{
		GEngine->AddOnScreenDebugMessage(1, 2.0f, FColor::Green, TEXT("_____Send Succ!"));
		UE_LOG(LogTemp, Warning, TEXT("_____Send Succ!"));
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(1, 2.0f, FColor::Green, TEXT("_____Send failed!"));
		UE_LOG(LogTemp, Warning, TEXT("_____Send failed!"));
	}
}

void Atcpclient::SocketReceive(bool& bReceive, FString& recvMessage)
{
	recvMessage = "";
	bReceive = false;
	if (!SocketClient)
	{
		return;
	}
	TArray<uint8> ReceiveData;
	uint32 size;
	uint8 element = 0;
	//while (!SocketClient->HasPendingData(size))
	//{
	//	std::this_thread::yield();
	//}
	while (SocketClient->HasPendingData(size))
	{
		ReceiveData.Init(element, FMath::Min(size, 65507u));
		int32 read = 0;
		SocketClient->Recv(ReceiveData.GetData(), ReceiveData.Num(), read);
	}
	if (ReceiveData.Num() <= 0)
	{
		return;
	}
	FString log = "Total Data read! num: " + FString::FromInt(ReceiveData.Num() <= 0);
	GEngine->AddOnScreenDebugMessage(1, 2.0f, FColor::Green, log);
	UE_LOG(LogTemp, Warning, TEXT("Recv log:  %s"), *log);
	const FString ReceivedUE4String = StringFromBinaryArray(ReceiveData);
	log = "Server:" + ReceivedUE4String;
	GEngine->AddOnScreenDebugMessage(1, 2.0f, FColor::Green, log);
	UE_LOG(LogTemp, Warning, TEXT("*** %s"), *log);
	recvMessage = ReceivedUE4String;
	bReceive = true;
}

FString Atcpclient::StringFromBinaryArray(TArray<uint8> BinaryArray)
{
	return FString(ANSI_TO_TCHAR(reinterpret_cast<const char*>(BinaryArray.GetData())));
}


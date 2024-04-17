// Fill out your copyright notice in the Description page of Project Settings.


#include "UdpClient.h"

// Sets default values
AUdpClient::AUdpClient()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	SenderSocket = NULL;
}

// Called when the game starts or when spawned
void AUdpClient::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AUdpClient::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AUdpClient::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	if (SenderSocket)
	{
		SenderSocket->Close();
		ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->DestroySocket(SenderSocket);
	}
}

bool AUdpClient::StartUDPSender(const FString& YourChosenSocketName, const FString& TheIP, const int32 ThePort, bool UDP)
{
	RemoteAddr = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateInternetAddr();
	bool bIsValid;
	RemoteAddr->SetIp(*TheIP, bIsValid);
	RemoteAddr->SetPort(ThePort);
	if (!bIsValid)
	{
		UE_LOG(LogTemp, Warning, TEXT("Rama UDP Sender>> IP address was not valid! "), *TheIP);
		return false;
	}

	SenderSocket = FUdpSocketBuilder(*YourChosenSocketName)
		.AsReusable()
		.WithBroadcast() // 广播
		.WithSendBufferSize(2 * 1024 * 1024);

	int32 SendSize = 2 * 1024 * 1024;
	SenderSocket->SetSendBufferSize(SendSize, SendSize);
	SenderSocket->SetReceiveBufferSize(SendSize, SendSize);
	if (bIsValid)
	{
		bIsValid = true;
	}
	return bIsValid;

}

bool AUdpClient::RamaUDPSender_SendString(FString ToSend)//发送消息
{
	if (!SenderSocket)
	{
		UE_LOG(LogTemp, Warning, TEXT("No sender socket"));
		return false;
	}
	//消息处理
	int32 BytesSent = 0;
	FString serialized = ToSend;
	TCHAR* serializedChar = serialized.GetCharArray().GetData();
	int32 size = FCString::Strlen(serializedChar);
	int32 sent = 0;
	SenderSocket->SendTo((uint8*)TCHAR_TO_UTF8(serializedChar), size, BytesSent, *RemoteAddr);
	if (BytesSent < 0)
	{
		const FString Str = "Socket is valid but the receiver received 0 bytes, make sure it is listening properly!";
		UE_LOG(LogTemp, Error, TEXT("%s"), *Str);
		return false;
	}
	UE_LOG(LogTemp, Warning, TEXT("UDP Send Succcess! INFO Sent = %s "), *ToSend);

	return true;

}

void AUdpClient::DataRecv(FString& str, bool& success)
{
	if (!SenderSocket)
	{
		UE_LOG(LogTemp, Warning, TEXT("No Send Sockets"));
		success = false;
		//return success;
	}
	TSharedRef<FInternetAddr> targetAddr = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateInternetAddr();
	TArray<uint8> ReceivedData;//定义一个接收器
	uint32 Size;
	if (SenderSocket->HasPendingData(Size))
	{
		success = true;
		str = "";
		uint8* Recv = new uint8[Size];
		int32 BytesRead = 0;

		ReceivedData.SetNumUninitialized(FMath::Min(Size, 65507u));
		SenderSocket->RecvFrom(ReceivedData.GetData(), ReceivedData.Num(), BytesRead, *targetAddr);//创建远程接收地址
		char ansiiData[1024];
		memcpy(ansiiData, ReceivedData.GetData(), BytesRead);//拷贝数据到接收器
		ansiiData[BytesRead] = 0;                            //判断数据结束
		FString debugData = ANSI_TO_TCHAR(ansiiData);         //字符串转换
		str = debugData;
		// memset(ansiiData,0,1024);//清空 

	}
	else
	{
		success = false;
	}
}


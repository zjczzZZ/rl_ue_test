// Fill out your copyright notice in the Description page of Project Settings.


#include "UdpServer.h"
#include <thread>
#include <chrono>

// Sets default values
AUdpServer::AUdpServer()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	ListenSocket = NULL;
}

// Called when the game starts or when spawned
void AUdpServer::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AUdpServer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AUdpServer::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	delete UDPReceiver;
	UDPReceiver = nullptr;

	//Clear all sockets!
	//      makes sure repeat plays in Editor dont hold on to old sockets!
	if (ListenSocket)
	{
		ListenSocket->Close();
		ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->DestroySocket(ListenSocket);
	}
}

void AUdpServer::StartUDPReceiver(const FString& YourChosenSocketName, const FString& TheIP, const int32 ThePort, const FString& sendIP, const int32 sendPort, bool& success)
{
	//创建远程地址
	RemoteAddr = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateInternetAddr();
	bool bIsValid;
	RemoteAddr->SetIp(*sendIP, bIsValid);
	RemoteAddr->SetPort(sendPort);
	if (!bIsValid)
	{
		UE_LOG(LogTemp, Warning, TEXT("Rama UDP Sender>> IP address was not valid! "), *TheIP);
		return;
	}


	TSharedRef<FInternetAddr> targetAddr = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateInternetAddr();
	FIPv4Address Addr;
	FIPv4Address::Parse(TheIP, Addr);
	FIPv4Endpoint Endpoint(FIPv4Address::Any, ThePort);  //所有ip地址本地
														 //FIPv4Endpoint Endpoint(Addr, ThePort);                 //指定ip地址
	ListenSocket = FUdpSocketBuilder(*YourChosenSocketName)
		.AsNonBlocking()
		.AsReusable()
		.BoundToEndpoint(Endpoint)
		.WithReceiveBufferSize(2 * 1024 * 1024);
	//BUFFER SIZE
	int32 BufferSize = 2 * 1024 * 1024;
	ListenSocket->SetSendBufferSize(BufferSize, BufferSize);
	ListenSocket->SetReceiveBufferSize(BufferSize, BufferSize);

	if (!ListenSocket)
	{
		UE_LOG(LogTemp, Warning, TEXT("No Scokets"));
		success = false;

	}
	if (ListenSocket)
	{
		UE_LOG(LogTemp, Warning, TEXT("The receiver is initialized"));
		success = true;
	}
}

void AUdpServer::DataRecv(FString& str, bool& success)
{
	if (!ListenSocket)
	{
		UE_LOG(LogTemp, Warning, TEXT("No Send Sockets"));
		success = false;
		//return success;
	}
	TSharedRef<FInternetAddr> targetAddr = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateInternetAddr();
	TArray<uint8> ReceivedData;//定义一个接收器
	uint32 Size;

	//while (!ListenSocket->HasPendingData(Size)) {
	//	std::this_thread::yield();
	//}

	if (ListenSocket->HasPendingData(Size))
	{
		success = true;
		str = "";
		uint8* Recv = new uint8[Size];
		int32 BytesRead = 0;

		ReceivedData.SetNumUninitialized(FMath::Min(Size, 65507u));
		ListenSocket->RecvFrom(ReceivedData.GetData(), ReceivedData.Num(), BytesRead, *targetAddr);//创建远程接收地址

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


bool AUdpServer::RamaUDPSender_SendString(FString ToSend)//发送消息
{
	if (!ListenSocket)
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
	ListenSocket->SendTo((uint8*)TCHAR_TO_UTF8(serializedChar), size, BytesSent, *RemoteAddr);
	if (BytesSent < 0)
	{
		const FString Str = "Socket is valid but the receiver received 0 bytes, make sure it is listening properly!";
		UE_LOG(LogTemp, Error, TEXT("%s"), *Str);
		return false;
	}
	UE_LOG(LogTemp, Warning, TEXT("UDP Send Succcess! INFO Sent = %s "), *ToSend);

	return true;

}